# Run fuzzing system tests.

function cmd_fuzz {
    Write-Host "==> Running fuzzing ($script:CONFIG)"
    ensure_cli_built

    $runId = Get-Date -Format "yyyyMMdd-HHmmss"
    if ($script:COVERAGE_MODE -eq 1) {
        $outDir = "$script:BUILD_ROOT\fuzzing\$runId"
    } else {
        $outDir = "$script:BUILD_ROOT\$script:CONFIG\fuzzing\$runId"
    }

    New-Item -ItemType Directory -Path $outDir -Force | Out-Null

    Push-Location "$script:REPO_ROOT\test\fuzzing"
    try {
        & $script:PYTHON_BIN main.py `
            --cdss_cli_path $script:CDSS_CLI `
            --drug_files_dir "$script:REPO_ROOT\test\drugfiles" `
            --language_files_dir "$script:REPO_ROOT\language" `
            --template_dir "$script:MAIN_BUILD_DIR\html_template" `
            --template_name "template" `
            --out_dir_path "$outDir\output" `
            --logfile_name "000_LOG.txt"
        if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    } finally {
        Pop-Location
    }

    Write-Host "==> Output: $outDir\output"
}
