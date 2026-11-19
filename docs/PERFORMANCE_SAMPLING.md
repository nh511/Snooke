# 独立物理性能采样

`ISnookerPhysicsService::GetPerformanceSamples()` 为每个成功固定物理步记录：步号、模拟时间、墙钟耗时、候选搜索耗时、球碰球/库边/袋角/袋内接触/袋口进入/弹回处理耗时、事件总数以及各事件分类次数。

```powershell
build/Debug/snooker_performance.exe data/bundles/six-pocket-engineering.bundle data/baselines/center/initial.snapshot data/baselines/center/center.input build/performance
```

输出：

- `performance.csv`：逐步记录和最后一行 `sample_kind=shot` 的整杆汇总
- `performance-summary.txt`：`snooker-performance-v1` 摘要、固定步数、求解器耗时和事件数

采样只读服务已经产生的物理结果，不改变状态或事件序列。Debug/Release 可用同一格式比较慢步和碰撞热点。Unreal Insights 标记、线程时间线和目标硬件采样仍需 UE5/目标设备，因此 `REL-001` 的整体状态为 `REVIEW`。
