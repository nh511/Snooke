# UE5 新电脑开发交接与低 Token 索引

本文用于把仓库迁移到另一台电脑后开始 Unreal Engine 5.8.4 开发。目标是：

1. 快速恢复工具链、独立核心和 UE 项目骨架。
2. 让 AI 按任务读取最小文件集合，避免先遍历整个仓库、构建目录、基线 CSV 和历史日志。

当前基准提交为 `8e13c28`。任务状态为 `DONE 66/121`、`REVIEW 14`、`BLOCKED 2`、`TODO 39`。独立 C++ 核心已经完成较大部分，但 UE5 项目仍是接入骨架，不是完整可玩的 MVP。

## 1. 最短阅读路径

新电脑第一次打开仓库时，只读取：

1. `README.md`
2. `docs/UE5_NEW_MACHINE_HANDOFF.md`，本文
3. `docs/TASK_STATUS.md`
4. `docs/DEVELOPMENT_PLAN.md` 的技术边界和 UE5 版本章节
5. `docs/UE_INTEGRATION.md`
6. `CMakeLists.txt`

然后按任务选择性读取：

| 任务 | 首选文件 | 默认不要读取 |
|---|---|---|
| UE 项目编译 | `UnrealProject/Snooker.uproject`、两个 `Target.cs`、相关 `Build.cs` | 全部 PhysicsCore 源码、历史日志 |
| Plugin 接入 | `UnrealProject/Plugins/SnookerPhysics/**`、`COR-007~010` 任务段、`ISnookerPhysicsService.h` | 所有基线 CSV、AI 源码 |
| DataAsset | 对应 `PhysicsCore/include/Snooker/Data/*.h`、同名 `.cpp`、对应 `data` 文件 | 全部工具和测试 |
| UE 物理 Actor | `ISnookerPhysicsService.h`、`PhysicsSnapshot.h`、`PhysicsFault.h`、`docs/PHYSICS_SERVICE.md` | 规则和 AI 源码 |
| UE 规则 | `RulesCore/include/Snooker/Rules/*.h`、对应 `.cpp`、`docs/RULE_EVENT_INPUT.md` | 动画、材质、性能工具 |
| UE 角色/输入 | `INT-*`、`CHA-*`、`PRE-*` 任务段、开发方案交互章节 | PhysicsCore 全部实现 |
| 回归失败 | 失败测试、对应实现、任务状态条目 | 全部 `build-evidence` |

### AI 任务提示模板

给 AI 分配任务时，优先使用：

```text
任务：<任务 ID 和一句话目标>
只读文件：<最多 3 个接口/实现/测试文件>
依赖文档：<最多 2 个 docs 文件>
禁止遍历：build、build-evidence、data/baselines、docs/evidence、无关模块
验收命令：<一个 Test-Current 或指定 CTest 命令>
约束：核心不含 Unreal 头文件；不要复制核心源码到 UE 插件
```

AI 应先用 `rg -n "符号名|任务 ID" <指定目录>` 定位，再读取命中文件。不要先递归读取整个仓库、二进制、CSV 或全部测试日志。

## 2. 新电脑环境

### 必需工具

- Unreal Engine **5.8.4**，不要用 5.9 Preview、Nightly 或其他主版本替代。
- Visual Studio 2022/Build Tools，包含 MSVC、Windows SDK 和 C++ 桌面开发工具。
- CMake 3.20+；当前验证环境为 CMake 4.4.4。
- Git 和 Git LFS。
- Node.js；CMake 配置阶段会强制查找 `node`。
- Chrome；仅用于 2D 查看器无界面验证。

当前验证工具链：

```text
MSVC 19.44.35229 / VS2022 Build Tools
Windows SDK 10.0.26100.0
CMake 4.4.4 / C++17 / x64
```

### 克隆

```powershell
git clone https://github.com/nh511/Snooke.git
cd Snooke
git lfs install --local
git lfs pull
git status
```

不要从旧电脑复制 `build/` 或 `build-evidence/`。这些目录是本地生成物，可能包含旧编译器、旧 CMake 缓存和旧测试路径。

### 先验证独立核心

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure --no-tests=error
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure --no-tests=error
```

或使用：

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/Test-Current.ps1 -Run new-machine-core
```

