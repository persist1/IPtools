# 🎯 平台构建快速参考

> **新功能！** 现在你可以选择构建特定平台，而不必每次都构建所有平台。

## 📌 快速使用

### 网页界面操作（推荐）

1. **打开 Actions 页面**
   ```
   https://github.com/YOUR_USERNAME/IPtools/actions
   ```

2. **选择工作流**
   - 点击左侧 "Build Multi-Platform"

3. **手动触发**
   - 点击右上角 "Run workflow" 下拉按钮
   - 选择分支（通常是 `main`）
   - 在 "选择构建平台" 下拉菜单中选择：

   | 选项 | 说明 | 构建时间 |
   |------|------|---------|
   | `all` | 所有平台 | 10-20分钟 |
   | `windows-x64` | Windows 64位 | 5-8分钟 |
   | `windows-x86` | Windows 32位 | 5-8分钟 |
   | `linux-x86_64` | Linux 64位 | 3-5分钟 |
   | `linux-arm64` | Linux ARM64 | 5-10分钟 |
   | `macos-x86_64` | macOS Intel | 5-8分钟 |
   | `macos-arm64` | macOS M1/M2/M3 | 5-8分钟 |

4. **开始构建**
   - 点击绿色 "Run workflow" 按钮

### 命令行操作（GitHub CLI）

```bash
# 安装 GitHub CLI
# Windows: winget install GitHub.cli
# macOS:   brew install gh
# Linux:   sudo apt install gh

# 登录
gh auth login

# 构建所有平台
gh workflow run "Build Multi-Platform" -f platform=all

# 仅构建 Windows 64位
gh workflow run "Build Multi-Platform" -f platform=windows-x64

# 仅构建 Linux
gh workflow run "Build Multi-Platform" -f platform=linux-x86_64
```

## 🎬 使用场景

### 场景 1：测试特定平台修改
```bash
# 你修改了 Windows 相关代码，只需测试 Windows
gh workflow run "Build Multi-Platform" -f platform=windows-x64
```

### 场景 2：快速验证编译
```bash
# Linux 编译最快，用于快速验证代码
gh workflow run "Build Multi-Platform" -f platform=linux-x86_64
```

### 场景 3：发布前完整构建
```bash
# 发布前测试所有平台
gh workflow run "Build Multi-Platform" -f platform=all
```

### 场景 4：节省配额
```bash
# 免费账户每月 2000 分钟配额
# 只构建需要的平台可以节省配额
gh workflow run "Build Multi-Platform" -f platform=windows-x64
```

## 📦 下载产物

构建完成后：

1. 进入工作流运行详情页
2. 滚动到底部 "Artifacts" 区域
3. 点击下载对应平台的压缩包

**文件命名规则：**
- `IPtools-Windows-x86_64.zip` - Windows 64位
- `IPtools-Windows-i686.zip` - Windows 32位
- `IPtools-Linux-x86_64.tar.gz` - Linux 64位
- `IPtools-Linux-aarch64.tar.gz` - Linux ARM64
- `IPtools-macOS-x86_64.dmg` - macOS Intel
- `IPtools-macOS-arm64.dmg` - macOS Apple Silicon

## ⚠️ 注意事项

### 缓存失败不影响构建
如果看到类似错误：
```
Failed to save: Cache service responded with 400
```
**不用担心！** 这是 GitHub 缓存服务的临时问题：
- ✅ 不影响构建结果
- ⏱️ 只会让构建时间稍长（需要重新下载 Qt）
- 🔄 可以点击 "Re-run failed jobs" 重试

### 自动触发仍然构建所有平台
- Push 到 main/develop 分支 → 构建所有平台
- Pull Request → 构建所有平台
- 只有手动触发时才能选择特定平台

### 构建时间对照

| 触发方式 | 平台数量 | 预计时间 |
|---------|---------|---------|
| 自动触发 | 6个平台 | 10-20分钟 |
| 手动 all | 6个平台 | 10-20分钟 |
| 手动单平台 | 1个平台 | 3-10分钟 |

## 💡 最佳实践

### 开发阶段
```bash
# 日常开发只测试主要平台
gh workflow run "Build Multi-Platform" -f platform=windows-x64
```

### 发布阶段
```bash
# 发布前构建所有平台
gh workflow run "Build Multi-Platform" -f platform=all

# 或推送标签自动发布
git tag -a v1.0.0 -m "Release v1.0.0"
git push origin v1.0.0
```

## 🔗 相关文档

- [BUILD_PLATFORM_GUIDE.md](BUILD_PLATFORM_GUIDE.md) - 完整详细指南
- [WORKFLOW_SETUP_SUMMARY.md](WORKFLOW_SETUP_SUMMARY.md) - 工作流配置总结
- [README.md](README.md) - 项目说明

## ❓ 常见问题

**Q: 可以同时选择多个平台吗？**
A: 不可以，但可以选择 `all` 构建所有平台。

**Q: 如何取消正在运行的构建？**
A: 在工作流运行页面点击右上角 "Cancel workflow"。

**Q: 构建失败怎么办？**
A: 点击 "Re-run failed jobs" 重试，通常第二次会成功。

**Q: 免费账户有限制吗？**
A: 有，每月 2000 分钟配额。选择性构建可以节省配额。

---

**更新时间**: 2025-11-02
**版本**: 1.0

