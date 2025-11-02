#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QMenu>
#include <QAction>
#include <QClipboard>
#include <QApplication>
#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <QEvent>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QUrl>
#include <QRegularExpression>
#include <QSet>
#include <QtGlobal>

#ifdef Q_OS_WIN
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600  // Windows Vista or later
#endif
#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>
#endif

#ifdef Q_OS_LINUX
#include <signal.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <QHash>

namespace {
QStringList buildLibrarySearchPaths(const QStringList &hintPaths)
{
    QSet<QString> unique;
    for (const QString &path : hintPaths) {
        if (!path.isEmpty()) {
            unique.insert(QDir(path).absolutePath());
        }
    }

    QByteArray ldLibraryEnv = qgetenv("LD_LIBRARY_PATH");
    if (!ldLibraryEnv.isEmpty()) {
        const QStringList envPaths = QString::fromLocal8Bit(ldLibraryEnv).split(':', QString::SkipEmptyParts);
        for (const QString &path : envPaths) {
            if (!path.isEmpty()) {
                unique.insert(QDir(path).absolutePath());
            }
        }
    }

    const QStringList defaults = {
        QStringLiteral("/lib"),
        QStringLiteral("/usr/lib"),
        QStringLiteral("/lib64"),
        QStringLiteral("/usr/lib64"),
        QStringLiteral("/usr/local/lib"),
        QStringLiteral("/usr/local/lib64")
    };

    for (const QString &path : defaults) {
        unique.insert(path);
    }

    return unique.values();
}

QString resolveLibraryPath(const QString &libName, const QStringList &hintPaths = QStringList())
{
    if (libName.isEmpty()) {
        return QString();
    }

    QFileInfo directInfo(libName);
    if (directInfo.exists()) {
        return directInfo.canonicalFilePath();
    }

    const QStringList searchPaths = buildLibrarySearchPaths(hintPaths);
    for (const QString &dirPath : searchPaths) {
        QFileInfo candidate(QDir(dirPath).filePath(libName));
        if (candidate.exists()) {
            return candidate.canonicalFilePath();
        }
    }

    static bool cacheInitialized = false;
    static QHash<QString, QString> ldconfigCache;

    if (!cacheInitialized) {
        cacheInitialized = true;
        QProcess ldconfig;
        ldconfig.start("ldconfig", QStringList() << "-p");
        if (ldconfig.waitForFinished(2000)) {
            QString output = QString::fromLocal8Bit(ldconfig.readAllStandardOutput());
            QRegularExpression re("\\s*(\\S+)\\s+\\(.*\\)\\s+=>\\s+(\\S+)");
            const QStringList lines = output.split('\n', QString::SkipEmptyParts);
            for (const QString &line : lines) {
                QRegularExpressionMatch match = re.match(line);
                if (match.hasMatch()) {
                    const QString name = match.captured(1);
                    const QString path = match.captured(2);
                    if (!name.isEmpty() && !path.isEmpty()) {
                        ldconfigCache.insert(name, path);
                    }
                }
            }
        }
    }

    if (ldconfigCache.contains(libName)) {
        return ldconfigCache.value(libName);
    }

    return QString();
}
}
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , networkManager(new QNetworkAccessManager(this))
    , ipQueryManager(new QNetworkAccessManager(this))
    , locationManager(new QNetworkAccessManager(this))
    , handleModel(new QStandardItemModel(this))
{
    ui->setupUi(this);
    
    setWindowTitle(QString::fromUtf8("IP工具箱 - 句柄查询 & IP查询"));
    
    // 设置拖放
    ui->lineEditBinaryPath->setAcceptDrops(true);
    ui->lineEditBinaryPath->installEventFilter(this);
    ui->lineEditPath->setAcceptDrops(true);
    ui->lineEditPath->installEventFilter(this);
    
    // 设置句柄查询表格模型
    handleModel->setColumnCount(3);
    handleModel->setHorizontalHeaderLabels(QStringList() 
        << QString::fromUtf8("进程名称") 
        << QString::fromUtf8("PID") 
        << QString::fromUtf8("进程路径"));
    ui->tableViewHandles->setModel(handleModel);
    ui->tableViewHandles->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tableViewHandles->horizontalHeader()->setStretchLastSection(true);
    ui->tableViewHandles->setColumnWidth(0, 200);
    ui->tableViewHandles->setColumnWidth(1, 80);
    ui->tableViewHandles->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableViewHandles->setSelectionBehavior(QAbstractItemView::SelectRows);
    
    // 设置文本框右键菜单
    ui->textEditDeps->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->textEditSymbols->setContextMenuPolicy(Qt::CustomContextMenu);
    
    // 连接右键菜单信号
    connect(ui->tableViewHandles, &QTableView::customContextMenuRequested,
            this, &MainWindow::showContextMenu);
    connect(ui->textEditDeps, &QTextEdit::customContextMenuRequested,
            this, &MainWindow::showTextEditContextMenu);
    connect(ui->textEditSymbols, &QTextEdit::customContextMenuRequested,
            this, &MainWindow::showTextEditContextMenu);
    
    // 连接网络请求信号
    connect(networkManager, &QNetworkAccessManager::finished,
            this, &MainWindow::onMyIPReplyFinished);
    connect(ipQueryManager, &QNetworkAccessManager::finished,
            this, &MainWindow::onIPQueryReplyFinished);
    connect(locationManager, &QNetworkAccessManager::finished,
            this, &MainWindow::onIPLocationReplyFinished);
    
    // 自动获取本机IP
    on_btnGetMyIP_clicked();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 浏览文件/文件夹
void MainWindow::on_btnBrowse_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, 
        QString::fromUtf8("选择文件"), 
        "", 
        QString::fromUtf8("所有文件 (*.*)"));
    
    if (!path.isEmpty()) {
        ui->lineEditPath->setText(path);
    }
}

// 搜索句柄
void MainWindow::on_btnSearchHandle_clicked()
{
    QString path = ui->lineEditPath->text().trimmed();
    
    if (path.isEmpty()) {
        QMessageBox::warning(this, 
            QString::fromUtf8("警告"), 
            QString::fromUtf8("请输入文件路径！"));
        return;
    }
    
    QFileInfo fileInfo(path);
    if (!fileInfo.exists()) {
        QMessageBox::warning(this, 
            QString::fromUtf8("警告"), 
            QString::fromUtf8("文件或路径不存在！"));
        return;
    }
    
    searchFileHandles(path);
}

