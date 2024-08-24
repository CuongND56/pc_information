.PHONY: all clean

CC = /home/cuong/Desktop/kernel_build/gcc-arm-10.3-2021.07-x86_64-arm-none-linux-gnueabihf/bin/arm-none-linux-gnueabihf-gcc
NAME = cpu_usage
CUR_DIR = .
BUILD_DIR = $(CUR_DIR)/build
INCS = -I$(CUR_DIR)
SRCS = $(shell find $(CUR_DIR) -maxdepth 1 -name '*.c')
OBJS = $(addprefix $(BUILD_DIR)/, $(SRCS:.c=.o))


all: $(NAME)

$(NAME): $(BUILD_DIR) $(OBJS)
	$(CC) $(OBJS) -o $(NAME) $(INCS)

$(BUILD_DIR):
	mkdir -p $@	

$(BUILD_DIR)/%.o: %.c 
	$(CC) -c -o $@ $< $(INCS)

clean:
	rm -rf $(BUILD_DIR) $(NAME)

.PHONY: all clean