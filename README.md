# IP工具箱

> 🚀 **首次使用？** 请先阅读下方“编译步骤”章节，或参考 [USAGE_GUIDE.md](USAGE_GUIDE.md) 获取更详细的操作说明。

一个基于Qt开发的跨平台系统工具，提供文件句柄查询、进程管理和IP地址查询功能。

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)](https://github.com/YOUR_USERNAME/IPtools/actions)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Qt](https://img.shields.io/badge/Qt-5.15.2-green.svg)](https://www.qt.io/)
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey)]()

> 💡 **提示**: 本项目已配置完整的 GitHub Actions 工作流，支持自动构建 Windows/Linux/macOS 多平台程序！

## 支持平台
- ✅ Windows (Vista 及更高版本)
- ✅ Linux (x86_64 / ARM64 / 其他架构)
- ⚠️ macOS (部分功能)


## 功能特性

### 1. 文件句柄查询
- ✅ 输入文件或文件夹路径查询占用进程
- ✅ 显示占用该文件的进程列表
- ✅ 显示进程名称、PID和进程完整路径
- ✅ 支持浏览选择文件
- ✅ 实时刷新功能
- ✅ **右键菜单支持结束进程**
- ✅ **可通过进程名或PID反向查询进程打开的所有文件**

### 2. 进程管理功能
- ✅ 输入进程名称或PID查询进程信息
- ✅ 显示进程加载的所有模块（DLL、EXE等）
- ✅ 右键菜单快速结束进程
- ✅ 安全确认机制防止误操作
- ✅ 需要管理员权限以访问系统进程

### 3. IP地址查询
- ✅ 自动获取本机公网IP地址
- ✅ **显示所有局域网IP地址（包括网卡信息、MAC地址）**
- ✅ 显示IP归属地信息（国家、省份、城市）
- ✅ 显示运营商和时区信息
- ✅ 支持查询任意IP地址
- ✅ 显示详细的地理位置、经纬度和组织信息
- ✅ **美化的信息展示界面**

## 编译环境

### 通用要求
- Qt 5.x 或更高版本（推荐 5.14.2+）
- Qt Creator 4.11.1 或更高版本
- C++11 支持

### Windows 平台
- MinGW 或 MSVC 编译器
- Windows Vista 或更高版本

### Linux 平台
- GCC 或 Clang 编译器
- 可选：`lsof` 工具（用于文件句柄查询）
- 支持架构：x86_64, ARM64, ARMv7 等

## 编译步骤

### 使用 Qt Creator
1. 打开 Qt Creator
2. 打开 `IPtools.pro` 文件
3. 选择合适的构建套件（Kit）
4. 点击"构建"按钮进行编译
5. 运行程序

### 命令行编译

**Windows:**
```bash
qmake IPtools.pro
mingw32-make
```

**Linux:**
```bash
qmake IPtools.pro
make
# 如需安装 lsof（用于文件句柄查询）
sudo apt install lsof  # Debian/Ubuntu
sudo yum install lsof  # CentOS/RHEL
```

### 交叉编译到 Linux ARM64
```bash
# 使用 ARM64 工具链
qmake-arm64 IPtools.pro
make
```

## 使用说明

### 文件句柄查询
1. 切换到"文件句柄查询"标签页
2. **方式1 - 通过文件查进程：**
   - 输入文件路径或点击"浏览"选择文件
   - 点击"查询"按钮
   - 查看占用进程列表
3. **方式2 - 通过进程查文件：**
   - 在"进程名/PID"输入框输入进程名（如：chrome.exe）或PID（如：1234）
   - 点击"查询进程"按钮
   - 查看该进程打开的所有文件和加载的模块
4. **结束进程：**
   - 在结果列表中右键点击进程
   - 选择"结束进程"
   - 确认操作

⚠️ **注意：某些系统进程需要管理员权限才能查询和操作。**

### IP地址查询
1. 切换到"IP地址查询"标签页
2. **查看本机IP：**
   - 程序启动时自动获取
   - 或点击"获取本机IP"按钮刷新
   - 显示局域网IP、公网IP及详细信息
3. **查询其他IP：**
   - 在查询框输入任意IP地址（如：8.8.8.8）
   - 点击"查询"按钮
   - 查看该IP的详细地理位置信息

## 依赖项

### Qt 模块
- Qt Core
- Qt Gui
- Qt Widgets
- Qt Network（网络请求和局域网信息）

### Windows 平台
- Windows API（psapi.dll, advapi32.dll）
- 需要管理员权限以访问所有进程

### Linux 平台
- `/proc` 文件系统（内核支持）
- `lsof` 工具（可选，用于文件句柄查询）
- root 权限（查看所有进程时需要）

## 注意事项

### Windows
- 建议以**管理员身份运行**以获取完整的进程信息
- 某些系统进程需要提升权限才能操作

### Linux
- 建议使用 **sudo** 运行以获取完整的进程信息
- 文件句柄查询需要安装 `lsof` 工具
- ARM64 平台完全支持

### 通用
- IP查询功能需要互联网连接
- 防火墙可能阻止网络请求

## GitHub 自动构建

本项目配置了 GitHub Actions 工作流，支持自动构建多平台程序。

### 支持的平台
- ✅ Windows (x86/x64)
- ✅ Linux (x86_64/ARM64)
- ✅ macOS (Intel/Apple Silicon)

### 使用方法

#### 🚀 自动构建（所有平台）
1. 推送代码到 `main` 或 `develop` 分支
2. 在 Actions 标签页查看构建结果
3. 构建完成后下载对应平台的安装包

#### 🎯 手动选择平台构建
1. 进入仓库的 **Actions** 标签页
2. 选择 "Build Multi-Platform" 工作流
3. 点击 **Run workflow** 按钮
4. 选择要构建的平台：
   - `all` - 构建所有平台
   - `windows-x64` - 仅 Windows 64位
   - `windows-x86` - 仅 Windows 32位
   - `linux-x86_64` - 仅 Linux x86_64
   - `linux-arm64` - 仅 Linux ARM64
   - `macos-x86_64` - 仅 macOS Intel
   - `macos-arm64` - 仅 macOS Apple Silicon
5. 点击绿色的 **Run workflow** 开始构建

如需更多工作流示例，可参考仓库的 GitHub Actions 配置文件。

### 快速发布
```bash
# 创建版本标签
git tag -a v1.0.0 -m "Release v1.0.0"
git push origin v1.0.0

# GitHub Actions 会自动：
# - 构建所有平台的程序
# - 创建 GitHub Release
# - 上传构建产物
```

## 本地构建脚本

项目提供了便捷的构建脚本：

**Windows:**
```batch
scripts\build-windows.bat release
```

**Linux:**
```bash
chmod +x scripts/build-linux.sh
./scripts/build-linux.sh release
```

**macOS:**
```bash
chmod +x scripts/build-macos.sh
./scripts/build-macos.sh release
```

## 贡献指南

欢迎贡献代码！请查看 [CONTRIBUTING.md](CONTRIBUTING.md) 了解详情。

## 相关文档
- [USAGE_GUIDE.md](USAGE_GUIDE.md) - 使用说明
- [CONTRIBUTING.md](CONTRIBUTING.md) - 贡献指南
- [CHANGELOG.md](CHANGELOG.md) - 更新日志

## 许可证

本项目仅供学习和个人使用。


<!-- Build triggered: 2025-11-03 01:44:21 -->

<!-- Build triggered: 2025-11-03 02:09:21 -->

<!-- Build triggered: 2025-11-04 03:10:22 -->

<!-- Build triggered: 2025-11-05 02:16:13 -->
