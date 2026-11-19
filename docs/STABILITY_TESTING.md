# Standalone Stability Testing

`snooker_stability bundle shots seed report.json` alternates fixed center shots
and seeded random legal direction/speed shots, with periodic rail-directed shots.
Each uses the public service to settle, rejects any fault/non-settling result and
hashes final snapshots. The report records elapsed milliseconds, working-set
start/end/growth, maximum steps/events and fault count.

CTest runs 50 shots in Debug and Release and parses JSON with CMake. The release
qualification run uses 2000 shots and requires memory growth <=10 MiB. Results
are toolchain/fixture regressions, not a production frame-rate benchmark or proof
of zero leaks under every game workflow.
