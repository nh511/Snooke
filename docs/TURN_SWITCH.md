# Turn Switching

ApplyTurnResult keeps the striker only when a completed non-foul shot has legal
scoring points and ContinueBreak. A legal miss or any foul switches to the other
player. Cue-ball pot additionally leaves CueBallInHand for the next player.
ShotInProgress is cleared. Scores and phase transitions are supplied by the
scoring result; this task does not recalculate them.

Incomplete input, empty foul reasons and inconsistent combined results are
transactional errors. Free Ball, Miss requests and frame scheduling are later
policies, not inferred by the turn switcher.
