#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QStandardItemModel>
#include <QMenu>

class QEvent;

#ifdef Q_OS_WIN
#include <windows.h>
typedef DWORD ProcessId;
#else
typedef unsigned long ProcessId;
#endif

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    // 句柄查询相关
    void on_btnSearchHandle_clicked();
    void on_btnBrowse_clicked();
    void on_btnRefreshHandle_clicked();
    void on_btnSearchProcess_clicked();
    
    // 右键菜单
    void showContextMenu(const QPoint &pos);
    void showTextEditContextMenu(const QPoint &pos);
    void killSelectedProcess();
    void copySelectedColumn(int column);
    void copySelectedRow();
    
    // IP查询相关
    void on_btnGetMyIP_clicked();
    void on_btnQueryIP_clicked();
    void onMyIPReplyFinished(QNetworkReply *reply);
    void onIPQueryReplyFinished(QNetworkReply *reply);
    void onIPLocationReplyFinished(QNetworkReply *reply);
    
    // 依赖分析相关
    void on_btnBrowseBinary_clicked();
    void on_btnQueryDeps_clicked();
    void on_btnQuerySymbols_clicked();

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager *networkManager;
    QNetworkAccessManager *ipQueryManager;
    QNetworkAccessManager *locationManager;
    QStandardItemModel *handleModel;
    
    // 辅助函数
    QString getLocalIPAddresses();
    void searchFileHandles(const QString &path);
    void searchProcessFiles(const QString &processNameOrPid);
    QString getProcessName(ProcessId processId);
    bool isFileInUse(const QString &filePath, ProcessId processId);
    bool killProcess(ProcessId processId);
    QStringList getProcessModules(ProcessId processId);
    
    // 依赖分析辅助函数
    void queryDependencies(const QString &filePath);
    void querySymbols(const QString &filePath);
};
#endif // MAINWINDOW_H