// 刷新句柄查询
void MainWindow::on_btnRefreshHandle_clicked()
{
    if (!ui->lineEditPath->text().isEmpty()) {
        on_btnSearchHandle_clicked();
    }
}

// 获取本机公网IP
void MainWindow::on_btnGetMyIP_clicked()
{
    ui->textEditMyIP->append(QString::fromUtf8("正在获取本机公网IP..."));
    
    // 使用多个IP查询服务
    QNetworkRequest request(QUrl("http://ip-api.com/json/?lang=zh-CN"));
    networkManager->get(request);
}

// 查询指定IP
void MainWindow::on_btnQueryIP_clicked()
{
    QString ip = ui->lineEditQueryIP->text().trimmed();
    
    if (ip.isEmpty()) {
        QMessageBox::warning(this, 
            QString::fromUtf8("警告"), 
            QString::fromUtf8("请输入要查询的IP地址！"));
        return;
    }
    
    ui->textEditIPResult->append(QString::fromUtf8("\n正在查询IP: %1...").arg(ip));
    
    QNetworkRequest request(QUrl(QString("http://ip-api.com/json/%1?lang=zh-CN").arg(ip)));
    ipQueryManager->get(request);
}

// 本机IP查询结果
void MainWindow::onMyIPReplyFinished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject obj = doc.object();
        
        if (obj["status"].toString() == "success") {
            QString ip = obj["query"].toString();
            QString country = obj["country"].toString();
            QString regionName = obj["regionName"].toString();
            QString city = obj["city"].toString();
            QString isp = obj["isp"].toString();
            QString timezone = obj["timezone"].toString();
            
            ui->textEditMyIP->clear();
            
            // 显示局域网IP信息
            ui->textEditMyIP->append(QString::fromUtf8("╔═══════════════════════════════════════════════╗"));
            ui->textEditMyIP->append(QString::fromUtf8("║         本机局域网IP地址信息                  ║"));
            ui->textEditMyIP->append(QString::fromUtf8("╚═══════════════════════════════════════════════╝"));
            ui->textEditMyIP->append(getLocalIPAddresses());
            ui->textEditMyIP->append("");
            
            // 显示公网IP信息
            ui->textEditMyIP->append(QString::fromUtf8("╔═══════════════════════════════════════════════╗"));
            ui->textEditMyIP->append(QString::fromUtf8("║         本机公网IP地址信息                    ║"));
            ui->textEditMyIP->append(QString::fromUtf8("╚═══════════════════════════════════════════════╝"));
            ui->textEditMyIP->append(QString::fromUtf8("🌐 公网IP地址: %1").arg(ip));
            ui->textEditMyIP->append(QString::fromUtf8("📍 国家地区:   %1").arg(country));
            ui->textEditMyIP->append(QString::fromUtf8("📍 省份:       %1").arg(regionName));
            ui->textEditMyIP->append(QString::fromUtf8("📍 城市:       %1").arg(city));
            ui->textEditMyIP->append(QString::fromUtf8("🏢 运营商:     %1").arg(isp));
            ui->textEditMyIP->append(QString::fromUtf8("🕐 时区:       %1").arg(timezone));
            ui->textEditMyIP->append(QString::fromUtf8("⏰ 查询时间:   %1")
                .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));
        } else {
            ui->textEditMyIP->append(QString::fromUtf8("获取IP信息失败！"));
        }
    } else {
        ui->textEditMyIP->append(QString::fromUtf8("网络错误: %1").arg(reply->errorString()));
    }
    
    reply->deleteLater();
}

// IP查询结果
void MainWindow::onIPQueryReplyFinished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject obj = doc.object();
        
        if (obj["status"].toString() == "success") {
            QString ip = obj["query"].toString();
            QString country = obj["country"].toString();
            QString regionName = obj["regionName"].toString();
            QString city = obj["city"].toString();
            QString isp = obj["isp"].toString();
            QString org = obj["org"].toString();
            QString as = obj["as"].toString();
            double lat = obj["lat"].toDouble();
            double lon = obj["lon"].toDouble();
            
            ui->textEditIPResult->append(QString::fromUtf8("========== IP查询结果 =========="));
            ui->textEditIPResult->append(QString::fromUtf8("IP地址: %1").arg(ip));
            ui->textEditIPResult->append(QString::fromUtf8("国家: %1").arg(country));
            ui->textEditIPResult->append(QString::fromUtf8("省份: %1").arg(regionName));
            ui->textEditIPResult->append(QString::fromUtf8("城市: %1").arg(city));
            ui->textEditIPResult->append(QString::fromUtf8("运营商: %1").arg(isp));
            ui->textEditIPResult->append(QString::fromUtf8("组织: %1").arg(org));
            ui->textEditIPResult->append(QString::fromUtf8("AS: %1").arg(as));
            ui->textEditIPResult->append(QString::fromUtf8("经纬度: %1, %2").arg(lat).arg(lon));
            ui->textEditIPResult->append(QString::fromUtf8("查询时间: %1")
                .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));
        } else {
            ui->textEditIPResult->append(QString::fromUtf8("查询失败: IP地址无效或不存在！"));
        }
    } else {
        ui->textEditIPResult->append(QString::fromUtf8("网络错误: %1").arg(reply->errorString()));
    }
    
    reply->deleteLater();
}

void MainWindow::onIPLocationReplyFinished(QNetworkReply *reply)
{
    reply->deleteLater();
}

