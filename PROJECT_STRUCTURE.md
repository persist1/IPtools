# 项目结构说明

本文档说明 IPtools 项目的目录结构和文件组织。

## 目录树

```
IPtools/
├── .github/                      # GitHub 配置
│   ├── workflows/               # GitHub Actions 工作流
│   │   ├── build.yml           # 自动构建工作流
│   │   ├── release.yml         # 发布工作流
│   │   ├── docker-cross-build.yml  # Docker 跨平台构建
│   │   ├── code-quality.yml    # 代码质量检查
│   │   └── stale.yml           # 自动关闭过期 Issue/PR
│   ├── ISSUE_TEMPLATE/         # Issue 模板
│   │   ├── bug_report.md       # Bug 报告模板
│   │   └── feature_request.md  # 功能请求模板
│   ├── pull_request_template.md  # PR 模板
│   ├── dependabot.yml          # Dependabot 配置
│   └── FUNDING.yml             # 赞助配置
│
├── scripts/                     # 构建和发布脚本
│   ├── build-windows.bat       # Windows 构建脚本
│   ├── build-linux.sh          # Linux 构建脚本
│   ├── build-macos.sh          # macOS 构建脚本
│   ├── create-release.sh       # 创建发布包脚本
│   └── test-docker-build.sh    # Docker 构建测试脚本
│
├── main.cpp                     # 程序入口
├── mainwindow.cpp              # 主窗口实现
├── mainwindow.h                # 主窗口头文件
├── mainwindow.ui               # Qt UI 设计文件
├── IPtools.pro                 # Qt 项目文件
├── IPtools.pro.user            # Qt Creator 用户设置（不提交）
│
├── Dockerfile                   # Docker 容器配置
├── docker-compose.yml          # Docker Compose 配置
│
├── .gitignore                  # Git 忽略文件
├── .editorconfig               # 编辑器配置
├── .clang-format               # 代码格式化配置
│
├── README.md                   # 项目说明
├── BUILD.md                    # 编译说明
├── CROSS_PLATFORM.md           # 跨平台编译指南
├── USAGE_GUIDE.md             # 使用指南
├── CHANGELOG.md               # 更新日志
├── CONTRIBUTING.md            # 贡献指南
├── WORKFLOW_GUIDE.md          # GitHub 工作流指南
├── SETUP_GITHUB.md            # GitHub 设置指南
├── QUICK_START.md             # 快速开始指南
├── PROJECT_STRUCTURE.md       # 项目结构说明（本文件）
├── LICENSE                    # 开源许可证
└── VERSION                    # 版本号文件
```

## 核心文件说明

### 源代码文件

| 文件 | 说明 |
|------|------|
| `main.cpp` | 程序入口点，初始化 Qt 应用 |
| `mainwindow.cpp/h` | 主窗口类，包含所有功能实现 |
| `mainwindow.ui` | Qt Designer 设计的界面布局 |
| `IPtools.pro` | Qt 项目配置文件 |

### 工作流文件

| 文件 | 触发条件 | 用途 |
|------|----------|------|
| `build.yml` | Push/PR | 自动构建所有平台 |
| `release.yml` | Git Tag | 创建正式发布 |
| `docker-cross-build.yml` | 手动 | Docker 多架构构建 |
| `code-quality.yml` | Push/PR | 静态代码分析 |
| `stale.yml` | 定时（每天） | 自动关闭过期 Issue/PR |

### 构建脚本

| 脚本 | 平台 | 功能 |
|------|------|------|
| `build-windows.bat` | Windows | 构建 Windows 版本 |
| `build-linux.sh` | Linux | 构建 Linux 版本 |
| `build-macos.sh` | macOS | 构建 macOS 版本 |
| `create-release.sh` | All | 创建发布包 |
| `test-docker-build.sh` | All | 测试 Docker 构建 |

### 文档文件

| 文档 | 内容 |
|------|------|
| `README.md` | 项目概述和快速入门 |
| `BUILD.md` | 详细编译步骤 |
| `CROSS_PLATFORM.md` | 跨平台编译详解 |
| `USAGE_GUIDE.md` | 功能使用说明 |
| `WORKFLOW_GUIDE.md` | GitHub Actions 详解 |
| `SETUP_GITHUB.md` | GitHub 仓库设置 |
| `QUICK_START.md` | 5 分钟快速开始 |
| `CONTRIBUTING.md` | 贡献代码指南 |
| `CHANGELOG.md` | 版本更新记录 |
| `PROJECT_STRUCTURE.md` | 项目结构说明 |

