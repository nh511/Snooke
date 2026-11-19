# AI Read-Only State Query

AICore's IAIStateQuery owns an immutable copy of MatchState, PhysicsSnapshot,
BallDefinition metadata, CueProfile and BallOnQuery. It exposes const references,
ID lookup and a stable FNV hash over canonical serialized state. It never exposes
mutable solver storage or a direct score/position setter.

Construction validates IDs, immutable physical properties and exactly one cue
ball. Identical input has identical hash on the tested build. AI actions must be
converted to ShotInput and submitted through the public physics/rules flow.
