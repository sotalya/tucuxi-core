# Run clang-tidy analysis.

Write-Host "==> Running clang-tidy"
require_cmd clang-tidy
require_cmd run-clang-tidy

$reportFile = "$script:BUILD_ROOT\tidy-report.txt"
$buildType = cmake_build_type

Write-Host "==> Ensuring compile_commands.json exists"
cmake -S $script:REPO_ROOT -B $script:MAIN_BUILD_DIR `
    -G Ninja `
    -DCMAKE_BUILD_TYPE=$buildType `
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

Write-Host "==> Running analysis (output -> $reportFile)"
$rawOutput = run-clang-tidy -quiet -p $script:MAIN_BUILD_DIR "$script:REPO_ROOT\src\tucucdss-core" 2>&1
$filtered = $rawOutput |
    Where-Object { $_ -notmatch '^clang-tidy' -and $_ -notmatch 'warnings generated\.$' } |
    ForEach-Object { $_ -replace '\x1B\[[0-9;]*[A-Za-z]', '' }
$filtered | Set-Content -Path $reportFile

Write-Host "==> Tidy complete"
