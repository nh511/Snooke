# 独立物理实验场景

`snooker_physics_lab` 是 `Map_PhysicsLab` 的无 UE5 替代方案。它不伪造三维世界，而是生成可被核心服务、脚本和未来查看器共同使用的真实快照与击球输入。

```powershell
build/Debug/snooker_physics_lab.exe data/bundles/six-pocket-engineering.bundle build/physics-lab
```

默认生成四个场景：

- `single-ball`：单球布面运动
- `ball-collision`：母球与目标球碰撞
- `cushion`：库边响应
- `pocket`：袋口进入、袋内接触、落袋或弹回

每个场景包含 `initial.snapshot`、`shot.input` 和 `scenario.json`，根目录包含 `manifest.json`。场景使用当前 PhysicsBundle 版本，验收脚本会检查清单、输入和版本关联。UE5 的 `Map_PhysicsLab`、Actor 和 3D 世界视图仍需后续在 Unreal Engine 5.8.4 中验收，因此 `VAL-001` 的整体状态为 `REVIEW`。
