CXX := g++
# 编译选项
CFLAGS = -std=c++14 -O3
# 文件路径
DIR := .
DIR += ./lib_json
DIR += ./json
# 依赖的头文件
INCS := $(foreach dir,$(DIR),-I$(dir))
# 源代码地址
SRCS := $(foreach dir,$(DIR),$(wildcard $(dir)/*.cpp))
# 目标文件存放地址 ./output
OBJS := $(patsubst %.cpp, %.o,$(SRCS))
# 依赖文件地址，保证每次修改头文件之后能够重新编译 依赖他的文件
DEPS := $(patsubst %.o, %.d,$(OBJS))


main : $(OBJS)
			@$(CXX) $(OBJS) -o ./main


$(OBJS) : %.o: %.cpp
			@echo complie
			$(CXX) -MMD -MP -c $(CFLAGS) $(INCS) $< -o $@

# 引入依赖文件，-表示如果依赖文件不存在则忽略错误继续
-include $(DEPS)

.PHONY : clean
clean :
			rm -rf $(OBJS) $(DEPS) ./main

.PHONY : test
test :
		@echo $(OBJS)

