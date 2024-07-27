#include "ili9341_lib.h"

static const u8 display_init_sequence[] = {
    1, ILI9341_PWCTR1, 0x23,
    1, ILI9341_PWCTR2, 0x10,
    2, ILI9341_VMCTR1, 0x3E, 0x28,
    1, ILI9341_VMCTR2, 0x86,
    1, ILI9341_MADCTL, 0x40, /* RGB */
    1, ILI9341_VSCRSADD, 0x00,
    1, ILI9341_PIXFMT, 0x66, /* 18 bits per pixel */
    2, ILI9341_FRMCTR1, 0x00, 0x18,
    3, ILI9341_DFUNCTR, 0x08, 0x82, 0x27,
    0x00,
};

int ili9341_init(struct ili9341_device *dev_data) {

    int status;
    u8 cmd_len;
    const u8 *buff;

    status = 0;
    buff = display_init_sequence;

    status = ili9341_software_reset(dev_data);
    if (status)
        return status;

    /* break on 0 at the end of the array */
    while (*buff) {
        cmd_len = *buff;
        status = ili9341_send_command_with_args(dev_data, *(buff + 1), (u8 *)buff + 2, cmd_len);
        if (status)
            return status;
        buff += (cmd_len + 2);
    }
    status = ili9341_display_on(dev_data);
    return status;
}

int ili9341_send_display_buff(struct ili9341_device *dev_data) {

    int status;
    u8 start_cmd;

    status = 0;
    start_cmd = 0x2C;
    status = ili9341_send_command(dev_data, &start_cmd, 1);
    if(status)
        return status;
    status = ili9341_send_data(dev_data, dev_data->display_buff, ILI9341_BUFFER_SIZE);
    return status;
}

int ili9341_display_on(struct ili9341_device *dev_data) {
    
    int status = 0;
    uint8_t on_sequence[] = { ILI9341_SLPOUT, ILI9341_DISPON };

    for (int i = 0; i < (sizeof(on_sequence) / sizeof(on_sequence[0])); i++) {
        status = ili9341_send_command(dev_data, &on_sequence[i], 1);
        if (status)
            return status;
        msleep(150);
    }
    return status;
}

int ili9341_software_reset(struct ili9341_device *dev_data) {

    int status = 0;
    uint8_t reset_cmd = ILI9341_SWRESET;

    status = ili9341_send_command(dev_data, &reset_cmd, 1);
    if (status)
        return status;
    msleep(150);
    return status;
}

int ili9341_send_command(struct ili9341_device *dev_data, u8 *buff, size_t len) {

    gpio_set_value(dev_data->dc_gpio, ILI9341_DC_COMMAND);
    return spi_write(dev_data->client, buff, len);
}

int ili9341_send_command_with_args(struct ili9341_device *dev_data, u8 cmd, u8 *args, size_t args_len) {

    int status;
    status = ili9341_send_command(dev_data, &cmd, 1);
    if (status)
        return status;
    status = ili9341_send_data(dev_data, args, args_len);
    return status;
}

int ili9341_send_data(struct ili9341_device *dev_data, u8 *buff, size_t len) {

    gpio_set_value(dev_data->dc_gpio, ILI9341_DC_DATA);
    return spi_write(dev_data->client, buff, len);
}

int ili9341_send_one(struct ili9341_device *dev_data, unsigned char c) {

    gpio_set_value(dev_data->dc_gpio, ILI9341_DC_DATA);
    return spi_write(dev_data->client, c, 1);
}

void setAddrWindow(struct ili9341_device *dev_data, int x0, int y0, int x1, int y1) {

	ili9341_send_command(dev_data, ILI9341_CASET);
	ili9341_send_one(dev_data, x0 >> 8);
	ili9341_send_one(dev_data, x0 & 0xFF);
	ili9341_send_one(dev_data, x1 >> 8);
	ili9341_send_one(dev_data, x1 & 0xFF);

	ili9341_send_command(dev_data, ILI9341_PASET);
	ili9341_send_one(dev_data, y0 >> 8);
	ili9341_send_one(dev_data, y0);
	ili9341_send_one(dev_data, y1 >> 8);
	ili9341_send_one(dev_data, y1);

	ili9341_send_command(dev_data, ILI9341_RAMWR);
}

void drawPixel(struct ili9341_device *dev_data, int x, int y, int color) {

	if ((x < 0) || (x >= width) || (y < 0) || (y >= height))
		return;
	setAddrWindow(dev_data, x, y, x + 1, y + 1);
	ili9341_send_one(dev_data, color >> 8);
	ili9341_send_one(dev_data, color);
}

void fillScreen(struct ili9341_device *dev_data, int color) {

	int x, y;

	for (x = 0; x < ILI9341_WIDTH; x++) {
		for (y = 0; y < ILI9341_HEIGHT; y++) {
			drawPixel(dev,x, y, color);
		}
	}
}











