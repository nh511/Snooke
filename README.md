# Snooker

一个面向 Unreal Engine 5.8.4 的斯诺克项目基础仓库。

当前仓库的重点是先完成**不依赖 Unreal Engine 的 C++ 核心、规则、回放、验证和 AI 基础**，再接入 UE5 的 Actor、Component、DataAsset、Blueprint、UI、音效和打包流程。独立核心使用 C++17 和 CMake 构建，不包含 Unreal 头文件。

> 当前项目仍在开发中，尚未完成完整可玩的斯诺克 MVP。UE5 相关验收需要 Unreal Engine 5.8.4 环境。

## 当前进度

任务书共 121 项。当前唯一任务状态为：

- `DONE`：66/121
- `REVIEW`：14 项，主要是独立部分已完成但仍等待 UE5 或目标环境验收
- `BLOCKED`：2 项，`FND-002`、`FND-003`，原因是当前没有 UE5 编辑器和真实 UE 资源验收环境
- `TODO`：39 项

最近一次完整独立验证：

- Debug：94/94 CTest 通过
- Release：94/94 CTest 通过
- 配置、构建和测试退出码全部为 `0`
- 22 球真实物理数据通过 Chrome/CDP 查看器验证
- 四类物理实验场景通过真实物理事件序列验证

详细任务状态见 [`docs/TASK_STATUS.md`](docs/TASK_STATUS.md)，历史验收摘要见 [`docs/VALIDATION_ARCHIVE.md`](docs/VALIDATION_ARCHIVE.md)。

## 已实现能力

### 物理核心

- 独立 `PhysicsCore` 静态库
- 固定步长物理时钟、布面摩擦、滑动/滚动/旋转衰减和睡眠状态
- 球碰球连续接触检测与响应
- 库边、袋角、袋口、袋内接触和落袋/弹回事件
- 六袋工程样例桌和闭合边界几何
- 物理快照 v3，以及 v2 快照迁移
- 统一 `ISnookerPhysicsService` 服务接口
- 多球整杆模拟、事件序列和中途快照恢复
- `NonFinite`、`Penetration`、`VelocityExplosion`、`Timeout`、`EventLoop`、`LeftTable`、`PocketEscape`、`SolverError` 故障保护

### 回放和验证

- `snooker-shot-v2` 单杆记录
- 独立重放和确定性比较
- 固定步轨迹 CSV、结果 JSON、事件序列和几何数据导出
- 中杆、高杆、低杆、旋转、碰库、袋口和镜像场景基线
- Chrome 本地 2D 多球查看器，支持：
  - Top/World 投影
  - 轨迹和时间轴
  - 速度向量和角速度向量
  - 接触点和接触法线
  - 睡眠状态高亮
  - 事件跳转、逐步播放和移动端布局

### 规则核心

- 独立 `RulesCore` 静态库
- 比赛状态 v2 和版本迁移
- 正常进球计分、母球进袋、首次接触、犯规分值
- 同杆多球进袋、回合切换和彩球复位
- 彩球阶段、Free Ball、Miss 和 frame 结束判断
- frame 存档、恢复和旧版本迁移

### AI 核心

- 独立 `AICore` 静态库
- 只读球局查询和状态哈希
- Ghost Ball 候选击球生成
- 使用物理服务实例评估候选击球
- 基础进攻 AI
- 固定种子难度、进攻/防守/风险权重和可读评分明细

### 独立调试、实验和性能工具

- `DebugOptions` 和 `snooker_simulate --debug=...`
- 四类物理实验场景：单球、球碰球、库边、袋口
- 每固定物理步、每杆、候选搜索和碰撞类型的性能采样
- 人工球桌测量记录 Markdown/CSV 模板
- 故障包生成和故障复现
- 2000 杆 Release 稳定性测试

## 目录结构

```text
PhysicsCore/       独立物理库
RulesCore/         独立规则库
AICore/            独立 AI 库
UnrealProject/     UE5 项目、插件和资源占位结构
data/              球桌、参数包、规则、快照和回归基线
tests/             C++、CMake 和 Node 验证脚本
tools/             模拟、回放、实验、性能和查看器工具
docs/              技术文档、任务状态和验收归档
```

`build/`、`build-evidence/` 和其他构建中间目录是本地生成目录，不应提交到 Git。关键验收摘要和截图已整理到 [`docs/evidence/`](docs/evidence/)。

## 环境要求

独立核心开发需要：

- Windows
- Visual Studio 2022 或 Build Tools，包含 MSVC 和 Windows SDK
- CMake 3.20 或更高版本
- Node.js，CMake 配置阶段会强制检查 `node`
- Git；仓库配置了 Git LFS 属性，用于未来的 UE 二进制资源

