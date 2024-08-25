#ifndef __ILI9341_LIB_H_
#define __ILI9341_LIB_H_

#include "ili9341_commands.h"
#include "spi_common/spi_common.h"
#include "font5x7.h"

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

#define RED2RED 0
#define GREEN2GREEN 1
#define BLUE2BLUE 2
#define BLUE2RED 3
#define GREEN2RED 4
#define RED2GREEN 5
#define BWHITE 6

#define CHAR_WIDTH 6
#define CHAR_HEIGHT 8

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
void drawFillTriangle(struct ili9341_device *dev_data, int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
int ringMeter1(struct ili9341_device *dev_data, int value, int vmin, int vmax, int x, int y, int r, int w, uint16_t bcolor, uint16_t scheme);

#endif /* __ILI9341_LIB_H_ */