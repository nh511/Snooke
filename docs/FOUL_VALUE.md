# Foul Value

CalculateFoulValue is a pure RulesCore projection. It consumes validated
ShotPhysicsResult, FirstContactResult, CueBallFoulResult and MatchState, then
returns a reason, max relevant ball value, minimum-four penalty and opponent
player index. It does not mutate score/turn/frame state; those are later tasks.

Relevant values include wrong first-contact type, cue-ball foul and illegally
potted object balls. A legal Ball On path has no foul fields. Free-ball states
are rejected until RUL-013 defines the nomination/value interaction. Tests use
explicit valid event sequences and reject inconsistent clocks/caches.
