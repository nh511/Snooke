# Rules Event Input

BuildShotRuleInput projects a validated completed PhysicsShotRecord into
ShotPhysicsResult. Ball definitions are explicit metadata; IDs never imply color
or cue type. Radius/mass/material must agree with both snapshots and exactly one
cue definition must match the struck cue ID.

FirstContact is the first BallBall event involving the cue, even when the cue is
the second ID. Potted preserves physical event order; CueBallPotted is derived.
Pot events must agree with initial/final pocket transitions, cannot duplicate,
and pre-existing potted balls must remain potted. Faulted/moving/incomplete shots
are rejected. Cached rule projections are revalidated against raw events.

No score/foul/turn mutation is performed by this interface. Tests consume actual
solver records and a real pot, then reject forged references/caches/order without
changing the output. RulesCore contains no engine, Actor, Blueprint or UI access.
