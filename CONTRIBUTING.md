# 贡献指南

感谢您对 IPtools 项目的关注！欢迎提交问题、建议或代码贡献。

## 目录
1. [开发环境设置](#开发环境设置)
2. [代码规范](#代码规范)
3. [提交代码](#提交代码)
4. [报告问题](#报告问题)

---

## 开发环境设置

### 前置要求
- **Qt**: 5.14.2 或更高版本
- **编译器**: 
  - Windows: MinGW 或 MSVC
  - Linux: GCC 7+ 或 Clang 5+
  - macOS: Clang (Xcode Command Line Tools)
- **Git**: 用于版本控制

### 克隆项目
```bash
git clone https://github.com/YOUR_USERNAME/IPtools.git
cd IPtools
```

### 构建项目
```bash
# Qt Creator (推荐)
# 1. 打开 IPtools.pro
# 2. 配置构建套件
# 3. 点击构建

# 命令行
qmake IPtools.pro
make  # Linux/macOS
mingw32-make  # Windows MinGW
```

---

## 代码规范

### C++ 代码风格
- **缩进**: 4个空格（不使用 Tab）
- **命名规范**:
  - 类名: `PascalCase` (如 `MainWindow`)
  - 函数名: `camelCase` (如 `queryProcesses`)
  - 变量名: `camelCase` (如 `processId`)
  - 常量: `UPPER_SNAKE_CASE` (如 `MAX_BUFFER_SIZE`)
- **括号**: K&R 风格
  ```cpp
  void function() {
      if (condition) {
          // code
      }
  }
  ```

### Qt 特定规范
- 使用 Qt 容器类 (`QList`, `QString`, `QVector`)
- 使用 `Q_UNUSED()` 宏标记未使用的参数
- 使用 Qt 信号槽机制
- 添加适当的 `Q_OBJECT` 宏

### 注释规范
```cpp
/**
 * @brief 查询进程信息
 * @param processName 进程名称或PID
 * @return 进程信息列表
 */
QList<ProcessInfo> queryProcessInfo(const QString& processName);

// 单行注释：解释为什么这样做，而不是做什么
```

---

## 提交代码

### 分支策略
- `main/master`: 稳定发布版本
- `develop`: 开发分支
- `feature/*`: 新功能分支
- `bugfix/*`: 错误修复分支
- `hotfix/*`: 紧急修复分支

### Fork 和 Pull Request 流程

1. **Fork 项目**
   - 点击 GitHub 页面右上角的 "Fork" 按钮

2. **创建功能分支**
   ```bash
   git checkout -b feature/your-feature-name
   # 或
   git checkout -b bugfix/issue-number-description
   ```

3. **提交更改**
   ```bash
   git add .
   git commit -m "feat: 添加新功能说明"
   ```

4. **提交规范** (遵循 [Conventional Commits](https://www.conventionalcommits.org/))
   ```
   feat: 新功能
   fix: 错误修复
   docs: 文档更新
   style: 代码格式调整（不影响功能）
   refactor: 代码重构
   test: 测试相关
   chore: 构建工具或辅助工具的变动
   perf: 性能优化
   ```

   示例:
   ```
   feat: 添加进程CPU使用率显示功能
   fix: 修复Windows下中文路径乱码问题
   docs: 更新README中的编译说明
   ```

5. **推送分支**
   ```bash
   git push origin feature/your-feature-name
   ```

6. **创建 Pull Request**
   - 访问您的 Fork 仓库
   - 点击 "Pull Request" 按钮
   - 填写详细的 PR 描述
   - 等待代码审查

### Pull Request 检查清单
- [ ] 代码符合项目代码规范
- [ ] 已在本地测试通过
- [ ] 添加了必要的注释
- [ ] 更新了相关文档（如果需要）
- [ ] 没有引入新的编译警告
- [ ] 通过了所有 CI 检查

---

## 报告问题

### 提交 Issue 前
1. 搜索现有的 Issues，确保问题未被报告
2. 使用最新版本重现问题
3. 收集详细信息

### Issue 模板

**Bug 报告:**
```markdown
## 问题描述
简短描述问题

## 复现步骤
1. 打开程序
2. 点击xxx
3. 看到错误

## 期望行为
描述您期望发生什么

## 实际行为
描述实际发生了什么

## 环境信息
- 操作系统: [例如 Windows 10, Ubuntu 20.04]
- 架构: [例如 x64, ARM64]
- Qt 版本: [例如 5.15.2]
- 编译器: [例如 MinGW 8.1]

## 截图或日志
如果可能，请附上截图或错误日志
```

**功能请求:**
```markdown
## 功能描述
描述您想要的功能

## 使用场景
为什么需要这个功能？

## 建议的实现方式
（可选）您认为应该如何实现
```

---

## 平台特定开发

### Windows
```bash
# 需要管理员权限进行调试
# 使用 Qt Creator 时：以管理员身份运行 Qt Creator
```

### Linux
```bash
# 需要 root 权限
sudo gdb ./IPtools

# 或使用 capabilities
sudo setcap cap_sys_ptrace=eip ./IPtools
```

### macOS
```bash
# macOS 部分功能受限，需要授予权限
# 系统偏好设置 → 安全性与隐私 → 隐私
```

---

## 测试

### 手动测试清单

**文件句柄查询:**
- [ ] 查询已打开的文件
- [ ] 查询不存在的文件
- [ ] 通过进程名查询
- [ ] 通过 PID 查询
- [ ] 结束进程功能

**IP 地址查询:**
- [ ] 获取本机 IP
- [ ] 查询外部 IP
- [ ] 显示网卡信息
- [ ] 显示 MAC 地址

**跨平台测试:**
- [ ] Windows x64
- [ ] Windows x86
- [ ] Linux x86_64
- [ ] Linux ARM64
- [ ] macOS Intel
- [ ] macOS Apple Silicon

---

## CI/CD 工作流

项目使用 GitHub Actions 进行自动构建：

- **构建工作流** (`.github/workflows/build.yml`): 每次推送或 PR 时自动构建所有平台
- **发布工作流** (`.github/workflows/release.yml`): 标签推送时自动创建发布
- **Docker 构建** (`.github/workflows/docker-cross-build.yml`): 支持 Docker 多架构构建
- **代码质量** (`.github/workflows/code-quality.yml`): 静态分析和代码检查

### 本地测试 CI
```bash
# 使用 act 工具在本地运行 GitHub Actions
# https://github.com/nektos/act
act -j build-windows
act -j build-linux
```

---

## 发布流程

发布由维护者执行：

```bash
# 1. 更新版本号和 CHANGELOG.md
vim CHANGELOG.md

# 2. 提交更改
git add .
git commit -m "chore: 准备发布 v1.x.x"

# 3. 创建标签
git tag -a v1.x.x -m "Release v1.x.x"

# 4. 推送标签（触发自动发布）
git push origin v1.x.x
```

GitHub Actions 会自动：
1. 为所有平台构建二进制文件
2. 创建 GitHub Release
3. 上传所有构建产物

---

## 获取帮助

- **文档**: 查看 [README.md](README.md), [BUILD.md](BUILD.md), [USAGE_GUIDE.md](USAGE_GUIDE.md)
- **Issues**: [GitHub Issues](https://github.com/YOUR_USERNAME/IPtools/issues)
- **讨论**: [GitHub Discussions](https://github.com/YOUR_USERNAME/IPtools/discussions)

---

## 许可证

提交代码即表示您同意将您的贡献按照项目许可证进行许可。

---

感谢您的贡献！🎉