干净源副本验证：

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/Verify-Standalone.ps1 -Run new-machine-clean-copy
```

### UE 项目文件

`UnrealProject/Snooker.uproject` 当前没有伪造 `EngineAssociation`，这是有意的。安装 UE5.8.4 后，用实际安装目录中的 UnrealVersionSelector 关联项目并生成 Visual Studio 文件，不要复制旧电脑的 `.sln`、`Intermediate` 或 `Binaries`。

典型命令形式：

```powershell
& "<UE_ROOT>\Engine\Binaries\Win64\UnrealVersionSelector-Win64-Shipping.exe" `
  "D:\path\to\Snooker\UnrealProject\Snooker.uproject"
```

`<UE_ROOT>` 必须替换为新电脑上的真实 UE5.8.4 路径。项目文件生成后，先编译 `SnookerEditor Win64 Development` 和 `SnookerEditor Win64 DebugGame`，然后再打开编辑器。FND-002 必须记录引擎版本、生成命令、两个配置的退出码和编辑器启动结果。

## 3. 架构边界

```text
UnrealProject/                     UE 表现层和适配层
  SnookerGame                      玩家、输入、Actor、组件、UI
  SnookerEditor                    Editor-only 工具
  Plugins/SnookerPhysics           计划中的物理插件
    SnookerPhysicsCore             UE 模块外壳，当前只有启动代码
    SnookerPhysicsUE               UE 类型转换/服务适配，当前只有启动代码
PhysicsCore/                       纯 C++17 物理核心
RulesCore/                         纯 C++17 规则核心
AICore/                            纯 C++17 AI 核心
data/                              参数、桌面、规则、快照、基线
tools/                             独立 CLI、回放、实验、性能、查看器
tests/                             CTest、C++、CMake 和 Node 验证
docs/                              设计、状态、验收和交接文档
```

核心原则：物理和规则留在独立核心；UE 负责资源、输入、镜头、Actor、Component、UMG、音效和打包；UE 不直接修改球状态；不要复制核心源码到 Plugin；不要让 Chaos 直接决定球局结果。

## 4. UE 骨架的真实状态

| 文件/模块 | 当前状态 | 新电脑第一步 |
|---|---|---|
| `UnrealProject/Snooker.uproject` | 项目描述，启用 Game、Editor、Plugin、EnhancedInput | 关联 UE5.8.4 并生成工程文件 |
| `Source/SnookerGame` | Game module 启动代码和 Build.cs | 确认 Editor Development 编译 |
| `Source/SnookerEditor` | Editor module 启动代码和 Build.cs | 确认 Editor target 编译 |
| `Plugins/SnookerPhysics/SnookerPhysics.uplugin` | 两个 Runtime module 声明 | 确认插件被编辑器发现 |
| `SnookerPhysicsCore` | 只有 `IMPLEMENT_MODULE` | COR-009：接入同一份核心源码 |
| `SnookerPhysicsUE` | 只有 `IMPLEMENT_MODULE` | COR-007/008：服务适配和类型转换 |
| `Content/Snooker/**/README.md` | 资源目录占位说明，不是真实资产 | 按任务创建 `.uasset/.umap` |

## 5. UE5 开发阶段

### 阶段 A：空项目和插件

任务：`FND-002`、`FND-003` 的 UE 部分。验收项目文件由 5.8.4 生成，`SnookerEditor Development`、`DebugGame` 编译通过，编辑器可打开，插件可发现。

### 阶段 B：Plugin 接入

任务：`COR-007`、`COR-008`、`COR-009`、`COR-010`。

推荐顺序：

1. 为 `SnookerPhysicsUE` 定义 `ISnookerPhysicsService` 的最小 UE 适配接口。
2. 根据 `PhysicsCore/include/Snooker/Math/SnookerCoordinateConvention.md` 实现 `FVector/FQuat/FTransform` 转换。
3. 测试 UE 坐标、单位、极向量和轴向量方向。
4. 在 `SnookerPhysicsCore.Build.cs` 中接入同一份核心源码，不复制文件、不引用预编译静态库。
5. 用相同 bundle、snapshot、shot input 比较独立 CLI 与 UE 事件和终态。

先让一个最小 UE Actor 调用 `BeginShot`、`Step`、`SaveSnapshot`、`GetEvents`，不要在此阶段做角色和 UI。

### 阶段 C：DataAsset 和实验地图

任务：`DAT-001~006` UE 适配、`VAL-001`、`VAL-002`。

