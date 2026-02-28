# 属性修改工具 - 用户指南

## 概述

Smart UE Assistant 提供强大的属性修改系统，让 AI 可以理解自然语言并修改场景中任何对象的属性。

---

## 核心能力

### ✅ 完整的工作流支持

**现在可以这样操作**:
```
用户: "把场景的灯光调整为红色"
AI: 
  ✅ 自动识别目标（所有灯光）
  ✅ 理解语义（颜色 → LightColor）
  ✅ 应用修改（设置为红色）
  ✅ 即时显示（自动刷新视口）
```

---

## 🔥 通用修改工具 (推荐)

### modify - 万能修改工具 ⭐

**一个工具搞定所有属性修改！**

#### 核心特性
- ✅ **AI语义理解**: 自动将"颜色"转换为"LightColor"
- ✅ **全类型支持**: Float, Int, Bool, Vector, Color等
- ✅ **自动刷新**: 修改后立即显示效果
- ✅ **支持撤销**: 完整的Undo/Redo支持

#### 使用方式

**基础用法**:
```
用户: "把灯光改成红色"
AI自动调用: modify {
  "Target": "Light",
  "PropertyName": "LightColor",
  "Value": "red"
}
```

**批量修改**:
```
用户: "把所有立方体隐藏"
AI自动调用: modify {
  "Target": "Cube",
  "PropertyName": "bHidden",
  "Value": true
}
```

**精确控制**:
```
用户: "调整灯光亮度为5000"
AI自动调用: modify {
  "Target": "Light",
  "PropertyName": "Intensity",
  "Value": 5000
}
```

---

## 支持的属性类型

### 1. 灯光属性 (Light)

| 用户表达 | UE属性名 | 值类型 | 示例 |
|---------|----------|--------|------|
| 颜色/color | LightColor | 颜色名或RGB | "red", {"R":1,"G":0,"B":0} |
| 亮度/intensity | Intensity | 数字 | 5000 |
| 半径/radius | AttenuationRadius | 数字 | 1000 |
| 温度/temperature | Temperature | 数字 | 6500 |
| 可见性/visible | bVisible | 布尔 | true, false |

### 2. 变换属性 (Transform)

| 用户表达 | UE属性名 | 值类型 | 示例 |
|---------|----------|--------|------|
| 位置/location | RelativeLocation | 向量 | {"X":0,"Y":0,"Z":100} |
| 旋转/rotation | RelativeRotation | 旋转 | {"Pitch":0,"Yaw":90,"Roll":0} |
| 缩放/scale | RelativeScale3D | 向量 | {"X":2,"Y":2,"Z":2} |

### 3. 通用属性 (Common)

| 用户表达 | UE属性名 | 值类型 | 示例 |
|---------|----------|--------|------|
| 可见性/visible | bHidden | 布尔 | false |
| 标签/tags | Tags | 字符串数组 | ["Important", "Level1"] |
| 名称/name | ActorLabel | 字符串 | "MyActor" |

---

## 高级用法

### 1. 组合选择与修改

```
用户: "把场景中所有红色灯光的亮度调到最大"
AI执行:
  1. 识别目标: 红色灯光
  2. modify {"Target":"Light[Color=Red]", "PropertyName":"Intensity", "Value":10000}
```

### 2. 相对调整

```
用户: "把选中的对象向上移动10米"
AI执行:
  1. 获取当前位置
  2. modify {"Target":"Selected", "PropertyName":"RelativeLocation", "Value":{"X":0,"Y":0,"Z":1000}}
```

### 3. 批量属性

```
用户: "把所有静态网格体设置为可移动"
AI执行:
  modify {"Target":"StaticMeshActor", "PropertyName":"Mobility", "Value":"Movable"}
```

---

## 支持的颜色值

**颜色名称** (中英文都支持):
- 红色/red → (1, 0, 0)
- 绿色/green → (0, 1, 0)
- 蓝色/blue → (0, 0, 1)
- 白色/white → (1, 1, 1)
- 黄色/yellow → (1, 1, 0)
- 青色/cyan → (0, 1, 1)
- 洋红/magenta → (1, 0, 1)
- 橙色/orange → (1, 0.5, 0)

**RGB 对象**:
```json
{"R": 1.0, "G": 0.0, "B": 0.0, "A": 1.0}
```

---

## 其他属性工具

除了通用的 `modify` 工具，还提供了一些专用工具供高级场景使用：

### set_actor_property - 精确属性设置

**当你知道确切的属性路径时使用**

#### 使用场景
- 精确控制特定属性
- 设置嵌套属性
- 避免歧义

#### 参数
```json
{
  "PropertyPath": "LightComponent.Intensity",
  "Value": 5000.0
}
```

#### 属性路径格式
- 直接属性: `"Intensity"`
- 组件属性: `"LightComponent.Intensity"`
- 嵌套属性: `"RootComponent.RelativeLocation"`

#### 支持的值类型
- **数值**: `5000.0`, `100`
- **布尔**: `true`, `false`
- **向量**: `{"X": 0, "Y": 0, "Z": 100}`
- **旋转**: `{"Pitch": 0, "Yaw": 90, "Roll": 0}`
- **颜色**: `{"R": 1, "G": 0, "B": 0}`

---

### batch_set_properties - 批量设置多属性

一次设置多个属性，适合配置复杂对象。

### get_available_properties - 发现可用属性

查询对象有哪些可编辑属性，用于探索和调试。

### adjust_property - 相对调整

支持数值属性的增量调整（加法/乘法）。

### copy_properties - 复制属性

从一个对象复制属性值到其他对象。

---

## 常见问题

### Q1: 为什么属性修改不生效？

**可能原因**:
1. 属性名拼写错误（区分大小写）
2. 对象类型不支持该属性
3. 值类型不匹配

**解决方法**:
- 查看日志输出中的详细错误信息
- 使用 `get_available_properties` 查看可用属性
- 确认值的类型正确

### Q2: AI如何知道属性名？

**答**: AI通过系统提示学习了常用的UE属性映射关系，例如：
- "颜色" → LightColor
- "亮度" → Intensity
- "位置" → RelativeLocation

### Q3: 支持哪些属性类型？

**当前支持**:
- ✅ 基础类型：Float, Int, Bool
- ✅ 向量类型：FVector, FRotator
- ✅ 颜色类型：FLinearColor
- ✅ 字符串类型：FString, FName

### Q4: 如何查看修改日志？

在编辑器输出日志中筛选 `LogSmartUEAssistantTools`，可以看到详细的执行过程。

---

## 技术特性

### 1. AI语义理解
AI自动将自然语言转换为精确的UE属性名，无需硬编码。

### 2. 反射系统
使用UE反射系统动态查找和修改属性，支持任意UObject。

### 3. 事务支持
所有修改支持Undo/Redo，与编辑器完全集成。

### 4. 自动刷新
修改后自动触发视口刷新和属性通知，即时显示效果。

---

**文档版本**: v1.0.6  
**最后更新**: 2025-10-09

