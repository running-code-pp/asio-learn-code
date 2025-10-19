cmake_minimum_required(VERSION 3.16)

project(AsioLearnCode
    VERSION 1.0.0
    DESCRIPTION "ASIO Learning Project"
)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 生成 compile_commands.json 供 VS Code IntelliSense 使用
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 包含自定义 CMake 脚本目录
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# 设置构建类型
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# 设置运行时输出目录（可执行文件、DLL）
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/bin)

# 编译选项（根据编译器类型设置）
if(MSVC)
    set(CMAKE_CXX_FLAGS "/W4")
    set(CMAKE_CXX_FLAGS_DEBUG "/Od /Zi /DEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG")
else()
    set(CMAKE_CXX_FLAGS "-Wall -Wextra")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
endif()

# Conan2 依赖管理
find_package(Threads REQUIRED)

# 添加 Conan 生成的包路径到搜索路径
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}")

# 查找 Conan2 安装的包
find_package(asio REQUIRED)
find_package(nlohmann_json REQUIRED)  
find_package(spdlog REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(OpenSSL REQUIRED)

# 包含目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
# include_directories(${CMAKE_CURRENT_SOURCE_DIR/include/asi})

# 共用库：收集通用源文件
file(GLOB_RECURSE COMMON_SOURCES "src/common/*.cpp" "src/common/*.cc")
file(GLOB_RECURSE COMMON_HEADERS "include/common/*.h" "include/common/*.hpp")

# 创建共用静态库（如果有共用代码）
if(COMMON_SOURCES)
    add_library(asio_common STATIC ${COMMON_SOURCES} ${COMMON_HEADERS})
    target_link_libraries(asio_common 
        Threads::Threads
        asio::asio
        nlohmann_json::nlohmann_json
        spdlog::spdlog
        yaml-cpp::yaml-cpp
    )
    if(WIN32)
        target_link_libraries(asio_common ws2_32 wsock32)
    endif()
endif()

# 定义通用链接库函数
function(add_asio_executable target_name source_file)
    add_executable(${target_name} ${source_file})
    target_link_libraries(${target_name} 
        Threads::Threads
        asio::asio
        nlohmann_json::nlohmann_json
        spdlog::spdlog
        yaml-cpp::yaml-cpp
        OpenSSL::SSL
        OpenSSL::Crypto
    )
    
    # 如果有共用库，链接它
    if(TARGET asio_common)
        target_link_libraries(${target_name} asio_common)
    endif()
    
    # Windows 特定链接库
    if(WIN32)
        target_link_libraries(${target_name} ws2_32 wsock32)

    # 定义ASIO_HAS_CO_AWAIT宏
    target_compile_definitions(${target_name} PRIVATE ASIO_HAS_CO_AWAIT)

    endif()
endfunction()

# === 主要应用程序 ===


add_asio_executable(tcp_echo_client "${CMAKE_CURRENT_SOURCE_DIR}/src/asio_learn/tcp_echo_client.cpp")
add_asio_executable(tcp_echo_server "${CMAKE_CURRENT_SOURCE_DIR}/src/asio_learn/tcp_echo_server.cpp")
add_asio_executable(asio_resolver "${CMAKE_CURRENT_SOURCE_DIR}/src/asio_learn/resolver.cpp")
add_asio_executable(asio_threadpool "${CMAKE_CURRENT_SOURCE_DIR}/src/asio_learn/thread_pool.cpp")
add_asio_executable(io_context_concurrency "${CMAKE_CURRENT_SOURCE_DIR}/src/asio_learn/io_context_concurrency.cpp")
add_asio_executable(strand_test "${CMAKE_CURRENT_SOURCE_DIR}/src/asio_learn/strand_test.cpp")
add_asio_executable(timer_test "${CMAKE_CURRENT_SOURCE_DIR}/src/asio_learn/timer_test.cpp")
add_asio_executable(tcp_server_onethonecli "${CMAKE_CURRENT_SOURCE_DIR}/src/asio_learn/tcp_server_onethonecli.cpp")
add_asio_executable(tcp_server_threadpool "${CMAKE_CURRENT_SOURCE_DIR}/src/asio_learn/tcp_server_threadpool.cpp")
add_asio_executable(tcp_server_masterWork "${CMAKE_CURRENT_SOURCE_DIR}/src/asio_learn/tcp_server_masterWork.cpp")
add_asio_executable(sessionTimeoutTest "${CMAKE_CURRENT_SOURCE_DIR}/src/asio_learn/sessionTimeoutTest.cpp")
add_asio_executable(coroutine_01 "${CMAKE_CURRENT_SOURCE_DIR}/src/coroutine/coroutine_01.cpp")
add_asio_executable(asio_ssl_example "${CMAKE_CURRENT_SOURCE_DIR}/src/asio_learn/asio_ssl_example.cpp")
# === 工具程序 ===
file(GLOB TOOL_SOURCES "src/tools/*.cpp")
foreach(TOOL_SOURCE ${TOOL_SOURCES})
    get_filename_component(TOOL_NAME ${TOOL_SOURCE} NAME_WE)
    add_asio_executable(tool_${TOOL_NAME} ${TOOL_SOURCE})
endforeach()

# === 示例程序 ===
file(GLOB EXAMPLE_SOURCES "examples/*.cpp")
foreach(EXAMPLE_SOURCE ${EXAMPLE_SOURCES})
    get_filename_component(EXAMPLE_NAME ${EXAMPLE_SOURCE} NAME_WE)
    add_asio_executable(example_${EXAMPLE_NAME} ${EXAMPLE_SOURCE})
endforeach()

# 测试配置
enable_testing()

# 查找测试文件
file(GLOB_RECURSE TEST_SOURCES "test/src/*.cpp")
if(TEST_SOURCES)
    # 可以使用 Google Test 或其他测试框架
    # find_package(GTest REQUIRED)
    
    add_executable(test_log ${TEST_SOURCES})
    target_link_libraries(test_log 
        Threads::Threads
        spdlog::spdlog
        yaml-cpp::yaml-cpp)
    if(WIN32)
        target_link_libraries(test_log 
            ws2_32 
            wsock32  
            spdlog::spdlog
            yaml-cpp::yaml-cpp)
    endif()
    
    # target_link_libraries(tests GTest::gtest_main)
    # add_test(NAME AllTests COMMAND tests)
endif()

# 安装配置 - 只安装实际存在的目标
set(INSTALL_TARGETS "")

# 收集所有存在的可执行文件目标
if(TARGET asio_client)
    list(APPEND INSTALL_TARGETS asio_client)
endif()

if(TARGET asio_server)
    list(APPEND INSTALL_TARGETS asio_server)
endif()

# 安装工具程序
file(GLOB TOOL_SOURCES "src/tools/*.cpp")
foreach(TOOL_SOURCE ${TOOL_SOURCES})
    get_filename_component(TOOL_NAME ${TOOL_SOURCE} NAME_WE)
    set(TOOL_TARGET "tool_${TOOL_NAME}")
    if(TARGET ${TOOL_TARGET})
        list(APPEND INSTALL_TARGETS ${TOOL_TARGET})
    endif()
endforeach()

# 安装示例程序
file(GLOB EXAMPLE_SOURCES "examples/*.cpp")
foreach(EXAMPLE_SOURCE ${EXAMPLE_SOURCES})
    get_filename_component(EXAMPLE_NAME ${EXAMPLE_SOURCE} NAME_WE)
    set(EXAMPLE_TARGET "example_${EXAMPLE_NAME}")
    if(TARGET ${EXAMPLE_TARGET})
        list(APPEND INSTALL_TARGETS ${EXAMPLE_TARGET})
    endif()
endforeach()

# 只安装存在的目标
if(INSTALL_TARGETS)
    install(TARGETS ${INSTALL_TARGETS}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
    )
endif()

install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)

# 打印配置信息
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Export compile commands: ${CMAKE_EXPORT_COMPILE_COMMANDS}")
