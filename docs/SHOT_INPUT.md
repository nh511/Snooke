# Shot Input and Cue Contract

## CLI Reproduction

From the repository root after building Debug:

    build/Debug/snooker_cue_strike.exe --files data/default.snooker data/validation/stationary.snapshot data/validation/center.input --output build/struck.snapshot
    build/Debug/snooker_single_ball.exe data/default.snooker build/struck.snapshot build/struck.csv build/struck-final.snapshot

Alternative input entry:

    build/Debug/snooker_cue_strike.exe --record data/validation/center-entry.shot --output build/record-struck.snapshot

Both cue entries load the same CoreConfig v2 and call ApplyCueImpulse. The record entry uses only its parameters/initial/input; it does NOT assume its stored final block is a computed result. CTest shot_entry_parity verifies identical struck states, cloth trajectories and final sleeping states. The CLI currently accepts one ball only and requires the stationary pre-shot phase/config/material agreement. The flat-cloth runner still rejects unsupported vertical motion or boundary crossing. These tools are not a full snooker PhysicsService or full ShotRecord replay.

ShotInput (legacy FShotInput alias) stores ball ID, cue profile ID, world-space travel direction, normalized transverse strike point, pullback (m), measured forward speed (m/s), metric transverse tip offset and input timestamp (s). Direction need not be unit length; consumers normalize it with finite-length checks. Inclination is represented by direction.Z, not a hidden screen angle.

StrikePointOnBall is the world-space radial-plane offset divided by ball radius, NOT a full sphere contact position or a screen coordinate. Center strike is (0,0,0). It must be perpendicular to cue travel. CueTipOffsetMeters must equal StrikePointOnBall times the actual cue-ball radius. ValidateFor enforces that relation, matching ball/profile IDs and cue limits. Do not combine two inconsistent offsets by adding them or silently choosing one.

The actual surface lever for the forthcoming impulse map is offset minus direction times sqrt(radius^2-offset^2), i.e. the incoming cue-facing hemisphere. Units and right-hand cross-product signs follow the core convention. A 3D UI must explicitly convert its local ball control coordinates into this contract. Pullback is a validated control limit, not a second multiplier on a measured forward speed. Input timestamp is recording metadata and must not alter physics.

The whitespace input field order remains unchanged, but transverse consistency is now validated before physical use. Standalone input persistence alone is not simulation determinism. PHY-009/010 now include an in-memory/file solver test plus a two-entry CLI parity test that compares struck snapshots, full generated CSV and settled snapshots byte-for-byte. No UI, DataAsset or UE input acceptance is claimed.

## Cue Impulse Baseline (PHY-010)

For stationary homogeneous solid sphere inertia I=2/5 mr^2, translating cue mass M and normalized direction d, contact lever r is computed from the incoming hemisphere. Effective inverse mass is 1/m+1/M+|r cross d|^2/I. Raw impulse J=(1+tipRestitution)*forwardSpeed/effectiveInverseMass; cap J by MaxImpulseNewtonSeconds. Apply delta-v=J*d/m and delta-omega=J*(r cross d)/I. Center strikes produce zero torque. Off-center total energy includes rotation; no table calibration, squirt, swerve, tip compliance or miscue simulation is claimed.

The current solver rejects any moving/spinning target ball; it does not silently discard initial motion. Positive impulse wakes the ball and resets sleep timing, zero speed/zero impulse leaves state untouched. Input pullback is a bound only, not an extra impulse multiplier. Errors are transactional for both ball and diagnostic result. Direction inclination affects initial 3D impulse, but the flat-cloth runner must reject unsupported vertical dynamics. Runtime configuration changes require parameter versions and full cue fields in records.
