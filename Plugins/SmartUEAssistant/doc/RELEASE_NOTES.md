# Smart UE Assistant - Release Notes

## v1.0.8 (2025-10-09) - AI输出文本可复制

### 🎯 核心改进

#### AI输出文本选择与复制功能
- **用户体验提升**: 解决AI输出文本无法选择和复制的问题
- **控件升级**: 将 `STextBlock` 替换为只读模式的 `SMultiLineEditableTextBox`
- **流式显示优化**: 流式显示消息同样支持文本选择和复制
- **视觉一致性**: 保持透明背景和原有样式，用户体验无缝衔接

### 🛠️ 技术实现

#### 文本控件改进
- **普通消息**: 使用 `SMultiLineEditableTextBox` 替代 `STextBlock`
- **只读模式**: 设置 `IsReadOnly(true)` 允许选择但不允许编辑
- **右键菜单**: 启用 `AllowContextMenu(true)` 支持复制操作
- **样式优化**: 透明背景、无边框、无滚动条，完美融入气泡

#### 流式显示支持
- **新增成员**: `CurrentAIStreamingEditableBlock` 跟踪可编辑文本框
- **实时更新**: 通过 `SetText()` 方法更新流式显示内容
- **性能优化**: 保持原有的打字机效果和流畅度

### ✅ 用户功能

现在可以：
- ✅ 鼠标拖选AI输出的任意文本
- ✅ 使用 `Ctrl+C` 快捷键复制
- ✅ 右键菜单选择复制
- ✅ 复制部分文本或全部内容
- ✅ 流式显示时也可以复制已显示的内容

保持不变：
- ✅ 视觉样式完全一致（颜色、字体、气泡）
- ✅ 流式显示的打字机效果
- ✅ 消息布局和间距
- ✅ 时间戳显示

### 📦 修改文件
- `AIAssistantWindow.h` - 新增可编辑文本框成员变量
- `AIAssistantWindow.cpp` - 实现文本选择和复制功能

### 兼容性
- 向后兼容：保持既有UI布局和交互
- 引擎版本：UE5.0+（在 UE5.6 上验证）
- 无需额外配置

### 升级指引
1. 重新编译插件
2. 重启Unreal Editor
3. AI输出文本即可选择和复制

---

## v1.0.6 (2025-10-09) - AI语义理解系统

### 🎯 核心改进

#### AI语义理解模式
- **重大改进**: 将属性语义理解职责从工具代码转移到AI层
- **代码简化**: 工具代码从100+行硬编码缩减至3行
- **智能提升**: AI可自动将用户的自然语言转换为精确的UE属性名

#### 通用修改工具
- **新增**: `modify` 工具 - 支持所有UE对象属性修改
- **参数优化**: 使用精确的UE属性名（如 `LightColor`, `Intensity`）
- **工具整合**: 替代之前的多个专用属性修改工具

#### 属性修改系统增强
- **新增**: `PropertyModificationHelper` 辅助类
- **支持**: 完整的事务系统（Undo/Redo）
- **自动刷新**: 修改后自动刷新视口，即时显示效果
- **通知机制**: 正确触发 `PostEditChangeProperty` 确保编辑器状态同步

#### 日志系统完善
- **详细追踪**: 属性修改全流程日志（查找→修改→通知→刷新）
- **问题诊断**: 便于排查属性修改失败的具体原因
- **性能监控**: 记录关键操作的执行状态

### 🛠️ 技术细节

#### 系统提示优化
- 新增详细的属性名映射表
- 提供标准使用示例
- 明确AI的语义理解职责

#### 支持的属性类型
- 基础类型：Float, Int, Bool
- 向量类型：FVector, FRotator, FLinearColor
- 字符串类型：FString, FName, FText

#### 使用示例
```
用户: "把灯光改成绿色"
AI调用: modify {"Target":"Light", "PropertyName":"LightColor", "Value":"green"}
结果: ✅ 灯光颜色立即变为绿色
```

### 📦 新增文件
- `PropertyModificationHelper.h/cpp` - 属性修改辅助类
- `UniversalModifyTool.h/cpp` - 通用修改工具
- `QuickLightTools.h/cpp` - 快捷灯光工具

### 🗑️ 移除
- 简化了部分冗余的专用属性工具
- 整合了分散的属性修改逻辑

### ⚡ 性能优化
- 减少硬编码字符串匹配
- 优化属性查找流程
- 自动批量刷新优化

### 兼容性
- 向后兼容：保持既有API接口
- 引擎版本：UE5.0+（在 UE5.6 上验证）
- 现有工具不受影响

### 升级指引
1. 直接升级并重启编辑器
2. AI会自动使用新的语义理解模式
3. 旧的自然语言指令仍然有效
4. 查看日志以了解详细执行过程

### 文档更新
- 新增 `DEVELOPMENT_SUMMARY.md` - 综合开发文档
- 更新 `PROPERTY_TOOLS_USER_GUIDE.md` - 属性工具指南
- 整合并删除冗余的临时文档

---

## v1.0.0 (2025-09-10)

### 新增与改进
- 服务层：注入受保护的 system 角色提示词，防止敏感约束被下游回显
- UI 层：聊天区增加敏感内容清洗，避免任何 system 文本泄露
- 日志：去敏与结构化输出，默认屏蔽密钥与请求体敏感字段
- 构建：Windows 本地构建脚本 BuildSmartUEAssistant.bat 验证通过

### 兼容性
- 向后兼容：保持既有接口，默认启用安全策略
- 引擎版本：UE5.0+（在 UE5.6 上验证）

### 升级指引
- 如未自定义配置，直接按常规方式升级并重启编辑器
- 已自定义日志或UI回显逻辑的项目，建议复核是否与新的清洗策略冲突

### 已知问题
- 暂无

---
如遇问题，请查看 Config/DefaultSmartUEAssistant.ini 与日志配置，并根据需要调整。

## v1.0.1 (2025-09-10)

### 修复
- 编辑器保存：将 EditorLoadingAndSavingUtils 实现替换为 UnrealEd 的 FileHelpers.h + FEditorFileUtils::SaveDirtyPackages，跨版本更稳健；按 UE5 签名传参（bPromptUserToSave=false, bSaveMapPackages=true, bSaveContentPackages=true）。
- AIService：为 GetSystemPrompt 增加 const，修正 (*Found)->Get().GetSpec() 误用为 (*Found)->GetSpec()。
- 依赖清理：移除不再需要的 EditorScriptingUtilities（Build.cs 与 .uplugin）。

### 验证
- Windows 平台，本地完整构建通过（BuildSmartUEAssistant.bat）。
- 关键文件：BuiltInEditorTools.cpp/h, AIService.cpp/h, SmartUEAssistant.Build.cs, SmartUEAssistant.uplugin。

### 兼容性与影响
- 仅影响编辑器“保存所有脏包/关卡”工具的内部实现；无交互保存，无运行时影响。

### 升级指引
- 直接升级；如自定义保存流程，请确认与新的 SaveDirtyPackages 调用兼容。

## v1.0.2 (2025-09-11)

### 修复与清理
- 弃用API清理：移除 ANY_PACKAGE 用法，改为使用具体 Actor 类型 StaticClass（APointLight/ADirectionalLight/AStaticMeshActor），并补充头文件包含。

### 验证
- Windows 平台，本地完整构建通过；未再出现 ANY_PACKAGE 弃用相关警告。

### 影响范围
- 仅影响内置编辑器工具中“创建基础Actor”功能的内部实现；无 UI 变化、无运行时影响。

### 升级指引
- 直接升级，无需额外配置或迁移步骤。