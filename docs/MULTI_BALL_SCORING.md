# Same-Shot Multi-Ball Classification

EvaluateMultiBallShot preserves physical pot order and classifies a completed
shot without mutating MatchState. During Reds, every potted red is a normal
scoring ball; any potted color is illegal. During ColorAfterRed, exactly one
nominated Ball On color may be a normal scoring ball; multiple colors are foul.
Cue-ball pot, wrong first contact and no contact always suppress normal scoring.

This layer deliberately does not assign penalty points, switch turns, respot
colors or apply Free Ball. RUL-008 consumes foul reasons for penalty values and
RUL-010 consumes the result for turn state. Tests reject duplicate/out-of-order
events and use explicit ball metadata rather than IDs or engine objects.
