# Git 自动安装脚本
# 右键点击此文件 -> "使用 PowerShell 运行"

Write-Host "======================================" -ForegroundColor Cyan
Write-Host "       Git 自动安装脚本" -ForegroundColor Cyan
Write-Host "======================================" -ForegroundColor Cyan
Write-Host ""

# 检查是否已安装 Git
$gitInstalled = Get-Command git -ErrorAction SilentlyContinue

if ($gitInstalled) {
    Write-Host "? Git 已经安装!" -ForegroundColor Green
    Write-Host "Git 版本: $(git --version)" -ForegroundColor Green
    Write-Host ""
    Write-Host "按任意键继续..."
    $null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
    exit 0
}

Write-Host "正在下载 Git 安装程序..." -ForegroundColor Yellow
Write-Host ""

# Git 下载地址（使用国内镜像加速）
$gitUrl = "https://registry.npmmirror.com/-/binary/git-for-windows/v2.43.0.windows.1/Git-2.43.0-64-bit.exe"
$installerPath = "$env:TEMP\Git-Setup.exe"

try {
    # 下载 Git 安装程序
    Write-Host "下载地址: $gitUrl" -ForegroundColor Gray
    Invoke-WebRequest -Uri $gitUrl -OutFile $installerPath -UseBasicParsing
    
    Write-Host "? 下载完成!" -ForegroundColor Green
    Write-Host ""
    Write-Host "正在安装 Git..." -ForegroundColor Yellow
    Write-Host "请在安装向导中：" -ForegroundColor Cyan
    Write-Host "  1. 一路点击 'Next'" -ForegroundColor White
    Write-Host "  2. 保持默认选项即可" -ForegroundColor White
    Write-Host "  3. 最后点击 'Install'" -ForegroundColor White
    Write-Host ""
    
    # 启动安装程序
    Start-Process -FilePath $installerPath -Wait
    
    Write-Host ""
    Write-Host "? Git 安装完成!" -ForegroundColor Green
    Write-Host ""
    Write-Host "重要提示:" -ForegroundColor Yellow
    Write-Host "  请关闭当前 PowerShell 窗口" -ForegroundColor White
    Write-Host "  然后重新打开，再运行上传脚本" -ForegroundColor White
    Write-Host ""
    
    # 清理安装程序
    Remove-Item $installerPath -ErrorAction SilentlyContinue
    
} catch {
    Write-Host "? 下载失败: $_" -ForegroundColor Red
    Write-Host ""
    Write-Host "备选方案：手动安装" -ForegroundColor Yellow
    Write-Host "  1. 访问: https://git-scm.com/download/win" -ForegroundColor White
    Write-Host "  2. 下载 64-bit Git for Windows Setup" -ForegroundColor White
    Write-Host "  3. 双击安装，一路 Next" -ForegroundColor White
    Write-Host "  4. 安装完成后，关闭并重新打开 PowerShell" -ForegroundColor White
    Write-Host ""
}

Write-Host "按任意键退出..."
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")

