# 🚀 从这里开始

欢迎使用 IPtools！本指南将帮助您快速上传项目到 GitHub 并启用自动构建。

## ✨ 已完成的配置

✅ **5 个 GitHub Actions 工作流**
- 自动构建（Windows/Linux/macOS）
- 自动发布
- Docker 多架构构建
- 代码质量检查
- 自动管理过期 Issue

✅ **6 个平台支持**
- Windows x86/x64
- Linux x86_64/ARM64  
- macOS Intel/Apple Silicon

✅ **完整文档**
- 快速开始指南
- 工作流详解
- 贡献指南
- 项目结构说明

✅ **本地构建脚本**
- Windows/Linux/macOS 构建脚本
- Docker 测试脚本
- 发布打包脚本

## 🎯 三步上传到 GitHub

### 第一步：创建 GitHub 仓库

访问 https://github.com/new
- 仓库名称: `IPtools`
- 可见性: Public（推荐）
- **不要**勾选任何初始化选项

### 第二步：推送代码

在项目目录打开终端，执行：

```bash
git init
git add .
git commit -m "feat: 完整的 IPtools 项目及自动构建工作流"
git remote add origin https://github.com/YOUR_USERNAME/IPtools.git
git branch -M main
git push -u origin main
```

> 💡 将 `YOUR_USERNAME` 替换为您的 GitHub 用户名

### 第三步：启用 Actions

1. 访问您的仓库页面
2. 点击 **Actions** 标签
3. 点击 **"I understand my workflows, go ahead and enable them"**

🎉 **完成！** 工作流将自动开始构建。

## 📱 查看构建结果

1. 在 **Actions** 标签页查看构建进度
2. 等待 10-20 分钟完成所有平台构建
3. 在 **Artifacts** 区域下载构建产物

## 🏷️ 创建第一个发布

```bash
# 创建标签
git tag -a v1.0.0 -m "Release v1.0.0"

# 推送标签
git push origin v1.0.0

# 等待 15-30 分钟
# 访问: https://github.com/YOUR_USERNAME/IPtools/releases
```

GitHub Actions 会自动：
- ✅ 构建所有平台
- ✅ 创建 Release 页面
- ✅ 上传所有安装包

## 📚 详细文档

### 新手必读
- **QUICK_START.md** - 5 分钟快速入门 ⭐
- **SETUP_GITHUB.md** - GitHub 完整设置教程 ⭐⭐
- **WORKFLOW_SETUP_SUMMARY.md** - 配置总结 ⭐

### 进阶指南
- **WORKFLOW_GUIDE.md** - GitHub Actions 详解
- **BUILD.md** - 编译说明
- **CROSS_PLATFORM.md** - 跨平台编译
- **CONTRIBUTING.md** - 贡献代码

### 参考文档
- **PROJECT_STRUCTURE.md** - 项目结构
- **USAGE_GUIDE.md** - 功能使用
- **README.md** - 项目概述

## 🔧 本地构建

不想等待 GitHub Actions？可以本地构建：

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

## 🐛 遇到问题？

### 推送失败
- 需要 Personal Access Token
- 访问: https://github.com/settings/tokens
- 勾选 `repo` 权限

### Actions 未运行
- 确认已启用 Actions
- 检查 `.github/workflows/` 目录存在

### 构建失败
- 查看 Actions 日志
- 检查 Qt 版本
- 创建 Issue 寻求帮助

## 🎊 推荐操作

完成上传后：

1. ✅ **添加徽章** 到 README.md
   ```markdown
   ![Build](https://github.com/YOUR_USERNAME/IPtools/workflows/Build%20Multi-Platform/badge.svg)
   ```

2. ✅ **配置分支保护**
   - Settings → Branches → Add rule
   - 要求 PR 审查和 CI 通过

3. ✅ **设置仓库描述**
   - Settings → General
   - 添加描述和主题标签

4. ✅ **创建第一个发布**
   - 推送 v1.0.0 标签
   - 下载并测试安装包

## 📊 构建时间

| 阶段 | 预期时间 |
|------|----------|
| 自动构建 | 10-20 分钟 |
| 发布构建 | 15-30 分钟 |
| 本地构建 | 2-5 分钟 |

## 🌟 支持的平台

### 自动构建
✅ Windows 10/11 (x86, x64)
✅ Ubuntu 20.04+ / Debian / CentOS (x86_64)
✅ Linux ARM64 (树莓派、ARM 服务器)
✅ macOS 12+ (Intel, Apple Silicon)

### Docker 构建
✅ linux/amd64
✅ linux/arm64  
✅ linux/arm/v7

## 💡 提示

- 📌 每次 Push 会触发自动构建
- 📌 创建 PR 会运行所有检查
- 📌 推送标签会自动发布
- 📌 构建产物保留 30 天
- 📌 Release 永久保存

## 🔗 快速链接

准备好后，访问：

- 🚀 **QUICK_START.md** - 立即开始
- 📖 **SETUP_GITHUB.md** - 详细教程
- 🎯 **WORKFLOW_SETUP_SUMMARY.md** - 配置说明

---

## ⚡ 超快速开始（专家模式）

如果您熟悉 Git 和 GitHub：

```bash
# 创建仓库
gh repo create IPtools --public --source=. --remote=origin

# 推送代码
git add .
git commit -m "feat: 完整项目及自动构建"
git push -u origin main

# 创建发布
git tag -a v1.0.0 -m "Release v1.0.0"
git push origin v1.0.0
```

完成！🎉

---

**需要帮助？** 查看详细文档或创建 Issue

**准备好了？** 开始使用 → **QUICK_START.md** ⭐

