# Python venv and dependency management.

function cmd_py_env {
    ensure_venv
    Write-Host "$script:VENV_DIR\Scripts\Activate.ps1"
}

function cmd_py_deps {
    Write-Host "==> Installing python deps into .venv"
    ensure_venv

    & $script:VENV_PIP install -r "$script:REPO_ROOT\test\fuzzing\requirements.txt"
    & $script:VENV_PIP install -r "$script:REPO_ROOT\test\covariate_boundaries_driver\requirements.txt"
    & $script:VENV_PIP install -r "$script:REPO_ROOT\test\system\imatinib\requirements.txt"

    Write-Host "==> Done."
    Write-Host "==> To activate in your shell:"
    Write-Host "   $script:VENV_DIR\Scripts\Activate.ps1"
}
