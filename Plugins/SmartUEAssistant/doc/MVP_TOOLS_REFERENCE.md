# Smart UE Assistant - MVP 工具快速参考

## 完整工具列表 (30个)

### 🎯 批量操作工具 (7个) - **MVP 新增**

| 工具ID | 功能 | 主要参数 | 权限 |
|--------|------|---------|------|
| `batch_rename_actors` | 批量重命名 | Prefix, Suffix, StartIndex | Modify |
| `batch_set_visibility` | 批量可见性 | Visible, ApplyToChildren | Modify |
| `batch_set_mobility` | 批量移动性 | Mobility (Static/Stationary/Movable) | Modify |
| `batch_move_to_level` | 移动到关卡 | LevelName | Modify |
| `batch_set_tags` | 批量标签 | Tags[], Mode (Set/Add/Remove) | Modify |
| `align_to_ground` | 对齐地面 | AlignRotation, Offset | Modify |
| `distribute_actors` | 分布模式 | Pattern (Line/Grid/Circle/Random), Spacing | Modify |

### 📷 相机书签工具 (4个) - **MVP 新增**

| 工具ID | 功能 | 主要参数 | 权限 |
|--------|------|---------|------|
| `save_camera_bookmark` | 保存书签 | Name, Description | Safe |
| `jump_to_camera_bookmark` | 跳转书签 | Name | Safe |
| `list_camera_bookmarks` | 列出书签 | 无 | Safe |
| `delete_camera_bookmark` | 删除书签 | Name | Modify |

### 📊 场景分析工具 (5个) - **MVP 新增**

| 工具ID | 功能 | 主要参数 | 权限 |
|--------|------|---------|------|
| `analyze_level_stats` | 关卡统计 | 无 | Safe |
| `find_missing_references` | 查找缺失引用 | 无 | Safe |
| `find_duplicate_names` | 查找重复名称 | 无 | Safe |
| `find_oversized_meshes` | 查找超大网格 | VertexThreshold | Safe |
| `validate_level` | 验证关卡 | 无 | Safe |

### 🎭 Actor 操作工具 (5个)

| 工具ID | 功能 | 主要参数 | 权限 |
|--------|------|---------|------|
| `select_focus_actor` | 选择并聚焦 | ActorName | Safe |
| `set_actor_transform` | 设置变换 | ActorName, Location, Rotation, Scale | Modify |
| `create_actor_basic` | 创建 Actor | Type, Location | Modify |
| `delete_actor` | 删除 Actor | ActorName | Dangerous |
| `transform_actors_delta` | 相对变换 | Delta{Location/Rotation/Scale} | Modify |

### 🔍 查询工具 (3个)

| 工具ID | 功能 | 主要参数 | 权限 |
|--------|------|---------|------|
| `list_actors` | 列出 Actor | ClassFilter | Safe |
| `get_actor_properties` | 获取属性 | ActorName | Safe |
| `list_selection_presets` | 列出预设 | 无 | Safe |

### ✅ 选择工具 (1个)

| 工具ID | 功能 | 主要参数 | 权限 |
|--------|------|---------|------|
| `select_actors_by_rule` | 规则选择 | Class, Name, Tag, Mode | Safe |

### 👁️ 视口工具 (1个)

| 工具ID | 功能 | 主要参数 | 权限 |
|--------|------|---------|------|
| `focus_viewport` | 聚焦视口 | Target | Safe |

### ⚙️ 系统工具 (3个)

| 工具ID | 功能 | 主要参数 | 权限 |
|--------|------|---------|------|
| `run_console_command` | 控制台命令 | Command | Dangerous |
| `save_level` | 保存关卡 | 无 | Dangerous |
| `pie_control` | PIE 控制 | Action (Start/Stop) | Safe |

### 💡 灯光工具 (1个)

| 工具ID | 功能 | 主要参数 | 权限 |
|--------|------|---------|------|
| `set_light_property` | 设置灯光属性 | ActorName, Property, Value | Modify |

---

## 快速使用指南

### 通过 AI 自然语言调用

```cpp
// 示例 1: 批量重命名
FAIService::Get().SendMessageWithTools(
    TEXT("将选中的 Actor 重命名为 'Prop_' 开头，从1开始编号"),
    Callback
);
// 自动调用: batch_rename_actors {"Prefix":"Prop_","StartIndex":1}

// 示例 2: 保存相机位置
FAIService::Get().SendMessageWithTools(
    TEXT("保存当前相机位置为 'MainView'"),
    Callback
);
// 自动调用: save_camera_bookmark {"Name":"MainView"}

// 示例 3: 分析场景
FAIService::Get().SendMessageWithTools(
    TEXT("分析当前关卡的统计信息"),
    Callback
);
// 自动调用: analyze_level_stats {}
```

### 直接通过注册表调用

```cpp
auto& Registry = FEditorAIToolRegistry::Get();

// 批量重命名
TSharedPtr<FJsonObject> Args = MakeShareable(new FJsonObject);
Args->SetStringField(TEXT("Prefix"), TEXT("Wall_"));
Args->SetNumberField(TEXT("StartIndex"), 1);
FAIToolResult Result = Registry.Dispatch(TEXT("batch_rename_actors"), Args);

// 保存相机书签
TSharedPtr<FJsonObject> BookmarkArgs = MakeShareable(new FJsonObject);
BookmarkArgs->SetStringField(TEXT("Name"), TEXT("TopView"));
BookmarkArgs->SetStringField(TEXT("Description"), TEXT("俯视图"));
Registry.Dispatch(TEXT("save_camera_bookmark"), BookmarkArgs);
```

---

## 权限级别说明

- **Safe**: 只读操作，无需确认
- **Modify**: 修改场景，可能需要预览
- **Dangerous**: 危险操作，需要用户确认

---

## 返回结构

所有工具返回统一的 `FAIToolResult` 结构：

```cpp
struct FAIToolResult {
    bool bSuccess;              // 是否成功
    FString Message;            // 结果消息
    TSharedPtr<FJsonObject> Data; // 可选的结构化数据
};
```

---

**文档版本**: v1.0.0  
**更新日期**: 2025-10-09  
**工具总数**: 30 (MVP新增16个)

