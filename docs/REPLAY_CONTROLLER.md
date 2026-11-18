# Checkpoint Replay Controller

ReplayController validates a PhysicsShotRecord by recomputing it, retaining solver
snapshots every 120 fixed steps and at the end. SeekStep restores the nearest
preceding checkpoint and recomputes the remainder. SeekTime selects the first
completed fixed step at or after the requested time. SeekEvent uses the event's
step, and SeekFault returns the actual last committed fault checkpoint.

Stored final state/events are never fed into the solver. Invalid seeks/records
leave the current snapshot unchanged. The controller accepts at most one million
steps and bounds initial accumulator debt for one-step replay. This is a pure
C++ data controller, not a UE camera or rendered animation system.

Tests compare every step against live simulation and exercise reverse event
seeks, real first collision, first pot and fault checkpoint. The browser inspector
is separate and reads exported trajectories; both use fixed-step semantics.
