# PhysicsSnapshot v2 (PHY-003)

Header fields: schema 2, phase (BeforeShot/DuringStep/AfterShot), physical parameter version token, completed step index, simulation time (s), fixed step (s), accumulated unprocessed time (s), ball count. Exactly count ball-state lines follow; only whitespace may follow the last record. Numeric format is classic locale and max_digits10.

V1 is explicitly rejected: it omitted fixed step and frame remainder, so restoring it silently would not reproduce scheduling. Resource count and unsigned fields are bounded/validated before allocation. Clock consistency is checked, along with ball IDs, finite state, material references, sleep timers and contact state. Loader and restore failures leave destination state unchanged.

Capture/Restore transfer all state used by the CURRENT uniform-motion clock and ball data model. Restore requires the caller's actually loaded parameter version to match; this snapshot contains a reference, not the parameter bytes. Later solver state (orientation, cloth transition, collision/pocket/event cursors, random state if introduced) must extend the schema BEFORE it influences results. No complete shot replay or event history is claimed. That remains COR-005/REP tasks.

Tests persist a snapshot with nonzero frame remainder, restore into a new clock/ball collection, continue 600 render frames, then compare serialized ball state and clock state against uninterrupted execution. This is real state restoration, not a prerecorded or mocked physics result.
