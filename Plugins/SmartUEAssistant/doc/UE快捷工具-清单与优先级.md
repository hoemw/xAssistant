# UE 快捷工具清单与优先级（v0.1 草案）

目标
- 先完成“最常用、最有价值”的编辑器快捷能力；统一入口（命令面板），优化日常高频操作。
- 中文优先描述与检索，兼容英文关键词。

设计原则
- 复用现有工具模块：SelectionTools / ViewportTools / ActorTools / SystemTools / QueryTools。
- 安全默认：危险操作需二次确认；遵守 bSkipConfirmForConsoleCommands。
- 一致的参数与返回：ToolId + 参数结构；返回执行日志与可回显信息。

A. 第一阶段（MVP）清单（最高优先级）
1) 选择与批量
- 按名称/类/标签选择（支持通配与模糊）
- 批量重命名选中 Actor（前缀/序号/日期）
- 批量设置可见性/可渲染/移动到关卡

2) 视口/相机
- 切换到选中 Actor 视角并对焦
- 保存/调用相机书签
- 快速飞行到坐标/对象

3) Actor 常见操作
- 放置指定类 Actor 到坐标/表面（含对齐地面/法线）
- 批量移动/旋转/缩放（支持相对/绝对）
- 批量删除（软删除至回收站）
- 批量重命名（与选择类工具共享）

4) 控制台命令快捷
- 执行常用 r.*、stat.*、t.MaxFPS 等命令（支持收藏与一键执行）
- 遵守 bSkipConfirmForConsoleCommands：常见安全命令可跳过确认

5) 场景查询
- 统计当前关卡 Actor 数量/种类/顶层层级
- 查询缺失引用/重复命名/超大网格（潜在性能问题）

B. 第二阶段（增强）
- 资产批处理：重命名、移动目录、替换材质
- 关卡级操作：打开/保存/切换关卡，打包场景快照
- 灯光环境：一键日夜切换、烘焙设置模板
- 片段库：常用蓝图/C++ 片段插入（只读建议）

C. 命令面板（入口）
- 全局快捷键呼出（默认 Ctrl+Alt+K，可在设置修改）；中文/英文搜索工具名与描述
- 展示参数表单（预设/最近使用）；执行预览与确认
- 分类与收藏；最近使用与推荐

D. 安全与撤销
- 所有非只读操作支持“确认/预览”；
- 尽量走 UE 内置撤销堆栈（可撤销操作）；
- 控制台命令按白名单与快捷清单管理；

E. 参数规范（示例）
- 统一结构：{"ToolId":"BatchRenameSelected","Params":{"Prefix":"Shot","StartIndex":1}}
- 返回：{"Success":true,"Logs":["Renamed 12 actors"],"Preview":null}

F. 与现有代码对接点
- 工具注册与调度：<mcfile name="EditorAIToolRegistry.h" path="d:\Ue\ProjWorkspace\MySmartUEAssistant\Plugins\SmartUEAssistant\Source\SmartUEAssistant\Public\EditorAIToolRegistry.h"></mcfile>
- 工具实现：Plugins/SmartUEAssistant/Source/SmartUEAssistant/Private/Tools/*.cpp
- UI 入口（命令面板挂接）：<mcfile name="AIAssistantWindow.cpp" path="d:\Ue\ProjWorkspace\MySmartUEAssistant\Plugins\SmartUEAssistant\Source\SmartUEAssistant\Private\AIAssistantWindow.cpp"></mcfile>

G. 验收清单（MVP）
- 命令面板可呼出并搜索执行上述工具项（≥ 12 个可用命令）
- 中文检索友好；英文关键词同样可命中
- 非只读操作默认二次确认，可配置跳过控制台安全命令
- 执行结果有清晰日志，可回滚（支持 UE 撤销）

已确认决策
1) MVP：采用 A. 第一阶段清单落地，并加上你日常最常用的一个额外命令（占位：待提供命令名/语句）。
2) 命令面板：使用全局快捷键，默认 Ctrl+Alt+K，可在设置中修改。
3) 控制台命令白名单：预置一批常用命令，并可按需增删：
   - stat.*：stat fps, stat unit, stat sceneRendering, stat memory
   - t.*：t.MaxFPS
   - r.*（安全项）：r.ScreenPercentage, r.ViewDistanceScale, r.BloomQuality, r.DepthOfFieldQuality, r.MotionBlurQuality

H. 参数与返回结构（JSON Schema 草案）
- ToolInvocationRequest
```
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "title": "ToolInvocationRequest",
  "type": "object",
  "required": ["ToolId", "Params"],
  "properties": {
    "ToolId": { "type": "string" },
    "Params": { "type": "object", "additionalProperties": true },
    "Options": {
      "type": "object",
      "properties": {
        "PreviewOnly": { "type": "boolean", "default": false },
        "SkipConfirm": { "type": "boolean", "default": false }
      },
      "additionalProperties": false
    }
  },
  "additionalProperties": false
}
```
- ToolInvocationResult
```
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "title": "ToolInvocationResult",
  "type": "object",
  "required": ["Success"],
  "properties": {
    "Success": { "type": "boolean" },
    "Logs": { "type": "array", "items": { "type": "string" } },
    "Preview": { "type": ["object", "null"] },
    "Error": { "type": ["string", "null"] }
  },
  "additionalProperties": false
}
```
- 示例：批量重命名
```
{
  "ToolId": "BatchRenameSelected",
  "Params": { "Prefix": "Shot", "StartIndex": 1, "Suffix": "_A" },
  "Options": { "PreviewOnly": false }
}
```
- 示例：执行控制台命令（白名单内跳过确认）
```
{
  "ToolId": "ExecConsoleCommand",
  "Params": { "Command": "t.MaxFPS 120" },
  "Options": { "SkipConfirm": true }
}
```

---

附录：Unreal 最实用快捷工具 TOP 20（建议）
1) 选择：按名称/类/标签/关卡批量选择
2) 批量重命名（可设前后缀、序号、起始编号）
3) 成组移动（相对/绝对坐标、对齐地面/法线、吸附网格）
4) 快速对焦/飞行到：选中对象/坐标
5) 保存/跳转相机书签
6) 批量设置可见性/可渲染/投影阴影/可见级别
7) 批量移动到关卡/子关卡
8) 批量删除（带预览与撤销支持）
9) 放置指定类 Actor（支持随机散布/规则网格）
10) 批量替换材质（按Slot/通道）
11) 批量设置 Mobility（Static/Stationary/Movable）
12) 统计场景（Actor 数量/种类/面数阈值警告）
13) 查询缺失引用/重复命名/超大网格
14) 打开内容浏览器并定位到所选资产
15) 批量重定向修复（Fix Up Redirectors）
16) 复制并偏移（沿轨迹/阵列）
17) 切换常见渲染调试：stat.*, r.*（白名单）
18) 一键日夜/环境光预设切换
19) 关卡快速打开/保存/切换
20) 导出场景快照（摄像机与关键统计）