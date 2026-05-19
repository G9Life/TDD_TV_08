# Regenerate Golden Master expected files under test/golden/
param(
    [string]$BuildDir = "build"
)

$ErrorActionPreference = "Stop"

if (-not (Test-Path $BuildDir)) {
    cmake -S . -B $BuildDir
    cmake --build $BuildDir
}

$env:TV_UPDATE_GOLDEN = "1"
ctest --test-dir $BuildDir -R TVControllerGoldenTest --output-on-failure
Remove-Item Env:TV_UPDATE_GOLDEN

Write-Host "Golden files updated in test/golden/"
