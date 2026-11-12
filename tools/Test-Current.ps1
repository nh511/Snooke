param([string]$Run, [string]$CMake)
$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent
if ($Run -notmatch '^[a-zA-Z0-9-]+$') { throw 'A simple unique Run name is required' }
$evidence = Join-Path $root "build-evidence/$Run"
if (Test-Path -LiteralPath $evidence) { throw 'Evidence already exists' }
New-Item -ItemType Directory -Path $evidence -Force | Out-Null
if (-not $CMake) {
    $command = Get-Command cmake -ErrorAction SilentlyContinue
    if ($command) { $CMake = $command.Source }
    elseif (Test-Path -LiteralPath (Join-Path $root 'build/CMakeCache.txt')) {
        $CMake = (Select-String -LiteralPath (Join-Path $root 'build/CMakeCache.txt') -Pattern '^CMAKE_COMMAND:INTERNAL=').Line.Split('=',2)[1]
    }
}
if (-not $CMake -or -not (Test-Path -LiteralPath $CMake)) { throw 'Pass -CMake with an installed cmake.exe path' }
function Run-Checked([string]$program, [string[]]$arguments, [string]$log) {
    $previous = $ErrorActionPreference
    $ErrorActionPreference = 'Continue'
    & $program @arguments 2>&1 | Tee-Object -FilePath (Join-Path $evidence $log)
    $code = $LASTEXITCODE
    $ErrorActionPreference = $previous
    Add-Content -LiteralPath (Join-Path $evidence 'exit-codes.txt') -Value "$log exit=$code"
    if ($code -ne 0) { throw "Exit $code; see $log" }
}
Run-Checked $cmake @('-S', $root, '-B', (Join-Path $root 'build'), '-G', 'Visual Studio 17 2022', '-A', 'x64') 'configure.txt'
foreach ($configuration in 'Debug','Release') {
    Run-Checked $cmake @('--build', (Join-Path $root 'build'), '--config', $configuration) "build-$configuration.txt"
    Run-Checked (Join-Path (Split-Path $cmake) 'ctest.exe') @('--test-dir', (Join-Path $root 'build'), '-C', $configuration, '--output-on-failure', '--no-tests=error') "test-$configuration.txt"
}
