# 🚀 多平台构建指南

本文档介绍如何使用 GitHub Actions 自动构建 IPtools 项目的不同平台版本。

## 📋 目录

- [支持的平台](#支持的平台)
- [自动构建](#自动构建)
- [手动选择平台构建](#手动选择平台构建)
- [下载构建产物](#下载构建产物)
- [常见问题](#常见问题)

## 🎯 支持的平台

| 平台 | 架构 | 选择器名称 | 说明 |
|------|------|-----------|------|
| 🪟 Windows | x64 (64位) | `windows-x64` | 推荐用于现代 Windows 系统 |
| 🪟 Windows | x86 (32位) | `windows-x86` | 用于旧版 Windows 系统 |
| 🐧 Linux | x86_64 | `linux-x86_64` | 标准 Linux 发行版 (包含 AppImage) |
| 🐧 Linux | ARM64 | `linux-arm64` | ARM 架构 Linux (树莓派等) |
| 🍎 macOS | x86_64 (Intel) | `macos-x86_64` | Intel Mac |
| 🍎 macOS | ARM64 (M1/M2/M3) | `macos-arm64` | Apple Silicon Mac |

## ⚡ 自动构建

### 触发条件

以下操作会自动触发**所有平台**的构建：

1. **推送代码到主分支**
   ```bash
   git push origin main
   ```

2. **推送代码到开发分支**
   ```bash
   git push origin develop
   ```

3. **创建 Pull Request**
   - 当你向 `main`、`master` 或 `develop` 分支提交 PR 时

### 查看构建状态

1. 访问你的 GitHub 仓库
2. 点击顶部的 **Actions** 标签
3. 找到 "Build Multi-Platform" 工作流
4. 查看各平台的构建状态

## 🎮 手动选择平台构建

### 使用场景

- ✅ 只需要测试某个特定平台
- ✅ 节省 GitHub Actions 使用配额
- ✅ 快速验证单个平台的问题
- ✅ 临时需要某个平台的构建

### 操作步骤

#### 方法一：通过 GitHub 网页界面

1. **进入 Actions 页面**
   - 访问你的仓库
   - 点击顶部的 **Actions** 标签

2. **选择工作流**
   - 在左侧找到 "Build Multi-Platform"
   - 点击该工作流

3. **手动触发**
   - 点击右上角的 **Run workflow** 按钮
   - 会弹出一个下拉菜单

4. **选择平台**
   - **分支选择**: 选择要构建的分支（通常是 `main`）
   - **平台选择**: 从下拉菜单中选择：
     - `all` - 构建所有平台（默认）
     - `windows-x64` - 仅 Windows 64位
     - `windows-x86` - 仅 Windows 32位
     - `linux-x86_64` - 仅 Linux x86_64
     - `linux-arm64` - 仅 Linux ARM64
     - `macos-x86_64` - 仅 macOS Intel
     - `macos-arm64` - 仅 macOS Apple Silicon

5. **开始构建**
   - 点击绿色的 **Run workflow** 按钮
   - 等待构建完成（5-20分钟，取决于选择的平台）

#### 方法二：通过 GitHub CLI

如果你安装了 [GitHub CLI](https://cli.github.com/)：

```bash
# 构建所有平台
gh workflow run "Build Multi-Platform" -f platform=all

# 仅构建 Windows x64
gh workflow run "Build Multi-Platform" -f platform=windows-x64

# 仅构建 Linux x86_64
gh workflow run "Build Multi-Platform" -f platform=linux-x86_64

# 仅构建 macOS ARM64
gh workflow run "Build Multi-Platform" -f platform=macos-arm64
```

## 📦 下载构建产物

### 步骤

1. **进入 Actions 页面**
   - 访问仓库的 Actions 标签

2. **选择工作流运行**
   - 找到你想下载的构建记录
   - 点击进入详情页

3. **下载 Artifacts**
   - 滚动到页面底部
   - 找到 **Artifacts** 部分
   - 点击对应平台的压缩包下载

### Artifacts 命名规则

| 文件名 | 内容 | 格式 |
|--------|------|------|
| `IPtools-Windows-x86_64.zip` | Windows 64位版本 | ZIP |
| `IPtools-Windows-i686.zip` | Windows 32位版本 | ZIP |
| `IPtools-Linux-x86_64.tar.gz` | Linux 64位版本 + AppImage | TAR.GZ + AppImage |
| `IPtools-Linux-aarch64.tar.gz` | Linux ARM64版本 | TAR.GZ |
| `IPtools-macOS-x86_64.dmg` | macOS Intel版本 | DMG |
| `IPtools-macOS-arm64.dmg` | macOS Apple Silicon版本 | DMG |

### 保留时间

- ⏱️ **30 天** - Actions 构建产物会保留 30 天
- ♾️ **永久** - Release 发布的文件永久保留

> 💡 提示：如果需要长期保存，建议创建 Release（推送 Git 标签）

## 🔍 构建时间参考

| 平台组合 | 预计时间 | 并行执行 |
|---------|---------|---------|
| 所有平台 (all) | 10-20 分钟 | ✅ 是 |
| Windows x64 | 5-8 分钟 | - |
| Windows x86 | 5-8 分钟 | - |
| Linux x86_64 | 3-5 分钟 | - |
| Linux ARM64 | 5-10 分钟 | - |
| macOS x86_64 | 5-8 分钟 | - |
| macOS ARM64 | 5-8 分钟 | - |

> 📝 注意：选择多个平台时会并行构建，总时间取决于最慢的平台。

## ❓ 常见问题

### Q1: 为什么有时构建会失败？

**A:** 可能的原因：

1. **GitHub 服务临时故障**
   - 错误信息包含 `Cache service responded with 400`
   - **解决方法**: 重新运行工作流（点击 "Re-run failed jobs"）

2. **依赖下载失败**
   - Qt 安装超时或失败
   - **解决方法**: 重新运行，通常第二次会成功

3. **代码编译错误**
   - 查看构建日志中的具体错误信息
   - **解决方法**: 修复代码后重新推送

### Q2: 如何只构建我需要的平台，节省时间？

**A:** 使用手动触发功能：

```bash
# 例如只需要 Windows 版本
gh workflow run "Build Multi-Platform" -f platform=windows-x64
```

或通过网页界面选择对应平台。

### Q3: Linux ARM64 构建失败怎么办？

**A:** Linux ARM64 的交叉编译需要 ARM64 Qt 库，目前的配置可能无法完全成功。建议：

- 使用 Docker 构建 ARM64 版本
- 或在实际的 ARM64 设备上本地构建

### Q4: 缓存失败会影响构建吗？

**A:** 不会影响构建结果，只是：

- ❌ 缓存失败 → 构建时间更长（需要重新下载 Qt）
- ✅ 缓存成功 → 构建时间更短（复用之前下载的 Qt）

### Q5: 如何查看构建日志？

**A:** 

1. 进入 Actions 页面
2. 点击某次构建
3. 点击左侧的具体任务（如 "Build Windows (x64)"）
4. 展开各个步骤查看详细日志

### Q6: 构建产物在哪里？

**A:** 

- **Actions 构建**: 在工作流详情页底部的 Artifacts 区域
- **Release 发布**: 在仓库的 Releases 页面

### Q7: 可以同时运行多个构建吗？

**A:** 可以，GitHub Actions 支持并发运行。但注意：

- 免费账户有并发限制
- 同时运行多个会更快消耗配额

## 🎨 高级用法

### 创建自动发布

推送带版本号的 Git 标签会自动创建 Release：

```bash
# 创建版本标签
git tag -a v1.0.0 -m "Release v1.0.0"

# 推送标签（会触发 release.yml 工作流）
git push origin v1.0.0
```

这会：
- ✅ 构建所有平台
- ✅ 自动创建 GitHub Release
- ✅ 上传所有安装包
- ✅ 生成 Release Notes

### 修改 Qt 版本

编辑 `.github/workflows/build.yml`：

```yaml
env:
  QT_VERSION: '5.15.2'  # 修改为你需要的版本
```

支持的版本可以在 [aqtinstall](https://ddalcino.github.io/aqt-list-server/) 查询。

### 添加构建徽章

在 `README.md` 中添加构建状态徽章：

```markdown
![Build Status](https://github.com/YOUR_USERNAME/IPtools/workflows/Build%20Multi-Platform/badge.svg)
```

## 📞 获取帮助

如果遇到问题：

1. 📖 查看 [WORKFLOW_SETUP_SUMMARY.md](./WORKFLOW_SETUP_SUMMARY.md)
2. 📖 查看 [CROSS_PLATFORM.md](./CROSS_PLATFORM.md)
3. 🔍 搜索 [GitHub Issues](https://github.com/YOUR_USERNAME/IPtools/issues)
4. 💬 创建新的 Issue 描述你的问题

## 📝 相关文档

- [WORKFLOW_SETUP_SUMMARY.md](./WORKFLOW_SETUP_SUMMARY.md) - 工作流配置总结
- [CROSS_PLATFORM.md](./CROSS_PLATFORM.md) - 跨平台编译详细指南
- [CONTRIBUTING.md](./CONTRIBUTING.md) - 贡献指南
- [README.md](./README.md) - 项目说明

---

**最后更新**: 2025-11-02
**适用版本**: GitHub Actions Workflows v1.0+

