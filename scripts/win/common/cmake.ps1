# CMake helpers for Windows PowerShell scripts.
# Dot-source this file:  . "$PSScriptRoot\common\cmake.ps1"

. "$PSScriptRoot\common.ps1"

function cmake_build_type {
    switch ($script:CONFIG) {
        "release" { return "Release" }
        default   { return "Debug" }
    }
}

function ensure_cli_built {
    if (Test-Path $script:CDSS_CLI) { return }

    Write-Host "==> CDSS CLI not found, building first..."

    $buildType = cmake_build_type
    $extra = @()

    if ($script:COVERAGE_MODE -eq 1) {
        $buildType = "Debug"
        $extra += "-Dconfig_coverage=ON"
    }

    $nproc = portable_nproc

    cmake -S $script:REPO_ROOT -B $script:MAIN_BUILD_DIR `
        -G Ninja `
        -DCMAKE_BUILD_TYPE=$buildType `
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON @extra
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

    cmake --build $script:MAIN_BUILD_DIR -j $nproc
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
}
