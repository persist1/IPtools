# GitHub Actions 常见问题解答

## ❓ 为什么我选择了单个平台，但所有平台都在构建？

### 原因

你**推送代码**（`git push`）触发了自动构建，而不是手动触发。

- **自动触发**（Push/PR）→ 构建所有平台 ✅
- **手动触发**（Actions 页面）→ 可选择单个平台 ✅

### 如何区分

| 触发方式 | 事件类型 | 能否选择平台 | 标识 |
|---------|---------|------------|------|
| `git push` | push | ❌ 否（全部） | "triggered via push" |
| Pull Request | pull_request | ❌ 否（全部） | "triggered via pull_request" |
| Actions 手动触发 | workflow_dispatch | ✅ 是 | "triggered via workflow_dispatch" |

### 解决方案

#### 如果想选择单个平台构建：

1. **不要使用 `git push` 触发**
2. 进入 GitHub 仓库的 **Actions** 页面
3. 选择 "Build Multi-Platform" 工作流
4. 点击右上角 **"Run workflow"** 按钮
5. 选择你想要的平台
6. 点击绿色 **"Run workflow"** 开始

#### 如果想推送代码但不触发构建：

在 commit 信息中添加 `[skip ci]` 或 `[ci skip]`：

```bash
git commit -m "docs: 更新文档 [skip ci]"
git push origin main
```

## ❓ 为什么 Windows 构建失败了？

### 错误信息

```
Project ERROR: Cannot run compiler 'g++'. Output:
==================
Maybe you forgot to setup the environment?
mingw32-make: *** No targets specified and no makefile found.
```

### 原因

1. **Qt 安装后的 MinGW 编译器环境变量未设置**
2. `jurplel/install-qt-action` 可能没有包含完整的 MinGW 工具链

### 解决方案

我已经在最新的 `build.yml` 中添加了以下修复：

```yaml
- name: Setup MinGW
  shell: pwsh
  run: |
    # 检查 Qt 环境
    Write-Host "Qt5_Dir = $env:Qt5_Dir"
    
    # 安装 MinGW
    choco install mingw -y --force
    
    # 刷新环境变量
    refreshenv
```

### 验证修复

推送更新的 `build.yml` 后，构建步骤会：
1. ✅ 检查 Qt 环境
2. ✅ 安装 MinGW 编译器
3. ✅ 验证 qmake 和 g++ 是否可用
4. ✅ 开始编译

## 🎯 最佳实践

### 日常开发

```bash
# 方式1：不触发构建
git commit -m "fix: 修复小bug [skip ci]"
git push origin main

# 方式2：推送后手动选择平台测试
git push origin main
# 然后在 Actions 页面手动触发单个平台构建
```

### 发布前测试

```bash
# 推送代码，自动构建所有平台
git commit -m "feat: 准备发布 v1.0.0"
git push origin main
# 等待所有平台构建完成

# 或手动触发所有平台
# Actions → Run workflow → 选择 "all"
```

### 创建发布

```bash
# 创建标签会触发 release.yml，自动构建并发布
git tag -a v1.0.0 -m "Release v1.0.0"
git push origin v1.0.0
```

## 📊 构建触发对照表

| 操作 | 触发工作流 | 构建平台 | 用途 |
|------|----------|---------|------|
| `git push main` | build.yml | 全部 | 自动测试 |
| `git push` + `[skip ci]` | 无 | 无 | 仅推送代码 |
| Actions 手动 → all | build.yml | 全部 | 完整测试 |
| Actions 手动 → windows-x64 | build.yml | 仅 Windows 64 | 快速测试 |
| `git push tag v*` | release.yml | 全部 | 发布版本 |

## 🔧 故障排除

### 问题：Actions 没有运行

**检查**：
1. Actions 是否已启用？（Settings → Actions → General）
2. `.github/workflows/` 目录是否存在？
3. YAML 文件语法是否正确？

### 问题：缓存失败

**错误**：
```
Failed to save: Cache service responded with 400
```

**解决**：
- ✅ 不影响构建结果
- ✅ 点击 "Re-run failed jobs" 重试
- ✅ 是 GitHub 服务的临时问题

### 问题：某个平台构建失败

**步骤**：
1. 点击失败的任务查看详细日志
2. 找到具体的错误信息
3. 根据错误类型修复：
   - 编译错误 → 检查代码
   - 依赖缺失 → 更新工作流安装步骤
   - 超时 → 增加超时时间或优化构建

## 📝 相关文档

- [BUILD_PLATFORM_GUIDE.md](BUILD_PLATFORM_GUIDE.md) - 完整的平台构建指南
- [PLATFORM_BUILD_QUICK_REF.md](PLATFORM_BUILD_QUICK_REF.md) - 快速参考
- [WORKFLOW_SETUP_SUMMARY.md](WORKFLOW_SETUP_SUMMARY.md) - 工作流配置总结

## 💡 提示

- 开发阶段建议使用 `[skip ci]` 避免不必要的构建
- 需要测试时在 Actions 页面手动选择单个平台
- 发布前一定要构建所有平台进行完整测试

---

**更新时间**: 2025-11-02
**版本**: 1.0

