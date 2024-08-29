BBB_KERNEL := /home/cuong/Desktop/kernel_build/kernelbuildscripts/KERNEL
TOOLCHAIN := /home/cuong/Desktop/kernel_build/kernelbuildscripts/dl/gcc-8.5.0-nolibc/arm-linux-gnueabi/bin/arm-linux-gnueabi-

EXTRA_CFLAGS=-Wall
obj-m := ili9341_oled_driver_1.o
ili9341_oled_driver_1-objs = ili9341_driver.o ili9341_lib.o ./spi_common/spi_common.o ./font/font8.o ./font/font12.o ./font/font16.o ./font/font20.o ./font/font24.o ./parser_handle.o

all:
	make ARCH=arm CROSS_COMPILE=$(TOOLCHAIN) -C $(BBB_KERNEL) M=$(shell pwd) modules
	
clean:
	make -C $(BBB_KERNEL) M=$(shell pwd) clean