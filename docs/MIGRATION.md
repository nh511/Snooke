# Save and Replay Migration

MigrationReport distinguishes current, migrated, recognized-incompatible,
unsupported-future and invalid input. Match v1 becomes Match v2 with documented
default flags. Snapshot v2 becomes v3 with all balls OnCloth and event cursor 0.

Legacy shot-v1 is recognized but cannot become physically replayable because it
lacks the complete PhysicsBundle/material registry. The API returns an explicit
message instead of inventing local defaults. Unknown future schema versions are
never guessed. `snooker_migrate kind file` exposes the same decisions to tooling.
