@echo off
chcp 65001 >nul
echo ======================================
echo    启动 GitHub 自动化助手
echo ======================================
echo.

REM 检查Python是否安装
python --version >nul 2>&1
if %errorlevel% neq 0 (
    echo [错误] Python 未安装!
    echo.
    echo 请访问 https://www.python.org/downloads/ 下载安装 Python
    echo.
    pause
    exit /b 1
)

echo [OK] Python 已安装
echo.
echo 正在启动工具...
echo.

REM 启动Python工具
python GitHubHelper.py

if %errorlevel% neq 0 (
    echo.
    echo [错误] 工具启动失败
    pause
)

