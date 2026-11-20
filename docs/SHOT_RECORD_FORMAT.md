# ShotRecord v1 (COR-005)

## Full Standalone Shot v2 (REP-001)

PhysicsShotRecord uses snooker-shot-v2 and embeds the full loaded PhysicsBundle,
initial/final snapshot v3, cue input, sequential PhysicsEvents, build version and
Settled/fault result. snooker_simulate writes record.shot in binary/LF with full
double precision. Fault results may embed a PhysicsFault record. Version/reference,
cursor, ball-property, event-order and settled-state checks are transactional.
The v1 type below remains the legacy single-ball transport, not full physics replay.
Parsing/round-trip proves transport integrity; REP-002 independently reruns the
record through the shared physics service and compares all events and final state.

Standalone UTF-8 text schema: snooker-shot-v1 build-version, config quoted-CoreConfig, initial quoted-PhysicsSnapshot-v2, input quoted-ShotInput, events count followed by event records, final quoted-PhysicsSnapshot-v2, result token, end. Quoting uses C++ std::quoted: embedded quotes/backslashes are escaped; embedded line breaks are real line breaks. Classic locale and full double precision are required.

Event: completed step, time (s), kind token, ball ID, optional other ball ID (0 for none), contact point XYZ (m), normal XYZ and impulse. Event order, finite fields, ball references and record time bounds are validated. `CollisionLog` remains a lower-level diagnostic format; `PhysicsShotRecord` is the current complete service-level record used by the replay tool.

The full PhysicsBundle v2 is embedded, including parameter version, fixed step, geometry, profiles and safety limits; both snapshots must reference that version/step. Build version is mandatory. Initial/final phases, ID sets, cue profile and pullback limits are checked. Ball states and pocket states cannot be dropped from the record. Input time remains input-clock metadata, not simulation time.

The schema fixture in data/example.shot is NOT a simulated or calibrated shot: zero elapsed time, no events, result schema-fixture. Tests create a real uniform-motion clock/state record, persist/reload it and compare every serialized field. Neither the fixture nor round-trip tests claim collision replay, valid strike dynamics, a settled result or a complete snooker shot. The separate center-entry fixture is input transport with a placeholder result, not a saved simulated final state; the CLI parity test ignores its final block and independently computes real cloth settling. Those remain physics/replay task acceptance.

Maximum input file is 64 MiB and maximum event count is 1,000,000 for resource safety. Unsupported schema/config/snapshot versions, malformed blocks, inconsistent parameters, unknown ball references, reversed event order and trailing data fail without modifying the destination record.
