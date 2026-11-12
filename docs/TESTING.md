# Automated Test Entry

Run tools/Test-Current.ps1 via powershell -NoProfile -ExecutionPolicy Bypass -File with a unique -Run name. Exit 0 means configure/build and CTest passed for both Debug and Release. Every task retains separate logs under build-evidence; no failures are overwritten. Both scripts reject reuse of evidence directory names, record stage exit codes and use --no-tests=error. Use -CMake with an absolute executable path if CMake is not on PATH. No existing build/cache is required when -CMake is supplied.

snooker_test_units --expect-centimeters 99 deliberately checks the wrong expectation against the real conversion function and must exit 1. CTest UnitsFailureExit uses WILL_FAIL only for this harness contract test. The bad-snapshot CLI test likewise expects rejection of a malformed resource count. No physics success test is skipped or inverted. Production assertions do not use assert(), so Release does not disable them.

UE5 Automation (not run, requires an actual editor and project):

UnrealEditor-Cmd.exe UnrealProject/Snooker.uproject -unattended -nop4 -NullRHI -ExecCmds="Automation RunTests Snooker; Quit" -TestExit="Automation Test Queue Empty" -ReportExportPath=<absolute-report-directory>

The UE runner must also inspect the exported test report for failures, zero tests and crashes; editor process exit alone is not acceptance. No UE tests currently exist, so this invocation is a reserved interface, not evidence of passing UE tests.

Portable logger sinks execute synchronously. A future UE sink maps the six categories and severity to UE_LOG only in the adapter, not by including UE headers in the core. That forwarding is not yet compiled or verified.
