# Common variables and helpers for Windows PowerShell scripts.
# Dot-source this file:  . "$PSScriptRoot\common\common.ps1"

$ErrorActionPreference = 'Stop'

if (-not $script:REPO_ROOT) {
    throw "REPO_ROOT must be set by entrypoint"
}

# Defaults
if (-not $script:CONFIG)        { $script:CONFIG = "debug" }
if (-not $script:COVERAGE_MODE) { $script:COVERAGE_MODE = 0 }
if (-not $script:BUILD_ROOT)    { $script:BUILD_ROOT = "$script:REPO_ROOT\build_win" }
if (-not $script:PYTHON_BIN)    { $script:PYTHON_BIN = "python" }

$script:SCRIPT_DIR = "$script:REPO_ROOT\scripts\win"

function die([string]$msg, [int]$code = 1) {
    Write-Error "==> ERROR: $msg"
    exit $code
}

function require_cmd([string]$name) {
    if (-not (Get-Command $name -ErrorAction SilentlyContinue)) {
        die "missing command '$name' (install it and retry)"
    }
}

function portable_nproc {
    $env:NUMBER_OF_PROCESSORS
    if (-not $env:NUMBER_OF_PROCESSORS) { return 1 }
}

function Show-Usage {
    Write-Host @"
Usage:
  scripts\win\run.ps1 <command> [-debug] [-release] [-coverage]

By default, commands build in debug mode. Use -release for release mode,
or -coverage to build with coverage flags (implies debug).

Commands:
  build           Configure + build main project
  build-gtest     Configure + build gtests
  run-gtest       Run gtests
  format          Run clang-format on src
  tidy            Run clang-tidy
  clean           Remove build dir

  py-env          Print activation command for .venv
  py-deps         Install python deps into .venv
  fuzz            Run fuzzing
  imatinib        Run imatinib system tests
  cb-driver       Run covariate boundaries driver

  help
"@
}

function recompute_paths {
    if ($script:COVERAGE_MODE -eq 1) {
        $script:BUILD_ROOT     = "$script:REPO_ROOT\build_win\coverage"
        $script:MAIN_BUILD_DIR = "$script:BUILD_ROOT\main"
        $script:GTEST_BUILD_DIR = "$script:BUILD_ROOT\gtest"
    } else {
        $script:BUILD_ROOT     = "$script:REPO_ROOT\build_win"
        $script:MAIN_BUILD_DIR = "$script:BUILD_ROOT\$script:CONFIG\main"
        $script:GTEST_BUILD_DIR = "$script:BUILD_ROOT\$script:CONFIG\gtest"
    }
    $script:CDSS_CLI = "$script:MAIN_BUILD_DIR\tucuxi_cdss_cli.exe"
}

function parse_global_opts([string[]]$opts) {
    foreach ($opt in $opts) {
        switch ($opt) {
            '--debug'    { $script:CONFIG = "debug" }
            '--release'  { $script:CONFIG = "release" }
            '--coverage' { $script:COVERAGE_MODE = 1 }
        }
    }
    recompute_paths
}
