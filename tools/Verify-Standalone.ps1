param([string]$CMake, [string]$Run = ('verify-' + [guid]::NewGuid().ToString('N')))
$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent
if (-not $CMake) {
    $command = Get-Command cmake -ErrorAction SilentlyContinue
    if ($command) { $CMake = $command.Source }
    elseif (Test-Path -LiteralPath (Join-Path $root 'build/CMakeCache.txt')) {
        $entry = Select-String -LiteralPath (Join-Path $root 'build/CMakeCache.txt') -Pattern '^CMAKE_COMMAND:INTERNAL='
        $CMake = $entry.Line.Split('=', 2)[1]
    }
}
if (-not $CMake -or -not (Test-Path -LiteralPath $CMake)) { throw 'Pass -CMake with an installed cmake.exe path' }
if ($Run -notmatch '^[a-zA-Z0-9-]+$') { throw 'Run must be a simple directory name' }
$destination = Join-Path $root "build-evidence/$Run"
if (Test-Path -LiteralPath $destination) { throw 'Evidence directory already exists; choose a new run' }
$source = Join-Path $destination 'source'
New-Item -ItemType Directory -Path $source -Force | Out-Null
Get-ChildItem -LiteralPath $root -Force | Where-Object {
    $_.Name -notmatch '^build($|-)' -and $_.Name -notin '.git', '.vs'
} | Copy-Item -Destination $source -Recurse
$build = Join-Path $destination 'binary'
function Invoke-Checked([string]$Program, [string[]]$Arguments, [string]$Log) {
    $previous = $ErrorActionPreference
    $ErrorActionPreference = 'Continue'
    & $Program @Arguments 2>&1 | Tee-Object -FilePath (Join-Path $destination $Log)
    $code = $LASTEXITCODE
    $ErrorActionPreference = $previous
    Add-Content -LiteralPath (Join-Path $destination 'exit-codes.txt') -Value "$Log exit=$code"
    if ($code -ne 0) { throw "$Program failed with exit code $code; see $Log" }
}
Invoke-Checked $CMake @('-S', $source, '-B', $build, '-G', 'Visual Studio 17 2022', '-A', 'x64') 'configure.txt'
foreach ($configuration in 'Debug', 'Release') {
    Invoke-Checked $CMake @('--build', $build, '--config', $configuration) "build-$configuration.txt"
    Invoke-Checked (Join-Path (Split-Path $CMake) 'ctest.exe') @('--test-dir', $build, '-C', $configuration, '--output-on-failure', '--no-tests=error') "test-$configuration.txt"
}
Write-Output "PASS independent source verification: $destination"
