#ifndef __SPI_COMMON_H_
#define __SPI_COMMON_H_

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

#define LOW                     0
#define HIGH                    1

#define SPI_COMMON_DC_DATA      1
#define SPI_COMMON_DC_COMMAND   0

#define END_OF_TABLE    { 0x00, 0, { } }



struct spi_cmd_list_data {
	u8 cmd;
	u8 length_data;
	u8 data[20];
};

struct spi_common {
    struct spi_device *client;
    struct gpio_desc *dc_gpio;
    struct gpio_desc *rs_gpio;
    struct gpio_desc *cs_gpio;
};

void spi_common_select(struct spi_common *spi_dev);
void spi_common_unselect(struct spi_common *spi_dev);
int spi_common_send_command(struct spi_common *spi_dev, u8 *buff, size_t len);
int spi_common_send_one_command(struct spi_common *spi_dev, u8 cmd);
int spi_common_send_data(struct spi_common *spi_dev, u8 *buff, size_t len);
int spi_common_send_one_data(struct spi_common *spi_dev, u8 c);
int spi_common_isEndOfTable(struct spi_cmd_list_data *list);
int spi_common_send_command_with_args(struct spi_common *spi_dev, struct spi_cmd_list_data *list, u32 msec);
void spi_common_hardware_reset(struct spi_common *spi_dev);

long double sinx(double x, int terms);

long double cosx(double x, int terms);

#endif