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
#include <linux/gpio.h>
#include "ili9341_commands.h"

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

#define ILI9341_CASET		        0x2A

#define ILI9341_DC_DATA             1

#define ILI9341_BLACK		0x0000
#define ILI9341_NAVY		0x000F
#define ILI9341_DARKGREEN	0x03E0
#define ILI9341_DARKCYAN	0x03EF
#define ILI9341_MAROON		0x7800
#define ILI9341_PURPLE		0x780F
#define ILI9341_OLIVE		0x7BE0
#define ILI9341_LIGHTGREY	0xC618
#define ILI9341_DARKGREY	0x7BEF
#define ILI9341_BLUE		0x001F
#define ILI9341_GREEN		0x07E0
#define ILI9341_CYAN		0x07FF
#define ILI9341_RED		0xF800
#define ILI9341_MAGENTA		0xF81F
#define ILI9341_YELLOW		0xFFE0
#define ILI9341_WHITE		0xFFFF
#define ILI9341_ORANGE		0xFD20
#define ILI9341_GREENYELLOW	0xAFE5
#define ILI9341_PINK		0xF81F

#define LOW                 0
#define HIGH                1

struct ili9341_device {
    struct spi_device *client;
    int dc_gpio;
    u8 *display_buff;
};

int ili9341_init(struct ili9341_device *dev_data);
int ili9341_send_display_buff(struct ili9341_device *dev_data);
int ili9341_display_on(struct ili9341_device *dev_data);
int ili9341_software_reset(struct ili9341_device *dev_data);
int ili9341_send_command(struct ili9341_device *dev_data, u8 *buff, size_t len);
int ili9341_send_command_with_args(struct ili9341_device *dev_data, u8 cmd, u8 *args, size_t args_len);
int ili9341_send_data(struct ili9341_device *dev_data, u8 *buff, size_t len);
void fillScreen(struct ili9341_device *dev_data, int color);

#endif /* __ILI9341_LIB_H_ */