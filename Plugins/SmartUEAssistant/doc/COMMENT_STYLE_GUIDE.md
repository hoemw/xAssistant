# 代码注释规范 / Code Comment Style Guide

**版本 / Version**: 1.0  
**更新日期 / Last Updated**: 2025-10-09

---

## 📋 目标 / Objectives

本规范旨在统一 SmartUEAssistant 插件的代码注释风格，确保：
This guide aims to standardize code comment style in SmartUEAssistant plugin, ensuring:

1. **双语支持 / Bilingual Support**: 中文 + 英文，方便国内外开发者
2. **一致性 / Consistency**: 统一的格式和风格
3. **可读性 / Readability**: 清晰易懂的说明
4. **可维护性 / Maintainability**: 便于后续更新和维护

---

## 🎯 核心原则 / Core Principles

### 1. 双语注释格式 / Bilingual Comment Format

**文件级注释 / File-Level Comments**:
```cpp
/**
 * 中文描述
 * English description
 * 
 * 详细说明（中文）
 * Detailed explanation (English)
 */
```

**函数/方法注释 / Function/Method Comments**:
```cpp
/**
 * 中文功能说明
 * English function description
 * 
 * @param ParamName 参数说明（中文）/ Parameter description (English)
 * @return 返回值说明（中文）/ Return value description (English)
 */
```

**成员变量注释 / Member Variable Comments**:
```cpp
/** 中文说明 / English description */
Type MemberVariable;
```

**行内注释 / Inline Comments**:
```cpp
// 中文说明
// English description
SomeCode();
```

---

## 📝 具体规范 / Detailed Guidelines

### 类/结构体注释 / Class/Struct Comments

```cpp
/**
 * 类的中文描述
 * Class description in English
 * 
 * 详细功能说明（中文）
 * 包含使用场景、设计意图等
 * 
 * Detailed functionality (English)
 * Including use cases, design intent, etc.
 * 
 * 示例用法 / Example Usage:
 *   FMyClass Instance;
 *   Instance.DoSomething();
 */
class FMyClass
{
    // ...
};
```

### 函数注释 / Function Comments

```cpp
/**
 * 函数的中文描述
 * Function description in English
 * 
 * @param InputValue 输入参数的中文说明 / English description of input parameter
 * @param bFlag 布尔参数说明（中文）/ Boolean parameter description (English)
 * @return 返回值的中文说明 / English description of return value
 */
bool MyFunction(int32 InputValue, bool bFlag)
{
    // 实现细节注释
    // Implementation detail comments
    return true;
}
```

### 成员变量注释 / Member Variable Comments

```cpp
class FMyClass
{
public:
    /** 公共成员的中文说明 / Public member description in English */
    int32 PublicValue;

private:
    /** 私有成员的中文说明 / Private member description in English */
    FString PrivateData;
    
    /** 线程安全访问锁 / Thread-safe access lock */
    FCriticalSection AccessLock;
};
```

### 枚举注释 / Enum Comments

```cpp
/**
 * 枚举类型的中文描述
 * Enum type description in English
 */
enum class EMyEnum : uint8
{
    /** 选项一的中文说明 / Option one description */
    Option1 = 0,
    
    /** 选项二的中文说明 / Option two description */
    Option2 = 1
};
```

### 宏定义注释 / Macro Comments

```cpp
/**
 * 宏的中文描述
 * Macro description in English
 * 
 * 用法 / Usage:
 *   MY_MACRO(Parameter)
 */
#define MY_MACRO(Param) \
    // 宏实现 / Macro implementation
```

---

## 🔧 特殊情况 / Special Cases

### 1. 复杂算法 / Complex Algorithms

对于复杂算法，提供分步骤的双语注释：
For complex algorithms, provide step-by-step bilingual comments:

```cpp
void ComplexAlgorithm()
{
    // 步骤 1: 初始化数据结构
    // Step 1: Initialize data structures
    InitializeData();
    
    // 步骤 2: 执行主要计算
    // Step 2: Perform main calculation
    PerformCalculation();
    
    // 步骤 3: 清理资源
    // Step 3: Cleanup resources
    Cleanup();
}
```

### 2. 临时代码/待办事项 / Temporary Code/TODOs

```cpp
// TODO(中文): 需要优化性能
// TODO(English): Need to optimize performance
SomeCode();

// FIXME(中文): 修复边界情况
// FIXME(English): Fix edge case
AnotherCode();
```

### 3. 已废弃的代码 / Deprecated Code