### 配置文件

| 文件 | 用途 |
|------|------|
| `.gitignore` | 指定 Git 忽略的文件 |
| `.editorconfig` | 统一不同编辑器的代码风格 |
| `.clang-format` | C++ 代码格式化规则 |
| `Dockerfile` | Docker 容器构建配置 |
| `docker-compose.yml` | Docker Compose 服务配置 |
| `LICENSE` | 开源许可证（MIT） |
| `VERSION` | 当前版本号 |

## 构建产物（不提交到 Git）

这些文件由构建过程生成，已添加到 `.gitignore`：

```
build/                  # 构建目录
debug/                  # Debug 构建
release/                # Release 构建
*.o, *.obj             # 目标文件
*.exe                  # Windows 可执行文件
moc_*.cpp              # Qt MOC 生成的文件
ui_*.h                 # Qt UI 生成的头文件
qrc_*.cpp              # Qt 资源生成的文件
Makefile*              # 生成的 Makefile
dist/                  # 发布包目录
*.zip, *.tar.gz        # 压缩包
*.dmg, *.AppImage      # 平台安装包
```

## GitHub Actions 产物

每次构建会生成以下产物（保留 30 天）：

```
IPtools-Windows-x86_64.zip
IPtools-Windows-i686.zip
IPtools-Linux-x86_64.tar.gz
IPtools-Linux-aarch64.tar.gz
IPtools-macOS-x86_64.dmg
IPtools-macOS-arm64.dmg
```

## 发布产物

创建 Git 标签时会生成正式发布（永久保存）：

```
IPtools-v1.0.0-Windows-x86_64.zip
IPtools-v1.0.0-Linux-x86_64.tar.gz
IPtools-v1.0.0-macOS-x86_64.dmg
...
```

## 开发工作流

### 1. 本地开发

```
main.cpp ─┐
mainwindow.cpp ─┤
mainwindow.h ─┼─→ qmake ─→ make ─→ IPtools
mainwindow.ui ─┘
```

### 2. Git 工作流

```
本地开发 ─→ git commit ─→ git push ─→ GitHub
                                      │
                                      ├─→ build.yml（自动构建）
                                      ├─→ code-quality.yml（代码检查）
                                      └─→ release.yml（标签触发）
```

### 3. CI/CD 流程

```
Push/PR ─→ GitHub Actions
           │
           ├─→ Windows (x86, x64)
           ├─→ Linux (x86_64, ARM64)
           └─→ macOS (Intel, ARM)
               │
               └─→ Artifacts（构建产物）
```

### 4. 发布流程

```
git tag v1.0.0 ─→ GitHub Release
                  │
                  ├─→ 构建所有平台
                  ├─→ 创建 Release 页面
                  └─→ 上传安装包
```

## 添加新文件

### 添加新的源文件

编辑 `IPtools.pro`：

```qmake
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    newfile.cpp       # 添加新文件

HEADERS += \
    mainwindow.h \
    newfile.h         # 添加新头文件
```

### 添加新的工作流

在 `.github/workflows/` 创建新的 `.yml` 文件：

```yaml
name: My Workflow
on: [push]
jobs:
  my-job:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      # 添加步骤
```

### 添加新的文档

在项目根目录创建 Markdown 文件，并更新 `README.md` 的链接。

## 维护任务

### 定期更新

- 更新 `CHANGELOG.md` 记录变更
- 更新 `VERSION` 文件
- 保持文档同步

### 代码质量

- 运行 `clang-format` 格式化代码
- 查看 `code-quality.yml` 的报告
- 修复编译警告

### 依赖更新

- Dependabot 会自动创建 PR 更新依赖
- 定期检查 Qt 版本更新
- 更新 GitHub Actions 版本

## 相关链接

- [Qt Project File](https://doc.qt.io/qt-5/qmake-project-files.html)
- [GitHub Actions](https://docs.github.com/en/actions)
- [Docker Multi-platform](https://docs.docker.com/buildx/working-with-buildx/)
- [EditorConfig](https://editorconfig.org/)
- [Clang-Format](https://clang.llvm.org/docs/ClangFormat.html)

---

**最后更新**: 2025-11-02