| UE 资产 | 独立核心来源 |
|---|---|
| `UBallDefinitionAsset` | `BallDefinition` |
| `UTableDefinitionAsset` | `TableDefinition` |
| `UClothProfileAsset` | `ClothProfile` |
| `UCushionProfileAsset` | `CushionProfile` |
| `UPocketProfileAsset` | `PocketProfile` |
| `UCueProfileAsset` | `CueProfile` |

DataAsset 是 UE 编辑入口，核心版本化文本格式仍是交换和回归依据。不要在 DataAsset、Blueprint 和核心配置中各维护一套参数。`Map_PhysicsLab` 先做无 UI 的单球、球碰球、库边、袋口四个预设，只负责展示和命令调用。

### 阶段 D：最小可玩击球闭环

任务：`INT-001~016`。推荐顺序：自由观察摄像机、球桌/球 Actor、选择球、球杆方向、打点、拉杆/出杆、提交 `ShotInput`、等待 `Settled`、读取规则结果，最后再做 UI 和辅助线。

击球提交前必须验证 CueBallId、CueProfileId、方向、打点、速度、后拉距离和 MatchState。不要在 Blueprint 中直接改球位置或速度。

### 阶段 E：角色、UI、音效和打包

任务：`CHA-*`、`PRE-*`、`REL-003`、`REL-005`。这些任务依赖最小击球闭环，先完成一杆单机流程，再扩展角色 IK、入位动画、试杆、出杆、UMG、音效、灯光、目标硬件和打包。

## 6. UE 层的最小接口形状

权威接口：

- `PhysicsCore/include/Snooker/Service/ISnookerPhysicsService.h`
- `PhysicsCore/include/Snooker/Physics/PhysicsSnapshot.h`
- `PhysicsCore/include/Snooker/Shot/ShotInput.h`
- `PhysicsCore/include/Snooker/Replay/PhysicsShotRecord.h`
- `RulesCore/include/Snooker/Rules/ShotPhysicsResult.h`
- `RulesCore/include/Snooker/Rules/MatchState.h`

数据流：

```text
UE input -> ShotInput -> BeginShot/Step
service -> PhysicsSnapshot / PhysicsEvent / PhysicsFault
events + definitions -> ShotPhysicsResult -> RulesCore
rules result -> UE score/UI/turn presentation
```

建议的 UE 类名仅供设计，不代表当前代码已经存在：

- `USnookerPhysicsComponent`：管理服务实例和 Tick。
- `ASnookerBall`：显示快照中的球状态。
- `ASnookerTable`：持有桌面几何和 DataAsset。
- `USnookerShotInputComponent`：把输入转换为 `ShotInput`。
- `USnookerReplayComponent`：调用 `ReplayController` 查询状态。

## 7. PhysicsCore 文件索引

### 数据、数学和序列化

| 文件 | 作用 |
|---|---|
| `Data/BallDefinition.h/.cpp` | 球 ID、类型、半径、质量、材质、初始位置 |
| `Data/ClothProfile.h/.cpp` | 布面滑动、滚动、旋转和睡眠参数 |
| `Data/CoreConfig.h/.cpp` | 旧 CoreConfig v2 兼容格式 |
| `Data/CushionProfile.h/.cpp` | 库边恢复、切向摩擦、接触高度 |
| `Data/PocketProfile.h/.cpp` | 袋口、袋角、袋底、袋内参数 |
| `Data/TableDefinition.h/.cpp` | 桌面边界、布面、库边、袋口、球点、初始球位 |
| `Math/CoreTypes.h` | 核心标量、向量、四元数、变换和 ID/时间别名 |
| `Math/SnookerUnits.h` | 单位、向量数学、有限值和字符串化 |
| `Math/SnookerCoordinateConvention.md` | 核心/UE 坐标和极向量/轴向量约定，COR-008 必读 |
| `Serialization/Text.h` | 受限文本解析和事务式失败辅助函数 |

### 物理状态和碰撞

