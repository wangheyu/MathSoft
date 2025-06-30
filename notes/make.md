# 自动化编译工具 make

`make` 是一种广泛使用的**自动化构建工具**，主要用于在软件开发过程中根据**依赖关系**自动执行编译、链接等任务。它最早由 **Stuart Feldman** 于 1977 年开发，至今仍在许多项目中被广泛使用。


## `make` 的基本作用

在大型项目中，源代码文件通常有多个，且文件之间存在依赖关系。每次修改一个文件后，如果手动编译会非常繁琐且容易出错。`make` 可以通过一个叫做 `Makefile` 的配置文件自动分析这些依赖，并自动执行必要的命令来更新目标文件。


## Makefile 简介

`make` 的行为由一个名为 `Makefile`（或 `makefile`）的文本文件控制。它定义了目标文件（target）、依赖（dependencies）和生成规则（commands）。

### 语法结构

```make
target: dependencies
[TAB]command
```

> 注意：命令前必须是 **Tab 键**，不能是空格！

### 示例

```make
# 一个简单的 Makefile 示例
hello: hello.o
	gcc -o hello hello.o

hello.o: hello.c
	gcc -c hello.c

clean:
	rm -f hello hello.o
```

这个 `Makefile` 指定了：
- 如何从 `hello.o` 构建可执行文件 `hello`
- 如何从 `hello.c` 编译出 `hello.o`
- `clean` 是一个“伪目标”，用于清理生成文件

## 关键概念

| 概念        | 说明 |
|-------------|------|
| **目标 (target)**      | 通常是要生成的文件，如 `.o` 文件或最终的可执行文件 |
| **依赖 (dependencies)** | 生成目标所依赖的文件或其他目标 |
| **命令 (commands)**     | 当依赖更新后，如何重新生成目标的指令 |
| **伪目标 (phony targets)** | 非实际文件目标，如 `clean`, `install` 等 |
| **自动变量** | 比如 `$@`（目标名）、`$<`（第一个依赖）、`$^`（所有依赖） |

## 常用命令

```bash
make        # 默认构建第一个目标
make hello  # 构建指定目标 hello
make clean  # 执行 clean 规则，通常用于清理
```

**`make` 中的命令默认是由 Shell（通常是 Bash）来执行的**，不过也可以指定使用其他 Shell。

在大多数系统中，`make` 默认使用的是 `/bin/sh` 来执行命令，而**不是 Bash 本身**，尽管在 Linux 上 `/bin/sh` 通常是 Bash 的符号链接（或 dash、ash 等更轻量的 shell）。

### 示例

```make
hello:
	echo "Hello, world!"
```

上面的 `echo` 命令会通过 `/bin/sh` 执行。

你可以验证 `make` 用的 shell：

```bash
make -p | grep '^SHELL'
```

输出可能是：

```
SHELL = /bin/sh
```

## 指定使用 Bash

如果你希望明确使用 Bash，可以在 Makefile 顶部指定：

```make
SHELL := /bin/bash
```

然后你就可以使用 Bash 的语法特性，比如数组、`[[ ]]` 条件判断、`source` 命令等。

### 示例：使用 Bash 的数组

```make
SHELL := /bin/bash

bash-array:
	arr=(a b c); for i in "${arr[@]}"; do echo "$$i"; done
```

> 注意：Makefile 中 `$` 是特殊字符，必须用 `$$` 来表示 shell 中的 `$`。


> **要使用 Bash 的语法特性（如数组、`[[ ]]`、`source` 等），**你通常**需要显式指定 `SHELL := /bin/bash`**，否则在某些系统上（尤其是 Debian/Ubuntu 系列）可能会失败。

**`make` 默认使用的是 `/bin/sh`**，在大多数 Unix/Linux 系统中，`make` 默认用 `/bin/sh` 执行命令：

```bash
make -p | grep ^SHELL
# 输出：SHELL = /bin/sh
```
**但并不是所有的 `/bin/sh` 都是 Bash。**

- 有时在 Ubuntu、Debian 上，`/bin/sh` 指向的是更轻量的 **Dash**。
- Dash 是 POSIX 兼容 shell，但 **不支持 Bash 专有语法**！

**Bash 专有语法示例**

