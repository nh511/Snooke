# Standalone Physics Safety

PHY-020 adds finite-state, cloth/pair penetration, linear speed, simulated time,
per-step event budget, cloth-center bounds, pocket depth and solver-error guards.
Defaults are engineering safety envelopes, not measured billiards parameters.

PocketSimulation checks before and after a step. A failed step preserves committed
ball/clock/event state, latches a fault and rejects further steps until Restore.
Its event budget is per step, not the accumulated history. Work is deterministically
bounded; the simulated-time timeout is not a wall-clock scheduling guarantee.

snooker-fault-v2 stores a valid pre-step checkpoint, failure step/time/code,
message, optional rejected-state diagnostic text and embedded configuration.
The loader accepts v1 and rejects malformed/tail data transactionally. NaN/Inf
input diagnostics are not fabricated valid/replayable states. Live service faults
retain their last valid checkpoint and actual loaded parameter bytes.

Run build/Debug/snooker_fault_replay.exe build/physics-eventloop.txt to recompute
a saved service fault in another process. CTest covers speed, timeout and event
budget replay, requiring matching code, step and serialized fault. Full shot
input/log packaging remains REL-004, and multi-ball service integration PHY-021.
