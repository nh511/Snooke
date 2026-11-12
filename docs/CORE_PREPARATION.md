# Standalone Core Implementation Status

Authority: DEVELOPMENT_PLAN.md and EXECUTABLE_TASKS.md. Task-by-task acceptance and evidence are in TASK_STATUS.md; this page is not a completion declaration.

## Implemented

- C++17 PhysicsCore and RulesCore, separate Tests and Tools build targets.
- Public math types in PhysicsCore/include/Snooker/Math: double vectors, quaternion and transform; SI and explicit UE basis conversion contract.
- BallDefinition with legacy FBallDefinition alias; runtime ball state, snapshot and shot-input data models.
- Invariant-locale text serialization, bounded snapshot loading, strict unsigned parsing and transactional failure behavior.
- Portable categorized logging with severity filters, shot correlation and synchronous stream/file sinks.
- Versioned core config v2 plus independent cushion/pocket/cue profiles and table-description fixtures; engineering examples only, not measured calibration. Cue speed/offset/restitution fields persist in config and embedded records; old config v1 is explicitly rejected.
- snooker_validation: shared config loading and empty-state/math checks. snooker_single_ball loads configuration and a moving snapshot, runs flat-cloth sliding/rolling/spin/sleep steps, exports CSV and saves the final snapshot.
- PhysicsClock, snapshot v2 clock restoration and ShotRecord transport schema/file persistence; full collision replay remains pending.
- Stationary-ball cue impulse with profile limits, file/record CLI entry and byte-identical settled trajectory parity.
- Swept ball-pair candidates, continuous sphere TOI, pair collision response with spin, rich ordered collision log and touching query. These are standalone functions, not a global multi-ball service.
- Local CSV trajectory canvas, timeline and strict parser; Chrome desktop/mobile interaction verification uses real generated data.
- Parameter-driven finite table geometry, explicit pocket-mouth/jaw/floor descriptions and SVG export from real generated primitives.
- Finite current-position rail contacts and local cushion impulse response, with side-spin and saved one/two-rail response reconstruction.
- Deterministic 22-ball opening/training/custom placement, occupied color reset errors, persisted MatchState, frame opening and read-only BallOn query.
- UE project, plugin and module descriptors/startup skeleton. These have NOT been built in UE and do not yet compile the shared core into the plugin.

## Reproduce

Run from repository root (CMake executable must be installed):

    cmake -S . -B build -G "Visual Studio 17 2022" -A x64
    cmake --build build --config Debug
    ctest --test-dir build -C Debug --output-on-failure --no-tests=error
    cmake --build build --config Release
    ctest --test-dir build -C Release --output-on-failure --no-tests=error

For independent source-copy verification use tools/Verify-Standalone.ps1 with -CMake and a unique -Run name, via a process-only execution policy as documented in TESTING.md. Source copies, build/test output and exit codes stay in build-evidence; they are not Git commits or clone-recovery evidence.

## Not Implemented / Not Verified

No chronological full-table multi-ball physics service, continuous cushion/pocket collision solver, pocket state machine, complete shot replay, full scoring/foul rules or playable game. Local pair/cushion responses, opening/state data and cue impulse functions are not a complete match simulator. The single-ball runner is limited to flat cloth with an explicit boundary-failure guard. Existing ShotRecord persistence is not simulated replay; the CSV viewer has no collision/pocket event display. No UE editor launch, generated assets, DataAsset import, plugin core integration or UE Automation/Development/DebugGame build. These tests do not complete M0/M1 or MVP.

Task status distinguishes independent verification from outstanding UE acceptance; partial mixed tasks remain REVIEW or BLOCKED. No task count is inferred from the number of passing tests.