// 搜索文件句柄
void MainWindow::searchFileHandles(const QString &path)
{
    handleModel->removeRows(0, handleModel->rowCount());
    ui->labelHandleStatus->setText(QString::fromUtf8("正在搜索..."));
    
#ifdef Q_OS_WIN
    
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        QMessageBox::critical(this, 
            QString::fromUtf8("错误"), 
            QString::fromUtf8("无法创建进程快照！"));
        return;
    }
    
    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);
    
    int foundCount = 0;
    
    if (Process32FirstW(snapshot, &pe32)) {
        do {
            DWORD processId = pe32.th32ProcessID;
            
            // 打开进程
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, 
                                         FALSE, processId);
            if (hProcess) {
                wchar_t processPath[MAX_PATH] = {0};
                DWORD pathLen = 0;
                
                // 尝试获取进程完整路径 - 使用GetModuleFileNameEx（兼容性更好）
                pathLen = GetModuleFileNameExW(hProcess, NULL, processPath, MAX_PATH);
                
                if (pathLen > 0) {
                    QString procPath = QString::fromWCharArray(processPath);
                    QString targetPath = QDir::toNativeSeparators(path);
                    
                    // 简单检查：如果进程路径在目标路径下，或者可能打开了文件
                    if (procPath.contains(targetPath, Qt::CaseInsensitive) ||
                        targetPath.contains(procPath, Qt::CaseInsensitive)) {
                        
                        QString processName = QString::fromWCharArray(pe32.szExeFile);
                        
                        QList<QStandardItem*> rowItems;
                        rowItems << new QStandardItem(processName);
                        rowItems << new QStandardItem(QString::number(processId));
                        rowItems << new QStandardItem(procPath);
                        
                        handleModel->appendRow(rowItems);
                        foundCount++;
                    }
                }
                
                CloseHandle(hProcess);
            }
        } while (Process32NextW(snapshot, &pe32));
    }
    
    CloseHandle(snapshot);
    
    ui->labelHandleStatus->setText(
        QString::fromUtf8("搜索完成，找到 %1 个相关进程").arg(foundCount));
    
    if (foundCount == 0) {
        QMessageBox::information(this, 
            QString::fromUtf8("提示"), 
            QString::fromUtf8("未找到占用该文件的进程。\n注意：需要管理员权限才能检测所有进程。"));
    }
    
#elif defined(Q_OS_LINUX)
    // Linux 使用 lsof 命令查找文件占用
    int foundCount = 0;
    
    QProcess process;
    QFileInfo targetInfo(path);
    QString targetPath = targetInfo.absoluteFilePath();
    QStringList arguments;

    if (targetInfo.isDir()) {
        // +D 会递归扫描目录
        arguments << "+D" << targetPath;
    } else {
        arguments << targetPath;
    }

    process.start("lsof", arguments);

    if (!process.waitForFinished(10000)) {
        process.kill();
        ui->labelHandleStatus->setText(QString::fromUtf8("lsof 执行超时"));
        QMessageBox::warning(this,
            QString::fromUtf8("超时"),
            QString::fromUtf8("lsof 查询耗时过长，已取消。请缩小查询范围或以 root 权限重试。"));
        return;
    }
    
    QString output = QString::fromLocal8Bit(process.readAllStandardOutput());
    QString errorText = QString::fromLocal8Bit(process.readAllStandardError());
    int exitCode = process.exitCode();

    if (!output.trimmed().isEmpty()) {
        QStringList lines = output.split('\n', QString::SkipEmptyParts);
        
        for (int i = 1; i < lines.size(); ++i) {  // 跳过标题行
            QStringList parts = lines[i].split(QRegExp("\\s+"), QString::SkipEmptyParts);
            if (parts.size() >= 2) {
                QString processName = parts[0];
                QString pidStr = parts[1];
                bool ok;
                ProcessId pid = pidStr.toULong(&ok);
                
                if (ok) {
                    QString procPath = QString("/proc/%1/exe").arg(pid);
                    QFileInfo fi(procPath);
                    QString exePath = fi.symLinkTarget();
                    
                    QList<QStandardItem*> rowItems;
                    rowItems << new QStandardItem(processName);
                    rowItems << new QStandardItem(pidStr);
                    rowItems << new QStandardItem(exePath.isEmpty() ? QString::fromUtf8("无法访问") : exePath);
                    
                    handleModel->appendRow(rowItems);
                    foundCount++;
                }
            }
        }

        ui->labelHandleStatus->setText(
            QString::fromUtf8("搜索完成，找到 %1 个相关进程").arg(foundCount));

        if (foundCount == 0) {
            QMessageBox::information(this, 
                QString::fromUtf8("提示"), 
                QString::fromUtf8("未找到占用该路径的进程。\n提示：可能需要 root 权限或安装 lsof 工具。"));
        } else if (exitCode != 0) {
            QString warningMsg = errorText.trimmed();
            if (warningMsg.isEmpty()) {
                warningMsg = QString::fromUtf8("lsof 返回代码 %1 (可能是权限或 FUSE 文件系统造成的警告)")
                    .arg(exitCode);
            }

            QMessageBox::information(this,
                QString::fromUtf8("部分结果"),
                QString::fromUtf8("lsof 返回了部分结果，但伴随以下警告：\n%1\n\n信息可能不完整，建议使用 sudo 重新执行或排除无法访问的挂载点。")
                    .arg(warningMsg));
        }
    } else {
        if (errorText.trimmed().isEmpty()) {
            errorText = QString::fromUtf8("lsof 返回代码: %1").arg(exitCode);
        }
        ui->labelHandleStatus->setText(QString::fromUtf8("查询失败"));
        QMessageBox::warning(this,
            QString::fromUtf8("查询失败"),
            QString::fromUtf8("无法执行 lsof 查询：%1\n请确认已安装 lsof，并具有足够权限。")
                .arg(errorText));
    }
    
#else
    Q_UNUSED(path);
    ui->labelHandleStatus->setText(QString::fromUtf8("此平台暂不支持"));
    QMessageBox::information(this, 
        QString::fromUtf8("提示"), 
        QString::fromUtf8("此功能暂不支持当前平台！"));
#endif
}

QString MainWindow::getProcessName(ProcessId processId)
{
#ifdef Q_OS_WIN
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, 
                                  FALSE, processId);
    if (hProcess) {
        wchar_t processName[MAX_PATH] = {0};
        if (GetModuleBaseNameW(hProcess, NULL, processName, MAX_PATH)) {
            CloseHandle(hProcess);
            return QString::fromWCharArray(processName);
        }
        CloseHandle(hProcess);
    }
    return QString::fromUtf8("未知");
#elif defined(Q_OS_LINUX)
    QString cmdlinePath = QString("/proc/%1/comm").arg(processId);
    QFile file(cmdlinePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString name = in.readLine().trimmed();
        file.close();
        return name.isEmpty() ? QString::fromUtf8("未知") : name;
    }
    return QString::fromUtf8("未知");
#else
    Q_UNUSED(processId);
    return QString::fromUtf8("未知");
#endif
}

bool MainWindow::isFileInUse(const QString &filePath, ProcessId processId)
{
    Q_UNUSED(filePath);
    Q_UNUSED(processId);
    // 这里需要使用更高级的API来检测文件句柄
    // 简化版本只检查进程路径
    return false;
}

// 杀死进程
bool MainWindow::killProcess(ProcessId processId)
{
#ifdef Q_OS_WIN
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processId);
    if (hProcess) {
        bool result = TerminateProcess(hProcess, 0);
        CloseHandle(hProcess);
        return result;
    }
    return false;
