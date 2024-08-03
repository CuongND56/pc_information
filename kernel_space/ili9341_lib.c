#include "ili9341_lib.h"

#define ILI9341_RS_PIN 30

#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }

// static struct spi_cmd_list_data init_cmd_list_data_t[] = {
// 	{ 0xCB, 5, { 0x39, 0x2C, 0x00, 0x34, 0x02 } },
// 	{ 0xCF, 3, { 0x00, 0xC1, 0x30 } },
// 	{ 0xE8, 3, { 0x85, 0x00, 0x78 } },
// 	{ 0xEA, 2, { 0x00, 0x00 } },
// 	{ 0xED, 4, { 0x64, 0x03, 0x12, 0x81 } },
// 	{ 0xF7, 1, { 0x20 } },
// 	{ ILI9341_PWCTR1, 1, { 0x23 } },
// 	{ ILI9341_PWCTR2, 1, { 0x10 } },
// 	{ ILI9341_VMCTR1, 2, { 0x3E, 0x28 } },
// 	{ ILI9341_VMCTR2, 1, { 0x86 } },
// 	{ ILI9341_MADCTL, 1, { 0x48 } },
// 	{ ILI9341_PIXFMT, 1, { 0x55 } },
// 	{ ILI9341_FRMCTR1, 2, { 0x00, 0x18 } },
// 	{ ILI9341_DFUNCTR, 3, { 0x08, 0x82, 0x27 } },
// 	{ 0xF2, 1, { 0x00 } },
// 	{ ILI9341_GAMMASET, 1, { 0x01 } },
// 	{ ILI9341_GMCTRP1, 15, { 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00 } },
// 	{ ILI9341_GMCTRN1, 15, { 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F } },
// 	{ ILI9341_SLPOUT, 0, { } },
// 	{ ILI9341_DISPON, 0, { } },
// 	{ ILI9341_MADCTL, 1, { ILI9341_ROTATION } }
// };

