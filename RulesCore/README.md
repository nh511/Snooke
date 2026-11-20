# RulesCore

Standalone C++17 public rules interfaces live in include/Snooker/Rules, implementation in src. `RulesCore` consumes `PhysicsCore` and never depends on Unreal Actors, widgets or headers.

Implemented independent rules include ball values, validated/persisted `MatchState` v2, deterministic frame opening and Ball On queries, shot-event projection, normal scoring, cue-ball fouls, first contact, foul values, multi-ball pots, turn switching, color respotting, final colors, frame end, Free Ball, Miss handling, frame saves and version migration. These paths are covered by the standalone C++/CTest suite.

RulesCore is not yet the complete playable UE5 game. Actor/UI presentation, input, animation, editor assets, packaging and in-engine acceptance remain in the UE5 integration scope.
