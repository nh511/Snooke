# 球桌人工测量记录模板

本模板用于把一条现实测量完整关联到一个可复现的物理参数版本。工程参数和实测数据分开保存；实测结果不能直接覆盖回归基线。

## 记录身份

| 字段 | 内容 |
|---|---|
| `measurement_id` | 唯一记录号，例如 `table-20261119-001` |
| `measured_at_utc` | ISO-8601 时间 |
| `operator` | 测量人 |
| `location` | 场地/房间 |
| `table_id` | 球桌资产或设备编号 |
| `physics_parameter_version` | 关联的 PhysicsBundle 版本，必填 |
| `rules_parameter_version` | 若涉及规则，填写规则参数版本 |
| `software_build` | 采集工具构建版本 |
| `source_files` | 视频、照片、原始 CSV 的相对路径 |

## 环境

记录温度、相对湿度、布面状态、球和库边清洁状态、照明、相机型号、帧率、快门、镜头和标定板。每个数值必须带单位，未测量项写 `not-measured`，不能留空。

## 几何和动力学字段

| 类别 | 字段示例 |
|---|---|
| 球桌 | 长、宽、布面边界、库边高度、袋口中心、袋口开口、袋深 |
| 球 | 直径、质量、初始位置、初速度、初始旋转 |
| 库边 | 入射速度、反射速度、入射角、反射角、接触点、恢复系数 |
| 袋口 | 进入/弹回/落袋结果、袋角接触点、袋底接触时间 |
| 布面 | 滑动减速度、滚动减速度、旋转衰减、停止时间 |

## 结论和文件约束

每个测量样本至少包含原始数据文件、处理后的数值、单位、误差/不确定度、处理脚本版本和备注。建议文件名为 `measurement_id.json`，并在 JSON 中保存 `physics_parameter_version`。一条记录不能引用多个参数版本；若参数发生变化，复制为新的 `measurement_id`。

CSV 表头可直接使用：

```text
measurement_id,measured_at_utc,operator,location,table_id,physics_parameter_version,quantity,value,unit,uncertainty,method,source_file,notes
```

这份模板不依赖 UE5，`VAL-010` 可独立完成；它记录的是测量流程，不把当前工程值宣称为实测校准值。
