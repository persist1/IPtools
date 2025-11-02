# 快速开始指南

5分钟快速上手 IPtools 项目的 GitHub 工作流。

## 目录
- [上传到 GitHub](#上传到-github)
- [验证自动构建](#验证自动构建)
- [创建发布](#创建发布)
- [本地构建](#本地构建)

---

## 上传到 GitHub

### 1. 创建 GitHub 仓库

访问 https://bgithub.xyz/new 创建新仓库：
- 名称: `IPtools`
- 可见性: Public（公开）或 Private（私有）
- **不要**勾选 "Add a README file"

### 2. 推送代码

在项目目录打开终端：

```bash
# 初始化（如果还没有）
git init

# 添加所有文件
git add .

# 提交
git commit -m "Initial commit: 添加IPtools及自动构建工作流"

# 添加远程仓库（替换 YOUR_USERNAME）
git remote add origin https://github.com/YOUR_USERNAME/IPtools.git

# 推送
git branch -M main
git push -u origin main
```

> 💡 **提示**: 如果要求输入密码，使用 GitHub Personal Access Token
> - 生成地址: https://github.com/settings/tokens
> - 需要勾选 `repo` 权限

---

## 验证自动构建

### 1. 启用 Actions

1. 访问您的仓库页面
2. 点击 **Actions** 标签
3. 如果显示需要启用，点击 "Enable workflows"

### 2. 查看构建状态

推送后自动触发构建：
- 访问 Actions 标签页
- 查看 "Build Multi-Platform" 工作流
- 等待约 10-20 分钟

### 3. 下载构建产物

构建完成后：
1. 点击完成的工作流运行
2. 滚动到底部 **Artifacts** 区域
3. 下载各平台的安装包

支持的平台：
- ✅ Windows x64/x86
- ✅ Linux x86_64/ARM64
- ✅ macOS Intel/ARM64

---

## 创建发布

### 快速发布命令

```bash
# 1. 创建版本标签
git tag -a v1.0.0 -m "首次发布"

# 2. 推送标签
git push origin v1.0.0

# 3. 等待自动构建（15-30分钟）

# 4. 查看发布
# 访问: https://github.com/YOUR_USERNAME/IPtools/releases
```

就这么简单！GitHub Actions 会自动：
1. ✅ 构建所有平台
2. ✅ 创建 Release 页面
3. ✅ 上传安装包
4. ✅ 生成发布说明

---

## 本地构建

### Windows

双击运行或在 PowerShell 中：
```batch
scripts\build-windows.bat release
```

构建结果: `release\IPtools.exe`

### Linux

```bash
chmod +x scripts/build-linux.sh
./scripts/build-linux.sh release
```

构建结果: `./IPtools`

### macOS

```bash
chmod +x scripts/build-macos.sh
./scripts/build-macos.sh release
```

构建结果: `IPtools.app` 或 `IPtools.dmg`

---

## 常用命令

### 日常开发

```bash
# 拉取最新代码
git pull

# 创建功能分支
git checkout -b feature/my-feature

# 提交更改
git add .
git commit -m "feat: 添加新功能"

# 推送并创建 PR
git push origin feature/my-feature
```

### 发布新版本

```bash
# 1. 更新 CHANGELOG.md
vim CHANGELOG.md

# 2. 提交更改
git add .
git commit -m "chore: 准备发布 v1.1.0"
git push

# 3. 创建并推送标签
git tag -a v1.1.0 -m "Release v1.1.0"
git push origin v1.1.0
```

### 查看构建状态

```bash
# 访问 Actions 页面
https://github.com/YOUR_USERNAME/IPtools/actions

# 或使用 GitHub CLI
gh run list
gh run watch
```

---

## 故障排除

### 问题: 工作流未运行

**解决**: 
1. 确认 Actions 已启用
2. 检查 `.github/workflows/` 目录存在
3. 重新推送触发构建

### 问题: 构建失败

**解决**: 
1. 查看 Actions 日志
2. 检查是否网络问题
3. 尝试手动重新运行

### 问题: 发布未创建

**解决**: 
1. 确认标签以 `v` 开头 (如 `v1.0.0`)
2. 检查 Actions 权限设置
3. 查看 Release 工作流日志

---

## 下一步

- 📖 阅读 [WORKFLOW_GUIDE.md](WORKFLOW_GUIDE.md) 了解详细工作流
- 📖 阅读 [SETUP_GITHUB.md](SETUP_GITHUB.md) 了解完整设置
- 📖 阅读 [CONTRIBUTING.md](CONTRIBUTING.md) 了解如何贡献
- 🐛 报告问题: [创建 Issue](https://github.com/YOUR_USERNAME/IPtools/issues/new)

---

## 快速链接

- 🚀 [GitHub Releases](https://github.com/YOUR_USERNAME/IPtools/releases)
- 🔧 [GitHub Actions](https://github.com/YOUR_USERNAME/IPtools/actions)
- 📝 [Issues](https://github.com/YOUR_USERNAME/IPtools/issues)
- 💬 [Discussions](https://github.com/YOUR_USERNAME/IPtools/discussions)

---

**问题求助**: 如有问题请创建 Issue 或查看 [SETUP_GITHUB.md](SETUP_GITHUB.md)

祝您使用愉快！ 🎉

