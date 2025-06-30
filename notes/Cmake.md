# Cmake 简洁

CMake 是一个 **跨平台的自动化构建系统**，用来控制软件的编译过程。它使用一个名为 `CMakeLists.txt` 
的配置文件来生成平台相关的构建文件（如 Makefile、Visual Studio 工程文件等）。

## CMake 的主要特点

- **跨平台支持**：支持 Windows、Linux、macOS 等多种平台。
- **多种构建系统输出**：可以生成 Makefile、Ninja 文件、Xcode 项目、Visual Studio 项目等。
- **模块化和可扩展性强**：支持各种模块（如查找库、测试、安装等）。
- **与 C/C++ 紧密集成**：特别适合 C 和 C++ 项目的构建。
- **支持外部项目和包管理**：如 FetchContent、ExternalProject、CPM、vcpkg、Conan 等。

## 🔧 基本使用流程

### 1. 创建项目目录结构

```bash
my_project/
├── CMakeLists.txt
└── main.cpp
```

### 2. 编写 `CMakeLists.txt`

```cmake
cmake_minimum_required(VERSION 3.10)
project(MyProject)

set(CMAKE_CXX_STANDARD 17)

add_executable(my_project main.cpp)
```

### 3. 使用 CMake 构建项目

```bash
# 1. 创建构建目录
mkdir build
cd build

# 2. 生成构建系统（Makefile 或其他）
cmake ..

# 3. 编译
cmake --build .
```

## 🔍 常用指令说明

| 指令                        | 说明 |
|-----------------------------|------|
| `project()`                 | 定义项目名称和语言 |
| `add_executable()`         | 添加可执行文件目标 |
| `add_library()`            | 添加库目标 |
| `target_link_libraries()`  | 链接库到目标 |
| `include_directories()`    | 添加头文件搜索路径 |
| `find_package()`           | 查找系统或第三方库 |
| `install()`                | 安装目标和文件 |
| `enable_testing()`         | 启用测试支持 |


CMake 本身是一个 **构建系统生成工具**，它通过 `find_package()`、`include()` 等机制支持 **查找并使用各类库**。它并不像包管理器那样“内置”很多库，而是通过 **模块系统（Modules）或配置文件（Config）** 来支持各种库。


## CMake 支持库的两种方式

### 1. **模块模式（Module mode）**

CMake 提供了一些内置的 `Find<Package>.cmake` 模块，用于查找常见的开源库。比如：

```cmake
find_package(OpenGL REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem)
find_package(Threads REQUIRED)
```

这些模块一般位于：

```
/usr/share/cmake-X.Y/Modules/
```

### 2. **配置模式（Config mode）**

现代 CMake 越来越推荐库作者提供自己的 **Config 文件**，比如：

```cmake
find_package(fmt CONFIG REQUIRED)
target_link_libraries(my_app fmt::fmt)
```

这种方式依赖库作者提供 `fmtConfig.cmake` 和相关的导出目标。


### 常见的 CMake 支持库（部分）

| 库名称       | 说明                              | 支持方式 |
|--------------|-----------------------------------|----------|
| Boost        | 常用 C++ 库集合                   | 模块     |
| OpenGL       | 图形库                            | 模块     |
| Threads      | 跨平台线程支持                    | 模块     |
| ZLIB         | 压缩库                            | 模块     |
| PNG          | 图像处理库                        | 模块     |
| Python       | 与 Python 交互                    | 模块     |
| Qt           | 跨平台 GUI 框架                   | 模块     |
| OpenCV       | 计算机视觉库                      | Config   |
| fmt          | 现代 C++ 字符串格式化库           | Config   |
| spdlog       | 现代 C++ 日志库                   | Config   |
| SDL2         | 游戏/多媒体库                     | 模块/Config |
| Eigen3       | 线性代数库                        | Config   |
| Protobuf     | Google Protocol Buffers           | Config   |
| CURL         | 网络库                            | 模块     |
| OpenSSL      | 加密库                            | 模块     |


你可以在终端使用下面命令查看：

```bash
ls /usr/share/cmake*/Modules/Find*.cmake
```

或者在 CMake 官网搜索模块文档：