| 语法特性 | 是否 Dash 支持 | 是否 Bash 支持 |
|----------|----------------|----------------|
| `[[ ... ]]` 条件判断 | ❌ 不支持 | ✅ 支持 |
| 数组 `arr=(a b c)`、`${arr[0]}` | ❌ 不支持 | ✅ 支持 |
| `source file.sh` | ❌ 不支持（用 `.` 替代） | ✅ 支持 |
| `${var,,}` 小写转换 | ❌ 不支持 | ✅ 支持 |
| 字符串替换 `${var/foo/bar}` | ⚠️ 有限支持 | ✅ 支持 |

#### 示例：这在 Dash 中会失败

```make
test:
	[[ -f myfile.txt ]] && echo "Exists"
```

报错：

```bash
/bin/sh: 1: [[: not found
```

正确做法：总是显式指定 Bash

```make
SHELL := /bin/bash

test:
	[[ -f myfile.txt ]] && echo "Exists"
```

这样就可以使用 Bash 独有的语法了。


**注意**

1. **指定 `SHELL` 必须在 Makefile 的第一行或规则前生效。**
2. 并不是所有系统都安装在 `/bin/bash`，你可以用 `which bash` 检查路径。
3. 使用 `SHELL := /usr/bin/env bash` 是一种更可移植的方式（如果支持）：

    ```make
    SHELL := /usr/bin/env bash
    ```

## 每个命令在子 shell 中执行

每一行命令默认在**独立的 shell 子进程**中执行，这意味着：

```make
test:
	cd /tmp
	pwd
```

这段代码中 `cd /tmp` 并不会影响 `pwd` 命令的执行，因为每行命令都是在不同的子 shell 中执行的。

### 正确写法（使用行连接）：

```make
test:
	cd /tmp && pwd
```

或者使用反斜杠连接多行：

```make
test:
	cd /tmp && \
	pwd
```

**`make` 不仅可以用于 C/C++ 项目，也完全可以用于 Python、MATLAB 等语言的项目**，只要你有一组具有明确依赖关系的任务，`make` 就能帮助你自动化这些过程。

`make` 的核心作用是：

- 描述“文件之间的依赖关系”
- 指定“当某个文件变了，我该执行哪些命令”

它**不关心用的是什么语言**，只要你能在命令行中用某种方式完成构建、处理、测试等工作，`make` 就能调度它。


### Python 中使用 `make` 的示例

假设你有一个 Python 脚本 `process.py`，它读取 `data.csv` 并生成 `result.json`：

```make
SHELL := /bin/bash

result.json: data.csv process.py
	python process.py data.csv result.json

clean:
	rm -f result.json
```

**用途**：

- 自动运行你的 Python 脚本
- 当 `data.csv` 或 `process.py` 发生变化时，自动重新生成 `result.json`
- 用 `make clean` 清理输出文件

###  MATLAB 中使用 `make` 的示例

假设你有 MATLAB 脚本 `analyze.m`，读取 `input.mat` 并生成 `output.mat`：

```make
output.mat: input.mat analyze.m
	matlab -batch "analyze"

clean:
	rm -f output.mat
```

> 使用 `matlab -batch` 可以在不打开 GUI 的情况下运行脚本。


### 常见用途

| 场景 | Python | MATLAB |
|------|--------|--------|
| 数据预处理 | ✅ `make` 调用数据脚本 | ✅ 调用 `.m` 处理函数 |
| 自动测试 | ✅ 用 `pytest`/`unittest` | ✅ 用 `runtests` |
| 模型训练 | ✅ 处理模型依赖、模型文件 | ✅ 训练后生成 `.mat` |
| 文档生成 | ✅ 调用 `Sphinx`、`pandoc` | ✅ 导出图表、报告 |
| 清理中间文件 | ✅ `make clean` | ✅ `make clean` |


### 使用建议

- 把 `.py`、`.m` 文件作为依赖项，让 `make` 检测变动
- 使用虚拟环境或 conda 时，可以在 `Makefile` 中激活环境
- 如果任务不产生文件（比如测试），可以使用“伪目标”：

```make
.PHONY: test
test:
	pytest
```

## 实际例子：三体问题

我们建立一个名为 ThreeBodyProblem 的目录，用来生成我们的三体项目。在我们的项目里，有三体问题算法的讨论，是latex格式；有三体问题的简单模拟，是C/C++格式。模拟的结果是记录了每个时刻物体位置的文本文件。然后通过自己编写的C/C++程序，逐时刻生成物体位置的bmp图，最后用ffmpeg程序将这些bmp图转换成动画。下面演示了这样的一个项目，如何设计目录文件结构，如何设计make。

