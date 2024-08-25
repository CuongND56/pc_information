#include "ili9341_lib.h"

#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }

static struct spi_cmd_list_data init_cmd_list_data_t[] = {
	{ 0xCB, 5, { 0x39, 0x2C, 0x00, 0x34, 0x02 } },
	{ 0xCF, 3, { 0x00, 0xC1, 0x30 } },
	{ 0xE8, 3, { 0x85, 0x00, 0x78 } },
	{ 0xEA, 2, { 0x00, 0x00 } },
	{ 0xED, 4, { 0x64, 0x03, 0x12, 0x81 } },
	{ 0xF7, 1, { 0x20 } },
	{ ILI9341_PWCTR1, 1, { 0x23 } },
	{ ILI9341_PWCTR2, 1, { 0x10 } },
	{ ILI9341_VMCTR1, 2, { 0x3E, 0x28 } },
	{ ILI9341_VMCTR2, 1, { 0x86 } },
	{ ILI9341_MADCTL, 1, { 0x48 } },
	{ ILI9341_PIXFMT, 1, { 0x55 } },
	{ ILI9341_FRMCTR1, 2, { 0x00, 0x18 } },
	{ ILI9341_DFUNCTR, 3, { 0x08, 0x82, 0x27 } },
	{ 0xF2, 1, { 0x00 } },
	{ ILI9341_GAMMASET, 1, { 0x01 } },
	{ ILI9341_GMCTRP1, 15, { 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00 } },
	{ ILI9341_GMCTRN1, 15, { 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F } },
	{ ILI9341_SLPOUT, 0, { } },
	{ ILI9341_DISPON, 0, { } },
	{ ILI9341_MADCTL, 1, { ILI9341_ROTATION } },
	END_OF_TABLE
};

int ili9341_init(struct ili9341_device *dev_data) {

    int status;

    status = 0;

	spi_common_select(&dev_data->spi_dev);
    ili9341_hardware_reset(dev_data);
    status = ili9341_software_reset(dev_data);
    if (status)
    	return status;
	mdelay(1000);
    spi_common_send_command_with_args(&dev_data->spi_dev, init_cmd_list_data_t, 0);
	spi_common_unselect(&dev_data->spi_dev);

    return status;
}

// int ili9341_send_display_buff(struct ili9341_device *dev_data) {

// 	struct spi_cmd_list_data list[] = { {0x2C, 1, { ILI9341_BUFFER_SIZE }}, END_OF_TABLE };
//     return spi_common_send_command_with_args(&dev_data->spi_dev, list, 0);
// }

int ili9341_display_on(struct ili9341_device *dev_data) {
    
    int i = 0;
    int status = 0;
    u8 on_sequence[] = { ILI9341_SLPOUT, ILI9341_DISPON };

	pr_info("ili9341_display_on\n");

	spi_common_select(&dev_data->spi_dev);
    for (i = 0; i < 2; i++) {
        status = spi_common_send_command(&dev_data->spi_dev, &on_sequence[i], 1);
        if (status) {
        	return status;
		}
	    msleep(150);
    }
	spi_common_unselect(&dev_data->spi_dev);

    return status;
}

int ili9341_software_reset(struct ili9341_device *dev_data) {

    int status = 0;
    uint8_t reset_cmd = ILI9341_SWRESET;

    status = spi_common_send_command(&dev_data->spi_dev, &reset_cmd, 1);
    if (status)
        return status;
    mdelay(150);
    return status;
}

void ili9341_hardware_reset(struct ili9341_device *dev_data) {
	spi_common_hardware_reset(&dev_data->spi_dev);
}

void setAddrWindow(struct ili9341_device *dev_data, int x0, int y0, int x1, int y1) {

	struct spi_cmd_list_data x_list[] = { {ILI9341_CASET, 4, { (x0 >> 8) & 0xFF, x0 & 0xFF, (x1 >> 8) & 0xFF, x1 & 0xFF }}, END_OF_TABLE };
	struct spi_cmd_list_data y_list[] = { {ILI9341_PASET, 4, { (y0 >> 8) & 0xFF, y0 & 0xFF, (y1 >> 8) & 0xFF, y1 & 0xFF }}, END_OF_TABLE };

	spi_common_send_command_with_args(&dev_data->spi_dev, x_list, 0);
	spi_common_send_command_with_args(&dev_data->spi_dev, y_list, 0);
	spi_common_send_one_command(&dev_data->spi_dev, ILI9341_RAMWR);
}