#elif defined(Q_OS_LINUX)
    return (kill(processId, SIGKILL) == 0);
#else
    Q_UNUSED(processId);
    return false;
#endif
}

// 获取进程加载的模块（DLL和文件）
QStringList MainWindow::getProcessModules(ProcessId processId)
{
    QStringList modules;
    
#ifdef Q_OS_WIN
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    
    if (hProcess) {
        HMODULE hMods[1024];
        DWORD cbNeeded;
        
        if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded)) {
            for (unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
                wchar_t szModName[MAX_PATH];
                if (GetModuleFileNameExW(hProcess, hMods[i], szModName, MAX_PATH)) {
                    modules.append(QString::fromWCharArray(szModName));
                }
            }
        }
        CloseHandle(hProcess);
    }
#elif defined(Q_OS_LINUX)
    QString mapsPath = QString("/proc/%1/maps").arg(processId);
    QFile file(mapsPath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QSet<QString> uniquePaths;  // 使用Set去重
        
        while (!in.atEnd()) {
            QString line = in.readLine();
            // maps 格式: address perms offset dev inode pathname
            QStringList parts = line.split(QRegExp("\\s+"));
            if (parts.size() >= 6) {
                QString path = parts.mid(5).join(" ").trimmed();
                if (!path.isEmpty() && !path.startsWith("[")) {
                    uniquePaths.insert(path);
                }
            }
        }
        file.close();
        modules = uniquePaths.toList();
    }
#else
    Q_UNUSED(processId);
#endif
    
    return modules;
}

// 获取本机所有局域网IP地址
QString MainWindow::getLocalIPAddresses()
{
    QString result;
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    
    int index = 1;
    for (const QNetworkInterface &netInterface : interfaces) {
        // 跳过未激活或回环接口
        if (!(netInterface.flags() & QNetworkInterface::IsUp) ||
            (netInterface.flags() & QNetworkInterface::IsLoopBack)) {
            continue;
        }
        
        QList<QNetworkAddressEntry> entries = netInterface.addressEntries();
        for (const QNetworkAddressEntry &entry : entries) {
            QHostAddress addr = entry.ip();
            
            // 只显示IPv4地址
            if (addr.protocol() == QAbstractSocket::IPv4Protocol) {
                result += QString::fromUtf8("🖧 网卡 %1: %2\n")
                    .arg(index)
                    .arg(netInterface.humanReadableName());
                result += QString::fromUtf8("   IP地址:    %1\n").arg(addr.toString());
                result += QString::fromUtf8("   子网掩码:  %1\n").arg(entry.netmask().toString());
                result += QString::fromUtf8("   MAC地址:   %1\n").arg(netInterface.hardwareAddress());
                result += "\n";
                index++;
            }
        }
    }
    
    if (result.isEmpty()) {
        result = QString::fromUtf8("⚠ 未找到有效的局域网连接\n");
    }
    
    return result;
}

// 显示右键菜单
void MainWindow::showContextMenu(const QPoint &pos)
{
    QModelIndex index = ui->tableViewHandles->indexAt(pos);
    if (!index.isValid()) {
        return;
    }
    
    QMenu contextMenu(this);
    QAction *copyProcessAction = contextMenu.addAction(QString::fromUtf8("📋 复制进程名称"));
    QAction *copyPidAction = contextMenu.addAction(QString::fromUtf8("📋 复制PID"));
    QAction *copyPathAction = contextMenu.addAction(QString::fromUtf8("📋 复制路径"));
    QAction *copyRowAction = contextMenu.addAction(QString::fromUtf8("📋 复制整行信息"));
    contextMenu.addSeparator();
    QAction *killAction = contextMenu.addAction(QString::fromUtf8("🔴 结束进程"));
    contextMenu.addSeparator();
    QAction *refreshAction = contextMenu.addAction(QString::fromUtf8("🔄 刷新列表"));
    
    QAction *selectedAction = contextMenu.exec(ui->tableViewHandles->viewport()->mapToGlobal(pos));
    
    if (selectedAction == copyProcessAction) {
        copySelectedColumn(0);
    } else if (selectedAction == copyPidAction) {
        copySelectedColumn(1);
    } else if (selectedAction == copyPathAction) {
        copySelectedColumn(2);
    } else if (selectedAction == copyRowAction) {
        copySelectedRow();
    } else if (selectedAction == killAction) {
        killSelectedProcess();
    } else if (selectedAction == refreshAction) {
        on_btnRefreshHandle_clicked();
    }
}

// 杀死选中的进程
void MainWindow::killSelectedProcess()
{
    QModelIndexList selection = ui->tableViewHandles->selectionModel()->selectedRows();
    if (selection.isEmpty()) {
        return;
    }
    
    int row = selection.first().row();
    QString processName = handleModel->item(row, 0)->text();
    QString pidStr = handleModel->item(row, 1)->text();
    
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,
        QString::fromUtf8("确认结束进程"),
        QString::fromUtf8("确定要结束进程 %1 (PID: %2) 吗？\n\n警告：强制结束进程可能导致数据丢失！")
            .arg(processName).arg(pidStr),
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        ProcessId pid = pidStr.toULong();
        if (killProcess(pid)) {
            QMessageBox::information(this,
                QString::fromUtf8("成功"),
                QString::fromUtf8("进程已成功结束！"));
            on_btnRefreshHandle_clicked();
        } else {
            QMessageBox::critical(this,
                QString::fromUtf8("失败"),
                QString::fromUtf8("无法结束进程！\n可能需要管理员/root权限。"));
        }
    }
}

void MainWindow::copySelectedColumn(int column)
{
    if (column < 0 || column >= handleModel->columnCount()) {
        return;
    }

    QModelIndexList selection = ui->tableViewHandles->selectionModel()->selectedRows(column);
    if (selection.isEmpty()) {
        return;
    }

    QString text = selection.first().data().toString();
    QApplication::clipboard()->setText(text);

    const QString preview = text.left(60);
    ui->labelHandleStatus->setText(QString::fromUtf8("已复制: %1%2")
        .arg(preview)
        .arg(text.length() > preview.length() ? QString::fromUtf8("...") : QString()));
}