你这个项目融合了 **LaTeX 文档撰写 + C/C++ 数值模拟 + 图像生成 + ffmpeg 视频合成**，是一个典型的跨语言科学计算项目。

一个项目设计的原则是：**功能模块清晰分层**，将不同的功能模块放在不同的子目录下，比如：

```
ThreeBodyProblem/
├── Makefile                    <- 顶层构建入口
├── README.md                  <- 项目说明
├── doc/                       <- LaTeX 文档部分
│   ├── main.tex
│   └── figures/               <- 放实验结果图像
├── src/                       <- C/C++ 源码
│   ├── simulate.cpp           <- 三体模拟程序
│   └── render.cpp             <- BMP 图像生成程序
├── data/                      <- 模拟输出的轨迹数据
│   └── positions.txt
├── images/                    <- 每帧生成的 bmp 图像
│   ├── frame0001.bmp
│   └── ...
├── video/                     <- 最终生成的视频
│   └── output.mp4
└── bin/                       <- 可执行文件目录
    ├── simulate
    └── render
```


而对应的 `Makefile` 设计思路则是要实现如下的自动化流程：

1. 构建模拟程序和图像渲染程序
2. 运行模拟，输出 `data/positions.txt`
3. 读取 `positions.txt`，逐帧生成 BMP 图像
4. 调用 `ffmpeg` 将 BMP 图片合成为 MP4
5. 编译 LaTeX 文档，引用生成图像


一个 `Makefile` 示例：

```make
# 设置变量
CXX := g++
CXXFLAGS := -O2 -Wall
BIN_DIR := bin
SRC_DIR := src
DATA_DIR := data
IMG_DIR := images
VIDEO_DIR := video
DOC_DIR := doc
FIG_DIR := $(DOC_DIR)/figures

SIM := $(BIN_DIR)/simulate
RENDER := $(BIN_DIR)/render
POSITION_FILE := $(DATA_DIR)/positions.txt
VIDEO_FILE := $(VIDEO_DIR)/output.mp4

# 默认目标
all: $(VIDEO_FILE) doc

# 构建可执行文件
$(SIM): $(SRC_DIR)/simulate.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

$(RENDER): $(SRC_DIR)/render.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

# 运行模拟生成数据
$(POSITION_FILE): $(SIM)
	@mkdir -p $(DATA_DIR)
	$(SIM) > $(POSITION_FILE)

# 生成图像序列
$(IMG_DIR)/frame%.bmp: $(POSITION_FILE) $(RENDER)
	@mkdir -p $(IMG_DIR)
	$(RENDER) $< $*

# 假设需要 300 帧（你可以根据模拟程序输出自动配置）
FRAMES := $(addprefix $(IMG_DIR)/frame, $(addsuffix .bmp, $(shell seq -w 0001 0300)))

# 生成全部图像
images: $(FRAMES)

# 合成视频
$(VIDEO_FILE): images
	@mkdir -p $(VIDEO_DIR)
	ffmpeg -y -framerate 30 -i $(IMG_DIR)/frame%04d.bmp -pix_fmt yuv420p $@

# 编译 LaTeX 文档
doc: $(DOC_DIR)/main.tex
	@cd $(DOC_DIR) && pdflatex main.tex

# 清理
clean:
	rm -rf $(BIN_DIR)/*.o $(BIN_DIR)/simulate $(BIN_DIR)/render
	rm -rf $(DATA_DIR)/*.txt
	rm -rf $(IMG_DIR)/*.bmp
	rm -rf $(VIDEO_DIR)/*.mp4
	rm -rf $(DOC_DIR)/*.aux $(DOC_DIR)/*.log $(DOC_DIR)/*.pdf

.PHONY: all images doc clean
```

上述设计的目的在于：

```bash
make            # 一键完成模拟 + 渲染 + 视频 + 文档
make clean      # 清理所有中间文件
make images     # 只生成图像
make doc        # 只编译 LaTeX 文档
```

具体思路是：

+ 编译模拟程序和渲染程序
    - 分别编译 `simulate.cpp` 和 `render.cpp`
    - 输出到 `bin/` 目录

+ 运行模拟
    - 将模拟结果写入 `data/positions.txt`

+ 渲染图像
    - 假设 `render` 程序根据 `positions.txt` 的第 N 帧数据生成 `frameNNNN.bmp`
    - 用 `make` 的自动规则生成每一张图

+ 视频合成
    - 使用 `ffmpeg` 将 `images/frame%04d.bmp` 合成为 `output.mp4`

