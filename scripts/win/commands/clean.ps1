# Clean the build directory.

Write-Host "==> Cleaning build directory"
if (Test-Path $script:BUILD_ROOT) {
    Remove-Item -Recurse -Force $script:BUILD_ROOT
}
Write-Host "==> Done."
