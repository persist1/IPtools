# 跨平台编译指南

## 目录
1. [支持的平台](#支持的平台)
2. [Windows 编译](#windows-编译)
3. [Linux x86_64 编译](#linux-x86_64-编译)
4. [Linux ARM64 编译](#linux-arm64-编译)
5. [交叉编译](#交叉编译)
6. [平台特定功能](#平台特定功能)

---

## 支持的平台

| 平台 | 架构 | 状态 | 说明 |
|------|------|------|------|
| Windows | x86/x64 | ✅ 完全支持 | Vista 及更高版本 |
| Linux | x86_64 | ✅ 完全支持 | 所有主流发行版 |
| Linux | ARM64 | ✅ 完全支持 | 树莓派4、ARM服务器等 |
| Linux | ARMv7 | ✅ 完全支持 | 树莓派3、嵌入式设备 |
| macOS | x86_64/ARM64 | ⚠️ 部分支持 | IP查询功能正常 |

---

## Windows 编译

### 环境准备
1. 安装 Qt 5.14.2+ （MinGW 或 MSVC 版本）
2. 安装 Qt Creator

### 编译步骤
```bash
# 打开 Qt Creator，加载 IPtools.pro
# 或使用命令行：
cd IPtools
qmake IPtools.pro
mingw32-make  # 或 nmake（MSVC）
```

### 运行
```bash
# 以管理员身份运行
debug\IPtools.exe
```

---

## Linux x86_64 编译

### 环境准备

**Ubuntu / Debian:**
```bash
sudo apt update
sudo apt install -y qt5-default qtbase5-dev qttools5-dev-tools
sudo apt install -y build-essential
# 可选：安装 lsof 用于文件句柄查询
sudo apt install -y lsof
```

**CentOS / RHEL / Fedora:**
```bash
sudo yum install -y qt5-qtbase-devel qt5-qttools-devel
sudo yum install -y gcc-c++ make
# 可选：安装 lsof
sudo yum install -y lsof
```

**Arch Linux:**
```bash
sudo pacman -S qt5-base qt5-tools
sudo pacman -S base-devel
sudo pacman -S lsof
```

### 编译步骤
```bash
cd IPtools
qmake IPtools.pro
make -j$(nproc)
```

### 运行
```bash
# 普通运行
./IPtools

# 以 root 权限运行（查看所有进程）
sudo ./IPtools
```

---

## Linux ARM64 编译

### 方法1：本地编译（在 ARM64 设备上）

**树莓派 4 / ARM64 Linux:**
```bash
# 安装依赖
sudo apt update
sudo apt install -y qt5-default qtbase5-dev qttools5-dev-tools
sudo apt install -y build-essential lsof

# 编译
cd IPtools
qmake IPtools.pro
make -j4  # 树莓派4有4个核心

# 运行
sudo ./IPtools
```

### 方法2：交叉编译（从 x86_64 编译到 ARM64）

#### 准备交叉编译工具链
```bash
# Ubuntu / Debian
sudo apt install -y gcc-aarch64-linux-gnu g++-aarch64-linux-gnu

# 下载或编译 ARM64 版本的 Qt
# 参考：https://doc.qt.io/qt-5/configure-linux-device.html
```

#### 配置 Qt Creator
1. 打开 Qt Creator
2. 工具 → 选项 → 构建套件
3. 添加新的 ARM64 工具链：
   - 编译器：aarch64-linux-gnu-g++
   - Qt 版本：ARM64 版 Qt
   - CMake 工具：arm64 cmake

#### 编译
```bash
# 设置交叉编译环境
export PATH=/path/to/arm64-qt/bin:$PATH
export CROSS_COMPILE=aarch64-linux-gnu-

# 编译
qmake IPtools.pro
make
```

---

## 交叉编译

### 使用 Docker 交叉编译

**创建 Dockerfile:**
```dockerfile
FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

# 安装交叉编译工具链
RUN apt-get update && apt-get install -y \
    gcc-aarch64-linux-gnu \
    g++-aarch64-linux-gnu \
    qt5-default \
    qtbase5-dev \
    make

WORKDIR /build
```

**编译命令:**
```bash
# 构建 Docker 镜像
docker build -t iptools-builder .

# 运行编译
docker run -v $(pwd):/build iptools-builder sh -c "qmake && make"
```

---

## 平台特定功能

### Windows 平台

**文件句柄查询:**
- 使用 Windows API: `CreateToolhelp32Snapshot`, `Process32First`, `Process32Next`
- 需要管理员权限

**进程管理:**
- 使用 `EnumProcessModules` 获取进程模块
- 使用 `TerminateProcess` 结束进程
- 使用 `GetModuleFileNameExW` 获取进程路径

### Linux 平台

**文件句柄查询:**
- 方式1：使用 `lsof` 命令（推荐）
- 方式2：读取 `/proc/[pid]/fd/` 目录
- 需要 root 权限访问所有进程

**进程管理:**
- 读取 `/proc/[pid]/comm` 获取进程名
- 读取 `/proc/[pid]/maps` 获取加载模块
- 使用 `kill(pid, SIGKILL)` 结束进程
- 读取 `/proc/[pid]/exe` 获取进程路径

**进程查询:**
```bash
# 查看进程打开的文件
lsof -p <PID>

# 查看文件被哪些进程占用
lsof /path/to/file
```

### 代码结构

```cpp
// 跨平台类型定义
#ifdef Q_OS_WIN
typedef DWORD ProcessId;
#else
typedef unsigned long ProcessId;
#endif

// 平台特定实现
#ifdef Q_OS_WIN
    // Windows 实现
#elif defined(Q_OS_LINUX)
    // Linux 实现
#else
    // 其他平台实现
#endif
```

---

## 性能对比

| 操作 | Windows x64 | Linux x64 | Linux ARM64 |
|------|-------------|-----------|-------------|
| 进程枚举 | 快速 | 快速 | 中速 |
| 模块枚举 | 快速 | 中速 | 慢速 |
| 文件句柄查询 | 中速 | 快速(lsof) | 中速(lsof) |
| IP查询 | 快速 | 快速 | 快速 |

---

## 常见问题

### Q: ARM64 编译时找不到 Qt？
**A:** 确保安装了 ARM64 版本的 Qt：
```bash
sudo apt install qt5-default:arm64
```

### Q: 交叉编译的程序无法运行？
**A:** 检查：
1. 目标设备的 Qt 库版本是否匹配
2. 是否缺少依赖库：`ldd ./IPtools`
3. 是否有执行权限：`chmod +x ./IPtools`

### Q: Linux 下查询进程提示权限不足？
**A:** 使用 sudo 运行：
```bash
sudo ./IPtools
```

### Q: ARM 设备性能慢？
**A:** 
1. 使用 Release 模式编译：`qmake CONFIG+=release`
2. 限制同时查询的进程数
3. 升级到更新的 Qt 版本

---

## 打包发布

### Windows
```bash
# 使用 windeployqt 打包
windeployqt release\IPtools.exe
```

### Linux
```bash
# 使用 linuxdeployqt 或创建 AppImage
linuxdeployqt IPtools -bundle-non-qt-libs
```

### 静态编译
```bash
# 配置静态 Qt
./configure -static -prefix /opt/qt-static
make && make install

# 静态编译程序
/opt/qt-static/bin/qmake CONFIG+=static
make
```

---

## 技术支持

遇到编译问题？
1. 检查 Qt 版本（推荐 5.14.2+）
2. 查看 `BUILD.md` 详细说明
3. 提交 Issue 并附上：
   - 操作系统和架构
   - Qt 版本
   - 完整编译日志

---

## 参考链接

- [Qt 跨平台编译文档](https://doc.qt.io/qt-5/supported-platforms.html)
- [Qt for Embedded Linux](https://doc.qt.io/qt-5/embedded-linux.html)
- [ARM 交叉编译指南](https://doc.qt.io/qt-5/configure-linux-device.html)

