# AI Candidate Evaluation

EvaluateCandidate converts a candidate through the normal ShotInput path and runs
the public physics service to settlement. It stores a full shot-v2 record, counts
legal/total pots, cue-ball pot and an engineering risk-weighted score. The query
hash must remain unchanged.

Every accepted evaluation is physically replayed with REP-002 and required to
match all events/final state. A mismatched bundle, invalid candidate, fault or
step-budget failure is rejected transactionally. The score is candidate ranking
data, not a direct MatchState update.
