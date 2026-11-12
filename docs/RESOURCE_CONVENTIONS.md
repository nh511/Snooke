# Resource and Source Layout

The standalone simulation is kept outside Unreal Content. The same PhysicsCore and RulesCore source files will later be compiled into the UE5 plugin.

## Repository Layout

- `PhysicsCore/`: standalone public headers and implementation.
- `RulesCore/`: standalone rules public headers and implementation.
- `UnrealProject/Source/`: gameplay and editor modules.
- `UnrealProject/Plugins/`: SnookerPhysics plugin and UE5 adapters.
- `UnrealProject/Content/Snooker/`: binary Unreal assets only; never core source code.
- `docs/`: plans, task evidence, conventions and engineering notes.

COR-002 migrated the legacy source and tests. Shared C++ tests live in `tests/`, CLI and build tooling in `tools/`, and versioned fixtures in `data/`. The old `Source/SnookerCore` contains no source implementations. No source is duplicated in Content.

## Content Folders

| Folder | Prefix | Example |
|---|---|---|
| Maps | `Map_` | `Map_PhysicsLab` |
| Meshes | `SM_`, `SK_` | `SM_Table`, `SK_PlayerPlaceholder` |
| Materials | `M_`, `MI_`, `T_` | `M_Cloth`, `MI_Cloth_Standard`, `T_Cloth_Normal` |
| Animation | `ABP_`, `Anim_`, `CR_` | `ABP_Player`, `Anim_PracticeStroke`, `CR_CueHold` |
| Audio | `S_`, `SC_` | `S_BallContact`, `SC_PocketDrop` |
| UI | `WBP_` | `WBP_ShotReview` |
| Data | `DA_` | `DA_Table_Standard`, `DA_Cloth_Standard` |
| Debug | `BP_Debug_` | `BP_Debug_Trajectory` |

Use PascalCase names with a type prefix. Keep related assets in a feature subfolder only when the folder has several assets. Do not store temporary assets, build products or core C++ files under Content.

The README files in Content are placement examples and tracked directory markers, not fabricated `.uasset` files. Actual Unreal assets require the UE5 editor and remain unverified until FND-002 is executable.
