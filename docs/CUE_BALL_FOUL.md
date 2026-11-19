# Cue-Ball Pot Foul

RUL-006 projects a validated physical result into a rule-only foul result. A
cue-ball Potted event sets Foul, CueBallInHand and SuppressObjectScoring, while
retaining the other object-ball IDs for RUL-009. It does not compute penalty
points; RUL-008 owns the foul value. No actor, UI or engine state is mutated.

Ball-in-hand placement uses an explicit table-independent D convention: x <=
-1.04, center y=0, radius 0.292 m, ball center z=radius, complete sphere within
the cloth and no overlap with occupied definitions. `PlaceCueBallInD` returns a
new definition transactionally. It does not choose an alternative placement or
move occupied balls.

Tests cover no cue pot, cue plus object pot, duplicate/inconsistent event cache,
legal D placement, outside-D, outside-cloth, wrong type and overlap. Penalty,
turn and respot precedence remain later rule tasks.
