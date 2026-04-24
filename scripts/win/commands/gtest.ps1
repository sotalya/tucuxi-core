# Build and/or run gtests.

function cmd_build_gtest {
    $buildType = cmake_build_type
    $nproc = portable_nproc

    Write-Host "==> Building gtests ($script:CONFIG)"
    cmake -S "$script:REPO_ROOT\test\gtest" -B $script:GTEST_BUILD_DIR `
        -DCMAKE_BUILD_TYPE=$buildType
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

    cmake --build $script:GTEST_BUILD_DIR -j $nproc
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
}

function cmd_run_gtest {
    $exe = "$script:GTEST_BUILD_DIR\tucu-cdss-tests.exe"

    Write-Host "==> Running gtests ($script:CONFIG)"
    if (-not (Test-Path $exe)) {
        Write-Host "==> gtest binary not found, building first..."
        cmd_build_gtest
    }

    & $exe
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
}