void MainWindow::copySelectedRow()
{
    QModelIndexList selection = ui->tableViewHandles->selectionModel()->selectedRows();
    if (selection.isEmpty()) {
        return;
    }

    int row = selection.first().row();
    QStringList parts;
    for (int col = 0; col < handleModel->columnCount(); ++col) {
        QStandardItem *item = handleModel->item(row, col);
        parts << (item ? item->text() : QString());
    }

    QString text = parts.join("\t");
    QApplication::clipboard()->setText(text);

    ui->labelHandleStatus->setText(QString::fromUtf8("已复制整行信息"));
}

// 文本编辑框右键菜单
void MainWindow::showTextEditContextMenu(const QPoint &pos)
{
    QTextEdit *textEdit = qobject_cast<QTextEdit*>(sender());
    if (!textEdit) {
        return;
    }
    
    QMenu contextMenu(this);
    QAction *copyAllAction = contextMenu.addAction(QString::fromUtf8("📋 复制全部内容"));
    QAction *copySelectedAction = nullptr;
    
    if (!textEdit->textCursor().selectedText().isEmpty()) {
        copySelectedAction = contextMenu.addAction(QString::fromUtf8("📋 复制选中内容"));
    }
    
    contextMenu.addSeparator();
    QAction *clearAction = contextMenu.addAction(QString::fromUtf8("🗑 清空"));
    
    QAction *selectedAction = contextMenu.exec(textEdit->mapToGlobal(pos));
    
    if (selectedAction == copyAllAction) {
        QApplication::clipboard()->setText(textEdit->toPlainText());
        QString widgetName = textEdit->objectName();
        if (widgetName == "textEditDeps") {
            // 可以在状态栏显示提示，但这个界面没有状态栏，可以暂时忽略
        }
    } else if (selectedAction == copySelectedAction) {
        QApplication::clipboard()->setText(textEdit->textCursor().selectedText().replace(QChar(0x2029), '\n'));
    } else if (selectedAction == clearAction) {
        textEdit->clear();
    }
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if ((watched == ui->lineEditBinaryPath || watched == ui->lineEditPath) && event) {
        if (event->type() == QEvent::DragEnter || event->type() == QEvent::DragMove) {
            QDragEnterEvent *dragEvent = static_cast<QDragEnterEvent*>(event);
            if (dragEvent && dragEvent->mimeData()->hasUrls()) {
                dragEvent->acceptProposedAction();
                return true;
            }
        } else if (event->type() == QEvent::Drop) {
            QDropEvent *dropEvent = static_cast<QDropEvent*>(event);
            if (dropEvent && dropEvent->mimeData()->hasUrls()) {
                const QList<QUrl> urls = dropEvent->mimeData()->urls();
                if (!urls.isEmpty()) {
                    const QString localPath = urls.first().toLocalFile();
                    if (!localPath.isEmpty()) {
                        if (watched == ui->lineEditBinaryPath) {
                            ui->lineEditBinaryPath->setText(localPath);
                        } else if (watched == ui->lineEditPath) {
                            ui->lineEditPath->setText(localPath);
                        }
                        dropEvent->acceptProposedAction();
                        return true;
                    }
                }
            }
        }
    }

    return QMainWindow::eventFilter(watched, event);
}

// 进程查询按钮点击
void MainWindow::on_btnSearchProcess_clicked()
{
    QString input = ui->lineEditProcess->text().trimmed();
    
    if (input.isEmpty()) {
        QMessageBox::warning(this,
            QString::fromUtf8("警告"),
            QString::fromUtf8("请输入进程名称或PID！"));
        return;
    }
    
    searchProcessFiles(input);
}

// 查询进程打开的文件
void MainWindow::searchProcessFiles(const QString &processNameOrPid)
{
    handleModel->removeRows(0, handleModel->rowCount());
    ui->labelHandleStatus->setText(QString::fromUtf8("正在搜索进程..."));
    
#ifdef Q_OS_WIN
    bool isPid = false;
    DWORD targetPid = processNameOrPid.toULong(&isPid);
    
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        QMessageBox::critical(this,
            QString::fromUtf8("错误"),
            QString::fromUtf8("无法创建进程快照！"));
        return;
    }
    
    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);
    
    int foundCount = 0;
    
    if (Process32FirstW(snapshot, &pe32)) {
        do {
            bool match = false;
            
            // 判断是否匹配
            if (isPid) {
                match = (pe32.th32ProcessID == targetPid);
            } else {
                QString procName = QString::fromWCharArray(pe32.szExeFile);
                match = procName.contains(processNameOrPid, Qt::CaseInsensitive);
            }
            
            if (match) {
                DWORD processId = pe32.th32ProcessID;
                QString processName = QString::fromWCharArray(pe32.szExeFile);
                
                // 获取进程加载的所有模块
                QStringList modules = getProcessModules(processId);
                
                if (!modules.isEmpty()) {
                    // 为每个模块添加一行
                    for (const QString &modulePath : modules) {
                        QList<QStandardItem*> rowItems;
                        rowItems << new QStandardItem(processName);
                        rowItems << new QStandardItem(QString::number(processId));
                        rowItems << new QStandardItem(modulePath);
                        
                        handleModel->appendRow(rowItems);
                        foundCount++;
                    }
                } else {
                    // 如果无法获取模块，至少显示进程信息
                    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
                    if (hProcess) {
                        wchar_t processPath[MAX_PATH] = {0};
                        DWORD pathLen = GetModuleFileNameExW(hProcess, NULL, processPath, MAX_PATH);
                        
                        if (pathLen > 0) {
                            QList<QStandardItem*> rowItems;
                            rowItems << new QStandardItem(processName);
                            rowItems << new QStandardItem(QString::number(processId));
                            rowItems << new QStandardItem(QString::fromWCharArray(processPath));
                            
                            handleModel->appendRow(rowItems);
                            foundCount++;
                        }
                        
                        CloseHandle(hProcess);
                    }
                }
            }
        } while (Process32NextW(snapshot, &pe32));
    }
    
    CloseHandle(snapshot);
    
    ui->labelHandleStatus->setText(
        QString::fromUtf8("搜索完成，找到 %1 个文件/模块").arg(foundCount));
    
    if (foundCount == 0) {
        QMessageBox::information(this,
            QString::fromUtf8("提示"),
            QString::fromUtf8("未找到匹配的进程或无法访问进程信息。\n注意：需要管理员权限才能查看所有进程。"));
    }
    
