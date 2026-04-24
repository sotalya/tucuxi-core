# Run imatinib system tests.

function cmd_imatinib {
    Write-Host "==> Running imatinib system tests ($script:CONFIG)"
    ensure_cli_built

    if ($script:COVERAGE_MODE -eq 1) {
        $outDir = "$script:BUILD_ROOT\system\imatinib\output"
    } else {
        $outDir = "$script:BUILD_ROOT\$script:CONFIG\system\imatinib\output"
    }

    New-Item -ItemType Directory -Path $outDir -Force | Out-Null

    & $script:PYTHON_BIN "$script:REPO_ROOT\test\system\imatinib\imatinib_tests.py" `
        "$script:REPO_ROOT\config.xml" `
        "$script:REPO_ROOT\test\system\imatinib" `
        --cdss-cli $script:CDSS_CLI `
        --drug-files-dir "$script:REPO_ROOT\test\drugfiles" `
        --language-files-dir "$script:REPO_ROOT\language" `
        --template-dir "$script:MAIN_BUILD_DIR\html_template" `
        --template-name "template" `
        --out-dir $outDir
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

    Write-Host "==> Output: $outDir"
}
