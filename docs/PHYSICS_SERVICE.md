# Unified Standalone Physics Service

`ISnookerPhysicsService` exposes BeginShot, Step, IsSettled, GetEvents,
SaveSnapshot, RestoreSnapshot and GetFault. The factory accepts a versioned
PhysicsBundle (currently shared with the single-ball laboratory config).
The core contains no Unreal headers. No UE build has been performed.

## Scheduling

The service uses fixed steps and configured gravity/cloth substeps. Within each
substep it advances every active ball only to the earliest event, applies the
response, and queries again. Canonical ball ID order and event-type/ID tie breaks
resolve equal-time events. Stationary pairs do not generate collision impulses.
An analytic finite-segment/cap sweep prevents high-speed rail tunneling.

This is a cloth-constrained engineering model: rails and jaws are planar walls;
the existing cushion friction/response is evaluated on a planar contact contour.
Cloth support absorbs vertical collision impulses (no jump shots). After mouth
crossing, gravity and existing finite 3D pocket geometry determine descent and
bottom contact. Potted balls stop participating and emit one Potted event.
Gravity uses piecewise constant-velocity substeps, not accelerated exact CCD.

The six-pocket fixture now has six rails. RequireClosedBoundary connects rail
ends to outer jaw ends, adds virtual mouth edges for topology only, and validates
a single non-self-intersecting contour. Virtual mouth edges never bounce a ball.
Missing/disconnected rails are errors, not filled by unbounded synthetic walls.
Physics bundle v2 persists this option; v1 remains readable but is not accepted
for full-table simulation unless explicitly upgraded and geometrically validated.

Faults are not appended as `PhysicsEventKind` values. A failed step latches
`PhysicsRunState::Faulted` and exposes the structured `PhysicsFault` through
`GetFault()`; the fault package and `fault.txt` preserve the replay checkpoint.

## State and Failure

Step takes elapsed frame seconds, retains bounded accumulator debt, and processes
at most 1024 fixed steps per call. Invalid frame input leaves the state unchanged.
On a physical fault it latches Faulted and retains the last completed fixed-step
state. IsSettled means every unpotted ball is asleep and no ball is descending.
At settling, render-time remainder is discarded: physics termination and final
snapshot are independent of how much time the render frame overran the last step.

Snapshot v3 stores paired ball/pocket states, event cursor and clock remainder.
Restore clears delivered history but retains sequence IDs; callers own already
delivered events. Repeating Step after AfterShot produces no new events.
Same loaded parameter bytes/version and solver build are required for replay.

## CLI and Tests

`snooker_simulate bundle snapshot shot-input output-directory` uses only the public
service. It writes `record.shot`, `result.json`, `viewer.json`, `trajectory.csv`,
`events.txt`, `final.snapshot` and `debug.json` for settled runs, or `fault.txt`
for faulted runs. It refuses to overwrite prior successful result files. The
record is the current `snooker-shot-v2` format and is independently replayed by
`snooker_replay`.

Tests cover FPS/event equality, irregular frame times, high-speed chain impacts
within one step, two rails with spin, once-only pocket capture, canonical snapshot
continuation, 22-ball opening with rail/ball events and a 22-ball training pot.
The CLI must match the API's final snapshot byte-for-byte. These fixtures are
engineering regressions, not empirical calibration or cross-platform guarantees.
