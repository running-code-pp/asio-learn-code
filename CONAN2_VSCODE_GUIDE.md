# Conan2 + CMake + VS Code 完整配置指南

## 🎯 项目配置特点

- ✅ **Conan2** 管理所有第三方依赖（ASIO、nlohmann_json、spdlog 等）
- ✅ **compile_commands.json** 自动生成并拷贝到根目录
- ✅ **VS Code IntelliSense** 完全自动化配置
- ✅ **跨平台支持** Windows/Linux

## 🚀 快速开始

### 1. 运行自动配置脚本

```bash
# Windows PowerShell 或 CMD
.\setup_conan_intellisense.bat
```

这个脚本会自动执行：
1. `conan install` - 下载并构建所有依赖
2. `cmake configure` - 配置项目，启用 compile_commands.json
3. `cmake build` - 构建项目，生成 IntelliSense 数据
4. 拷贝 `compile_commands.json` 到根目录
5. 验证 VS Code 配置

### 2. 手动步骤（如果需要）

```bash
# 1. 安装 Conan 依赖
conan install . --output-folder=build --build=missing -s build_type=Debug

# 2. 配置 CMake
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_TOOLCHAIN_FILE=build/conan_toolchain.cmake

# 3. 构建项目
cmake --build build --config Debug
```

## 📁 项目结构

```
your-project/
├── conanfile.txt              # Conan 依赖定义
├── CMakeLists.txt             # CMake 配置
├── compile_commands.json      # 自动生成的编译数据库（给VS Code用）
├── .vscode/
│   ├── c_cpp_properties.json  # VS Code C++ 配置
│   ├── tasks.json             # 构建任务
│   └── launch.json            # 调试配置
├── build/                     # 构建目录
│   ├── compile_commands.json  # CMake 生成的原始文件
│   └── conan_toolchain.cmake  # Conan 生成的工具链
├── src/
├── include/
└── examples/
```

## 🔧 工作流程

### 添加新依赖

1. **编辑 conanfile.txt**：
   ```ini
   [requires]
   asio/1.29.0
   nlohmann_json/3.11.3
   spdlog/1.13.0
   your-new-package/1.0.0  # 添加这行
   ```

2. **重新运行配置脚本**：
   ```bash
   .\setup_conan_intellisense.bat
   ```

3. **VS Code 自动更新**：
   - IntelliSense 会自动识别新的包含路径
   - 代码补全立即可用

### 日常开发

- **构建项目**：`cmake --build build`
- **清理重建**：删除 `build` 目录，重新运行配置脚本
- **调试**：直接在 VS Code 中按 F5

## 📋 依赖管理

### 当前依赖（conanfile.txt）

```ini
[requires]
asio/1.29.0          # 异步网络库
nlohmann_json/3.11.3 # JSON 处理
spdlog/1.13.0        # 日志库
yaml-cpp/0.8.0       # YAML 处理
gtest/1.14.0         # 单元测试框架

[generators]
CMakeDeps            # 生成 CMake find_package 文件
CMakeToolchain       # 生成 CMake 工具链文件
```

### 在 CMake 中使用

```cmake
# 查找包（Conan 自动生成）
find_package(asio REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(spdlog REQUIRED)

# 链接到目标
target_link_libraries(your_target
    asio::asio
    nlohmann_json::nlohmann_json
    spdlog::spdlog
)
```

## 🎯 VS Code IntelliSense 工作原理

1. **CMake 生成**：`CMAKE_EXPORT_COMPILE_COMMANDS=ON` 让 CMake 生成 `compile_commands.json`
2. **自动拷贝**：CMake 后构建事件将文件拷贝到根目录
3. **VS Code 读取**：C/C++ 插件自动读取根目录的 `compile_commands.json`
4. **实时更新**：每次构建后，IntelliSense 自动获取最新的包含路径和编译选项

## ❓ 常见问题

### Q: Conan 安装失败？
A: 
```bash
# 检查 Conan 版本
conan --version

# 检查 profile
conan profile detect --force

# 手动创建 profile
conan profile new default --detect
```

### Q: compile_commands.json 没有生成？
A: 确保有实际的构建目标（如 main.cpp 存在）

### Q: VS Code 还是找不到头文件？
A: 
1. 检查 `c_cpp_properties.json` 中的路径
2. 重启 VS Code
3. 运行 "C/C++: Reset IntelliSense Database"

### Q: 想要 Release 构建？
A: 修改脚本中的 `build_type=Debug` 为 `build_type=Release`

## 🎉 完成！

现在您有了一个完全自动化的现代 C++ 开发环境：
- 依赖管理全自动
- IntelliSense 零配置
- 构建和调试开箱即用

享受高效的 C++ 开发体验！ 🚀