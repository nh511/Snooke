# Final Colors

When no reds remain, EvaluateFinalColor enforces yellow, green, brown, blue,
pink, black. A legal pot scores the color, removes it permanently and advances
BallOn; no respot is requested. The final black completes the color sequence and
sets Finished. Wrong contact, missing pot, cue pot and illegal multiple pots are
explicit illegal results for the foul/turn layers.

Respotted-black tie handling is not part of RUL-012; RUL-015 owns frame-end and
tie policy. Output is transactional and consumes only validated physics metadata.
