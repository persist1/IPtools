# 编译说明

## 修复的问题

### 1. QueryFullProcessImageNameW 错误
- **问题**: MinGW 32位版本可能不支持 `QueryFullProcessImageNameW` 函数
- **解决方案**: 改用 `GetModuleFileNameExW` 函数（兼容性更好）

### 2. Windows API 版本
- **添加**: `_WIN32_WINNT=0x0600` 宏定义（Windows Vista 或更高版本）
- **位置**: 在 `IPtools.pro` 和 `mainwindow.cpp` 中都已定义

### 3. 未使用参数警告
- **修复**: 添加 `Q_UNUSED()` 宏来消除警告

## 编译步骤

### 方法1: Qt Creator（推荐）

1. **打开项目**
   - 启动 Qt Creator
   - 文件 → 打开文件或项目
   - 选择 `IPtools.pro`

2. **清理旧的构建**
   - 点击菜单：构建 → 清除全部
   - 或者手动删除 `build-IPtools-*` 文件夹

3. **重新运行 qmake**
   - 右键点击项目 → 运行 qmake
   - 等待 qmake 完成

4. **构建项目**
   - 点击左下角 🔨 **构建** 按钮
   - 或者按 `Ctrl+B`

5. **运行程序**
   - 点击左下角 ▶️ **运行** 按钮
   - 或者按 `Ctrl+R`

### 方法2: 命令行

```bash
# 进入项目目录
cd D:\Software\QTWork\WorkProject\IPtools

# 清理
del Makefile*
rmdir /s /q debug
rmdir /s /q release

# 生成 Makefile
qmake IPtools.pro

# 编译
mingw32-make

# 运行
debug\IPtools.exe
```

## 常见问题

### Q: 编译时提示找不到 psapi.lib
**A**: 确保 `.pro` 文件中包含：
```
win32: LIBS += -lpsapi -ladvapi32
```

### Q: 运行时提示缺少 DLL
**A**: 将 Qt 的 bin 目录添加到系统 PATH，或者使用 `windeployqt` 工具：
```bash
windeployqt debug\IPtools.exe
```

### Q: 无法查询某些进程
**A**: 以管理员身份运行程序：
- 右键点击 `IPtools.exe`
- 选择"以管理员身份运行"

## 系统要求

- **操作系统**: Windows Vista 或更高版本
- **Qt 版本**: 5.x 或更高
- **编译器**: MinGW 或 MSVC
- **网络**: 需要互联网连接（用于 IP 查询）

## 功能说明

### 文件句柄查询
- ✅ 输入文件或文件夹路径，查询占用该文件的进程
- ✅ 输入进程名称或PID，查询该进程打开的所有文件
- ✅ 显示进程名称、PID、完整路径
- ✅ 右键菜单支持结束进程
- ✅ 安全确认机制

### IP 地址查询
- ✅ 自动获取本机公网 IP 和局域网 IP
- ✅ 显示网卡信息、MAC地址、子网掩码
- ✅ 查询任意 IP 地址的归属地
- ✅ 显示国家、省份、城市、运营商等详细信息
- ✅ 美化的界面展示

## 技术细节

### 使用的 Windows API
- `CreateToolhelp32Snapshot` - 创建进程快照
- `Process32FirstW/Process32NextW` - 遍历进程
- `OpenProcess` - 打开进程句柄
- `GetModuleFileNameExW` - 获取进程路径
- `EnumProcessModules` - 枚举进程模块
- `TerminateProcess` - 结束进程
- `GetModuleBaseNameW` - 获取模块名称

### 使用的 Qt 模块
- `QtCore` - 核心功能
- `QtGui` - GUI 基础
- `QtWidgets` - 界面控件
- `QtNetwork` - 网络请求和局域网信息

### IP 查询 API
- 使用 `ip-api.com` 提供的免费 API
- 支持中文显示
- JSON 格式返回数据

## 许可证

本项目仅供学习和个人使用。