查看器的自动截图验证还需要本机安装 Chrome。UE 相关任务需要 Unreal Engine 5.8.4，但当前独立核心构建不依赖 UE5。

## 快速构建和测试

在仓库根目录执行：

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure --no-tests=error

cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure --no-tests=error
```

也可以使用项目验证脚本，自动完成 Debug/Release 配置、构建和测试，并将日志写入 `build-evidence/<运行名>`：

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/Test-Current.ps1 -Run local-check
```

独立源代码副本验证：

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/Verify-Standalone.ps1 -Run clean-copy-check
```

## 常用工具

### 模拟一杆并导出结果

```powershell
build/Debug/snooker_simulate.exe `
  data/bundles/six-pocket-engineering.bundle `
  data/baselines/center/initial.snapshot `
  data/baselines/center/center.input `
  build/my-shot `
  --debug=trajectory,collision-points,contact-normals,sleeping,physics-steps
```

输出目录包括：

- `record.shot`
- `result.json`
- `trajectory.csv`
- `events.txt`
- `final.snapshot`
- `viewer.json`
- `debug.json`

### 生成物理实验场景

```powershell
build/Debug/snooker_physics_lab.exe `
  data/bundles/six-pocket-engineering.bundle `
  build/physics-lab
```

默认生成 `single-ball`、`ball-collision`、`cushion` 和 `pocket` 四类场景，每个场景包含初始快照、击球输入和场景清单。

### 生成性能采样

```powershell
build/Debug/snooker_performance.exe `
  data/bundles/six-pocket-engineering.bundle `
  data/baselines/follow/initial.snapshot `
  data/baselines/follow/center.input `
  build/performance
```

输出 `performance.csv` 和 `performance-summary.txt`，可定位固定步、候选搜索以及球碰球、库边、袋角和袋口处理耗时。

### 打开 2D 查看器

直接在浏览器打开 [`tools/shot-viewer.html`](tools/shot-viewer.html)，选择同一模拟输出目录中的 `result.json` 和 `trajectory.csv`。也可以使用：

```powershell
node tools/verify-shot-viewer.cjs build/service-cli-Debug-<run>
```

该验证脚本会使用本地 Chrome 的独立用户目录，不依赖网络开发服务器。

## 重要文档

- [`docs/DEVELOPMENT_PLAN.md`](docs/DEVELOPMENT_PLAN.md)：总体技术路线
- [`docs/EXECUTABLE_TASKS.md`](docs/EXECUTABLE_TASKS.md)：121 项可执行任务书
- [`docs/TASK_STATUS.md`](docs/TASK_STATUS.md)：任务状态和验收记录
- [`docs/CORE_PREPARATION.md`](docs/CORE_PREPARATION.md)：独立核心边界和当前能力
- [`docs/VALIDATION_ARCHIVE.md`](docs/VALIDATION_ARCHIVE.md)：历史验收摘要、故障修复和截图索引
- [`docs/SHOT_VIEWER.md`](docs/SHOT_VIEWER.md)：多球 2D 查看器使用说明
- [`docs/DEBUG_OPTIONS.md`](docs/DEBUG_OPTIONS.md)：独立调试开关
- [`docs/PHYSICS_LAB.md`](docs/PHYSICS_LAB.md)：物理实验场景替代方案
- [`docs/PERFORMANCE_SAMPLING.md`](docs/PERFORMANCE_SAMPLING.md)：性能采样说明
- [`docs/MEASUREMENT_RECORD_TEMPLATE.md`](docs/MEASUREMENT_RECORD_TEMPLATE.md)：人工测量记录模板
- [`docs/NETWORK_SYNC_DESIGN.md`](docs/NETWORK_SYNC_DESIGN.md)：联机同步设计

## 当前限制

以下内容仍需要 UE5.8.4 环境，不能用独立 C++ 测试替代完整验收：

- UE 编辑器启动、项目编译和插件实际加载
- DataAsset、Actor、Component、Blueprint 和真实 UE 资源
- UE 调试绘制、Developer Settings 和 Console Variables
- UMG、动画、音效、输入、联机运行时和打包
- Unreal Insights 性能分析
- 最终 3D 世界视图和可玩游戏流程

仓库中的参数和基线是工程回归数据，不代表现实球桌的实测校准结果。真实测量应使用 [`docs/MEASUREMENT_RECORD_TEMPLATE.md`](docs/MEASUREMENT_RECORD_TEMPLATE.md) 记录，并关联明确的 PhysicsBundle 版本。

## 许可和项目状态

当前仓库处于内部开发和验证阶段，未声明正式开源许可证，也不应被视为已经发布的完整游戏。
