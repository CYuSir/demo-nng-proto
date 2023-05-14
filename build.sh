#!/bin/bash

# 删除现有的 build 目录并创建新的目录
# rm -rf build
# cmake -E make_directory build
cmake -B build -S .

# 使用 CMake 构建项目
cmake --build build