# AI Shot Candidate

CreatePotCandidate uses a ghost-ball construction: target-to-pocket direction,
target/cue radius offset, then cue-center-to-ghost direction. Candidate data holds
IDs, pocket, unit direction, ghost center, speed, cut angle and an engineering
risk estimate. Only a currently legal Ball On type is accepted.

ToShotInput uses the same CueProfile and validation as player input. The AI has no
separate impulse API and cannot move a ball directly. Candidate generation does
not claim the path is unobstructed or the pot succeeds; AIC-003 simulates it.
