# Single-Ball Trajectory Inspection

Run from the repository root after building Debug:

    build/Debug/snooker_single_ball.exe data/default.snooker data/validation/single-ball.snapshot build/trajectory.csv build/final.snapshot
    node tests/trajectory-data.cjs build/trajectory.csv

Open tools/trajectory.html directly in a browser; use its file control to load build/trajectory.csv. No server, network dependency or fabricated animation is required. The canvas, time slider, step controls, play/pause and numeric linear/angular state read the imported CSV only. Data remains local. The viewer expects the exact CSV schema emitted by snooker_single_ball and limits file size and row count.

Current engineering fixture settles at 18.25 s after 4380 steps. This is output from the actual flat-cloth solver, not a measured physical baseline or a cue strike. The runner starts from a supplied snapshot. snooker_cue_strike can now produce that moving snapshot from a stationary ball and recorded cue input. More than one ball, unsupported vertical motion, configuration mismatch, boundary contact and a 120 s simulation timeout fail explicitly. Failure may leave a partial CSV for diagnosis; it must NOT be presented as a successful trajectory. Final snapshot is written only on settling success.

The runtime currently uses default ClothEnvironment gravity 9.80665 m/s^2. The CSV's parameter version references its config; it does not yet embed all environment values or rich event history. Sleep displacement tolerance is derived as configured sleep linear speed times fixed step; the standalone ClothProfile type also supports an explicit position threshold. Full parameter unification remains necessary before replay acceptance.

Automated checks: node tools/verify-viewer.cjs launches local headless Chrome with a separate profile and records screenshots/pixel/interaction results under build-evidence/viewer-<timestamp>. It does not close unrelated user browser processes. This is native Chrome/CDP verification, not Playwright; no Playwright dependency is installed.

COR-006 remains REVIEW: no cushion/pocket geometry overlay, collision points, pocket events or complete shot event timeline has been implemented. VAL-003 is also not complete: this CSV is diagnostic single-ball output, not full shot snapshots/parameters/collision-event export.