🔗 [https://cmake.org/cmake/help/latest/manual/cmake-modules.7.html](https://cmake.org/cmake/help/latest/manual/cmake-modules.7.html)

## 更高级功能

- 支持 **条件编译**
- 支持 **构建选项与配置（`option()`, `if()`, `set()`）**
- 支持 **测试（CTest）**
- 支持 **打包（CPack）**
- 支持 **构建依赖管理（如 FetchContent）**

## 学习资源

- 官方文档：https://cmake.org/documentation/
- CMake 教程：https://cmake.org/cmake/help/latest/guide/tutorial/index.html
- GitHub 上的开源 CMake 项目


## CMake 和 Make 的关系

| 工具 | 角色 | 作用 |
|------|------|------|
| **CMake** | **构建系统的生成器** | 自动生成构建文件，例如 Makefile、Ninja 文件、Visual Studio 工程等 |
| **Make** | **构建工具本身** | 读取 Makefile 并执行编译命令来生成程序 |


- **CMake 是 Make 的上游工具**。
- CMake 不直接编译程序，而是通过读取你的 `CMakeLists.txt` 文件，生成 **Make 可以使用的 Makefile**。
- 然后你用 `make`（或 `cmake --build .`）去执行这些 Makefile，完成编译。


### 构建流程示例

1. 编写 `CMakeLists.txt`：

   ```cmake
   cmake_minimum_required(VERSION 3.10)
   project(MyApp)
   add_executable(my_app main.cpp)
   ```

2. 使用 CMake 生成构建文件（默认使用的是 Make）：

   ```bash
   mkdir build && cd build
   cmake ..
   ```

   这一步会生成 `Makefile`。

3. 使用 Make 编译：

   ```bash
   make
   ```

   或者：

   ```bash
   cmake --build .
   ```


### CMake 支持多种构建工具

除了 `make`，CMake 还可以生成其他构建系统的文件：

| 构建工具 | 用途/平台 | CMake 生成方式 |
|-----------|-----------|----------------|
| **Make** | Unix/Linux/BSD | 默认 |
| **Ninja** | 快速构建系统 | `cmake -G Ninja ..` |
| **Visual Studio** | Windows IDE | `cmake -G "Visual Studio 17 2022" ..` |
| **Xcode** | macOS | `cmake -G Xcode ..` |

你可以通过 `-G` 参数指定生成器：

```bash
cmake -G "Unix Makefiles" ..
cmake -G Ninja ..
```

## 测试例子

我们将 BMP 的 Makefile 转成 CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.10)

# === 项目设置 ===
project(BitmapProject VERSION 1.0 LANGUAGES CXX)

# === 编译器设置 ===
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fPIC")

# === 目录设置 ===
set(SRC_DIR src)
set(INC_DIR include)
set(BIN_DIR bin)
set(LIB_DIR lib)

# === 文件路径 ===
set(LIB_NAME libbitmap.so)
set(LIB_TARGET ${LIB_DIR}/${LIB_NAME})
set(TEST_SRC ${SRC_DIR}/test.cpp)
set(TEST_BIN ${BIN_DIR}/test)
set(DOXYFILE ${DOC_DIR}/Doxyfile)

# === 生成共享库 ===
add_library(bitmap SHARED ${SRC_DIR}/bitmap.cpp)
target_include_directories(bitmap PUBLIC ${INC_DIR})
set_target_properties(bitmap PROPERTIES OUTPUT_NAME "bitmap")
set_target_properties(bitmap PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${LIB_DIR})

# === 编译测试程序 ===
add_executable(test ${TEST_SRC})
target_include_directories(test PUBLIC ${INC_DIR})
target_link_libraries(test bitmap)
set_target_properties(test PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})

# === 运行测试程序 ===
add_custom_target(run
    COMMAND ${CMAKE_COMMAND} -E env LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/lib ${CMAKE_BINARY_DIR}/bin/test
    DEPENDS test
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)