int ili9341_init(struct ili9341_device *dev_data) {

    int status = 0;
    u8 cmd_len;
    const u8 *buff;

    status = 0;

	ili9341_select(dev_data);

    ili9341_hardware_reset(dev_data);


    status = ili9341_software_reset(dev_data);
    if (status)
        return status;
	
	mdelay(1000);

    ili9341_send_one_command(dev_data, 0xCB);
	uint8_t data_1[] = {0x39, 0x2C, 0x00, 0x34, 0x02};
	ili9341_send_data(dev_data, data_1, sizeof(data_1));

	ili9341_send_one_command(dev_data, 0xCF);
	uint8_t data_2[] = {0x00, 0xC1, 0x30};
	ili9341_send_data(dev_data, data_2, sizeof(data_2));

	ili9341_send_one_command(dev_data, 0xE8);
	uint8_t data_3[] = {0x85, 0x00, 0x78};
	ili9341_send_data(dev_data, data_3, sizeof(data_3));

	ili9341_send_one_command(dev_data, 0xEA);
	uint8_t data_4[] = {0x00, 0x00};
	ili9341_send_data(dev_data, data_4, sizeof(data_4));

    ili9341_send_one_command(dev_data, 0xED);
	uint8_t data_5[] = {0x64, 0x03, 0x12, 0x81};
	ili9341_send_data(dev_data, data_5, sizeof(data_5));

    ili9341_send_one_command(dev_data, 0xF7);
    ili9341_send_one_data(dev_data, 0x20);

	ili9341_send_one_command(dev_data, ILI9341_PWCTR1);
	ili9341_send_one_data(dev_data, 0x23);

	ili9341_send_one_command(dev_data, ILI9341_PWCTR2);
	ili9341_send_one_data(dev_data, 0x10);

	ili9341_send_one_command(dev_data, ILI9341_VMCTR1);
	uint8_t data_6[] = {0x3E, 0x28};
	ili9341_send_data(dev_data, data_6, sizeof(data_6));

	ili9341_send_one_command(dev_data, ILI9341_VMCTR2);
	ili9341_send_one_data(dev_data, 0x86);

	ili9341_send_one_command(dev_data, ILI9341_MADCTL);
	ili9341_send_one_data(dev_data, 0x48);

	ili9341_send_one_command(dev_data, ILI9341_PIXFMT);
	ili9341_send_one_data(dev_data, 0x55);

	ili9341_send_one_command(dev_data, ILI9341_FRMCTR1);
	uint8_t data_7[] = {0x00, 0x18};
	ili9341_send_data(dev_data, data_7, sizeof(data_7));

	ili9341_send_one_command(dev_data, ILI9341_DFUNCTR);
	uint8_t data_8[] = {0x08, 0x82, 0x27};
	ili9341_send_data(dev_data, data_8, sizeof(data_8));

	ili9341_send_one_command(dev_data, 0xF2);
	ili9341_send_one_data(dev_data, 0x00);

	ili9341_send_one_command(dev_data, ILI9341_GAMMASET);
	ili9341_send_one_data(dev_data, 0x01);

	ili9341_send_one_command(dev_data, ILI9341_GMCTRP1);
	uint8_t data_9[] = { 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00 };
	ili9341_send_data(dev_data, data_9, sizeof(data_9));

	ili9341_send_one_command(dev_data, ILI9341_GMCTRN1);
	uint8_t data_10[] = { 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F };
	ili9341_send_data(dev_data, data_10, sizeof(data_10));

	ili9341_send_one_command(dev_data, ILI9341_SLPOUT);
	mdelay(120);
	ili9341_send_one_command(dev_data, ILI9341_DISPON);
	ili9341_send_one_command(dev_data, ILI9341_MADCTL);
	uint8_t data_12[] = {ILI9341_ROTATION};
	ili9341_send_data(dev_data, data_12, sizeof(data_12));

	ili9341_unselect(dev_data);

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
    
    int i = 0;
    int status = 0;
    uint8_t on_sequence[] = { ILI9341_SLPOUT, ILI9341_DISPON };

    for (i = 0; i < 2; i++) {
        status = ili9341_send_command(dev_data, &on_sequence[i], 1);
		pr_info("ili9341_display_on, status[%d]: %d\n", i, status);
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
    mdelay(150);
    return status;
}

void ili9341_hardware_reset(struct ili9341_device *dev_data) {

	// gpio_set_value(ILI9341_RS_PIN, LOW);
	gpiod_set_value(dev_data->rs_gpio, LOW);
	mdelay(5);
	gpiod_set_value(dev_data->rs_gpio, HIGH);
	// gpio_set_value(ILI9341_RS_PIN, HIGH);
}


int ili9341_send_command(struct ili9341_device *dev_data, u8 *buff, size_t len) {

	gpiod_set_value(dev_data->dc_gpio, ILI9341_DC_COMMAND);
    // gpio_set_value(dev_data->dc_gpio, ILI9341_DC_COMMAND);
    return spi_write(dev_data->client, buff, len);
}

int ili9341_send_one_command(struct ili9341_device *dev_data, u8 cmd) {

	gpiod_set_value(dev_data->dc_gpio, ILI9341_DC_COMMAND);
    // gpio_set_value(dev_data->dc_gpio, ILI9341_DC_COMMAND);
    return spi_write(dev_data->client, (void *)&cmd, 1);
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

	gpiod_set_value(dev_data->dc_gpio, ILI9341_DC_DATA);
    // gpio_set_value(dev_data->dc_gpio, ILI9341_DC_DATA);
    return spi_write(dev_data->client, buff, len);
}

int ili9341_send_one_data(struct ili9341_device *dev_data, unsigned char c) {

	gpiod_set_value(dev_data->dc_gpio, ILI9341_DC_DATA);
    // gpio_set_value(dev_data->dc_gpio, ILI9341_DC_DATA);
    return spi_write(dev_data->client, (void *)&c, 1);
}

void setAddrWindow(struct ili9341_device *dev_data, int x0, int y0, int x1, int y1) {

    uint8_t cmds[] = { ILI9341_CASET, ILI9341_PASET, ILI9341_RAMWR };

	uint8_t data_x[] = { (x0 >> 8) & 0xFF, x0 & 0xFF, (x1 >> 8) & 0xFF, x1 & 0xFF };
	uint8_t data_y[] = { (y0 >> 8) & 0xFF, y0 & 0xFF, (y1 >> 8) & 0xFF, y1 & 0xFF };

	ili9341_send_command(dev_data, (u8 *)&cmds[0], 1);
	ili9341_send_data(dev_data, data_x, sizeof(data_x));
	// ili9341_send_one_data(dev_data, x0 >> 8);
	// ili9341_send_one_data(dev_data, x0 & 0xFF);
	// ili9341_send_one_data(dev_data, x1 >> 8);
	// ili9341_send_one_data(dev_data, x1 & 0xFF);

	ili9341_send_command(dev_data, (u8 *)&cmds[1], 1);
	ili9341_send_data(dev_data, data_y, sizeof(data_y));
	// ili9341_send_one_data(dev_data, y0 >> 8);
	// ili9341_send_one_data(dev_data, y0);
	// ili9341_send_one_data(dev_data, y1 >> 8);
	// ili9341_send_one_data(dev_data, y1);

	ili9341_send_command(dev_data, (u8 *)&cmds[2], 1);
}

void drawPixel(struct ili9341_device *dev_data, int x, int y, int color) {

	uint8_t data_color[] = { color >> 8, color & 0xFF };

	if ((x < 0) || (x >= ILI9341_WIDTH) || (y < 0) || (y >= ILI9341_HEIGHT))
		return;
	ili9341_select(dev_data);
	setAddrWindow(dev_data, x, y, x + 1, y + 1);
	ili9341_send_data(dev_data, data_color, sizeof(data_color));
	ili9341_unselect(dev_data);
	// ili9341_send_one_data(dev_data, color >> 8);
	// ili9341_send_one_data(dev_data, color);
}

void fillScreen(struct ili9341_device *dev_data, int color) {

	int x, y;

	for (x = 0; x < ILI9341_WIDTH; x++) {
		for (y = 0; y < ILI9341_HEIGHT; y++) {
			drawPixel(dev_data, x, y, color);
		}
	}
}

void drawChar(struct ili9341_device *dev_data, char c, int x, int y, int color, int size, int bgcolor)
{
	char character = c;
	int i, j;
	int k;

	if (character < ' ') {
		character = 0;
	} else {
		character -= 32;
	}

	char temp[CHAR_WIDTH];
	for (k = 0; k < CHAR_WIDTH; k++) {
		temp[k] = font[character][k];
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

void drawRect(struct ili9341_device *dev_data, int x, int y, int w, int h, int color)
{
	drawFastHLine(dev_data, x, y, w, color);
	drawFastHLine(dev_data, x, y + h - 1, w, color);
	drawFastVLine(dev_data, x, y, h, color);
	drawFastVLine(dev_data, x + w - 1, y, h, color);
}

void drawFastVLine(struct ili9341_device *dev_data, int x, int y, int h, int color)
{
	drawLine(dev_data, x, y, x, y + h - 1, color);
}

void drawFastHLine(struct ili9341_device *dev_data, int x, int y, int w, int color)
{
	drawLine(dev_data, x, y, x + w - 1, y, color);
}

void drawLine(struct ili9341_device *dev_data, int x0, int y0, int x1, int y1, int color)
{
	int steep = abs(y1 - y0) > abs(x1 - x0);
	int dx, dy;

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

	int err = dx / 2;
	int ystep;

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

void drawText(struct ili9341_device *dev_data, const char *text, int x, int y, int color, int size, int bgcolor)
{
	while (*text) {
		drawChar(dev_data, *text, x, y, color, size, bgcolor);
		x += CHAR_WIDTH * size;
	}
}

void ili9341_select(struct ili9341_device *dev_data) {
	gpiod_set_value(dev_data->cs_gpio, LOW);
}

void ili9341_unselect(struct ili9341_device *dev_data) {
	gpiod_set_value(dev_data->cs_gpio, HIGH);
}