#elif defined(Q_OS_LINUX)
    // Linux 通过 /proc 查找进程
    bool isPid = false;
    ProcessId targetPid = processNameOrPid.toULong(&isPid);
    int foundCount = 0;
    
    QDir procDir("/proc");
    QStringList entries = procDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    
    for (const QString &entry : entries) {
        bool ok;
        ProcessId pid = entry.toULong(&ok);
        if (!ok) continue;
        
        QString processName = getProcessName(pid);
        bool match = false;
        
        if (isPid) {
            match = (pid == targetPid);
        } else {
            match = processName.contains(processNameOrPid, Qt::CaseInsensitive);
        }
        
        if (match) {
            QStringList modules = getProcessModules(pid);
            
            if (!modules.isEmpty()) {
                for (const QString &modulePath : modules) {
                    QList<QStandardItem*> rowItems;
                    rowItems << new QStandardItem(processName);
                    rowItems << new QStandardItem(QString::number(pid));
                    rowItems << new QStandardItem(modulePath);
                    
                    handleModel->appendRow(rowItems);
                    foundCount++;
                }
            } else {
                // 至少显示进程主路径
                QString exePath = QString("/proc/%1/exe").arg(pid);
                QFileInfo fi(exePath);
                QString path = fi.symLinkTarget();
                
                if (!path.isEmpty()) {
                    QList<QStandardItem*> rowItems;
                    rowItems << new QStandardItem(processName);
                    rowItems << new QStandardItem(QString::number(pid));
                    rowItems << new QStandardItem(path);
                    
                    handleModel->appendRow(rowItems);
                    foundCount++;
                }
            }
        }
    }
    
    ui->labelHandleStatus->setText(
        QString::fromUtf8("搜索完成，找到 %1 个文件/模块").arg(foundCount));
    
    if (foundCount == 0) {
        QMessageBox::information(this,
            QString::fromUtf8("提示"),
            QString::fromUtf8("未找到匹配的进程或无法访问进程信息。\n注意：可能需要 root 权限才能查看所有进程。"));
    }
    
#else
    Q_UNUSED(processNameOrPid);
    ui->labelHandleStatus->setText(QString::fromUtf8("此平台暂不支持"));
    QMessageBox::information(this,
        QString::fromUtf8("提示"),
        QString::fromUtf8("此功能暂不支持当前平台！"));
#endif
}

// 浏览二进制文件
void MainWindow::on_btnBrowseBinary_clicked()
{
    QString filter;
#ifdef Q_OS_WIN
    filter = QString::fromUtf8("可执行文件和库 (*.exe *.dll);;所有文件 (*.*)");
#else
    filter = QString::fromUtf8("共享库和可执行文件 (*.so *.so.*);;所有文件 (*)");
#endif
    
    QString path = QFileDialog::getOpenFileName(this,
        QString::fromUtf8("选择二进制文件"),
        "",
        filter);
    
    if (!path.isEmpty()) {
        ui->lineEditBinaryPath->setText(path);
    }
}

// 查询依赖
void MainWindow::on_btnQueryDeps_clicked()
{
    QString path = ui->lineEditBinaryPath->text().trimmed();
    
    if (path.isEmpty()) {
        QMessageBox::warning(this,
            QString::fromUtf8("警告"),
            QString::fromUtf8("请选择二进制文件！"));
        return;
    }
    
    QFileInfo fileInfo(path);
    if (!fileInfo.exists()) {
        QMessageBox::warning(this,
            QString::fromUtf8("警告"),
            QString::fromUtf8("文件不存在！"));
        return;
    }
    
    queryDependencies(path);
}

// 查询符号
void MainWindow::on_btnQuerySymbols_clicked()
{
    QString path = ui->lineEditBinaryPath->text().trimmed();
    
    if (path.isEmpty()) {
        QMessageBox::warning(this,
            QString::fromUtf8("警告"),
            QString::fromUtf8("请选择二进制文件！"));
        return;
    }
    
    QFileInfo fileInfo(path);
    if (!fileInfo.exists()) {
        QMessageBox::warning(this,
            QString::fromUtf8("警告"),
            QString::fromUtf8("文件不存在！"));
        return;
    }
    
    querySymbols(path);
}

