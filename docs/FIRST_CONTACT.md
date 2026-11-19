# First Contact Judgement

EvaluateFirstContact reads the first validated BallBall event involving the cue.
The other ball is resolved through explicit BallDefinition metadata and compared
to MatchState.BallOn. It returns LegalBallOn, WrongBall or NoContact. Cue-ball
potting, penalty value, Miss and turn mutation are intentionally outside RUL-007.

Unknown IDs, cue-as-target, invalid match phase, in-progress shots and Finished
frames are rejected transactionally. The result is a pure RulesCore value and
contains no Actor, Component, Blueprint or UI dependency.
