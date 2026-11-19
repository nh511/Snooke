# Free Ball

QuerySnookered examines both extreme cue-center paths to every active Ball On and
uses balls only as blockers. Free Ball is offered only when every target has both
paths blocked. Nomination is an explicit non-Ball-On type stored in MatchState.

A nominated ball is first-contact legal, scores the Ball On value and is returned
for respot when potted. If it alone creates the next snooker, the shot is foul;
the final-colors pink/black exception is supported. Cue pot and other illegal pots
remain fouls. The current v2 state nominates by type, so callers should provide an
unambiguous active ball of that type. Cushion/jaw obstruction is not modeled.