// 查询依赖库
void MainWindow::queryDependencies(const QString &filePath)
{
    ui->textEditDeps->clear();
    ui->textEditDeps->append(QString::fromUtf8("正在查询依赖库...\n"));
    
#ifdef Q_OS_LINUX
    struct DependencyInfo {
        QString name;
        QString path;
        QString note;
    };

    QList<DependencyInfo> dependencies;
    QStringList warnings;
    bool staticBinary = false;

    auto addDependency = [&](const QString &name, const QString &path, const QString &note = QString()) {
        DependencyInfo info;
        info.name = name;
        info.path = path;
        info.note = note;
        dependencies.append(info);
    };

    auto canonicalize = [](const QString &path) -> QString {
        if (path.isEmpty()) {
            return QString();
        }
        QFileInfo info(path);
        if (info.exists()) {
            return info.canonicalFilePath();
        }
        return path;
    };

    bool parsedByLdd = false;

    QProcess process;
    process.start("ldd", QStringList() << filePath);
    if (!process.waitForFinished(5000)) {
        process.kill();
        warnings << QString::fromUtf8("ldd 查询超时");
    }

    QString lddOutput = QString::fromLocal8Bit(process.readAllStandardOutput());
    QString lddError = QString::fromLocal8Bit(process.readAllStandardError());

    if (!lddOutput.isEmpty()) {
        const QStringList lines = lddOutput.split('\n');
        for (const QString &line : lines) {
            QString trimmed = line.trimmed();
            if (trimmed.isEmpty()) {
                continue;
            }

            if (trimmed.contains(QStringLiteral("not a dynamic executable")) ||
                trimmed.contains(QStringLiteral("statically linked"))) {
                staticBinary = true;
                continue;
            }

            if (trimmed.startsWith(QStringLiteral("ldd:"))) {
                warnings << trimmed;
                continue;
            }

            QString dependencyName;
            QString path;
            QString note;

            if (trimmed.contains(QStringLiteral("=>"))) {
                QString left = trimmed.section(QStringLiteral("=>"), 0, 0).trimmed();
                QString right = trimmed.section(QStringLiteral("=>"), 1).trimmed();

                int addrIndex = right.indexOf(QStringLiteral(" ("));
                if (addrIndex != -1) {
                    note = right.mid(addrIndex).trimmed();
                    right = right.left(addrIndex).trimmed();
                }

                dependencyName = left;

                if (right.compare(QStringLiteral("not found"), Qt::CaseInsensitive) == 0) {
                    note = note.isEmpty() ? QString::fromUtf8("⚠ 未找到该库")
                                          : note + QString::fromUtf8("，且未找到该库");
                } else if (!right.isEmpty()) {
                    path = canonicalize(right);
                }
            } else {
                dependencyName = trimmed;
                int addrIndex = trimmed.indexOf(QStringLiteral(" ("));
                if (addrIndex != -1) {
                    note = trimmed.mid(addrIndex).trimmed();
                    trimmed = trimmed.left(addrIndex).trimmed();
                }
                path = canonicalize(trimmed);
            }

            if (!dependencyName.isEmpty()) {
                addDependency(dependencyName, path, note);
                parsedByLdd = true;
            }
        }
    }

    if (!lddError.trimmed().isEmpty()) {
        warnings << lddError.trimmed();
    }

    if ((!parsedByLdd || dependencies.isEmpty()) && !staticBinary) {
        // Fallback to readelf for交叉架构或静态分析
        QProcess readelf;
        readelf.start("readelf", QStringList() << "-d" << filePath);
        if (!readelf.waitForFinished(5000)) {
            readelf.kill();
            warnings << QString::fromUtf8("readelf 查询超时");
        } else {
            QString readelfOutput = QString::fromLocal8Bit(readelf.readAllStandardOutput());
            QString readelfError = QString::fromLocal8Bit(readelf.readAllStandardError());

            if (readelfOutput.contains(QStringLiteral("There is no dynamic section"))) {
                staticBinary = true;
            }

            QRegularExpression regex(QStringLiteral("\\(NEEDED\\)\\s+Shared library:\\s*\\[(.+)\\]"));
            const QStringList lines = readelfOutput.split('\n', QString::SkipEmptyParts);
            bool found = false;
            for (const QString &line : lines) {
                QRegularExpressionMatch match = regex.match(line);
                if (match.hasMatch()) {
                    QString libName = match.captured(1).trimmed();
                    QString resolved = resolveLibraryPath(libName, {QFileInfo(filePath).absolutePath()});
                    QString note;
                    if (resolved.isEmpty()) {
                        note = QString::fromUtf8("⚠ 未在标准库路径中找到该库");
                        resolved = libName;
                    }
                    addDependency(libName, resolved, note);
                    found = true;
                }
            }

            if (!found && dependencies.isEmpty()) {
                if (!readelfError.trimmed().isEmpty()) {
                    warnings << readelfError.trimmed();
                } else if (!readelfOutput.trimmed().isEmpty()) {
                    warnings << QString::fromUtf8("readelf 输出: %1").arg(readelfOutput.trimmed());
                }
            }
        }
    }

    ui->textEditDeps->clear();
    ui->textEditDeps->append(QString::fromUtf8("╔═══════════════════════════════════════════════╗"));
    ui->textEditDeps->append(QString::fromUtf8("║              依赖库列表                        ║"));
    ui->textEditDeps->append(QString::fromUtf8("╚═══════════════════════════════════════════════╝\n"));

    if (!dependencies.isEmpty()) {
        int index = 1;
        for (const DependencyInfo &info : dependencies) {
            ui->textEditDeps->append(QString::fromUtf8("%1. %2")
                .arg(index)
                .arg(info.name));
            if (!info.path.isEmpty()) {
                ui->textEditDeps->append(QString::fromUtf8("    ➜ %1")
                    .arg(info.path));
            }
            if (!info.note.isEmpty()) {
                ui->textEditDeps->append(QStringLiteral("    %1").arg(info.note));
            }
            ui->textEditDeps->append(QString());
            ++index;
        }

        ui->textEditDeps->append(QString::fromUtf8("════════════════════════════════════════════════"));
        ui->textEditDeps->append(QString::fromUtf8("✅ 共找到 %1 个依赖项")
            .arg(dependencies.size()));
    } else {
        if (staticBinary) {
            ui->textEditDeps->append(QString::fromUtf8("⚠ 该文件似乎为静态链接或不包含动态依赖。"));
        } else {
            ui->textEditDeps->append(QString::fromUtf8("❌ 未能解析出任何依赖项。"));
        }
    }

    if (!warnings.isEmpty()) {
        ui->textEditDeps->append(QString());
        ui->textEditDeps->append(QString::fromUtf8("⚠ 提示/警告:"));
        for (const QString &warning : warnings) {
            ui->textEditDeps->append(QStringLiteral("- %1").arg(warning));
        }
    }

    return;

#elif defined(Q_OS_WIN)
    // Windows上尝试使用 dumpbin（需要Visual Studio）
    QProcess process;
    process.setProgram("dumpbin");
    process.setArguments(QStringList() << "/dependents" << filePath);
    process.start();
    
    if (!process.waitForFinished(5000)) {
        process.kill();
        // 如果dumpbin不可用，提示用户
        ui->textEditDeps->clear();
        ui->textEditDeps->append(QString::fromUtf8("╔═══════════════════════════════════════════════╗"));
        ui->textEditDeps->append(QString::fromUtf8("║              依赖库列表                        ║"));
        ui->textEditDeps->append(QString::fromUtf8("╚═══════════════════════════════════════════════╝\n"));
        ui->textEditDeps->append(QString::fromUtf8("⚠ Windows平台需要安装 Visual Studio 的 dumpbin 工具"));
        ui->textEditDeps->append(QString::fromUtf8("\n💡 提示："));
        ui->textEditDeps->append(QString::fromUtf8("1. 安装 Visual Studio（包含 C++ 开发工具）"));
        ui->textEditDeps->append(QString::fromUtf8("2. 使用 Developer Command Prompt 运行本程序"));
        ui->textEditDeps->append(QString::fromUtf8("3. 或使用第三方工具如 Dependency Walker"));
        return;
    }
    
    QString output = QString::fromLocal8Bit(process.readAllStandardOutput());
    
    if (!output.isEmpty()) {
        ui->textEditDeps->clear();
        ui->textEditDeps->append(QString::fromUtf8("╔═══════════════════════════════════════════════╗"));
        ui->textEditDeps->append(QString::fromUtf8("║              依赖库列表                        ║"));
        ui->textEditDeps->append(QString::fromUtf8("╚═══════════════════════════════════════════════╝\n"));
        ui->textEditDeps->append(output);
    } else {
        ui->textEditDeps->append(QString::fromUtf8("\n❌ 无法获取依赖信息，请确保 dumpbin 工具可用"));
    }
    
#else
    ui->textEditDeps->clear();
    ui->textEditDeps->append(QString::fromUtf8("⚠ 此平台暂不支持依赖查询功能"));
#endif
}

