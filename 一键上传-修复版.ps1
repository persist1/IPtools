# IPtools Upload to GitHub (Fixed Version)
# Right-click this file -> "Run with PowerShell"

Write-Host "======================================" -ForegroundColor Cyan
Write-Host "   IPtools Upload to GitHub" -ForegroundColor Cyan
Write-Host "======================================" -ForegroundColor Cyan
Write-Host ""

# Check Git installation
$gitInstalled = Get-Command git -ErrorAction SilentlyContinue
if (-not $gitInstalled) {
    Write-Host "X Error: Git not installed!" -ForegroundColor Red
    Write-Host ""
    Write-Host "Solution:" -ForegroundColor Yellow
    Write-Host "  1. Run 'install-git.ps1' to install Git" -ForegroundColor White
    Write-Host "  2. Close and reopen PowerShell" -ForegroundColor White
    Write-Host "  3. Run this script again" -ForegroundColor White
    Write-Host ""
    Write-Host "Press any key to exit..."
    $null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
    exit 1
}

Write-Host "OK Git installed: $(git --version)" -ForegroundColor Green
Write-Host ""

# Project configuration
$repoUrl = "https://github.com/persist1/IPtools.git"
$username = "persist1"
$email = "317627074@qq.com"

Write-Host "Project Info:" -ForegroundColor Cyan
Write-Host "  Repository: $repoUrl" -ForegroundColor White
Write-Host "  Username: $username" -ForegroundColor White
Write-Host ""

# Configure Git user info
Write-Host "[1/6] Configuring Git user info..." -ForegroundColor Yellow
git config user.name "$username" 2>$null
git config user.email "$email" 2>$null
Write-Host "OK Configuration completed" -ForegroundColor Green
Write-Host ""

# Check if already initialized
$isGitRepo = Test-Path ".git"

if (-not $isGitRepo) {
    Write-Host "[2/6] Initializing Git repository..." -ForegroundColor Yellow
    git init 2>$null
    Write-Host "OK Initialized" -ForegroundColor Green
} else {
    Write-Host "[2/6] Git repository exists" -ForegroundColor Green
}
Write-Host ""

# Add remote repository
Write-Host "[3/6] Configuring remote repository..." -ForegroundColor Yellow
git remote remove origin 2>$null
git remote add origin $repoUrl 2>$null
Write-Host "OK Remote configured" -ForegroundColor Green
Write-Host ""

# Add all files
Write-Host "[4/6] Adding files..." -ForegroundColor Yellow
git add . 2>$null
Write-Host "OK Files added" -ForegroundColor Green
Write-Host ""

# Commit (using English message to avoid encoding issues)
Write-Host "[5/6] Committing changes..." -ForegroundColor Yellow
git commit -m "Initial commit: IPtools project with GitHub Actions workflows" 2>$null

if ($LASTEXITCODE -eq 0) {
    Write-Host "OK Changes committed" -ForegroundColor Green
} else {
    Write-Host "! No new changes to commit (or already committed)" -ForegroundColor Yellow
    
    # Check if we have any commits at all
    $hasCommits = git rev-parse HEAD 2>$null
    if (-not $hasCommits) {
        Write-Host ""
        Write-Host "Creating initial commit..." -ForegroundColor Yellow
        git commit --allow-empty -m "Initial commit" 2>$null
        Write-Host "OK Initial commit created" -ForegroundColor Green
    }
}
Write-Host ""

# Push to GitHub
Write-Host "[6/6] Pushing to GitHub..." -ForegroundColor Yellow
Write-Host ""
Write-Host "======================================" -ForegroundColor Cyan
Write-Host "  Login Window Will Appear" -ForegroundColor Yellow
Write-Host "======================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Please enter your GitHub credentials:" -ForegroundColor White
Write-Host "  Username: $email" -ForegroundColor Cyan
Write-Host "  Password: Xbb5241625" -ForegroundColor Cyan
Write-Host ""
Write-Host "Note: If login fails multiple times," -ForegroundColor Gray
Write-Host "      use Personal Access Token instead" -ForegroundColor Gray
Write-Host "      Generate at: https://github.com/settings/tokens" -ForegroundColor Gray
Write-Host ""
Write-Host "Press any key to continue..." -ForegroundColor Yellow
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
Write-Host ""

# Set main branch
git branch -M main 2>$null

# Push to GitHub
git push -u origin main 2>&1

if ($LASTEXITCODE -eq 0) {
    Write-Host ""
    Write-Host "======================================" -ForegroundColor Green
    Write-Host "     OK Upload Successful!" -ForegroundColor Green
    Write-Host "======================================" -ForegroundColor Green
    Write-Host ""
    Write-Host "Next Steps:" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "1. Visit your repository:" -ForegroundColor White
    Write-Host "   https://github.com/persist1/IPtools" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "2. Enable GitHub Actions:" -ForegroundColor White
    Write-Host "   - Click 'Actions' tab" -ForegroundColor Gray
    Write-Host "   - Click 'I understand my workflows, go ahead and enable them'" -ForegroundColor Gray
    Write-Host ""
    Write-Host "3. View automatic build:" -ForegroundColor White
    Write-Host "   - Build takes 10-20 minutes" -ForegroundColor Gray
    Write-Host "   - Download builds for all platforms when complete" -ForegroundColor Gray
    Write-Host ""
    Write-Host "4. Create first release (optional):" -ForegroundColor White
    Write-Host "   git tag -a v1.0.0 -m `"Release v1.0.0`"" -ForegroundColor Gray
    Write-Host "   git push origin v1.0.0" -ForegroundColor Gray
    Write-Host ""
} else {
    Write-Host ""
    Write-Host "X Push Failed" -ForegroundColor Red
    Write-Host ""
    Write-Host "Possible reasons:" -ForegroundColor Yellow
    Write-Host "  1. Incorrect username or password" -ForegroundColor White
    Write-Host "  2. Network connection issue" -ForegroundColor White
    Write-Host "  3. Repository permission issue" -ForegroundColor White
    Write-Host ""
    Write-Host "Solutions:" -ForegroundColor Yellow
    Write-Host "  1. Check username and password" -ForegroundColor White
    Write-Host "  2. Use Personal Access Token instead of password:" -ForegroundColor White
    Write-Host "     Visit: https://github.com/settings/tokens" -ForegroundColor Cyan
    Write-Host "     Generate token and use it as password" -ForegroundColor White
    Write-Host "  3. Check network connection" -ForegroundColor White
    Write-Host ""
}

Write-Host "Press any key to exit..."
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")

