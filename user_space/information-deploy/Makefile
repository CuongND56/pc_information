.PHONY: all clean

# CC = /home/cuong/Desktop/kernel_build/gcc-arm-10.3-2021.07-x86_64-arm-none-linux-gnueabihf/bin/arm-none-linux-gnueabihf-gcc
CC = gcc
NAME = main
CUR_DIR = .
BUILD_DIR = $(CUR_DIR)/build

# CPU_DIR = $(CUR_DIR)/cpu
# MEMORY_DIR = $(CUR_DIR)/memory
# THREAD_DIR = $(CUR_DIR)/thread
# PRG_DIR = $(CUR_DIR) $(CPU_DIR) $(MEMORY_DIR) $(THREAD_DIR)

PRG_DIR = $(shell find $(CUR_DIR) -type d)

SRCS = $(shell find $(PRG_DIR) -maxdepth 1 -name '*.c')
SRCS_NODIR = $(notdir $(SRCS))
OBJS = $(addprefix $(BUILD_DIR)/, $(SRCS_NODIR:.c=.o))
INC_FLAGS = $(addprefix -I, $(PRG_DIR)) -lpthread 


all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(OBJS) -o $(NAME)

$(BUILD_DIR)/%.o: $(SRCS) | $(BUILD_DIR)
	$(CC) -c -o $@ $(filter %/$*.c,$(SRCS)) $(INC_FLAGS)

$(BUILD_DIR):
	mkdir -p $@	

clean:
	rm -rf $(BUILD_DIR) $(NAME)

.PHONY: all clean