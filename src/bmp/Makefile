# 定义编译器
CC = g++

# 定义编译选项（可根据需要调整）
CFLAGS = -Wall -std=c++17

# 定义目标可执行文件
TARGET = test

# 定义源文件
SRCS = bitmap.cpp test_bmp.cpp

# 自动生成对象文件列表
OBJS = $(SRCS:.cpp=.o)

# 默认目标：生成可执行文件
all: $(TARGET)

# 生成可执行文件
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@

# 编译 .cpp 文件为 .o 文件
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# 清理生成的文件
clean:
	rm -f $(OBJS) $(TARGET)

# 重新构建整个项目
rebuild: clean all

.PHONY: all clean rebuild
