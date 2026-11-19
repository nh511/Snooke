# Network Synchronization Design

## Scope and Authority

The first network model is shot-atomic, server authoritative and uses the same
PhysicsCore/RulesCore on every peer. The server owns MatchState, legal input,
shot start, rule result, respots and final snapshot. Clients may simulate locally
for presentation but never commit scores, fouls, turns or ball positions.

This is a design contract, not an implemented socket transport. Online play does
not block the offline MVP.

## Connection Handshake and Version Rejection

Before joining, peers exchange and compare:

- game build version;
- standalone solver build (`standalone-solver-v1` or successor);
- exact PhysicsBundle bytes plus SHA-256;
- RulesProfile bytes plus SHA-256;
- snapshot schema (currently v3);
- shot record/event schema (currently shot-v2);
- frame-save schema and migration capability.

Any mismatch is rejected before gameplay. No peer downloads replacement physics
parameters mid-frame, and no local defaults are substituted. Compatible save
migration does not imply compatible live deterministic simulation.
A version mismatch is rejected explicitly and reported to the connecting peer.

## Synchronized and Reconstructed State

Authoritative synchronized data:

- MatchState and frame number;
- complete pre-shot PhysicsSnapshot including pocket state/event cursor;
- validated ShotInput and monotonically increasing shot sequence;
- explicit ball definitions and parameter/rules versions;
- authoritative shot result hash, event hash and final snapshot hash;
- respot/cue-ball-in-hand decisions and resulting post-rule snapshot.

Locally reconstructed data:

- all fixed physics steps and collision/pocket events from snapshot + input;
- rendered transforms, interpolation, camera, UI, audio and effects;
- trajectory viewer/debug overlays.

Presentation state is never included in deterministic hashes.

## Shot Protocol

1. Server validates player/turn/rules and broadcasts `ShotStart(sequence,
   preShotSnapshot, ShotInput, version hashes)`.
2. All peers retain the pre-shot snapshot and simulate using the public service.
3. Clients may stream progress acknowledgements, but the server alone declares
   settlement/fault and evaluates rules.
4. At settlement peers report hashes of canonical events and final snapshot.
5. Server broadcasts authoritative PhysicsShotRecord, rule result and post-rule
   snapshot. Clients commit only after hash agreement or resynchronization.

Canonical serialization uses classic locale, max_digits10, ID order and LF. Hash
input is the exact validated serialized bytes, not JSON display formatting.

## Latency, Prediction, Rollback and Replay

Aim/cue animation can be predicted locally before server acceptance. Ball motion
may start speculatively after ShotStart, but score/turn changes wait for the
authoritative result. Input is never revised after a sequence begins.

Rollback granularity is one shot: restore the retained pre-shot snapshot, discard
speculative events/presentation and replay the authoritative input. Sparse replay
controller checkpoints may accelerate inspection but are not network authority.
Mid-shot player input is absent, so continuous frame-by-frame rollback is not
required for the initial design.

## Desynchronization and Full Snapshot Resync

On the first event/final hash mismatch the client:

1. stops local rule commitment and records a fault/diagnostic package;
2. compares build/bundle/rules/schema versions again;
3. requests the authoritative complete post-shot snapshot, MatchState, ball
   definitions and last PhysicsShotRecord;
4. restores those bytes and verifies the restored hash;
5. disconnects if restore/version/hash validation still fails.

Partial ball patches are forbidden because pocket state, sleep timers, clock
remainder and event cursor are part of deterministic continuation.

## Failure, Security and Resource Limits

The server rejects malformed IDs, nonfinite input, impossible cue limits, stale
sequence numbers, wrong player/phase, resource limits and mismatched hashes.
Clients cannot submit final positions, scores or pot events. Transport must provide
authentication, integrity and replay protection; encryption/session technology is
selected by the later network implementation, not by PhysicsCore.

Timeout/faulted shots are authoritative failures and distribute a fault package,
not an alternate client result. Reconnect resumes only from a validated complete
frame save or authoritative shot boundary.

## Validation Plan Before Production Networking

- same-machine two-peer deterministic records;
- Debug/Release and target CPU comparisons using the comparator;
- injected latency/loss/reordering at ShotStart/result messages;
- forced event/final hash mismatches and complete resync;
- reconnect from frame save;
- rejected build/bundle/rules/schema mismatch;
- long-run memory/fault-package collection.

Networking remains disabled until these tests exist and the exact supported
platform/toolchain matrix is documented.
