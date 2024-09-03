#ifndef __ILI9341_DRIVER_H_
#define __ILI9341_DRIVER_H_

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/vmalloc.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/fb.h>
#include <linux/init.h>
#include <linux/backlight.h>
#include <linux/platform_device.h>
#include <linux/uaccess.h>
#include <linux/spi/spi.h>
#include <linux/gpio/consumer.h>
#include <linux/gpio.h>

struct device_t {
    struct spi_device *client;
    struct gpio_desc *dc_gpio;
    struct gpio_desc *rs_gpio;
    struct gpio_desc *cs_gpio;
};

#endif /* __ILI9341_DRIVER_H_ */