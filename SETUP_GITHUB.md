# GitHub 仓库设置指南

本指南帮助您将项目上传到 GitHub 并启用自动构建工作流。

## 目录
1. [准备工作](#准备工作)
2. [创建 GitHub 仓库](#创建-github-仓库)
3. [上传代码](#上传代码)
4. [验证工作流](#验证工作流)
5. [创建第一个发布](#创建第一个发布)
6. [故障排除](#故障排除)

---

## 准备工作

### 1. 安装 Git

**Windows:**
- 下载: https://git-scm.com/download/win
- 安装时建议选择 "Git Bash" 选项

**Linux:**
```bash
# Ubuntu/Debian
sudo apt install git

# CentOS/RHEL
sudo yum install git

# Arch
sudo pacman -S git
```

**macOS:**
```bash
# 使用 Homebrew
brew install git

# 或使用 Xcode Command Line Tools
xcode-select --install
```

### 2. 配置 Git

```bash
git config --global user.name "您的名字"
git config --global user.email "your.email@example.com"

# 可选：配置默认分支名
git config --global init.defaultBranch main
```

### 3. 创建 GitHub 账号

访问 https://github.com 注册账号（如果还没有）

---

## 创建 GitHub 仓库

### 方法 1: 通过 GitHub 网页

1. 登录 GitHub
2. 点击右上角 "+" → "New repository"
3. 填写仓库信息：
   - **Repository name**: `IPtools`
   - **Description**: "基于Qt的跨平台系统工具 - IP查询、进程管理、文件句柄查询"
   - **Public/Private**: 选择 Public（公开）或 Private（私有）
   - **不要**勾选 "Initialize this repository with a README"
4. 点击 "Create repository"

### 方法 2: 使用 GitHub CLI

```bash
# 安装 GitHub CLI: https://cli.github.com
gh auth login
gh repo create IPtools --public --description "基于Qt的跨平台系统工具"
```

---

## 上传代码

### 初次上传

在项目根目录打开终端，执行：

```bash
# 1. 初始化 Git 仓库（如果还没有）
git init

# 2. 添加所有文件
git add .

# 3. 创建第一次提交
git commit -m "Initial commit: 添加IPtools项目及GitHub工作流"

# 4. 设置远程仓库（替换为您的用户名）
git remote add origin https://github.com/YOUR_USERNAME/IPtools.git

# 5. 推送代码
git branch -M main
git push -u origin main
```

### 如果遇到认证问题

**使用 Personal Access Token (推荐):**

1. 访问 GitHub Settings → Developer settings → Personal access tokens
2. 点击 "Generate new token (classic)"
3. 设置权限：
   - `repo` (完整仓库访问)
   - `workflow` (如果要修改工作流)
4. 复制生成的 token
5. 推送时使用 token 作为密码：
   ```bash
   # 用户名: 您的 GitHub 用户名
   # 密码: 粘贴您的 Personal Access Token
   git push -u origin main
   ```

**使用 SSH (替代方法):**

```bash
# 1. 生成 SSH 密钥
ssh-keygen -t ed25519 -C "your.email@example.com"

# 2. 查看公钥
cat ~/.ssh/id_ed25519.pub

# 3. 将公钥添加到 GitHub
# Settings → SSH and GPG keys → New SSH key

# 4. 使用 SSH URL
git remote set-url origin git@github.com:YOUR_USERNAME/IPtools.git
git push -u origin main
```

---

## 验证工作流

### 1. 检查 Actions 是否启用

1. 访问您的 GitHub 仓库
2. 点击 **Actions** 标签
3. 如果看到 "Workflows disabled"：
   - 点击 "I understand my workflows, go ahead and enable them"

### 2. 触发首次构建

推送代码后，工作流会自动运行：

```bash
# 查看 Actions 标签页
# 应该看到 "Build Multi-Platform" 工作流正在运行
```

### 3. 查看构建日志

1. 点击正在运行的工作流
2. 展开各个构建任务查看详细日志
3. 等待所有任务完成（通常需要 10-20 分钟）

### 4. 下载构建产物

构建完成后：
1. 滚动到页面底部
2. 在 **Artifacts** 区域下载各平台的构建结果

---

## 创建第一个发布

### 更新 CHANGELOG

编辑 `CHANGELOG.md` 添加版本信息：

```markdown
# 更新日志

## [1.0.0] - 2025-11-02

### 新增
- 文件句柄查询功能
- 进程管理功能
- IP地址查询功能
- GitHub Actions 自动构建工作流

### 平台支持
- Windows (x86/x64)
- Linux (x86_64/ARM64)
- macOS (Intel/Apple Silicon)
```

### 创建发布标签

```bash
# 1. 提交 CHANGELOG
git add CHANGELOG.md
git commit -m "docs: 更新 CHANGELOG for v1.0.0"
git push

# 2. 创建标签
git tag -a v1.0.0 -m "Release v1.0.0

- 首次正式发布
- 支持多平台自动构建
- 完整的功能实现"

# 3. 推送标签（触发发布工作流）
git push origin v1.0.0
```

### 监控发布过程

1. 访问 **Actions** 标签页
2. 查看 "Release Multi-Platform" 工作流
3. 等待所有平台构建完成（15-30 分钟）
4. 完成后访问 **Releases** 标签页

### 查看发布结果

1. 点击 **Releases** 标签
2. 应该看到新创建的 `v1.0.0` 发布
3. 包含：
   - 发布说明
   - 所有平台的安装包
   - 源代码压缩包

---

## 仓库设置（可选）

### 1. 配置分支保护

保护主分支，防止直接推送：

1. Settings → Branches → Add rule
2. 设置规则：
   - Branch name pattern: `main`
   - ✅ Require pull request reviews before merging
   - ✅ Require status checks to pass before merging
   - 选择 CI 工作流作为必需检查

### 2. 配置 Issue 模板

模板已包含在项目中：
- `.github/ISSUE_TEMPLATE/bug_report.md`
- `.github/ISSUE_TEMPLATE/feature_request.md`

### 3. 配置 Pull Request 模板

模板文件：`.github/pull_request_template.md`

### 4. 添加项目描述

Settings → 编辑仓库描述和主题：
- **Description**: "基于Qt的跨平台系统工具 - IP查询、进程管理、文件句柄查询"
- **Topics**: `qt`, `cpp`, `windows`, `linux`, `macos`, `network-tools`, `process-management`

### 5. 设置社交预览图

Settings → General → Social preview
- 上传一张 1280x640 的项目截图

---

## 故障排除

### 问题 1: 工作流无法运行

**症状**: Actions 标签页显示 "No workflows"

**解决方案**:
```bash
# 确保 .github/workflows/*.yml 文件存在
ls -la .github/workflows/

# 确保文件已提交
git add .github/workflows/
git commit -m "fix: 添加工作流文件"
git push
```

### 问题 2: Windows 构建失败

**症状**: "Qt installation failed"

**解决方案**:
- 检查 `build.yml` 中的 Qt 版本是否可用
- 尝试更换为 `5.15.2` 或其他稳定版本

### 问题 3: Linux ARM64 构建失败

**症状**: "Cross-compilation failed"

**原因**: ARM64 交叉编译需要特殊设置

**解决方案**: 使用 Docker 构建工作流：
1. 访问 Actions → Docker Cross-Platform Build
2. Run workflow
3. 输入 `amd64,arm64`

### 问题 4: 发布工作流权限错误

**症状**: "Resource not accessible by integration"

**解决方案**:
1. Settings → Actions → General
2. Workflow permissions → 选择 "Read and write permissions"
3. ✅ Allow GitHub Actions to create and approve pull requests
4. Save

### 问题 5: 构建产物过大

**症状**: 上传失败或超时

**解决方案**:
在 `build.yml` 中排除不必要的文件：
```yaml
- name: Deploy Qt Dependencies
  run: |
    windeployqt IPtools.exe --release --no-translations --no-opengl-sw
```

### 问题 6: 标签推送后无反应

**症状**: 标签推送但没有触发发布

**解决方案**:
```bash
# 检查标签格式（必须以 v 开头）
git tag -l

# 删除错误的标签
git tag -d 1.0.0
git push origin :refs/tags/1.0.0

# 重新创建正确格式的标签
git tag -a v1.0.0 -m "Release v1.0.0"
git push origin v1.0.0
```

---

## 后续维护

### 日常开发流程

```bash
# 1. 创建功能分支
git checkout -b feature/new-feature

# 2. 开发和提交
git add .
git commit -m "feat: 添加新功能"

# 3. 推送分支
git push origin feature/new-feature

# 4. 在 GitHub 创建 Pull Request

# 5. CI 通过后合并到 main

# 6. 删除功能分支
git branch -d feature/new-feature
git push origin --delete feature/new-feature
```

### 创建新版本

```bash
# 1. 更新版本号和 CHANGELOG
vim CHANGELOG.md

# 2. 提交
git add .
git commit -m "chore: 准备发布 v1.1.0"
git push

# 3. 创建标签
git tag -a v1.1.0 -m "Release v1.1.0"
git push origin v1.1.0

# 4. 等待自动发布完成
```

### 监控构建状态

添加徽章到 README.md：

```markdown
![Build Status](https://github.com/YOUR_USERNAME/IPtools/workflows/Build%20Multi-Platform/badge.svg)
![Release](https://img.shields.io/github/v/release/YOUR_USERNAME/IPtools)
![License](https://img.shields.io/github/license/YOUR_USERNAME/IPtools)
```

---

## 检查清单

完成设置后，确保：

- [ ] 代码已推送到 GitHub
- [ ] Actions 已启用
- [ ] 至少一次构建工作流成功运行
- [ ] 能够下载构建产物
- [ ] 创建了第一个发布标签
- [ ] 发布工作流成功完成
- [ ] Releases 页面显示正确
- [ ] 所有平台的安装包都可下载
- [ ] README.md 包含项目信息和徽章
- [ ] 配置了 Issue 和 PR 模板

---

## 相关资源

- [GitHub Actions 文档](https://docs.github.com/en/actions)
- [Git 教程](https://git-scm.com/book/zh/v2)
- [语义化版本](https://semver.org/lang/zh-CN/)
- [Keep a Changelog](https://keepachangelog.com/zh-CN/1.0.0/)
- [Conventional Commits](https://www.conventionalcommits.org/zh-hans/)

---

## 获取帮助

如果遇到问题：

1. 查看 [WORKFLOW_GUIDE.md](WORKFLOW_GUIDE.md) 了解工作流详情
2. 查看 GitHub Actions 运行日志
3. 搜索 GitHub Issues 中的类似问题
4. 创建新的 Issue 寻求帮助

---

**祝您使用愉快！** 🎉

