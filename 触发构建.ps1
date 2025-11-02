# Trigger GitHub Actions Build
# This will make a small change to trigger the first build

Write-Host "======================================" -ForegroundColor Cyan
Write-Host "   Trigger GitHub Actions Build" -ForegroundColor Cyan
Write-Host "======================================" -ForegroundColor Cyan
Write-Host ""

# Create a small change to trigger build
$readmeContent = Get-Content README.md -Encoding UTF8
$readmeContent += ""
$readmeContent += "<!-- Build triggered: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss') -->"
$readmeContent | Set-Content README.md -Encoding UTF8

Write-Host "Making a small change to trigger build..." -ForegroundColor Yellow
git add README.md
git commit -m "chore: trigger initial GitHub Actions build"
git push origin main

if ($LASTEXITCODE -eq 0) {
    Write-Host ""
    Write-Host "OK Build triggered successfully!" -ForegroundColor Green
    Write-Host ""
    Write-Host "Next:" -ForegroundColor Cyan
    Write-Host "  1. Visit: https://github.com/persist1/IPtools/actions" -ForegroundColor White
    Write-Host "  2. You should see 'Build Multi-Platform' workflow running" -ForegroundColor White
    Write-Host "  3. Wait 10-20 minutes for completion" -ForegroundColor White
    Write-Host "  4. Download build artifacts when done" -ForegroundColor White
    Write-Host ""
    
    # Open Actions page
    Start-Process "https://github.com/persist1/IPtools/actions"
    
} else {
    Write-Host "X Failed to push" -ForegroundColor Red
}

Write-Host "Press any key to exit..."
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")

