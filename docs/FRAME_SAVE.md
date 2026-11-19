# Frame Save

FrameSave uses snooker-frame-v1 and embeds game version, full physics bundle,
MatchState v2, authoritative current snapshot v3, explicit ball definitions and
shot-v2 history. Validation ties IDs and immutable physical properties together,
requires exactly one cue ball and rejects mixed parameter histories.

`snooker_frame --continue save.frame shot.input output.frame` restores a settled
state, starts a new BeforeShot record, computes to rest and appends it. The match
state is preserved; the rules caller may update it before serializing the next
save. CTest uses separate processes twice and compares with uninterrupted API
continuation byte-for-byte. Schema migration is REP-005, not implicit here.
