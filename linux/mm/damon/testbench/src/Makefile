# 编译器设置
CC = gcc
CFLAGS = -O0 -Wall -Wextra

# 获取所有的 .c 文件
SRCS = $(wildcard *.c)

# 生成目标文件名列表（去掉 .c 后缀）
TARGETS = $(SRCS:.c=)

# 默认目标：编译所有程序
all: $(TARGETS)

# 编译规则：每个 .c 文件生成一个可执行文件
%: %.c
	$(CC) $(CFLAGS) $< -o $@
	@echo "Compiled: $@"

# 清理所有编译产物
clean:
	rm -f $(TARGETS)
	@echo "Cleaned all binaries"

# 列出所有可用的目标程序
list:
	@echo "Available programs:"
	@for target in $(TARGETS); do \
		echo "  $$target"; \
	done

# 运行指定的程序，使用方式：make run PROG=program_name
run:
	@if [ -z "$(PROG)" ]; then \
		echo "Please specify a program name: make run PROG=program_name"; \
		echo "Available programs:"; \
		for target in $(TARGETS); do \
			echo "  $$target"; \
		done; \
		exit 1; \
	fi
	@if [ -f "$(PROG)" ]; then \
		./$(PROG); \
	else \
		echo "Program '$(PROG)' not found. Please compile first."; \
		exit 1; \
	fi

# 声明伪目标
.PHONY: all clean list run 