# === 清理构建产物 ===
add_custom_target(clean_all
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${BIN_DIR} ${LIB_DIR} ${SRC_DIR}/*.o
    COMMENT "清理构建产物..."
)

# === 显示帮助信息 ===
add_custom_target(show_help
    COMMAND ${CMAKE_COMMAND} -E echo "  可用命令："
    COMMAND ${CMAKE_COMMAND} -E echo "  make            - 编译共享库和测试程序"
    COMMAND ${CMAKE_COMMAND} -E echo "  make run        - 运行测试程序"
    COMMAND ${CMAKE_COMMAND} -E echo "  make clean_all  - 清理构建产物"
)

```

因为 `clean` 和 `help` 是 CMake 的关键字，所以被改成了 `clean_all` 和 `show_help`。

如果你手动将 **GNU Scientific Library（libgsl）安装在 `$HOME/usr/gsl`**（即 `$USER/usr/gsl`）目录下，而不是系统默认路径（如 `/usr/lib`），你需要在 **CMake 中显式指定 GSL 的头文件和库路径**。

下面是如何在 CMake 中正确引入这个自定义路径的 GSL 安装的完整方法。

---

## ✅ 假设目录结构如下

```bash
$HOME/usr/gsl/
├── include/              # gsl/gsl_matrix.h 等头文件
└── lib/                  # libgsl.a / libgsl.so / libgslcblas.so 等库文件
```

---

## ✅ 方法 1：手动指定 include 和 link 路径

在你的 `CMakeLists.txt` 中添加：

```cmake
cmake_minimum_required(VERSION 3.10)
project(MyGSLApp C)

# 添加头文件目录
include_directories("$ENV{HOME}/usr/gsl/include")

# 添加库文件目录
link_directories("$ENV{HOME}/usr/gsl/lib")

# 添加可执行文件
add_executable(my_app main.c)

# 链接 GSL 库
target_link_libraries(my_app
    PRIVATE
    gsl
    gslcblas
    m  # 数学库（有些系统需要）
)
```

> ✅ 使用 `$ENV{HOME}` 可以跨用户，不依赖具体用户名。

---

## ✅ 方法 2：使用 `find_library()`（更自动化）

你也可以使用 CMake 的 `find_library()` 查找库路径（适合写成函数/模块）：

```cmake
find_library(GSL_LIB gsl PATHS "$ENV{HOME}/usr/gsl/lib" REQUIRED)
find_library(GSLCBLAS_LIB gslcblas PATHS "$ENV{HOME}/usr/gsl/lib" REQUIRED)

include_directories("$ENV{HOME}/usr/gsl/include")

add_executable(my_app main.c)
target_link_libraries(my_app PRIVATE ${GSL_LIB} ${GSLCBLAS_LIB} m)
```

---

## ✅ 方法 3：设置环境变量供 `find_package(GSL)` 使用

如果你想使用 `find_package(GSL REQUIRED)`，你需要让 CMake 找到 GSL 的配置文件（如 `GSLConfig.cmake`），但 GSL 默认不会提供这个文件（除非用像 vcpkg 这样安装）。因此这个方法通常不可行，除非你自己写一个 config 文件。

---

## ✅ 方法 4：命令行传入 include/link 路径

你也可以在运行 CMake 的时候传入路径：

```bash
cmake -DCMAKE_INCLUDE_PATH=$HOME/usr/gsl/include -DCMAKE_LIBRARY_PATH=$HOME/usr/gsl/lib ..
```

然后在 `CMakeLists.txt` 中使用：

```cmake
find_library(GSL_LIB gsl REQUIRED)
find_library(GSLCBLAS_LIB gslcblas REQUIRED)

add_executable(my_app main.c)
target_link_libraries(my_app PRIVATE ${GSL_LIB} ${GSLCBLAS_LIB} m)
```

---

## ✅ 方法 5：建议设置 PKG_CONFIG_PATH（高级）

如果你使用 `pkg-config`，可以设置：

```bash
export PKG_CONFIG_PATH=$HOME/usr/gsl/lib/pkgconfig:$PKG_CONFIG_PATH
```

然后在 CMake 中使用 `pkg_check_modules`（需要 `FindPkgConfig`）：

```cmake
find_package(PkgConfig REQUIRED)
pkg_check_modules(GSL REQUIRED gsl)

include_directories(${GSL_INCLUDE_DIRS})
link_directories(${GSL_LIBRARY_DIRS})

add_executable(my_app main.c)
target_link_libraries(my_app PRIVATE ${GSL_LIBRARIES})
```

---

## ✅ 验证路径是否正确

你可以使用以下命令检查你的安装：

```bash
ls $HOME/usr/gsl/include/gsl/gsl_matrix.h
ls $HOME/usr/gsl/lib/libgsl.so
```

---

## 🧠 总结

| 方法 | 是否推荐 | 说明 |
|------|-----------|------|
| `include_directories` + `link_directories` | ✅ 推荐 | 简单直接，适用大多数场景 |
| `find_library()` | ✅ 推荐 | 更自动化，适合多平台项目 |
| `find_package(GSL)` | ❌ 不推荐 | libgsl 没有提供标准 cmake 配置 |
| 设置 `CMAKE_INCLUDE_PATH` | ✅ 可选 | 命令行方式，适合自动化脚本 |
| 使用 `pkg-config` | ✅ 高级 | 依赖系统支持 `.pc` 文件 |

---

如果你希望我为你生成一个完整的项目结构（带 CMake 和源码），并默认使用 `$HOME/usr/gsl`，我可以立即为你生成。是否需要？