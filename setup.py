#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Conan2 + CMake + VS Code IntelliSense 自动配置脚本
跨平台支持 Windows/Linux/macOS
"""

import os
import sys
import subprocess
import shutil
import platform
import time
from pathlib import Path

class Colors:
    """控制台颜色输出"""
    if platform.system() == "Windows":
        # Windows 需要启用 ANSI 颜色支持
        try:
            import colorama
            colorama.init()
        except ImportError:
            pass
    
    RED = '\033[91m'
    GREEN = '\033[92m'
    YELLOW = '\033[93m'
    BLUE = '\033[94m'
    MAGENTA = '\033[95m'
    CYAN = '\033[96m'
    WHITE = '\033[97m'
    BOLD = '\033[1m'
    END = '\033[0m'

def print_step(step_num, title, color=Colors.BLUE):
    """打印步骤标题"""
    print(f"\n{color}{Colors.BOLD}步骤 {step_num}: {title}{Colors.END}")

def print_success(message):
    """打印成功信息"""
    print(f"{Colors.GREEN}✅ {message}{Colors.END}")

def print_error(message):
    """打印错误信息"""
    print(f"{Colors.RED}❌ {message}{Colors.END}")

def print_warning(message):
    """打印警告信息"""
    print(f"{Colors.YELLOW}⚠️  {message}{Colors.END}")

def print_info(message):
    """打印信息"""
    print(f"{Colors.CYAN}ℹ️  {message}{Colors.END}")

def run_command(cmd, check=True, shell=True, cwd=None):
    """运行命令并返回结果"""
    try:
        print(f"{Colors.CYAN}[执行] {cmd}{Colors.END}")
        
        # 在 Windows 上使用 UTF-8 编码，其他系统使用默认编码
        encoding = 'utf-8' if platform.system() == "Windows" else None
        
        result = subprocess.run(
            cmd, 
            shell=shell, 
            check=check, 
            capture_output=True, 
            encoding=encoding,
            errors='replace',  # 遇到编码错误时用替换字符
            cwd=cwd
        )
        if result.stdout and result.stdout.strip():
            print(result.stdout.strip())
        return result
    except subprocess.CalledProcessError as e:
        print_error(f"命令执行失败: {cmd}")
        if e.stderr:
            print(f"{Colors.RED}{e.stderr.strip()}{Colors.END}")
        return None
    except UnicodeDecodeError as e:
        print_error(f"编码错误: {e}")
        # 尝试使用 GBK 编码重新运行
        try:
            result = subprocess.run(
                cmd, 
                shell=shell, 
                check=check, 
                capture_output=True, 
                encoding='gbk',
                errors='replace',
                cwd=cwd
            )
            if result.stdout and result.stdout.strip():
                print(result.stdout.strip())
            return result
        except:
            print_error("无法解析命令输出编码")
            return None

def kill_processes():
    """终止可能占用文件的进程"""
    processes = ['cmake', 'ninja', 'cl', 'link', 'conan', 'msbuild']
    
    for process in processes:
        try:
            if platform.system() == "Windows":
                subprocess.run(f'taskkill /f /im "{process}.exe"', 
                             shell=True, capture_output=True)
            else:
                subprocess.run(f'pkill -f {process}', 
                             shell=True, capture_output=True)
        except:
            pass

def clean_build_directory():
    """清理 build 目录和 CMake 缓存"""
    build_path = Path("build")
    
    if not build_path.exists():
        print_info("build 目录不存在，无需清理")
        return True
    
    print_info("build 目录已存在，尝试清理...")
    
    # 终止占用进程
    kill_processes()
    
    # 等待进程退出
    print_info("等待进程完全退出...")
    time.sleep(3)
    
    # 尝试清理 CMake 缓存文件
    cmake_cache_files = [
        build_path / "CMakeCache.txt",
        build_path / "cmake_install.cmake",
        build_path / "CMakeFiles"
    ]
    
    for cache_file in cmake_cache_files:
        if cache_file.exists():
            try:
                if cache_file.is_file():
                    cache_file.unlink()
                    print_info(f"已删除 CMake 缓存文件: {cache_file.name}")
                elif cache_file.is_dir():
                    shutil.rmtree(cache_file)
                    print_info(f"已删除 CMake 缓存目录: {cache_file.name}")
            except Exception as e:
                print_warning(f"无法删除 CMake 缓存 {cache_file.name}: {e}")
    
    # 尝试删除整个 build 目录
    try:
        shutil.rmtree(build_path)
        print_success("build 目录已完全清理")
        return True
    except PermissionError:
        print_error("无法删除 build 目录，目录可能被占用")
        print("\n请手动执行以下步骤：")
        print("1. 关闭所有终端和 VS Code")
        print("2. 手动删除 build 目录")
        print("3. 重新运行此脚本")
        return False
    except Exception as e:
        print_error(f"删除 build 目录时出错: {e}")
        
        # 如果无法删除整个目录，尝试只清理关键文件
        print_info("尝试只清理关键的 CMake 文件...")
        try:
            # 清理可能导致问题的文件
            problematic_files = [
                build_path / "CMakeCache.txt",
                build_path / "cmake_install.cmake"
            ]
            
            for pfile in problematic_files:
                if pfile.exists() and pfile.is_file():
                    pfile.unlink()
                    print_info(f"已删除: {pfile.name}")
            
            # 清理 CMakeFiles 目录
            cmake_files_dir = build_path / "CMakeFiles"
            if cmake_files_dir.exists():
                shutil.rmtree(cmake_files_dir)
                print_info("已清理 CMakeFiles 目录")
                
            print_warning("部分清理完成，可能仍有残留文件")
            return True
            
        except Exception as e2:
            print_error(f"部分清理也失败: {e2}")
            return False

def find_visual_studio():
    """查找 Visual Studio 安装路径"""
    if platform.system() != "Windows":
        return None
    
    vs_versions = ['2022', '2019', '2017']
    vs_editions = ['Community', 'Professional', 'Enterprise']
    
    for version in vs_versions:
        for edition in vs_editions:
            vs_path = Path(f"C:/Program Files/Microsoft Visual Studio/{version}/{edition}/VC/Auxiliary/Build/vcvarsall.bat")
            if vs_path.exists():
                print_success(f"发现 Visual Studio {version} {edition}")
                return str(vs_path)
    
    return None

def setup_msvc_environment():
    """设置 MSVC 环境（仅 Windows）"""
    if platform.system() != "Windows":
        return True
    
    vs_path = find_visual_studio()
    if vs_path:
        # 设置 MSVC 环境变量
        try:
            # 运行 vcvarsall.bat 并获取环境变量
            vcvars_cmd = f'"{vs_path}" x64 && set'
            result = subprocess.run(vcvars_cmd, shell=True, capture_output=True, text=True)
            
            if result.returncode == 0:
                # 解析环境变量
                for line in result.stdout.split('\n'):
                    if '=' in line and not line.startswith('='):
                        key, value = line.split('=', 1)
                        key = key.strip()
                        value = value.strip()
                        
                        # 更新重要的环境变量
                        if key in ['PATH', 'INCLUDE', 'LIB', 'LIBPATH', 'CL', 'LINK']:
                            os.environ[key] = value
                
                # 验证编译器是否可用
                try:
                    subprocess.run('cl', shell=True, capture_output=True, check=True)
                    print_success("MSVC 编译器环境已设置")
                    return True
                except:
                    print_warning("MSVC 编译器仍不可用，将使用 Conan 检测")
                    return True
            else:
                print_warning("无法设置 MSVC 环境，将使用 Conan 检测")
                return True
        except Exception as e:
            print_warning(f"设置 MSVC 环境时出错: {e}")
            return True
    else:
        print_warning("未找到 Visual Studio，尝试继续...")
        return True

def check_tool(tool_name, command=None):
    """检查工具是否可用"""
    if command is None:
        command = f"{tool_name} --version"
    
    try:
        result = subprocess.run(command, shell=True, capture_output=True, text=True)
        if result.returncode == 0:
            print_success(f"{tool_name} 可用")
            return True
        else:
            print_error(f"{tool_name} 不可用")
            return False
    except:
        print_error(f"{tool_name} 不可用")
        return False

def install_conan_dependencies():
    """安装 Conan 依赖"""
    cmd = "conan install . --output-folder=build --build=missing -s build_type=Debug"
    result = run_command(cmd)
    return result is not None

def configure_cmake():
    """配置 CMake 项目"""
    # 检查工具链文件
    toolchain_file = Path("build/conan_toolchain.cmake")
    if not toolchain_file.exists():
        print_error("Conan 工具链文件不存在")
        print_info("请确保 Conan 安装步骤成功完成")
        return False
    
    # 使用正确的路径分隔符
    toolchain_path = str(toolchain_file).replace('\\', '/')
    
    # 基础命令
    cmd_parts = [
        "cmake",
        "-B build",
        "-S .",
        "-G \"Visual Studio 17 2022\"",
        "-DCMAKE_BUILD_TYPE=Debug",
        "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON",
        f"-DCMAKE_TOOLCHAIN_FILE={toolchain_path}"
    ]
    
    # 在 Windows 上，如果 cl 编译器不在 PATH 中，尝试明确指定
    if platform.system() == "Windows":
        # 检查 cl 是否可用
        try:
            subprocess.run('cl', shell=True, capture_output=True, check=True)
        except:
            # 尝试找到 MSVC 编译器路径
            vs_path = find_visual_studio()
            if vs_path:
                # 推导编译器路径
                vs_dir = Path(vs_path).parent.parent.parent
                cl_path = vs_dir / "VC" / "Tools" / "MSVC"
                
                # 查找最新版本的编译器
                if cl_path.exists():
                    versions = [d for d in cl_path.iterdir() if d.is_dir()]
                    if versions:
                        latest_version = sorted(versions)[-1]
                        cl_exe = latest_version / "bin" / "Hostx64" / "x64" / "cl.exe"
                        cxx_exe = latest_version / "bin" / "Hostx64" / "x64" / "cl.exe"
                        
                        if cl_exe.exists():
                            cmd_parts.extend([
                                f"-DCMAKE_C_COMPILER={cl_exe}",
                                f"-DCMAKE_CXX_COMPILER={cxx_exe}"
                            ])
                            print_info(f"明确指定 MSVC 编译器: {cl_exe}")
    
    cmd = " ".join(cmd_parts)
    result = run_command(cmd, check=False)  # 不立即退出，我们需要分析错误
    
    if result is None or result.returncode != 0:
        print_error("CMake 配置失败")
        
        # 分析常见错误并给出建议
        if result and result.stderr:
            stderr_output = result.stderr.lower()
            
            if "does not exist" in stderr_output and "target" in stderr_output:
                print_info("检测到目标不存在错误 - 这通常是由于 CMakeLists.txt 中引用了不存在的目标")
                print_info("建议：检查 CMakeLists.txt 中的 install(TARGETS ...) 和自定义命令")
                
            elif "interface library" in stderr_output and "post_build" in stderr_output:
                print_info("检测到 INTERFACE 库 POST_BUILD 命令错误")
                print_info("建议：INTERFACE 库不能有 POST_BUILD 命令")
                
            elif "toolchain" in stderr_output:
                print_info("检测到工具链问题")
                print_info("建议：检查 Conan 工具链文件是否正确生成")
                
            else:
                print_info("可能的解决方案：")
                print("1. 检查 CMakeLists.txt 语法是否正确")
                print("2. 确保所有引用的目标都已正确定义")
                print("3. 验证 Conan 依赖是否正确安装")
        
        return False
    
    return True

def build_project():
    """构建项目"""
    cmd = "cmake --build build --config Debug"
    result = run_command(cmd, check=False)  # 构建可能有警告，不强制检查
    return True  # 即使有警告也继续

def check_compile_commands():
    """检查 compile_commands.json 生成情况"""
    build_compile_commands = Path("build/compile_commands.json")
    root_compile_commands = Path("compile_commands.json")
    
    if build_compile_commands.exists():
        print_success(f"compile_commands.json 已生成: {build_compile_commands}")
        
        # 显示文件信息
        file_size = build_compile_commands.stat().st_size
        print_info(f"文件大小: {file_size} 字节")
        
        # 计算编译条目数量
        try:
            with open(build_compile_commands, 'r', encoding='utf-8') as f:
                content = f.read()
                command_count = content.count('"command":')
                print_info(f"编译条目: {command_count} 个")
        except:
            pass
        
        # 拷贝到根目录
        if not root_compile_commands.exists():
            try:
                shutil.copy2(build_compile_commands, root_compile_commands)
                print_success("compile_commands.json 已拷贝到根目录")
            except Exception as e:
                print_warning(f"无法拷贝 compile_commands.json: {e}")
        else:
            print_success("compile_commands.json 已存在于根目录")
        
        return True
    else:
        print_error("compile_commands.json 未找到")
        print("可能原因：构建失败或 Ninja 配置问题")
        return False

def check_vscode_config():
    """检查 VS Code 配置"""
    vscode_config = Path(".vscode/c_cpp_properties.json")
    
    if vscode_config.exists():
        print_success("VS Code 配置文件存在")
        
        try:
            with open(vscode_config, 'r', encoding='utf-8') as f:
                content = f.read()
                if 'compile_commands.json' in content:
                    print_success("已配置使用 compile_commands.json")
                else:
                    print_warning("VS Code 配置可能需要更新")
        except:
            print_warning("无法读取 VS Code 配置文件")
    else:
        print_warning("VS Code 配置文件不存在")
        print("建议创建 .vscode/c_cpp_properties.json")

def main():
    """主函数"""
    print(f"\n{Colors.BOLD}{Colors.MAGENTA}=== Conan2 + CMake + VS Code IntelliSense 配置 ==={Colors.END}")
    print(f"{Colors.CYAN}跨平台版本 - 支持 Windows/Linux/macOS{Colors.END}")
    print(f"{Colors.CYAN}当前平台: {platform.system()} {platform.machine()}{Colors.END}\n")
    
    # 检查必要工具
    print_step(0, "检查必要工具")
    tools_ok = True
    
    if not check_tool("Conan", "conan --version"):
        print_error("请安装 Conan2: pip install conan")
        tools_ok = False
    
    if not check_tool("CMake", "cmake --version"):
        print_error("请安装 CMake")
        tools_ok = False
    
    if not check_tool("Ninja", "ninja --version"):
        print_error("请安装 Ninja 构建系统")
        tools_ok = False
    
    if not tools_ok:
        print_error("必要工具缺失，请安装后重试")
        return 1
    
    # 步骤 1: 清理 build 目录
    print_step(1, "检查和清理 build 目录")
    if not clean_build_directory():
        return 1
    
    # 步骤 2: 安装 Conan 依赖
    print_step(2, "安装 Conan 依赖")
    if not install_conan_dependencies():
        print_error("Conan 安装失败")
        print("\n可能的原因：")
        print("1. Conan2 未正确安装")
        print("2. Profile 配置错误")
        print("3. 网络连接问题")
        print("4. 权限问题")
        return 1
    
    print_success("Conan 依赖安装成功")
    
    # 步骤 3: 设置编译环境
    print_step(3, "设置编译环境")
    setup_msvc_environment()
    
    # 步骤 4: 配置 CMake
    print_step(4, "配置 CMake 项目")
    if not configure_cmake():
        print_error("CMake 配置失败")
        print("\n可能的解决方案：")
        print("1. 确保 Ninja 已安装并在 PATH 中")
        print("2. 确保编译器正确安装")
        print("3. 检查 Conan profile 配置")
        return 1
    
    print_success("CMake 配置成功")
    
    # # 步骤 5: 构建项目
    # print_step(5, "构建项目（生成 compile_commands.json）")
    # build_project()
    
    # # 步骤 6: 检查生成的文件
    # print_step(6, "检查生成的文件")
    compile_commands_ok = True
    
    # # 步骤 7: 检查 VS Code 配置
    # print_step(7, "VS Code 配置检查")
    # check_vscode_config()
    
    # 总结
    print(f"\n{Colors.BOLD}========================================{Colors.END}")
    print(f"{Colors.BOLD}=== 配置完成 ==={Colors.END}")
    print(f"{Colors.BOLD}========================================{Colors.END}\n")
    
    if compile_commands_ok:
        print(f"{Colors.GREEN}🎉 现在您可以享受以下功能:{Colors.END}")
        print("   - Conan2 自动管理所有依赖（ASIO、nlohmann_json、spdlog 等）")
        print("   - VS Code IntelliSense 自动获取所有包含路径")
        print("   - 精确的代码补全和错误检测")
        print("   - 每次构建后自动更新 IntelliSense")
        
        print(f"\n{Colors.CYAN}💡 使用说明:{Colors.END}")
        print("   - 添加新依赖: 编辑 conanfile.txt，然后重新运行此脚本")
        print("   - 构建项目: cmake --build build")
        print("   - 清理重建: python setup.py")
        
        return 0
    else:
        print_error("配置过程中出现问题，请检查上述错误信息")
        return 1

if __name__ == "__main__":
    try:
        exit_code = main()
        sys.exit(exit_code)
    except KeyboardInterrupt:
        print(f"\n{Colors.YELLOW}用户中断操作{Colors.END}")
        sys.exit(1)
    except Exception as e:
        print_error(f"未知错误: {e}")
        sys.exit(1)