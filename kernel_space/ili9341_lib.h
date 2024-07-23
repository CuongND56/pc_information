#ifndef __ILI9341_LIB_H_
#define __ILI9341_LIB_H_

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/slab.h> 
#include <linux/fs.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/types.h>
#include <linux/slab.h> 
#include <linux/spi/spi.h>
#include <linux/gpio/consumer.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <ili9341_commands.h>

#define ILI9341_HEIGHT              320
#define ILI9341_WIDTH               240
#define ILI9341_BYTES_PER_PIXEL     3
#define ILI9341_BITS_PER_PIXEL      (8 * ILI9341_BYTES_PER_PIXEL)
#define ILI9341_COLOR_LENGTH        6
#define ILI9341_BUFFER_SIZE         (ILI9341_HEIGHT * ILI9341_WIDTH * ILI9341_BYTES_PER_PIXEL)
#define ILI9341_LINE_LENGTH         (ILI9341_WIDTH * ILI9341_BYTES_PER_PIXEL)

#define ILI9341_RED_OFFSET          18
#define ILI9341_GREEN_OFFSET        10
#define ILI9341_BLUE_OFFSET         2

struct ili9341_device {
    struct spi_device *client;
    struct gpio_desc *dc_gpio;
    u8 *display_buff;
};

int ili9341_init(struct ili9341_device *dev_data);
int ili9341_send_display_buff(struct ili9341_device *dev_data);
int ili9341_display_on(struct ili9341_device *dev_data);
int ili9341_software_reset(struct ili9341_device *dev_data);
int ili9341_send_command(struct device_data *dev_data, u8 *buff, size_t len);
int ili9341_send_command_with_args(struct device_data *dev_data, u8 cmd, u8 *args, size_t args_len);
int ili9341_send_data(struct device_data *dev_data, u8 *buff, size_t len);

#endif /* __ILI9341_LIB_H_ */