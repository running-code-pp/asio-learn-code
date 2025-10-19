# asio-learn-code
a collection of asio learning code

# build
安装python 3.7以上的版本\
安装conan2\
安装cmake,git
```bash
windows:
 打开x64 Native Tools Command Prompt for VS2022
 进入到项目根目录下
 conan install . --output-folder=build --build=missing --settings=build_type=Debug
 cmake -S . -B build --preset conan-default
 cmake --build build -j 16

Linux:
进入到项目根目录下
 conan install . --output-folder=build --build=missing --settings=build_type=Debug
 cmake -S . -B build --preset conan-default
 cmake --build build -j 16
```