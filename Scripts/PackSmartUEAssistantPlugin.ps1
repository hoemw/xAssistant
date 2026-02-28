param(
    [string]$Version,
    [string]$OutputRoot = "dist",
    [switch]$VerboseLog
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

# 路径解析
$RepoRoot = Split-Path -Parent $PSCommandPath
$PluginRoot = Join-Path $RepoRoot "Plugins/SmartUEAssistant"
if (!(Test-Path $PluginRoot)) {
    throw "未找到插件目录: $PluginRoot"
}

# 若未显式指定版本号，则从 .uplugin 中读取 VersionName（优先）或 Version（回退），最终再回退到时间戳
if (-not $Version -or [string]::IsNullOrWhiteSpace($Version)) {
    $UpluginPath = Join-Path $PluginRoot "SmartUEAssistant.uplugin"
    if (!(Test-Path $UpluginPath)) { throw "未找到 uplugin 文件: $UpluginPath" }
    try {
        $uplugin = Get-Content -Path $UpluginPath -Raw | ConvertFrom-Json
        if ($uplugin.PSObject.Properties.Name -contains 'VersionName' -and $uplugin.VersionName) {
            $Version = [string]$uplugin.VersionName
            Write-Host "[info] 使用 uplugin VersionName: $Version" -ForegroundColor DarkGray
        }
        elseif ($uplugin.PSObject.Properties.Name -contains 'Version' -and $uplugin.Version) {
            $Version = [string]$uplugin.Version
            Write-Host "[info] 使用 uplugin Version: $Version" -ForegroundColor DarkGray
        }
        else {
            $Version = (Get-Date -Format "yyyyMMdd-HHmm")
            Write-Host "[info] uplugin 未包含版本字段，改用时间戳: $Version" -ForegroundColor DarkGray
        }
    } catch {
        throw "读取 uplugin 版本失败: $($_.Exception.Message)"
    }
}
# 统一清洗版本字符串，避免非法文件名字符
$Version = ($Version | Out-String).Trim()
$Version = $Version -replace '[^0-9A-Za-z\._-]','_'

# 目录规划：
# staging 目录包含 SmartUEAssistant 顶层目录，最终 zip 根目录即 SmartUEAssistant/
$StagingRoot = Join-Path $RepoRoot (Join-Path $OutputRoot "staging")
$StagingBundle = Join-Path $StagingRoot ("SmartUEAssistant_" + $Version)
$StagingPluginDir = Join-Path $StagingBundle "SmartUEAssistant"
$ZipRoot = Join-Path $RepoRoot $OutputRoot
$ZipPath = Join-Path $ZipRoot ("SmartUEAssistant_" + $Version + ".zip")

# 清理旧目录与文件
if (Test-Path $StagingBundle) { Remove-Item -Recurse -Force $StagingBundle }
if (!(Test-Path $StagingBundle)) { New-Item -ItemType Directory -Force -Path $StagingPluginDir | Out-Null } else { New-Item -ItemType Directory -Force -Path $StagingPluginDir | Out-Null }
if (!(Test-Path $ZipRoot)) { New-Item -ItemType Directory -Force -Path $ZipRoot | Out-Null }
if (Test-Path $ZipPath) { Remove-Item -Force $ZipPath }

Write-Host "[1/3] 复制插件文件到 staging..." -ForegroundColor Cyan
# 使用 robocopy 更稳健地复制，并排除构建临时产物
$null = & robocopy `
    $PluginRoot `
    $StagingPluginDir `
    *.* /E /NFL /NDL /NJH /NJS /NP `
    /XD ".git" "Intermediate" "Saved" ".vs"  `
    /XF "*.obj" "*.pdb" "*.ipdb" "*.iobj" "*.tmp"
$rc = $LASTEXITCODE
# robocopy 返回码：0/1 为成功，>1 视为有问题
if ($rc -gt 3) { throw "robocopy 失败，返回码: $rc" }

# 强制验证 staging 中包含 Config 目录
$ConfigPath = Join-Path $StagingPluginDir "Config"
if (!(Test-Path $ConfigPath)) {
    throw "校验失败：staging 中未发现 Config 目录 => $ConfigPath"
}
Write-Host "[check] 已包含 Config: $ConfigPath" -ForegroundColor Green

Write-Host "[2/3] 生成 ZIP 包..." -ForegroundColor Cyan
Add-Type -AssemblyName System.IO.Compression.FileSystem
# 压缩 SmartUEAssistant_<ver> 目录，使 zip 根目录包含 SmartUEAssistant/
[System.IO.Compression.ZipFile]::CreateFromDirectory($StagingBundle, $ZipPath)

# 读取 zip 并验证是否包含 Config 条目
Write-Host "[3/3] 验证 ZIP 内容..." -ForegroundColor Cyan
$zip = [System.IO.Compression.ZipFile]::OpenRead($ZipPath)
try {
    $hasConfig = $false
    foreach ($entry in $zip.Entries) {
        if ($entry.FullName -match "^SmartUEAssistant/Config/" -or $entry.FullName -match "^SmartUEAssistant\\Config\\") {
            $hasConfig = $true
            break
        }
    }
    if (-not $hasConfig) {
        throw "校验失败：ZIP 内未发现 SmartUEAssistant/Config/* 条目"
    }
}
finally {
    $zip.Dispose()
}

Write-Host "打包完成：$ZipPath" -ForegroundColor Green
if ($VerboseLog) {
    Write-Host "ZIP 内容预览 (前50项)：" -ForegroundColor DarkCyan
    $zipR = [System.IO.Compression.ZipFile]::OpenRead($ZipPath)
    try {
        $zipR.Entries | Select-Object -First 50 | ForEach-Object { $_.FullName }
    } finally {
        $zipR.Dispose()
    }
}

exit 0