+ 文档编译
    - 使用 `pdflatex` 编译 `doc/main.tex`，可以引用 `figures/` 或 `images/` 中的图（比如：自动引用 `\includegraphics{../images/frame0300}`）

### 注意和 `bash` 的区别

**变量赋值的方式确实和 Bash 不一样，特别是 `:=` 和 `=` 的区别，这是 `GNU make` 的一个重要特性。**

> **`=` 是延迟赋值（lazy evaluation）**，**`:=` 是立即赋值（immediate evaluation）**。

1. `=`（延迟赋值 / 递归赋值）
    - **变量值在使用时才会展开**。
        ```make
        FOO = $(BAR)
        BAR = hello
    所以 `FOO` 实际展开时是 `hello`。
    - 你可以写成“变量引用变量”，形成“递归”。
        ```make
        A = $(B)
        B = world

        all:
	        echo $(A)
        ```
    输出：
        ```
        echo world
        ```
2. `:=`（立即赋值 / 简单赋值）
    - **变量值在定义时就立即展开**，不会再递归查找新值。
        ```make
        FOO := $(BAR)
        BAR := hello
        ```
        所以即使 `BAR` 之后改变，`FOO` 也不会变。
        ```make
        A := $(B)
        B := world

        all:
	        echo $(A)
        ```
        输出：
        ```
        echo
        ```
        因为 `B` 在 `A := $(B)` 时还没定义。
3. 还有其他赋值方式：

| 符号 | 含义 | 说明 |
|------|------|------|
| `=` | 延迟赋值 | 最常用；值在用的时候才展开 |
| `:=` | 立即赋值 | 适用于需要立即展开的变量 |
| `?=` | 条件赋值 | 如果变量未定义则赋值 |
| `+=` | 追加赋值 | 向已有变量追加内容 |


实操建议：

| 场景 | 推荐用法 |
|------|----------|
| 变量依赖其他变量，想让它们动态更新 | `=` |
| 变量值不依赖其他变量，固定就好 | `:=` |
| 定义编译器命令、生成目录路径等 | `:=`（更快、更稳定） |


```make
# 延迟赋值
SRC_DIR = src
OBJ_DIR = $(SRC_DIR)/obj

# 立即赋值
SRC_DIR := src
OBJ_DIR := $(SRC_DIR)/obj
```

上面两个都能工作，但如果你写的是 `:=`，`OBJ_DIR` 的值会马上变成 `src/obj`，不会被后续的 `SRC_DIR` 修改影响。

**自动变量的使用**

`make` 的自动变量（automatic variables）是在规则执行时由 `make` 自动设置的特殊变量，它们可以让你更简洁、灵活地编写规则，而无需手动指定每个目标和依赖名称。

> 自动变量是在 `make` 的规则中，用 `$@`、`$<` 等符号引用目标文件、依赖文件、文件名等信息的变量。

它们在每条规则执行时**自动根据上下文赋值**，不需要你手动定义。

**常见自动变量列表**

| 自动变量 | 含义 | 示例值（假设规则为 `foo.o: foo.cpp bar.hpp`） |
|----------|------|--------------------|
| `$@`     | 目标文件名（左边）       | `foo.o` |
| `$<`     | 第一个依赖文件（右边第一个） | `foo.cpp` |
| `$^`     | 所有依赖文件（去重）     | `foo.cpp bar.hpp` |
| `$+`     | 所有依赖文件（不去重）   | `foo.cpp bar.hpp` |
| `$?`     | 所有比目标新的依赖文件   | `foo.cpp`（如果它被更新） |
| `$*`     | 不含扩展名的文件名（目标或依赖的 stem） | `foo` |


**举例说明**

1. 编译 `.cpp` 到 `.o`

```make
%.o: %.cpp
	$(CXX) -c $< -o $@
```
等价于：

```bash
g++ -c main.cpp -o main.o
```

- `$<` → `main.cpp`（第一个依赖）
- `$@` → `main.o`（目标）

2. 链接目标程序

```make
main: main.o utils.o
	$(CXX) $^ -o $@
```

- `$^` → `main.o utils.o`（所有依赖）
- `$@` → `main`

3. 只编译更新过的源文件

```make
all: program

program: main.o utils.o
	$(CXX) $? -o $@
```

- `$?` → 只包含比 `program` 更新过的 `.o` 文件
- 避免不必要的重链接

4. 文件名去扩展名（`$*`）

```make
%.png: %.dot
	dot -Tpng $< -o $@
