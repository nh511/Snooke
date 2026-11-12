# Version Control and Toolchain

Git with Git LFS is the local strategy. Never commit generated builds, Unreal caches, secrets or downloaded toolchains. Keep source, configuration, asset metadata and reproducible evidence summaries in Git.

Binary Unreal assets, FBX, WAV, EXR and PSD are LFS-tracked by .gitattributes. Run git lfs install --local after cloning, then git lfs pull. LFS attributes alone do not prove a remote LFS server is configured.

Use task IDs in commit subjects, e.g. FND-005: define coordinate conventions. No commits or author identity are generated automatically. Review git status and git lfs status before committing. A clean clone and LFS recovery must be verified after an initial user-authorized commit and remote are available.

Validated toolchain: CMake 4.4.4, Visual Studio 2022 Build Tools, MSVC 19.44.35229, Windows SDK 10.0.26100.0, C++17, x64. UE5 exact version is NOT SELECTED; no engine/plugin compatibility is claimed. SnookerPhysics is the planned plugin, not an installed plugin.

Standalone reproduction: powershell -NoProfile -ExecutionPolicy Bypass -File Tools/Verify-Standalone.ps1 -CMake <absolute-cmake.exe>. This creates a fresh independent source copy and separate Debug/Release logs; no existing directory is deleted. The process-level policy setting does not modify system execution policy.
