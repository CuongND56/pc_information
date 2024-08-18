#ifndef __ILI9341_LIB_H_
#define __ILI9341_LIB_H_

#include "ili9341_commands.h"
#include "spi_common/spi_common.h"

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
#define ILI9341_DC_COMMAND          0

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

struct ili9341_device {
	struct spi_common spi_dev;
    u8 *display_buff;
};

int ili9341_init(struct ili9341_device *dev_data);
int ili9341_send_display_buff(struct ili9341_device *dev_data);
int ili9341_display_on(struct ili9341_device *dev_data);
int ili9341_software_reset(struct ili9341_device *dev_data);
void fillScreen(struct ili9341_device *dev_data, int color);
void ili9341_hardware_reset(struct ili9341_device *dev_data);
void drawChar(struct ili9341_device *dev_data, char c, int x, int y, int color, int size, int bgcolor);
void drawRect(struct ili9341_device *dev_data, int x, int y, int w, int h, int color);
void drawFastVLine(struct ili9341_device *dev_data, int x, int y, int h, int color);
void drawRect(struct ili9341_device *dev_data, int x, int y, int w, int h, int color);
void drawFastVLine(struct ili9341_device *dev_data, int x, int y, int h, int color);
void drawFastHLine(struct ili9341_device *dev_data, int x, int y, int w, int color);
void drawLine(struct ili9341_device *dev_data, int x0, int y0, int x1, int y1, int color);
void drawText(struct ili9341_device *dev_data, const char *text, int x, int y, int color, int size, int bgcolor);

#define CHAR_WIDTH 6
#define CHAR_HEIGHT 8

struct spi_cmd_list_data init_cmd_list_data_t[] = {
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

#endif /* __ILI9341_LIB_H_ */