```

或者：

```make
%.log: %.txt
	echo "Processing $*" > $@
```

- `$*` → `data`（如果目标是 `data.log`）

### 实际构建

先构建一个小项目，就是我们用来处理 BMP 位图的小程序，项目结构假设为：

```plaintext
BMP/
├── Makefile
├── bin/            # 可执行文件输出
│   └── test
├── doc/            # 文档（空）
├── include/        # 头文件
│   └── bitmap.h
├── lib/            # 静态库或动态库输出
│   └── libbitmap.so
├── src/            # 源文件
    ├── bitmap.cpp
    └── test.cpp    # 包含 main 函数
```

项目构建目标梳理如下：

| 目标 | 说明 |
|------|------|
| 生成 `libbitmap.so` | 从 `bitmap.cpp` 编译为共享库（`.so`） |
| 编译 `test.cpp` | 作为测试程序，依赖 `bitmap.h`，链接 `libbitmap.so` |
| 输出可执行文件到 `bin/` |
| 支持 `make clean` | 清理中间文件和输出 |
| 支持 `make run` | 运行测试程序 |

对应的 Makefile 内容如下：

```make
# === 编译器设置 ===
CXX := g++
CXXFLAGS := -std=c++17 -Wall -fPIC -Iinclude
# -std=c++17：使用 C++17 标准。
# -Wall：开启所有警告信息。
# -fPIC：生成位置无关代码（Position Independent Code），用于构建共享库（.so）。
# -Iinclude：指定头文件搜索路径为 include/ 目录。

LDFLAGS := -Llib -lbitmap
# 这个其实是在规定库文件编译完成之后如何调用
# -Llib 表示去 lib 目录下寻找
# -lbitmap 表示目标库文件会是 libbitmap.a 或者 libbitmap.so

# === 目录设置 ===
SRC_DIR := src
INC_DIR := include
BIN_DIR := bin
LIB_DIR := lib

# === 文件路径 ===
LIB_NAME := libbitmap.so
LIB_TARGET := $(LIB_DIR)/$(LIB_NAME)
TEST_SRC := $(SRC_DIR)/test.cpp
TEST_OBJ := $(SRC_DIR)/test.o
TEST_BIN := $(BIN_DIR)/test
# LIB_NAME：要构建的共享库名称。
# LIB_TARGET：完整路径：lib/libbitmap.so。
# TEST_SRC：测试程序源文件：src/test.cpp。
# TEST_OBJ：测试程序目标文件（中间文件）：src/test.o。
# TEST_BIN：最终可执行文件路径：bin/test。


# === 默认目标 ===
all: $(LIB_TARGET) $(TEST_BIN)

# === 生成共享库 ===
$(LIB_TARGET): $(SRC_DIR)/bitmap.cpp $(INC_DIR)/bitmap.h
	@mkdir -p $(LIB_DIR)
	$(CXX) $(CXXFLAGS) -shared -o $@ $<
# 该规则用于构建共享库文件 lib/libbitmap.so。
# 依赖项是 src/bitmap.cpp 和 include/bitmap.h。
# 命令：
# mkdir -p lib：确保输出目录存在。这里 @ 表示显式执行的命令和过程，除非出错。
# $(CXX)：调用 g++ 编译器。
# $(CXXFLAGS)：编译选项（如前所述）。
# -shared：表示生成共享库（.so）。
# -o $@：输出文件为目标名（即 lib/libbitmap.so）。
# $<：第一个依赖文件（这里是 src/bitmap.cpp）。

# === 编译测试程序 ===
$(TEST_BIN): $(TEST_SRC) $(LIB_TARGET)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDFLAGS)
# 构建测试程序 bin/test。
# 依赖项是 src/test.cpp 和 lib/libbitmap.so。
# 命令：
# 创建 bin/ 目录。
# 使用 g++ 编译 test.cpp，并链接 libbitmap 库。
# 输出到 bin/test。


# === 运行测试程序 ===
run: $(TEST_BIN)
	LD_LIBRARY_PATH=$(LIB_DIR) ./$(TEST_BIN)
# 执行 make run 将运行测试程序。
# 设置 LD_LIBRARY_PATH 环境变量以确保系统能找到 libbitmap.so。
# 然后运行 ./bin/test。