| 文件 | 作用 |
|---|---|
| `Physics/BallState.h/.cpp` | 球运行时状态、速度、旋转、接触和睡眠 |
| `Physics/PhysicsClock.h` | 固定步和累积时间 |
| `Physics/PhysicsSnapshot.h/.cpp` | 快照 v3、球/袋状态、事件游标和迁移 |
| `Physics/PocketState.h/.cpp` | OnCloth、InPocket、Potted、Ejected |
| `Physics/ClothFriction.h`、`ClothFriction.cpp` | 布面摩擦模型 |
| `Physics/RollingResistance.cpp` | 滚动阻力 |
| `Physics/AngularUpdate.cpp` | 角速度更新 |
| `Physics/MotionState.h`、`Sleep.h`、`Touching.h/.cpp` | 运动、睡眠和贴球查询 |
| `Physics/BallPairs.h/.cpp` | 球对候选和稳定排序 |
| `Physics/BallContact.h/.cpp` | 球-球连续接触时间 |
| `Physics/BallResponse.h/.cpp` | 球-球冲量和旋转响应 |
| `Physics/CushionContact.h/.cpp` | 库边接触查询 |
| `Physics/CushionResponse.h/.cpp` | 库边响应 |
| `Physics/PocketContact.h/.cpp` | 袋角、袋唇、斜面、袋底接触 |
| `Physics/PocketSimulation.h/.cpp` | 单球袋口实验服务 |
| `Table/TableGeometry.h/.cpp` | 桌面几何、闭合边界、袋口生成 |
| `Table/BallPlacement.h/.cpp` | 开局、训练球位和球点占用 |

### 服务、故障和回放

| 文件 | 作用 |
|---|---|
| `Service/ISnookerPhysicsService.h` | UE 最重要的公共服务接口、事件、性能样本和工厂 |
| `Physics/PhysicsService.cpp` | 多球固定步、最早事件调度和状态推进 |
| `Physics/PhysicsFault.h/.cpp` | 故障码、故障快照和安全检查 |
| `Validation/DebugOptions.h/.cpp` | 独立调试开关；UE 控制台变量复用同名项 |
| `Validation/Log.h` | 分类日志和级别过滤 |
| `Shot/CueProfile.h/.cpp` | 球杆参数和输入限制 |
| `Shot/CueImpulse.h/.cpp` | ShotInput 到母球冲量/旋转 |
| `Shot/ShotInput.h/.cpp` | CLI/UE 共用击球输入格式和校验 |
| `Replay/PhysicsShotRecord.h/.cpp` | `snooker-shot-v2` 完整单杆记录 |
| `Replay/ShotReplay.h/.cpp` | 重新模拟并比较记录 |
| `Replay/ReplayController.h/.cpp` | 按步、时间、事件和故障点查询 |
| `Replay/CollisionLog.h/.cpp` | 低层球碰撞诊断日志 |
| `Replay/ShotRecord.h/.cpp` | 旧版兼容单球格式 |

## 8. RulesCore 文件索引

同名头文件在 `RulesCore/include/Snooker/Rules/`，实现位于 `RulesCore/src/`：

| 文件 | 作用 |
|---|---|
| `BallValues` | 红球和彩球分值 |
| `MatchState` | frame 阶段、比分、Ball On、待处理状态、v2 序列化 |
| `FrameOpening` | 开局球位和新 frame |
| `BallOn` | 当前合法目标球 |
| `ShotPhysicsResult` | 从 PhysicsShotRecord 构造规则输入 |
| `ShotScoring` | 正常计分、点名和阶段推进 |
| `CueBallFoul` | 母球进袋和 D 区自由放置 |
| `FirstContact` | 首次接触合法性 |
| `FoulValue` | 最低 4 分和犯规分值 |
| `MultiBallShot` | 同杆多球和母球进袋组合 |
| `TurnSwitch` | 继续击球、换人、自由放置 |
| `ColorRespot` | 彩球本点/替代点/无点复位 |
| `FinalColors` | 黄到黑的彩球阶段 |
| `FrameEnd` | 胜负、平分重摆黑球、下一 frame |
| `FreeBall` | 被做斯诺克、自由球点名、计分和复位 |
| `Miss` | 比赛/练习 Miss 和恢复重打 |
| `FrameSave` | frame 存档和恢复 |
| `Migration` | MatchState/Snapshot 等旧格式迁移 |

## 9. AICore 文件索引

| 文件 | 作用 |
|---|---|
| `AI/StateQuery.h`、`src/StateQuery.cpp` | 只读球局查询、Ball On、状态哈希 |
| `AI/ShotCandidate.h`、`src/ShotCandidate.cpp` | Ghost Ball 候选、目标、袋口、切角、风险 |
| `AI/ShotEvaluation.h`、`src/ShotEvaluation.cpp` | 物理服务评估候选击球 |
| `AI/BasicScoringAI.h`、`src/BasicScoringAI.cpp` | 简单进攻候选选择 |
| `AI/WeightedAI.h`、`src/WeightedAI.cpp` | 难度、攻防风险权重和可解释明细 |

