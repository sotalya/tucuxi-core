# Run covariate boundaries driver system tests.

function cmd_cb_driver {
    Write-Host "==> Running cb-driver ($script:CONFIG)"
    ensure_cli_built

    if ($script:COVERAGE_MODE -eq 1) {
        $outBase = "$script:BUILD_ROOT\cb_driver"
    } else {
        $outBase = "$script:BUILD_ROOT\$script:CONFIG\cb_driver"
    }

    New-Item -ItemType Directory -Path $outBase -Force | Out-Null

    & $script:PYTHON_BIN "$script:REPO_ROOT\test\covariate_boundaries_driver\cb_driver.py" `
        "$script:REPO_ROOT\config.xml" `
        "$script:REPO_ROOT\test\covariate_boundaries_driver\imatinib_template.tqf" `
        --cdss-cli "$script:MAIN_BUILD_DIR\tucuxi_cdss_cli.exe" `
        --drug-files-dir "$script:REPO_ROOT\test\drugfiles" `
        --language-files-dir "$script:REPO_ROOT\language" `
        --html-templates-dir "$script:MAIN_BUILD_DIR\html_template" `
        --template-name "template" `
        --tqf-dir "$outBase\tqf" `
        --out-dir "$outBase\out" `
        > "$outBase\run.log" 2>&1
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

    Write-Host "==> Output: $outBase\out"
}
