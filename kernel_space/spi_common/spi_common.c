#include "spi_common.h"

static long double factorialx(int n);
static long double powerx(double base, int exp);

void spi_common_select(struct spi_common *spi_dev) {
	gpiod_set_value(spi_dev->cs_gpio, LOW);
}

void spi_common_unselect(struct spi_common *spi_dev) {
	gpiod_set_value(spi_dev->cs_gpio, HIGH);
}

int spi_common_send_command(struct spi_common *spi_dev, u8 *buff, size_t len) {
	gpiod_set_value(spi_dev->dc_gpio, SPI_COMMON_DC_COMMAND);
    return spi_write(spi_dev->client, buff, len);
}

int spi_common_send_one_command(struct spi_common *spi_dev, u8 cmd) {
	gpiod_set_value(spi_dev->dc_gpio, SPI_COMMON_DC_COMMAND);
    return spi_write(spi_dev->client, (void *)&cmd, 1);
}

int spi_common_send_data(struct spi_common *spi_dev, u8 *buff, size_t len) {
	gpiod_set_value(spi_dev->dc_gpio, SPI_COMMON_DC_DATA);
    return spi_write(spi_dev->client, buff, len);
}

int spi_common_send_one_data(struct spi_common *spi_dev, unsigned char c) {
	gpiod_set_value(spi_dev->dc_gpio, SPI_COMMON_DC_DATA);
    return spi_write(spi_dev->client, (void *)&c, 1);
}

int
spi_common_isEndOfTable(struct spi_cmd_list_data *list) {

    int status;

    status = 1;
    if (list == NULL) return -1;
    if ((list->cmd == 0x00) && (list->length_data == 0)) {
        status = 0;
    }
    return status;
}

int 
spi_common_send_command_with_args(struct spi_common *spi_dev, struct spi_cmd_list_data *list, u32 msec) {

    int status;
    // int i;

    status = 0;
    if (spi_dev == NULL || list == NULL) return -1;
    // pr_info("-------------------------------------------------------------------------------------");
    while (spi_common_isEndOfTable(list) == 1) {
        // pr_info("cmd: %x, data: ", list->cmd);
        // i = 0;
        // for (i = 0; i < list->length_data; i ++) {
        //     pr_info("%x, ", list->data[i]);
        // }
        // pr_info("\n");
        status = spi_common_send_one_command(spi_dev, list->cmd);
        if (status != 0) {
            pr_err("%s, %d, status: %d\n", __func__, __LINE__, status);
            return status;
        }
        if (list->length_data != 0) {
            mdelay(msec);
            spi_common_send_data(spi_dev, list->data, list->length_data);
            if (status != 0) {
                pr_err("%s, %d, status: %d\n", __func__, __LINE__, status);
                return status;
            }
        }
    
        list ++;
    }
    // pr_info("-------------------------------------------------------------------------------------");


    return status;

}

void spi_common_hardware_reset(struct spi_common *spi_dev) {

	gpiod_set_value(spi_dev->rs_gpio, LOW);
	mdelay(5);
	gpiod_set_value(spi_dev->rs_gpio, HIGH);
}

// Function to calculate factorial of a number
static long double factorialx(int n) {
    long double fact = 1.0;
    int i = 1;
    for (; i <= n; i++) {
        fact *= i;
    }
    return fact;
}

// Function to calculate power of a number
static long double powerx(double base, int exp) {
    long double result = 1.0;
    int i = 1;
    for (; i <= exp; i++) {
        result *= base;
    }
    return result;
}

// Define the sinTaylor function
long double sinx(double x, int terms) {
    long double sum = 0.0;
    int n = 0;
    for (; n < terms; n++) {
        // Calculate the nth term
        long double term = powerx(-1, n) * powerx(x, 2*n + 1) / factorialx(2*n + 1);
        sum += term;
    }
    
    return sum;
}

long double cosx(double x, int terms) {
    long double sum = 1.0;
    int n = 1;
    
    for (; n < terms; n++) {
        // Calculate the nth term
        long double term = powerx(-1, n) * powerx(x, 2*n) / factorialx(2*n);
        sum += term;
    }
    
    return sum;
}


