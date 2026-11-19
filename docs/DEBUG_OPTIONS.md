# 独立开发者调试开关

`PhysicsCore/include/Snooker/Validation/DebugOptions.h` 定义了不依赖 UE5 的调试开关。开关使用稳定的命令行名称，工具和未来的 UE Developer Settings/Console Variable 可以共享同一组名称。

| 开关 | 用途 |
|---|---|
| `trajectory` | 输出和显示逐步轨迹 |
| `collision-points` | 显示碰撞/接触点 |
| `contact-normals` | 显示接触法线 |
| `sleeping` | 高亮睡眠球 |
| `physics-steps` | 显示固定物理步信息 |

使用逗号分隔，可使用 `all` 或 `none`：

```powershell
build/Debug/snooker_simulate.exe data/bundles/six-pocket-engineering.bundle data/baselines/center/initial.snapshot data/baselines/center/center.input build/debug-shot --debug=trajectory,collision-points,contact-normals,sleeping,physics-steps
```

成功运行会生成 `debug.json`，其中包含 `snooker-debug-v1`、位掩码、启用名称和规范化字符串。非法参数会在修改输出前拒绝。UE5 接入时建议将这些名称映射到 `snooker.Debug.*` 控制台变量；本轮不声称 UE 编辑器或运行时验收完成。
