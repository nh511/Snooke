# RulesCore

Standalone C++17 public rules interfaces live in include/Snooker/Rules, implementation in src. Target RulesCore consumes PhysicsCore, never Unreal Actors or widgets. Implemented: ball score lookup, validated/persisted MatchState, deterministic frame opening and read-only BallOn query. Scoring transitions, first-contact/pot/foul input processing, FreeBall/Miss handling, frame end and a complete match remain TODO. State flags are not rule implementations.