// 查询符号表和导出函数
void MainWindow::querySymbols(const QString &filePath)
{
    ui->textEditSymbols->clear();
    ui->textEditSymbols->append(QString::fromUtf8("正在查询符号表...\n"));
    
#ifdef Q_OS_LINUX
    // 先尝试查询动态符号（导出的函数）
    QProcess process;
    process.start("nm", QStringList() << "-D" << "--defined-only" << filePath);
    
    if (!process.waitForFinished(10000)) {
        process.kill();
        ui->textEditSymbols->append(QString::fromUtf8("\n❌ 查询超时"));
        return;
    }
    
    QString output = QString::fromLocal8Bit(process.readAllStandardOutput());
    QString errorText = QString::fromLocal8Bit(process.readAllStandardError());
    
    if (!output.isEmpty()) {
        ui->textEditSymbols->clear();
        ui->textEditSymbols->append(QString::fromUtf8("╔═══════════════════════════════════════════════╗"));
        ui->textEditSymbols->append(QString::fromUtf8("║           动态符号表（导出函数）               ║"));
        ui->textEditSymbols->append(QString::fromUtf8("╚═══════════════════════════════════════════════╝\n"));
        
        QStringList lines = output.split('\n', QString::SkipEmptyParts);
        int funcCount = 0;
        int dataCount = 0;
        
        for (const QString &line : lines) {
            QStringList parts = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
            if (parts.size() >= 3) {
                QString type = parts[1];
                QString symbol = parts[2];
                
                // T/t = 代码段符号（函数）, D/d = 数据段符号
                if (type == "T" || type == "t") {
                    ui->textEditSymbols->append(QString::fromUtf8("🔵 [函数] %1").arg(symbol));
                    funcCount++;
                } else if (type == "D" || type == "d" || type == "B" || type == "b") {
                    ui->textEditSymbols->append(QString::fromUtf8("🟢 [数据] %1").arg(symbol));
                    dataCount++;
                } else {
                    ui->textEditSymbols->append(QString::fromUtf8("⚪ [其他] %1 (%2)").arg(symbol).arg(type));
                }
            }
        }
        
        ui->textEditSymbols->append(QString::fromUtf8("\n════════════════════════════════════════════════"));
        ui->textEditSymbols->append(QString::fromUtf8("✅ 统计: %1 个函数, %2 个数据符号")
            .arg(funcCount).arg(dataCount));
        
        if (!errorText.isEmpty() && !errorText.contains("no symbols")) {
            ui->textEditSymbols->append(QString::fromUtf8("\n⚠ 警告: %1").arg(errorText));
        }
    } else {
        // 如果没有动态符号，尝试查看所有符号
        process.start("nm", QStringList() << filePath);
        process.waitForFinished(10000);
        
        output = QString::fromLocal8Bit(process.readAllStandardOutput());
        
        if (!output.isEmpty()) {
            ui->textEditSymbols->clear();
            ui->textEditSymbols->append(QString::fromUtf8("╔═══════════════════════════════════════════════╗"));
            ui->textEditSymbols->append(QString::fromUtf8("║              所有符号表                        ║"));
            ui->textEditSymbols->append(QString::fromUtf8("╚═══════════════════════════════════════════════╝\n"));
            ui->textEditSymbols->append(QString::fromUtf8("⚠ 注意：这是静态链接的可执行文件，显示所有符号\n"));
            
            QStringList lines = output.split('\n', QString::SkipEmptyParts);
            int count = 0;
            for (const QString &line : lines) {
                if (count < 1000) {  // 限制显示数量
                    ui->textEditSymbols->append(line);
                    count++;
                }
            }
            
            if (lines.size() > 1000) {
                ui->textEditSymbols->append(QString::fromUtf8("\n... (共 %1 个符号，仅显示前 1000 个)")
                    .arg(lines.size()));
            }
        } else {
            ui->textEditSymbols->append(QString::fromUtf8("\n❌ 无符号信息"));
            ui->textEditSymbols->append(QString::fromUtf8("提示：文件可能已被 strip 或不是有效的二进制文件"));
            if (!errorText.isEmpty()) {
                ui->textEditSymbols->append(QString::fromUtf8("\n错误: %1").arg(errorText));
            }
        }
    }
    
#elif defined(Q_OS_WIN)
    // Windows上使用 dumpbin /exports
    QProcess process;
    process.setProgram("dumpbin");
    process.setArguments(QStringList() << "/exports" << filePath);
    process.start();
    
    if (!process.waitForFinished(10000)) {
        process.kill();
        ui->textEditSymbols->clear();
        ui->textEditSymbols->append(QString::fromUtf8("╔═══════════════════════════════════════════════╗"));
        ui->textEditSymbols->append(QString::fromUtf8("║           导出函数列表                         ║"));
        ui->textEditSymbols->append(QString::fromUtf8("╚═══════════════════════════════════════════════╝\n"));
        ui->textEditSymbols->append(QString::fromUtf8("⚠ Windows平台需要安装 Visual Studio 的 dumpbin 工具"));
        ui->textEditSymbols->append(QString::fromUtf8("\n💡 提示："));
        ui->textEditSymbols->append(QString::fromUtf8("1. 安装 Visual Studio（包含 C++ 开发工具）"));
        ui->textEditSymbols->append(QString::fromUtf8("2. 使用 Developer Command Prompt 运行本程序"));
        ui->textEditSymbols->append(QString::fromUtf8("3. 或使用第三方工具如 CFF Explorer"));
        return;
    }
    
    QString output = QString::fromLocal8Bit(process.readAllStandardOutput());
    
    if (!output.isEmpty()) {
        ui->textEditSymbols->clear();
        ui->textEditSymbols->append(QString::fromUtf8("╔═══════════════════════════════════════════════╗"));
        ui->textEditSymbols->append(QString::fromUtf8("║           导出函数列表                         ║"));
        ui->textEditSymbols->append(QString::fromUtf8("╚═══════════════════════════════════════════════╝\n"));
        ui->textEditSymbols->append(output);
    } else {
        ui->textEditSymbols->append(QString::fromUtf8("\n❌ 无法获取导出函数信息"));
        ui->textEditSymbols->append(QString::fromUtf8("提示：该文件可能没有导出任何函数，或 dumpbin 工具不可用"));
    }
    
#else
    ui->textEditSymbols->clear();
    ui->textEditSymbols->append(QString::fromUtf8("⚠ 此平台暂不支持符号查询功能"));
#endif
}