AI 只能查询和提交 `ShotInput`，不能直接修改快照、比分或球位。

## 10. UE5 文件索引

| 文件 | 作用 |
|---|---|
| `UnrealProject/Snooker.uproject` | 项目描述，加载 Game、Editor、Plugin、EnhancedInput |
| `Source/Snooker.Target.cs` | Game target |
| `Source/SnookerEditor.Target.cs` | Editor target |
| `Source/SnookerGame/SnookerGame.Build.cs` | Game 模块依赖 |
| `Source/SnookerGame/Private/SnookerGameModule.cpp` | Game 模块启动 |
| `Source/SnookerEditor/SnookerEditor.Build.cs` | Editor-only 依赖 |
| `Source/SnookerEditor/Private/SnookerEditorModule.cpp` | Editor 模块启动 |
| `Plugins/SnookerPhysics/SnookerPhysics.uplugin` | 插件和两个 Runtime module 声明 |
| `SnookerPhysicsCore/SnookerPhysicsCore.Build.cs` | 计划编译共享核心的 UE 模块；当前只有 Core/Cpp17 配置 |
| `SnookerPhysicsCore/Private/SnookerPhysicsCoreModule.cpp` | Plugin Core 启动 |
| `SnookerPhysicsUE/SnookerPhysicsUE.Build.cs` | 计划放 UE 转换层；当前依赖 Core module |
| `SnookerPhysicsUE/Private/SnookerPhysicsUEModule.cpp` | UE adapter 启动 |

当前没有真实 Actor、Component、DataAsset、Blueprint 或 UMG 类。

## 11. Tools 和 Tests 索引

| 文件/组 | 作用 |
|---|---|
| `tools/simulate.cpp` | 通过公共服务模拟整杆并导出结果 |
| `tools/replay.cpp` | 重放 `snooker-shot-v2` |
| `tools/frame.cpp`、`migrate.cpp` | frame 存档和旧格式迁移 |
| `tools/physics_lab.cpp` | 四类无 UE5 实验场景 |
| `tools/performance.cpp` | 固定步/搜索/碰撞类型性能 CSV |
| `tools/stability.cpp` | 多杆稳定性和内存增长 |
| `tools/fault_package.cpp`、`fault_replay.cpp` | 故障包生成和复现 |
| `tools/pocket_bundle.cpp` | 参数生成 PhysicsBundle |
| `tools/table_geometry.cpp` | 桌面几何 SVG |
| `tools/baseline_seed.cpp`、`cue_strike.cpp` | 基线和击球输入工具 |
| `tools/single_ball.cpp`、`validation.cpp`、`core_info.cpp` | 旧单球/参数/信息工具 |
| `tools/compare.cjs` | 轨迹、事件、终态差异比较 |
| `tools/shot-viewer.*` | 多球 JSON/CSV 2D 查看器 |
| `tools/trajectory.*` | 旧单球查看器 |
| `tools/verify-*.cjs` | Chrome/CDP 查看器验证 |
| `tools/Test-Current.ps1` | 当前工作树 Debug/Release 验证 |
| `tools/Verify-Standalone.ps1` | 干净源副本验证 |
| `tests/physics_service.cpp` | 服务、FPS、快照恢复、多球核心回归 |
| `tests/physics_record.cpp`、`shot_replay.cpp`、`replay_controller.cpp` | 记录、重放和定位 |
| `tests/physics_fault.cpp`、`fault-package.cmake` | 故障保护和故障包 |
| `tests/pocket*.cpp`、`pocket_baseline.cpp` | 袋口状态和基线 |
| `tests/rail_baseline.cpp`、`cushion*.cpp` | 库边和碰库 |
| `tests/scoring.cpp`、`cue_ball_foul.cpp`、`first_contact.cpp`、`foul_value.cpp`、`multi_ball_shot.cpp` | 规则输入、计分和犯规 |
| `tests/free_ball.cpp`、`miss.cpp`、`frame_end.cpp`、`frame_save.cpp`、`migration.cpp` | 复杂规则、frame、迁移 |
| `tests/ai_*.cpp`、`basic_ai.cpp`、`weighted_ai.cpp` | AI 查询、候选、评估和难度 |
| `tests/debug_options.cpp`、`physics-lab.cjs`、`performance.cjs` | 最近独立调试/实验/性能回归 |

