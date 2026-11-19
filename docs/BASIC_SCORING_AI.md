# Basic Scoring AI

The basic AI enumerates every active legal target, generated pocket and three
speed presets. Each candidate is physically evaluated and replay-checked. Ranking
rewards legal pots, penalizes cue/illegal pots and engineering risk, then uses a
stable candidate tuple for ties.

The selected command remains a normal ShotInput. No ball teleport, event injection
or direct score mutation is available. This is a simple attack search, not pot
probability estimation or tactical defense; AIC-005 adds weighted behavior.
