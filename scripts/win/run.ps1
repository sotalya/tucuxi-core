# Entrypoint for Windows PowerShell build scripts.
# Usage: .\scripts\win\run.ps1 <command> [--debug|--release|--coverage]

$ErrorActionPreference = 'Stop'

$script:REPO_ROOT = (Resolve-Path "$PSScriptRoot\..\..").Path

. "$PSScriptRoot\common\common.ps1"
. "$PSScriptRoot\common\cmake.ps1"
. "$PSScriptRoot\common\python.ps1"

$cmd = if ($args.Count -ge 1) { $args[0] } else { "help" }
$remaining = if ($args.Count -ge 2) { $args[1..($args.Count - 1)] } else { @() }

parse_global_opts $remaining

switch ($cmd) {
    { $_ -in "help", "-h", "--help" } {
        Show-Usage
    }
    "build" {
        . "$PSScriptRoot\commands\build.ps1"
    }
    "build-gtest" {
        . "$PSScriptRoot\commands\gtest.ps1"
        cmd_build_gtest
    }
    "run-gtest" {
        . "$PSScriptRoot\commands\gtest.ps1"
        cmd_run_gtest
    }
    "py-env" {
        . "$PSScriptRoot\commands\python_deps.ps1"
        cmd_py_env
    }
    "py-deps" {
        . "$PSScriptRoot\commands\python_deps.ps1"
        cmd_py_deps
    }
    "fuzz" {
        . "$PSScriptRoot\commands\fuzz.ps1"
        cmd_fuzz
    }
    "imatinib" {
        . "$PSScriptRoot\commands\imatinib.ps1"
        cmd_imatinib
    }
    "cb-driver" {
        . "$PSScriptRoot\commands\cb_driver.ps1"
        cmd_cb_driver
    }
    "format" {
        . "$PSScriptRoot\commands\format.ps1"
    }
    "tidy" {
        . "$PSScriptRoot\commands\tidy.ps1"
    }
    "clean" {
        . "$PSScriptRoot\commands\clean.ps1"
    }
    default {
        die "Unknown command: $cmd" 2
    }
}
