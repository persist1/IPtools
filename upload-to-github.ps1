# 一键上传到 GitHub 脚本
# 右键点击此文件 -> "使用 PowerShell 运行"

Write-Host "======================================" -ForegroundColor Cyan
Write-Host "   IPtools 一键上传到 GitHub" -ForegroundColor Cyan
Write-Host "======================================" -ForegroundColor Cyan
Write-Host ""

# 检查 Git 是否安装
$gitInstalled = Get-Command git -ErrorAction SilentlyContinue
if (-not $gitInstalled) {
    Write-Host "? 错误: Git 未安装!" -ForegroundColor Red
    Write-Host ""
    Write-Host "解决方案:" -ForegroundColor Yellow
    Write-Host "  1. 双击运行 'install-git.ps1' 安装 Git" -ForegroundColor White
    Write-Host "  2. 安装完成后，关闭并重新打开 PowerShell" -ForegroundColor White
    Write-Host "  3. 再次运行此脚本" -ForegroundColor White
    Write-Host ""
    Write-Host "按任意键退出..."
    $null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
    exit 1
}

Write-Host "? Git 已安装: $(git --version)" -ForegroundColor Green
Write-Host ""

# 项目配置
$repoUrl = "https://github.com/persist1/IPtools.git"
$username = "persist1"
$email = "317627074@qq.com"

Write-Host "项目信息:" -ForegroundColor Cyan
Write-Host "  仓库地址: $repoUrl" -ForegroundColor White
Write-Host "  用户名: $username" -ForegroundColor White
Write-Host ""

# 配置 Git 用户信息
Write-Host "[1/5] 配置 Git 用户信息..." -ForegroundColor Yellow
try {
    git config user.name "$username"
    git config user.email "$email"
    Write-Host "? 配置完成" -ForegroundColor Green
} catch {
    Write-Host "? 配置失败: $_" -ForegroundColor Red
    Write-Host "按任意键退出..."
    $null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
    exit 1
}
Write-Host ""

# 检查是否已经初始化
$isGitRepo = Test-Path ".git"

if (-not $isGitRepo) {
    # 初始化 Git 仓库
    Write-Host "[2/5] 初始化 Git 仓库..." -ForegroundColor Yellow
    try {
        git init
        Write-Host "? 初始化完成" -ForegroundColor Green
    } catch {
        Write-Host "? 初始化失败: $_" -ForegroundColor Red
        Write-Host "按任意键退出..."
        $null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
        exit 1
    }
    Write-Host ""
    
    # 添加远程仓库
    Write-Host "[3/5] 添加远程仓库..." -ForegroundColor Yellow
    try {
        git remote add origin $repoUrl
        Write-Host "? 远程仓库已添加" -ForegroundColor Green
    } catch {
        # 如果已存在，更新 URL
        git remote set-url origin $repoUrl
        Write-Host "? 远程仓库已更新" -ForegroundColor Green
    }
} else {
    Write-Host "[2/5] Git 仓库已存在" -ForegroundColor Green
    Write-Host "[3/5] 检查远程仓库..." -ForegroundColor Yellow
    
    $remoteExists = git remote | Select-String "origin"
    if (-not $remoteExists) {
        git remote add origin $repoUrl
        Write-Host "? 远程仓库已添加" -ForegroundColor Green
    } else {
        git remote set-url origin $repoUrl
        Write-Host "? 远程仓库已更新" -ForegroundColor Green
    }
}
Write-Host ""

# 添加所有文件并提交
Write-Host "[4/5] 添加文件并提交..." -ForegroundColor Yellow
try {
    git add .
    git commit -m "feat: 完整的IPtools项目及自动构建工作流" -ErrorAction SilentlyContinue
    
    $commitStatus = $?
    if ($commitStatus) {
        Write-Host "? 文件已提交" -ForegroundColor Green
    } else {
        Write-Host "! 没有新的更改需要提交" -ForegroundColor Yellow
    }
} catch {
    Write-Host "! 提交警告（可能没有新文件）: $_" -ForegroundColor Yellow
}
Write-Host ""

# 推送到 GitHub
Write-Host "[5/5] 推送到 GitHub..." -ForegroundColor Yellow
Write-Host ""
Write-Host "======================================" -ForegroundColor Cyan
Write-Host "  重要：接下来会弹出登录窗口" -ForegroundColor Yellow
Write-Host "======================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "请输入您的 GitHub 凭据：" -ForegroundColor White
Write-Host "  用户名: $email" -ForegroundColor Cyan
Write-Host "  密码: (您的 GitHub 密码)" -ForegroundColor Cyan
Write-Host ""
Write-Host "提示：如果多次失败，建议使用 Personal Access Token" -ForegroundColor Gray
Write-Host "      生成地址: https://github.com/settings/tokens" -ForegroundColor Gray
Write-Host ""
Write-Host "按任意键继续..." -ForegroundColor Yellow
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
Write-Host ""

try {
    # 设置主分支为 main
    git branch -M main
    
    # 推送到 GitHub
    git push -u origin main
    
    Write-Host ""
    Write-Host "======================================" -ForegroundColor Green
    Write-Host "     ? 上传成功！" -ForegroundColor Green
    Write-Host "======================================" -ForegroundColor Green
    Write-Host ""
    Write-Host "下一步操作：" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "1. 访问您的仓库：" -ForegroundColor White
    Write-Host "   https://github.com/persist1/IPtools" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "2. 启用 GitHub Actions：" -ForegroundColor White
    Write-Host "   - 点击 'Actions' 标签" -ForegroundColor Gray
    Write-Host "   - 点击 'I understand my workflows, go ahead and enable them'" -ForegroundColor Gray
    Write-Host ""
    Write-Host "3. 查看自动构建：" -ForegroundColor White
    Write-Host "   - 构建需要 10-20 分钟" -ForegroundColor Gray
    Write-Host "   - 完成后可以下载各平台的程序" -ForegroundColor Gray
    Write-Host ""
    Write-Host "4. 创建第一个发布（可选）：" -ForegroundColor White
    Write-Host "   git tag -a v1.0.0 -m `"Release v1.0.0`"" -ForegroundColor Gray
    Write-Host "   git push origin v1.0.0" -ForegroundColor Gray
    Write-Host ""
    
} catch {
    Write-Host ""
    Write-Host "? 推送失败" -ForegroundColor Red
    Write-Host ""
    Write-Host "可能的原因：" -ForegroundColor Yellow
    Write-Host "  1. 用户名或密码错误" -ForegroundColor White
    Write-Host "  2. 网络连接问题" -ForegroundColor White
    Write-Host "  3. 仓库权限问题" -ForegroundColor White
    Write-Host ""
    Write-Host "解决方案：" -ForegroundColor Yellow
    Write-Host "  1. 检查用户名和密码是否正确" -ForegroundColor White
    Write-Host "  2. 使用 Personal Access Token 代替密码：" -ForegroundColor White
    Write-Host "     访问: https://github.com/settings/tokens" -ForegroundColor Cyan
    Write-Host "     生成 token 后，用 token 作为密码" -ForegroundColor White
    Write-Host "  3. 确保网络连接正常" -ForegroundColor White
    Write-Host ""
    Write-Host "错误详情: $_" -ForegroundColor Red
    Write-Host ""
}

Write-Host "按任意键退出..."
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")