void drawPixel(struct ili9341_device *dev_data, int x, int y, int color) {

	uint8_t data_color[] = { color >> 8, color & 0xFF };

	if ((x < 0) || (x >= ILI9341_WIDTH) || (y < 0) || (y >= ILI9341_HEIGHT))
		return;
	spi_common_select(&dev_data->spi_dev);
	setAddrWindow(dev_data, x, y, x + 1, y + 1);
	spi_common_send_data(&dev_data->spi_dev, data_color, sizeof(data_color));
	spi_common_unselect(&dev_data->spi_dev);
}

void fillScreen(struct ili9341_device *dev_data, int color) {

	int x, y;

	for (x = 0; x < ILI9341_WIDTH; x++) {
		for (y = 0; y < ILI9341_HEIGHT; y++) {
			drawPixel(dev_data, x, y, color);
		}
	}
}

void drawChar(struct ili9341_device *dev_data, char c, int x, int y, int color, int size, int bgcolor) {

	char temp[CHAR_WIDTH];
	char character = c;
	int i, j;
	int k;

	if (character < ' ') {
		character = 0;
	} else {
		character -= 32;
	}

	for (k = 0; k < CHAR_WIDTH; k++) {
		temp[k] = font[(uint8_t)character][k];
	}

	drawRect(dev_data, x, y, CHAR_WIDTH * size, CHAR_HEIGHT * size, bgcolor);

	for (j = 0; j < CHAR_WIDTH; j++) {
		for (i = 0; i < CHAR_HEIGHT; i++) {
			if (temp[j] & (1 << i)) {
				if (size == 1)
					drawPixel(dev_data, x + j, y + i, color);
				else
					drawRect(dev_data, x + (j * size), y + (i * size), size, size, color);
			}
		}
	}
}

void drawRect(struct ili9341_device *dev_data, int x, int y, int w, int h, int color) {

	drawFastHLine(dev_data, x, y, w, color);
	drawFastHLine(dev_data, x, y + h - 1, w, color);
	drawFastVLine(dev_data, x, y, h, color);
	drawFastVLine(dev_data, x + w - 1, y, h, color);
}

void drawFastVLine(struct ili9341_device *dev_data, int x, int y, int h, int color) {
	drawLine(dev_data, x, y, x, y + h - 1, color);
}

void drawFastHLine(struct ili9341_device *dev_data, int x, int y, int w, int color) {
	drawLine(dev_data, x, y, x + w - 1, y, color);
}

void drawLine(struct ili9341_device *dev_data, int x0, int y0, int x1, int y1, int color) {

	int dx, dy, err, ystep;
	int steep = abs(y1 - y0) > abs(x1 - x0);

	if (steep) {
		_swap_int16_t(x0, y0);
		_swap_int16_t(x1, y1);
	}

	if (x0 > x1) {
		_swap_int16_t(x0, x1);
		_swap_int16_t(y0, y1);
	}

	dx = x1 - x0;
	dy = abs(y1 - y0);

	err = dx / 2;

	if (y0 < y1)
		ystep = 1;
	else
		ystep = -1;

	for (; x0 <= x1; x0++) {
		if (steep)
			drawPixel(dev_data, y0, x0, color);
		else
			drawPixel(dev_data, x0, y0, color);
		err -= dy;
		if (err < 0) {
			y0 += ystep;
			err += dx;
		}
	}
}

void drawText(struct ili9341_device *dev_data, const char *text, int x, int y, int color, int size, int bgcolor) {

	while (*text) {
		drawChar(dev_data, *text, x, y, color, size, bgcolor);
		x += CHAR_WIDTH * size;
	}
}