# === 清理 ===
clean:
	rm -rf $(BIN_DIR) $(LIB_DIR) $(SRC_DIR)/*.o

.PHONY: all clean run
```

在考虑用 `make` 构建项目时，我们一般要考虑得更多一些，目前基本上主流项目代码的注释和文档一般交给 `doxygen` 生成，所以可以将这两者一起考虑进去。也就是说，我们顺便将 `Doxygen` 配置文件设置好，同时，我们将考虑将文档生成过程，也就是 `doxygen` 命令加入 `make` 管理。

然后我们可以更新我们的 `Makefile` 文件，将文档生成和清理加入：

```make
# === Doxygen 文档生成 ===
.PHONY: doc
doc:
	@echo "正在生成 Doxygen 文档..."
	@cd . && doxygen $(DOXYFILE)
	@echo "文档已生成在 $(DOC_DIR)/$(DOC_OUTPUT_DIR)/ 目录。"

# === 打开 Doxygen 文档（仅限 Linux/macOS） ===
.PHONY: doc-open
doc-open:
	@xdg-open $(DOC_DIR)/$(DOC_OUTPUT_DIR)/index.html 2>/dev/null || open $(DOC_DIR)/$(DOC_OUTPUT_DIR)/index.html || echo "请手动打开 $(DOC_DIR)/$(DOC_OUTPUT_DIR)/index.html"

# === 清理 Doxygen 文档输出 ===
.PHONY: doc-clean
doc-clean:
	@echo "正在清理文档输出目录 $(DOC_OUTPUT_DIR)/ ..."
	@rm -rf $(DOC_DIR)/$(DOC_OUTPUT_DIR)
	@echo "清理完成。"

# === 清理构建产物 ===
.PHONY: clean
clean:
	rm -rf $(BIN_DIR) $(LIB_DIR) $(SRC_DIR)/*.o

# === 显示帮助信息 ===
.PHONY: help
help:
	@echo "  可用命令："
	@echo "  make            - 编译共享库和测试程序"
	@echo "  make run        - 运行测试程序"
	@echo "  make clean      - 清理构建产物"
	@echo "  make doc        - 生成 Doxygen 文档"
	@echo "  make doc-open   - 打开 HTML 文档（Linux/macOS）"
	@echo "  make doc-clean  - 删除 Doxygen 文档输出"
```

## 使用 AI 生成 `Makefile` 的提示词

目录 ThreeBodyProblem 下是一个关于三体问题的项目。
用 C/C++ 对 2D 的三体系统经行模拟，并且产生每个时刻各个物体位置的数据。
然后用自己编写的程序，在 BMP 目录下的代码的支持下，
将每个时刻的位置转换成一副bmp图像，最后用 ffmpeg 将所有的 bmp 图像转换成一个 avi 电影。
在 doc 目录下将存放全部的用 doxygen 生成的代码文档，和整个项目的报告 report.tex 和 展示稿 slide.tex。
整个项目用 git 进行代码管理。

这是当前的文件目录关系：
ls -lR
.:
total 8
drwxr-xr-x  7 hywang hywang 4096 May 11 23:02 BMP
drwxr-xr-x 11 hywang hywang 4096 May 11 19:25 ThreeBodyProblem

./BMP:
total 32
drwxr-xr-x 2 hywang hywang 4096 May 11 23:02 bin
drwxr-xr-x 2 hywang hywang 4096 May 11 22:56 doc
drwxr-xr-x 2 hywang hywang 4096 May 11 19:52 include
drwxr-xr-x 2 hywang hywang 4096 May 11 23:02 lib
-rw-r--r-- 1 hywang hywang 1882 May 11 19:39 LICENSE
-rw-r--r-- 1 hywang hywang 2026 May 11 20:40 Makefile
-rw-r--r-- 1 hywang hywang 2111 May 11 19:46 README.md
drwxr-xr-x 2 hywang hywang 4096 May 11 20:16 src

./BMP/bin:
total 20
-rwxr-xr-x 1 hywang hywang 16472 May 11 23:02 test

./BMP/doc:
total 120
-rw-r--r-- 1 hywang hywang 119246 May 11 20:29 Doxyfile

./BMP/include:
total 4
-rw-r--r-- 1 hywang hywang 1965 May 11 20:25 bitmap.h

./BMP/lib:
total 16
-rwxr-xr-x 1 hywang hywang 15664 May 11 23:02 libbitmap.so

./BMP/src:
total 8
-rw-r--r-- 1 hywang hywang 1371 May 11 19:25 bitmap.cpp
-rw-r--r-- 1 hywang hywang  526 May 11 19:28 test.cpp

./ThreeBodyProblem:
total 40
drwxr-xr-x 2 hywang hywang 4096 May 11 19:25 bin
drwxr-xr-x 2 hywang hywang 4096 May 11 19:25 config
drwxr-xr-x 2 hywang hywang 4096 May 11 19:25 data
drwxr-xr-x 2 hywang hywang 4096 May 11 19:25 doc
drwxr-xr-x 2 hywang hywang 4096 May 11 19:25 images
drwxr-xr-x 2 hywang hywang 4096 May 11 22:58 include
-rw-r--r-- 1 hywang hywang  954 May 11 19:25 Makefile
drwxr-xr-x 2 hywang hywang 4096 May 11 19:25 obj
-rw-r--r-- 1 hywang hywang    0 May 11 19:25 README.md
drwxr-xr-x 2 hywang hywang 4096 May 11 19:25 src
drwxr-xr-x 2 hywang hywang 4096 May 11 19:25 video

./ThreeBodyProblem/bin:
total 0

./ThreeBodyProblem/config:
total 4
-rw-r--r-- 1 hywang hywang 282 May 11 19:25 config

./ThreeBodyProblem/data:
total 0

./ThreeBodyProblem/doc:
total 0

./ThreeBodyProblem/images:
total 0

./ThreeBodyProblem/include:
total 24
-rw-r--r-- 1 hywang hywang 4993 May 11 19:25 Body.h
-rw-r--r-- 1 hywang hywang  836 May 11 19:25 Constants.h
-rw-r--r-- 1 hywang hywang 5350 May 11 19:25 Universe.h
-rw-r--r-- 1 hywang hywang 2883 May 11 19:25 Vec.h

./ThreeBodyProblem/obj:
total 36
-rw-r--r-- 1 hywang hywang 33216 May 11 19:25 data2bmp.o

./ThreeBodyProblem/src:
total 12
-rw-r--r-- 1 hywang hywang 2804 May 11 19:25 data2bmp.cpp
-rw-r--r-- 1 hywang hywang 4153 May 11 19:25 main.cpp

./ThreeBodyProblem/video:
total 0

请分析一下是否合理。


考虑给ThreeBodyProblem 项目设计一个Makefile，其中：
data2bmp.o 由 data2bmp.cpp 生成，依赖 BMP/include/bitmap.h 和 BMP/lib/libbitmap.so
编译 main.cpp 需要依赖 Body.h, Constants.h, Universe.h, Vec.h
编译生成后的可执行文件运行时需要 config 作为配置文件
运行生成每个时刻的物体位置数据，由 data2bmp 编译产生的可执行文件转换成系列 bmp 位图，再由 ffmepg 转换成 avi 文件。
同时考虑用 latex 生成报告，报告源码在 doc 中的 report.tex
用 latex 生成展示，源码在 doc 中的 slide.tex
考虑用 doxygen 产生文档，目标目录也是 doc.

### 对应的生成结果

从项目结构来看，整体设计是合理的，但有一些地方可以优化或需要注意：

目录结构:

ThreeBodyProblem 目录用于存放三体问题的模拟代码和相关资源。
BMP 目录用于处理图像生成的代码和库。
两个目录的分离是合理的，因为它们处理不同的任务。

文件和目录的用途:

ThreeBodyProblem/src 和 BMP/src 分别存放源代码。
ThreeBodyProblem/include 和 BMP/include 分别存放头文件。
ThreeBodyProblem/bin 和 BMP/bin 用于存放编译后的可执行文件。
ThreeBodyProblem/doc 和 BMP/doc 用于存放文档。
ThreeBodyProblem/data 用于存放模拟产生的数据。
ThreeBodyProblem/images 用于存放生成的图像。
ThreeBodyProblem/video 用于存放最终生成的视频。

文件的内容:

Makefile 文件用于自动化编译过程，这是标准的做法。
README.md 文件用于提供项目的基本信息和使用说明。
LICENSE 文件用于说明项目的版权和许可信息。
Doxyfile 用于配置 Doxygen 文档生成。

代码管理:

使用 Git 进行代码管理是标准的做法，可以有效地跟踪代码的变化。

优化建议:

确保 ThreeBodyProblem/data 和 ThreeBodyProblem/images 目录在模拟运行后有数据和图像生成。
ThreeBodyProblem/video 目录应该在生成视频后有内容。
ThreeBodyProblem/doc 目录应该包含生成的文档文件。
BMP/lib 目录中的 libbitmap.so 是动态库，确保它在编译时正确链接。
ThreeBodyProblem/obj 目录用于存放中间编译文件，可以在编译完成后清理以节省空间。
确保 report.tex 和 slide.tex 文件在 ThreeBodyProblem/doc 目录中存在，并且内容完整。

总体来说，项目结构是合理的，但需要确保各个目录在项目运行后都有相应的内容生成，并且文档和报告文件完整。


为了实现你的需求，我们可以设计一个 Makefile 来自动化整个构建和生成过程。这个 Makefile 将包括编译源代码、生成可执行文件、运行模拟、生成图像和视频，以及生成文档和报告。以下是一个可能的 Makefile 示例：

```make
# Compiler and flags
CXX = g++
CXXFLAGS = -I./include -I../BMP/include -L../BMP/lib -lbitmap -std=c++11

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
DATA_DIR = data
IMG_DIR = images
VIDEO_DIR = video
DOC_DIR = doc

# Files
MAIN_SRC = $(SRC_DIR)/main.cpp
DATA2BMP_SRC = $(SRC_DIR)/data2bmp.cpp
DATA2BMP_OBJ = $(OBJ_DIR)/data2bmp.o
EXECUTABLE = $(BIN_DIR)/three_body_simulation
DATA2BMP_EXEC = $(BIN_DIR)/data2bmp
CONFIG_FILE = config/config
REPORT_SRC = $(DOC_DIR)/report.tex
SLIDE_SRC = $(DOC_DIR)/slide.tex
DOXYFILE = $(DOC_DIR)/Doxyfile

# Targets
.PHONY: all clean run report slides doc

all: $(EXECUTABLE) $(DATA2BMP_EXEC)

$(EXECUTABLE): $(MAIN_SRC) $(DATA2BMP_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(MAIN_SRC) $(DATA2BMP_OBJ)

$(DATA2BMP_OBJ): $(DATA2BMP_SRC) ../BMP/include/bitmap.h
	$(CXX) $(CXXFLAGS) -c -o $@ $(DATA2BMP_SRC)

$(DATA2BMP_EXEC): $(DATA2BMP_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(DATA2BMP_OBJ)

run: $(EXECUTABLE)
	@echo "Running simulation..."
	./$(EXECUTABLE) $(CONFIG_FILE) $(DATA_DIR)/positions.txt
	@echo "Converting data to BMP images..."
	./$(DATA2BMP_EXEC) $(DATA_DIR)/positions.txt $(IMG_DIR)
	@echo "Creating video from images..."
	ffmpeg -framerate 24 -i $(IMG_DIR)/image%03d.bmp -c:v libx264 -pix_fmt yuv420p $(VIDEO_DIR)/simulation.avi

report:
	@echo "Generating report..."
	pdflatex -output-directory=$(DOC_DIR) $(REPORT_SRC)

slides:
	@echo "Generating slides..."
	pdflatex -output-directory=$(DOC_DIR) $(SLIDE_SRC)

doc:
	@echo "Generating documentation..."
	doxygen $(DOXYFILE)

clean:
	@echo "Cleaning up..."
	rm -f $(OBJ_DIR)/*.o
	rm -f $(BIN_DIR)/*
	rm -f $(IMG_DIR)/*.bmp
	rm -f $(VIDEO_DIR)/*.avi
	rm -f $(DOC_DIR)/*.aux $(DOC_DIR)/*.log $(DOC_DIR)/*.pdf
	rm -rf $(DOC_DIR)/html $(DOC_DIR)/latex
```

### 说明：

**编译和链接：**

`CXXFLAGS` 包含了头文件和库的路径。
`$(EXECUTABLE)` 是主程序的可执行文件，依赖于 `main.cpp` 和 `data2bmp.o`。
`$(DATA2BMP_OBJ)` 是 `data2bmp.cpp` 编译生成的目标文件，依赖于 `bitmap.h`。

**运行和生成：**

`run` 目标会运行模拟程序，生成位置数据，然后调用 `data2bmp` 转换为 `BMP` 图像，最后用 `ffmpeg` 生成 `AVI` 视频。

**文档和报告：**

`report` 和 `slides` 目标使用 `pdflatex` 生成 `PDF` 文档。
`doc` 目标使用 `doxygen` 生成代码文档。

**清理：**

`clean` 目标用于删除生成的中间文件和输出文件。

确保在运行 `Makefile` 之前，所有的目录结构和文件路径都是正确的，并且 `ffmpeg` 和 `pdflatex` 已经安装在系统中。

