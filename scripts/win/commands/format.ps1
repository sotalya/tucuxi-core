# Run clang-format on source files.

Write-Host "==> Running clang-format"
require_cmd clang-format

Get-ChildItem -Path "$script:REPO_ROOT\src" -Recurse -Include *.cpp, *.hpp, *.h | ForEach-Object {
    clang-format -i $_.FullName
}

Write-Host "==> Format complete"
