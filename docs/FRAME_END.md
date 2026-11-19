# Frame End and Respotted Black

ResolveFrameEnd finalizes an unequal Finished score. A tied Finished score becomes
a deterministic respotted-black state: FinalColors, BallOn black, equal scores,
explicit starter and RespottedBlack flag. Once that state has a score difference,
it finalizes and returns the winner. The caller supplies any real-world draw.

StartNextFrame requires a completed non-tied state and calls the deterministic
opening generator with frame number +1 and explicit breaker. Previous state is
not mutated, so saved frame/shot history remains replayable.