## 12. 数据和证据阅读规则

| 路径 | 用途 | 默认策略 |
|---|---|---|
| `data/bundles/` | 当前 PhysicsBundle | 物理任务读取指定 bundle |
| `data/table-*.snooker` | 桌面和球位样例 | 只读取指定桌面 |
| `data/profiles/` | 布面、库边、袋口、球杆参数 | 只读取相关 profile |
| `data/rules/` | 比赛/练习规则参数 | 规则任务再读取 |
| `data/baselines/` | 大型输入、记录、轨迹 CSV | 默认只读 README 和输入 |
| `docs/evidence/` | 已归档截图、退出码、摘要 | 只在需要历史证据时读取 |
| `build/`、`build-evidence/` | 本地生成物 | 不提交、不遍历、不作为源码上下文 |

参数和基线是工程回归值，不是现实球桌实测校准值。实测记录使用 `docs/MEASUREMENT_RECORD_TEMPLATE.md`。

## 13. 任务到文件的最短路径

| UE 任务 | 最小阅读集合 |
|---|---|
| `COR-007` | `docs/UE_INTEGRATION.md`、Plugin 源码、`ISnookerPhysicsService.h` |
| `COR-008` | `CoreTypes.h`、`SnookerUnits.h`、`SnookerCoordinateConvention.md`、UE Build.cs |
| `COR-009` | `CMakeLists.txt` source list、`SnookerPhysicsCore.Build.cs`、`docs/UE_INTEGRATION.md` |
| `COR-010` | `tools/simulate.cpp`、`tools/replay.cpp`、`tests/service_cli.cmake` |
| `DAT-001~006` | 对应 Data header、同名 cpp、对应 data profile/table |
| `VAL-001` | `tools/physics_lab.cpp`、`tests/physics_lab.cmake`、`tests/physics-lab.cjs`、Maps README |
| `VAL-002` | `tools/shot-viewer.*`、`shot-data.js`、`verify-shot-viewer.cjs`、Debug README |
| `INT-015` | `ShotInput.h/.cpp`、`ISnookerPhysicsService.h`、`ShotPhysicsResult.h/.cpp` |
| `PRE-004` | `docs/SHOT_VIEWER.md`、`docs/DEBUG_OPTIONS.md`、`PhysicsEvent` 定义 |
| `REL-005` | `.uproject`、`.uplugin`、所有 `Build.cs`、`docs/UE_INTEGRATION.md` |

## 14. UE5 开发禁区

- 不要提交 `build/`、`build-evidence/`、`.vs/`、`Intermediate/`、`Binaries/`、`DerivedDataCache/` 或 `.sln`。
- 不要把核心 `.cpp` 复制到 `UnrealProject` 或 Plugin。
- 不要在 Blueprint/Actor 中直接修改球状态。
- 不要用 Chaos 结果替换 `ISnookerPhysicsService` 的球局结果。
- 不要把资源目录 README 当作 `.uasset/.umap` 验收证据。
- 不要为通过测试而放宽确定性容差、修改基线期望或跳过 CTest/Node。
- 不要在确认 UE5.8.4 前手工填写 `EngineAssociation`。
- 不要把 `REVIEW` 任务改为 `DONE`，除非 UE5/目标环境验收也完成。

## 15. 新电脑第一周建议

1. 安装并确认 UE5.8.4、VS2022、Windows SDK、CMake、Node、Git LFS。
2. 克隆仓库并运行独立 Debug/Release CTest。
3. 生成 UE 项目文件，编译 `SnookerEditor Development` 和 `DebugGame`。
4. 记录 FND-002 的引擎版本、生成命令和构建日志。
5. 让空 `SnookerPhysics` 插件在编辑器中加载。
6. 完成 COR-007 最小服务适配接口设计。
7. 完成 COR-008 单位/坐标转换测试。
8. 完成 COR-009 同源代码接入，保持独立 CMake 不回归。
9. 用一个最小 UE Actor 调用一杆服务并显示 `PhysicsSnapshot`。
10. 再进入 DataAsset、实验地图和 INT/CHA/PRE 任务。

## 16. 任务完成证据

