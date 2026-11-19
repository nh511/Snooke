# Miss Policy

RulesProfile files select match or practice Miss behavior. Match mode declares
Miss only when the shot has no legal first contact, at least one Ball On has both
extreme paths clear before the shot, and the striker does not already require
snookers to recover the score. Practice mode records the foul but never Miss.

ResolveMiss supports play from the current final snapshot by the opponent, or
restore the exact pre-shot snapshot and require the offender to replay. It clears
MissPending transactionally. Cushion visibility and referee discretion are not
modeled, and the three-Miss frame-loss rule is intentionally not implemented.