```cpp
/**
 * 已废弃：请使用 NewFunction() 代替
 * Deprecated: Use NewFunction() instead
 * 
 * @deprecated 将在 v2.0 中移除 / Will be removed in v2.0
 */
void OldFunction()
{
    // ...
}
```

---

## ✅ 最佳实践 / Best Practices

### 1. 保持简洁 / Keep It Concise

✅ **好的示例 / Good Example**:
```cpp
/** 注册新工具 / Register a new tool */
void Register(TSharedRef<IEditorAITool> Tool);
```

❌ **不好的示例 / Bad Example**:
```cpp
/** 这个函数用于注册一个新的工具到注册表中，工具必须实现 IEditorAITool 接口...（过长）
    This function is used to register a new tool into the registry, the tool must implement... (too long) */
void Register(TSharedRef<IEditorAITool> Tool);
```

### 2. 先中文后英文 / Chinese First, Then English

保持一致的顺序：
Maintain consistent order:

```cpp
/** 中文说明 / English description */
```

### 3. 使用标准术语 / Use Standard Terminology

| 中文 | English |
|------|---------|
| 注册 | Register |
| 注销 | Unregister |
| 调度 | Dispatch |
| 执行 | Execute |
| 初始化 | Initialize |
| 清理 | Cleanup |
| 验证 | Validate |
| 配置 | Configure |

---

## 📚 代码示例 / Code Examples

### 完整类示例 / Complete Class Example

```cpp
/**
 * 自动工具注册系统
 * Automatic tool registration system
 * 
 * 该类提供工具的自动注册机制，消除手动注册的需要。
 * This class provides automatic tool registration mechanism, eliminating manual registration.
 */
class FToolAutoRegister
{
public:
    /**
     * 构造函数 - 自动注册工具
     * Constructor - Automatically registers the tool
     */
    FToolAutoRegister()
    {
        // 注册工具到全局注册表
        // Register tool to global registry
        FEditorAIToolRegistry::Get().Register(CreateTool());
    }
    
    /**
     * 析构函数 - 清理资源
     * Destructor - Cleanup resources
     */
    ~FToolAutoRegister()
    {
        // 可选：注销工具
        // Optional: Unregister tool
    }

private:
    /**
     * 创建工具实例
     * Create tool instance
     * 
     * @return 工具的共享引用 / Shared reference to the tool
     */
    TSharedRef<IEditorAITool> CreateTool();
    
    /** 工具名称 / Tool name */
    FString ToolName;
};
```

---

## 🔍 检查清单 / Checklist

在提交代码前，确保：
Before submitting code, ensure:

- [ ] 所有公共接口都有双语注释
- [ ] All public interfaces have bilingual comments

- [ ] 函数参数和返回值都有说明
- [ ] Function parameters and return values are documented

- [ ] 复杂逻辑有详细的实现注释
- [ ] Complex logic has detailed implementation comments

- [ ] 中英文注释内容一致
- [ ] Chinese and English comments are consistent

- [ ] 使用了标准术语
- [ ] Standard terminology is used

- [ ] 注释格式符合规范
- [ ] Comment format follows the guidelines

---

## 🛠️ 工具支持 / Tool Support

### VS Code / Visual Studio

推荐使用以下扩展辅助双语注释：
Recommended extensions for bilingual comments:

1. **Better Comments** - 高亮不同类型的注释
2. **Doxygen Documentation Generator** - 快速生成文档注释
3. **Code Spell Checker** - 拼写检查（支持中英文）

### 注释模板 / Comment Templates

创建代码片段模板：
Create code snippet templates:

```json
{
    "Bilingual Function Comment": {
        "prefix": "cfunc",
        "body": [
            "/**",
            " * ${1:中文描述}",
            " * ${2:English description}",
            " * ",
            " * @param ${3:param} ${4:参数说明} / ${5:Parameter description}",
            " * @return ${6:返回值说明} / ${7:Return description}",
            " */"
        ]
    }
}
```

---

## 📖 参考资源 / References

- [Doxygen 文档规范](https://www.doxygen.nl/manual/docblocks.html)
- [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
- [Unreal Engine Coding Standard](https://docs.unrealengine.com/en-US/ProductionPipelines/DevelopmentSetup/CodingStandard/)

---

## 🔄 版本历史 / Version History

### v1.0 (2025-10-09)
- ✅ 初始版本发布 / Initial release
- ✅ 定义双语注释规范 / Define bilingual comment standards
- ✅ 提供示例和最佳实践 / Provide examples and best practices

---

**维护者 / Maintainer**: SmartUEAssistant Team  
**反馈 / Feedback**: 欢迎提出改进建议 / Suggestions for improvement are welcome