每个任务至少记录：任务 ID、实现文件、测试文件、验收命令、UE 版本、Target、Configuration、平台、退出码、失败原因和修复方式。独立 CLI 与 UE 对照时记录 bundle、snapshot、shot input 和第一处差异。

独立回归仍使用：

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/Test-Current.ps1 -Run ue-handoff-regression
```

UE 日志保存在本地 `build-evidence/`，只把可复现的摘要、截图和关键退出码整理进 `docs/evidence/`。

## 17. 工具和测试完整文件清单

下面的文件名是当前仓库中的实际代码文件。核心库文件已在前面的 PhysicsCore、RulesCore、AICore 表中解释；本节补齐工具和测试文件，避免 AI 为了确认职责再次遍历目录。

### `tools/`

| 文件 | 作用 |
|---|---|
| `baseline_seed.cpp` | 生成中心基线初始快照和输入 |
| `compare.cjs` | 比较轨迹、结果、事件和第一处差异 |
| `core_info.cpp` | 输出核心/规则基础信息 |
| `cue_strike.cpp` | 从单球快照和输入生成击球后状态 |
| `fault_package.cpp` | 生成故障复现包 |
| `fault_replay.cpp` | 仅使用故障包复现故障 |
| `frame.cpp` | frame 存档 CLI |
| `migrate.cpp` | 旧格式迁移 CLI |
| `performance.cpp` | 固定步、搜索和碰撞类型耗时采样 |
| `physics_lab.cpp` | 生成四类独立物理实验场景 |
| `pocket_bundle.cpp` | 从桌面和 profile 文件生成 PhysicsBundle |
| `replay.cpp` | 重放和比较 `snooker-shot-v2` |
| `simulate.cpp` | 使用统一服务完成整杆模拟和导出 |
| `single_ball.cpp` | 旧单球布面轨迹工具 |
| `stability.cpp` | 固定种子多杆稳定性和工作集采样 |
| `table_geometry.cpp` | 导出实际桌面几何 SVG |
| `validation.cpp` | 旧独立参数和快照验证入口 |
| `simulation_export.h` | JSON、CSV、debug 输出辅助函数 |
| `shot-data.js` | 多球查看器 JSON/CSV 解析器 |
| `shot-viewer.html` | 多球查看器页面和控件 |
| `shot-viewer.js` | 多球查看器绘制、播放、事件跳转和投影 |
| `trajectory-data.js` | 旧单球 CSV 解析器 |
| `trajectory.html` | 旧单球轨迹查看器页面 |
| `verify-shot-viewer.cjs` | Chrome/CDP 多球查看器自动验证和截图 |
| `verify-viewer.cjs` | Chrome/CDP 旧单球查看器自动验证 |
| `Test-Current.ps1` | 当前工作树配置、Debug/Release 构建和 CTest |
| `Verify-Standalone.ps1` | 干净源副本配置、构建和 CTest |

### `tests/`：基础和数据

| 文件 | 作用 |
|---|---|
| `main.cpp` | 基础测试入口 |
| `units.cpp` | 单位和坐标转换 |
| `math.cpp` | 向量、四元数、变换数学 |
| `parsing.cpp` | 文本解析边界、失败事务和区域设置 |
| `definition.cpp` | BallDefinition 数据定义 |
| `config.cpp` | CoreConfig 加载、版本和资源限制 |
| `table.cpp` | TableDefinition 文件和边界 |
| `state.cpp` | BallState 和状态不变量 |
| `clock.cpp` | 固定时钟、累积时间和恢复 |
| `snapshot.cpp` | PhysicsSnapshot 序列化、恢复和迁移 |
| `log.cpp` | 日志类别、级别和 sink |
| `debug_options.cpp` | 调试开关解析和失败行为 |
| `cloth.cpp` | 布面参数和边界 |
| `friction.cpp` | 滑动摩擦 |
| `rolling.cpp` | 滚动阻力 |
| `angular.cpp` | 角速度和旋转 |
| `motion.cpp` | 运动状态 |
| `sleep.cpp` | 睡眠和唤醒 |

### `tests/`：碰撞、桌面和物理服务

| 文件 | 作用 |
|---|---|
| `pairs.cpp` | 球对候选和稳定顺序 |
| `contact.cpp` | 球接触几何 |
| `response.cpp` | 球碰撞响应和守恒 |
| `collision_log.cpp` | 碰撞日志序列化和重放 |
| `touching.cpp` | 贴球/接触查询 |
| `impulse.cpp` | 球杆冲量映射 |
| `cue.cpp` | CueProfile 和击球输入 |
| `shot_input.cpp` | ShotInput 序列化和限制 |
| `cushion.cpp` | 库边局部响应 |
| `cushion_contact.cpp` | 库边接触检测 |
| `cushion_response.cpp` | 库边响应细节 |
| `geometry.cpp` | 桌面几何和闭合边界 |
| `placement.cpp` | 球位生成和点位占用 |
| `pocket.cpp` | 袋口接触和边界 |
| `pocket_contact.cpp` | 袋内接触 |
| `pocket_state.cpp` | 袋状态转换 |
| `pocket_simulation.cpp` | 单球袋口服务 |
| `physics_service.cpp` | 统一多球服务、FPS、恢复、性能样本 |
| `physics_fault.cpp` | 故障保护和故障快照 |
| `physics_record.cpp` | `snooker-shot-v2` 记录校验 |
| `shot_replay.cpp` | 真实物理重放 |
| `replay_controller.cpp` | 步/时间/事件/故障定位 |

### `tests/`：基线、导出和 CMake 验证

| 文件 | 作用 |
|---|---|
| `baseline.cmake` | 中心杆基线、重复运行和扰动差异 |
| `side-baseline.cmake` | 左右塞镜像基线 |
| `side-baseline.cjs` | 左右塞结果检查 |
| `spin-baseline.cmake` | 高杆/低杆基线 |
| `spin-baseline.cjs` | 旋转状态检查 |
| `rail-baseline.cmake` | 一库、两库、贴库和镜像库边基线 |
| `rail_baseline.cpp` | 库边基线辅助验证 |
| `pocket_baseline.cpp` | 正入、擦角、停口、弹出、落袋 |
| `bundle.cmake` | PhysicsBundle 重新生成和字节比较 |
| `service_cli.cmake` | 统一 CLI、记录、Node 导出和 replay 联测 |
| `shot_entry.cmake` | 文件输入和记录输入一致性 |
| `shot-data.cjs` | 多球结果/CSV 解析和终态一致性 |
| `simulation-export.cjs` | JSON、CSV、事件和快照导出检查 |
| `trajectory-data.cjs` | 旧单球 CSV 解析检查 |
| `compare.cjs` | 比较器契约测试 |
| `stability.cmake` | 50 杆稳定性测试入口 |
| `fault-package.cmake` | 故障包生成和只用包复现 |
| `frame-save.cmake` | 两进程 frame 存档恢复 |
| `network-doc.cmake` | 联机同步设计文档契约 |
| `physics_lab.cmake` | 物理实验场景生成和真实事件序列 |
| `physics-lab.cjs` | 实验场景清单和事件类型断言 |
| `performance.cmake` | 性能工具和 Node 报告检查 |
| `performance.cjs` | 性能 CSV/摘要字段和非零碰撞耗时 |
| `measurement-template.cjs` | 人工测量模板字段和版本关联 |

### `tests/`：RulesCore 和 AICore

| 文件 | 作用 |
|---|---|
| `rules.cpp` | 基础规则不变量 |
| `ball_on.cpp` | Ball On 查询 |
| `match_state.cpp` | MatchState 序列化和状态约束 |
| `frame_opening.cpp` | 开局和新 frame |
| `scoring.cpp` | 正常计分和点名 |
| `cue_ball_foul.cpp` | 母球犯规和 D 区 |
| `first_contact.cpp` | 首次接触 |
| `foul_value.cpp` | 犯规分值 |
| `multi_ball_shot.cpp` | 同杆多球 |
| `turn_switch.cpp` | 回合切换 |
| `color_respot.cpp` | 彩球复位 |
| `final_colors.cpp` | 最终彩球阶段 |
| `frame_end.cpp` | frame 结束和平分重摆 |
| `free_ball.cpp` | Free Ball |
| `miss.cpp` | Miss |
| `frame_save.cpp` | frame 存档 |
| `migration.cpp` | 版本迁移 |
| `rule_input.cpp` | 物理事件到规则输入 |
| `ai_state_query.cpp` | AI 只读查询 |
| `ai_candidate.cpp` | Ghost Ball 候选 |
| `ai_evaluation.cpp` | AI 单杆物理评估 |
| `basic_ai.cpp` | 基础得分 AI |
| `weighted_ai.cpp` | 难度和风险权重 AI |


