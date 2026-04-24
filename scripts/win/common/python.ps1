# Python venv helpers for Windows PowerShell scripts.
# Dot-source this file:  . "$PSScriptRoot\common\python.ps1"

. "$PSScriptRoot\common.ps1"

$script:VENV_DIR = "$script:REPO_ROOT\.venv"
$script:VENV_PY  = "$script:VENV_DIR\Scripts\python.exe"
$script:VENV_PIP = "$script:VENV_DIR\Scripts\pip.exe"

function ensure_venv {
    require_cmd python

    $result = & python -c "import venv" 2>&1
    if ($LASTEXITCODE -ne 0) {
        die "python venv module is missing. Install Python with the venv package."
    }

    if (-not (Test-Path $script:VENV_PY)) {
        Write-Host "==> Creating venv at $script:VENV_DIR"
        & python -m venv $script:VENV_DIR
    }

    Write-Host "==> Ensuring pip is up to date in venv"
    & $script:VENV_PY -m pip install --upgrade pip setuptools wheel | Out-Null
}
