# Match State v1 (RUL-001)

## Ball On Query (RUL-004)

GetBallOn reads a validated MatchState without changing it. Reds exposes red only; ColorAfterRed exposes all six colors and preserves the explicit nominated color, including after the last red. FinalColors exposes only the state BallOn cursor; Finished exposes no legal target. The state cursor must be advanced by future scoring/color-phase logic, not guessed from scores. FreeBallAvailable returns an explicit unsupported error until RUL-013 defines nomination/replacement semantics; it is not silently treated as ordinary red/color play. Query failures preserve output. No physical first-contact judgment or scoring is performed by this API.

Engine-independent MatchState stores frame number, phase, zero-based current player (0/1), two unsigned 32-bit scores, BallOn type, remaining reds (0..15), cue-ball-in-hand/free-ball/miss/shot-in-progress flags. No scoring, referee determination or automatic transition happens in this data model. All scores are integer points; overflow is rejected on load, not wrapped.

Phases: Reds, ColorAfterRed (can have zero reds after the last red), FinalColors, Finished. Reds requires remaining reds and red BallOn; color phases use a color BallOn. FinalColors/Finished require zero reds. Finished has no pending shot or referee/input flags. BallOn in color phases is an explicit nominated/current color; the set of legal choices and nomination workflow is RUL-004, not implemented here. FreeBall/Miss flags only preserve state; they do not implement those rules.

Schema tokens: snooker-match-v1 frame phase currentPlayer score0 score1 BallOn remainingReds cueBallInHand freeBallAvailable missPending shotInProgress. Boolean fields must be 0/1, numbers must be unsigned within field range, and trailing data is rejected. Parse validates a candidate before replacing output. Tests save/restore actual files and print all four phases with non-default state, reject bad enums/overflow/truncation and preserve destination state after errors.

This standalone format is NOT a complete match save: ball positions, configuration, per-shot inputs/events and migration are separate replay/save tasks. No actor/UI or UE headers are referenced by RulesCore.

## Frame Opening (RUL-003)

CreateFrameOpening consumes the DAT-008 generator, explicit breaker (0/1), positive frame number, selected physics version and supported fixed step. Result includes canonical 22-ball definitions, stationary cloth-contact snapshot, zero scores, red BallOn, 15 remaining reds and cleared flags. It also retains table/layout version references. Repeated restart erases old scores/flags/positions only after a complete new opening validates; failure preserves the original frame. Breaker alternation, D-area legality, complete save/replay and match scheduling are not inferred or implemented by this factory.
