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

static lcdPropertiesTypeDef  lcdProperties = { ILI9341_WIDTH, ILI9341_HEIGHT, ORIENTATION_PORTRAIT };

static unsigned char lcdPortraitConfig = 0;
static unsigned char lcdLandscapeConfig = 0;
static unsigned char lcdPortraitMirrorConfig = 0;
static unsigned char lcdLandscapeMirrorConfig = 0;

static unsigned char lcdBuildMemoryAccessControlConfig(
                        bool rowAddressOrder,
                        bool columnAddressOrder,
                        bool rowColumnExchange,
                        bool verticalRefreshOrder,
                        bool colorOrder,
                        bool horizontalRefreshOrder);
						

// static uint16_t rainbow(uint16_t value);
// static long map(long x, long in_min, long in_max, long out_min, long out_max);
static void lcdFillCircleHelper(struct ili9341_device *dev_data, int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color);

int ili9341_init(struct ili9341_device *dev_data) {

    int status = 0;

	lcdPortraitConfig = lcdBuildMemoryAccessControlConfig(
														MemoryAccessControlNormalOrder,		// rowAddressOrder
														MemoryAccessControlReverseOrder,	// columnAddressOrder
														MemoryAccessControlNormalOrder,		// rowColumnExchange
														MemoryAccessControlNormalOrder,		// verticalRefreshOrder
														MemoryAccessControlColorOrderBGR,	// colorOrder
														MemoryAccessControlNormalOrder);	// horizontalRefreshOrder

	lcdLandscapeConfig = lcdBuildMemoryAccessControlConfig(
														MemoryAccessControlNormalOrder,		// rowAddressOrder
														MemoryAccessControlNormalOrder,		// columnAddressOrder
														MemoryAccessControlReverseOrder,	// rowColumnExchange
														MemoryAccessControlNormalOrder,		// verticalRefreshOrder
														MemoryAccessControlColorOrderBGR,	// colorOrder
														MemoryAccessControlNormalOrder);	// horizontalRefreshOrder

	lcdPortraitMirrorConfig = lcdBuildMemoryAccessControlConfig(
														MemoryAccessControlReverseOrder,	// rowAddressOrder
														MemoryAccessControlNormalOrder,		// columnAddressOrder
														MemoryAccessControlNormalOrder,		// rowColumnExchange
														MemoryAccessControlNormalOrder,		// verticalRefreshOrder
														MemoryAccessControlColorOrderBGR,	// colorOrder
														MemoryAccessControlNormalOrder);	// horizontalRefreshOrder

	lcdLandscapeMirrorConfig = lcdBuildMemoryAccessControlConfig(
														MemoryAccessControlReverseOrder,	// rowAddressOrder
														MemoryAccessControlReverseOrder,	// columnAddressOrder
														MemoryAccessControlReverseOrder,	// rowColumnExchange
														MemoryAccessControlNormalOrder,		// verticalRefreshOrder
														MemoryAccessControlColorOrderBGR,	// colorOrder
														MemoryAccessControlNormalOrder);	// horizontalRefreshOrder

	spi_common_select(&dev_data->spi_dev);
    ili9341_hardware_reset(dev_data);
    status = ili9341_software_reset(dev_data);
    if (status)
    	return status;
	mdelay(1000);
    spi_common_send_command_with_args(&dev_data->spi_dev, init_cmd_list_data_t, 0);
	spi_common_unselect(&dev_data->spi_dev);

	lcdSetOrientation(dev_data, ORIENTATION_LANDSCAPE);

	fillScreen(dev_data, ILI9341_WHITE);

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

	// if ((x < 0) || (x >= ILI9341_WIDTH) || (y < 0) || (y >= ILI9341_HEIGHT))
	if ((x < 0) || (x >= lcdProperties.width) || (y < 0) || (y >= lcdProperties.height))
		return;
	spi_common_select(&dev_data->spi_dev);
	setAddrWindow(dev_data, x, y, x + 1, y + 1);
	spi_common_send_data(&dev_data->spi_dev, data_color, sizeof(data_color));
	spi_common_unselect(&dev_data->spi_dev);
}

void fillScreen(struct ili9341_device *dev_data, int color) {

	int x, y;

	for (x = 0; x < lcdProperties.width; x++) {
		for (y = 0; y < lcdProperties.height; y++) {
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

	drawRect(dev_data, x, y, x + CHAR_WIDTH * size, y + CHAR_HEIGHT * size, bgcolor);

	for (j = 0; j < CHAR_WIDTH; j++) {
		for (i = 0; i < CHAR_HEIGHT; i++) {
			if (temp[j] & (1 << i)) {
					drawPixel(dev_data, x + j, y + i, color);

				// if (size == 1)
				// 	drawPixel(dev_data, x + j, y + i, color);
				// else
				// 	drawRect(dev_data, x + (j * size), y + (i * size), size, size, color);
			}
		}
	}
}

void drawRect(struct ili9341_device *dev_data, int x, int y, int w, int h, int color) {

	drawFastHLine(dev_data, x, y, w, color);
	drawFastHLine(dev_data, x, h - 1, w, color);
	drawFastVLine(dev_data, x, y, h, color);
	drawFastVLine(dev_data, w - 1, y, h, color);
}

void drawFastVLine(struct ili9341_device *dev_data, int x, int y, int h, int color) {
	drawLine(dev_data, x, y, x, h - 1, color);
}

void drawFastHLine(struct ili9341_device *dev_data, int x, int y, int w, int color) {
	// drawLine(dev_data, x, y, x + w - 1, y, color);
	drawLine(dev_data, x, y, w - 1, y, color);
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
		text ++;
	}
}

/*!
   @brief     Draw a triangle with color-fill
    @param    x0  Vertex #0 x coordinate
    @param    y0  Vertex #0 y coordinate
    @param    x1  Vertex #1 x coordinate
    @param    y1  Vertex #1 y coordinate
    @param    x2  Vertex #2 x coordinate
    @param    y2  Vertex #2 y coordinate
    @param    color 16-bit 5-6-5 Color to fill/draw with
*/
/**************************************************************************/
void drawFillTriangle(struct ili9341_device *dev_data, int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {

	int16_t a, b, y, last;
	int16_t dx01, dy01, dx02, dy02, dx12, dy12;
	int32_t sa = 0, sb = 0;

	// Sort coordinates by Y order (y2 >= y1 >= y0)
	if (y0 > y1) {
		swap(y0, y1);
		swap(x0, x1);
	}

	if (y1 > y2) {
		swap(y2, y1);
		swap(x2, x1);
	}

	if (y0 > y1) {
		swap(y0, y1);
		swap(x0, x1);
	}

	if (y0 == y2) { // Handle awkward all-on-same-line case as its own thing
		a = b = x0;
		if (x1 < a) 		a = x1;
		else if (x1 > b) 	b = x1;
		
		if (x2 < a) 		a = x2;
		else if (x2 > b) 	b = x2;
		
		drawFastHLine(dev_data, a, y0, b + 1, color);

		return;
	}


	dx01 = x1 - x0;
	dy01 = y1 - y0;
	dx02 = x2 - x0;
	dy02 = y2 - y0;
	dx12 = x2 - x1;
	dy12 = y2 - y1;

	// For upper part of triangle, find scanline crossings for segments
	// 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
	// is included here (and second loop will be skipped, avoiding a /0
	// error there), otherwise scanline y1 is skipped here and handled
	// in the second loop...which also avoids a /0 error here if y0=y1
	// (flat-topped triangle).
	if (y1 == y2) {
		last = y1; // Include y1 scanline
	} else {
		last = y1 - 1; // Skip it
	}

	for (y = y0; y <= last; y++) {
		a = x0 + sa / dy01;
		b = x0 + sb / dy02;
		sa += dx01;
		sb += dx02;
		/* longhand:
		a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
		b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
		*/
		if (a > b) {
			swap(a, b);
		}

		drawFastHLine(dev_data, a, y, b + 1, color);
	}

	// For lower part of triangle, find scanline crossings for segments
	// 0-2 and 1-2.  This loop is skipped if y1=y2.
	sa = (int32_t)dx12 * (y - y1);
	sb = (int32_t)dx02 * (y - y0);
	for (; y <= y2; y++) {

		a = x1 + sa / dy12;
		b = x0 + sb / dy02;
		sa += dx12;
		sb += dx02;
		/* longhand:
		a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
		b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
		*/
		if (a > b) {
			swap(a, b);
		}

		drawFastHLine(dev_data, a, y, b + 1, color);
	}
}

void lcdSetOrientation(struct ili9341_device *dev_data, OrientationTypeDef value) {

	lcdProperties.orientation = value;

	spi_common_select(&dev_data->spi_dev);

	spi_common_send_one_command(&dev_data->spi_dev, ILI9341_MEMCONTROL);

	switch (lcdProperties.orientation)
	{
		case ORIENTATION_PORTRAIT:
			spi_common_send_one_data(&dev_data->spi_dev, lcdPortraitConfig);
			lcdProperties.width = ILI9341_WIDTH;
			lcdProperties.height = ILI9341_HEIGHT;
			break;
		case ORIENTATION_PORTRAIT_MIRROR:
			spi_common_send_one_data(&dev_data->spi_dev, lcdPortraitMirrorConfig);
			lcdProperties.width = ILI9341_WIDTH;
			lcdProperties.height = ILI9341_HEIGHT;
			break;
		case ORIENTATION_LANDSCAPE:
			spi_common_send_one_data(&dev_data->spi_dev, lcdLandscapeConfig);
			lcdProperties.width = ILI9341_HEIGHT;
			lcdProperties.height = ILI9341_WIDTH;
			break;
		case ORIENTATION_LANDSCAPE_MIRROR:
			spi_common_send_one_data(&dev_data->spi_dev, lcdLandscapeMirrorConfig);
			lcdProperties.width = ILI9341_HEIGHT;
			lcdProperties.height = ILI9341_WIDTH;
			break;
		default:
			break;
	}

	setAddrWindow(dev_data, 0, 0, lcdProperties.width - 1, lcdProperties.height - 1);
	spi_common_unselect(&dev_data->spi_dev);
}

static unsigned char lcdBuildMemoryAccessControlConfig( bool rowAddressOrder,
														bool columnAddressOrder,
														bool rowColumnExchange,
														bool verticalRefreshOrder,
														bool colorOrder,
														bool horizontalRefreshOrder ) {

	unsigned char value 				= 0;
	if(horizontalRefreshOrder) value 	|= ILI9341_MADCTL_MH;
	if(colorOrder) value 				|= ILI9341_MADCTL_BGR;
	if(verticalRefreshOrder) value 		|= ILI9341_MADCTL_ML;
	if(rowColumnExchange) value 		|= ILI9341_MADCTL_MV;
	if(columnAddressOrder) value 		|= ILI9341_MADCTL_MX;
	if(rowAddressOrder) value 			|= ILI9341_MADCTL_MY;
	return value;
}

void LCD_Font(struct ili9341_device *dev_data, uint16_t x, uint16_t y, const char *text, const GFXfont *p_font, uint8_t size, uint32_t color24) {

	int16_t cursor_x = x;
	int16_t cursor_y = y;
	uint16_t text_pos;
	GFXfont font;

	memcpy(&font, p_font, sizeof(GFXfont));
	for (text_pos = 0; text_pos < strlen(text); text_pos ++) {
		char c = text[text_pos];
		if (c == '\n') {
			cursor_x = x;
			cursor_y += font.yAdvance * size;
		}
		else if (c >= font.first && c <= font.last && c != '\r') {
			GFXglyph glyph;
			memcpy(&glyph, &font.glyph[c - font.first], sizeof(GFXglyph));
			LCD_Char(dev_data, cursor_x, cursor_y, &glyph, &font, size, color24);
			cursor_x += glyph.xAdvance * size;
		}
	}
}

void LCD_Char(struct ili9341_device *dev_data, int16_t x, int16_t y, const GFXglyph *glyph, const GFXfont *font, uint8_t size, uint32_t color24) {

	uint8_t  *bitmap = font -> bitmap;
	uint16_t bo = glyph -> bitmapOffset;
	uint8_t bits = 0, bit = 0;
	uint16_t set_pixels = 0;
	uint8_t  cur_x, cur_y;

	for (cur_y = 0; cur_y < glyph->height; cur_y++) {

		for (cur_x = 0; cur_x < glyph->width; cur_x++) {

			if (bit == 0) {
				bits = (*(const unsigned char *)(&bitmap[bo++]));
				bit  = 0x80;
			}

			if (bits & bit) set_pixels++;
			else if (set_pixels > 0) {
				lcdFillRect(dev_data, x + (glyph->xOffset + cur_x - set_pixels) * size, y + (glyph->yOffset + cur_y) * size, size * set_pixels, size, color24);
				set_pixels = 0;
			}
			bit >>= 1;
		}
		if (set_pixels > 0) { 
			lcdFillRect(dev_data, x + (glyph->xOffset + cur_x - set_pixels) * size, y + (glyph->yOffset + cur_y) * size, size * set_pixels, size, color24);
			set_pixels = 0;
		}
	}
}

void lcdFillRect(struct ili9341_device *dev_data, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t fillcolor) {

	uint16_t y1;
	// clipping
	if((x >= lcdProperties.width) || (y >= lcdProperties.height)) return;
	if((x + w - 1) >= lcdProperties.width) w = lcdProperties.width - x;
	if((y + h - 1) >= lcdProperties.height) h = lcdProperties.height - y;

	for(y1 = y; y1 <= y + h; y1++)
	{
		drawFastHLine(dev_data, x, y1, x + w, fillcolor);
	}
}

void lcdFillRoundRect(struct ili9341_device *dev_data, int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
	// smarter version
	lcdFillRect(dev_data, x + r, y, w - 2 * r, h, color);

	// draw four corners
	lcdFillCircleHelper(dev_data, x + w - r - 1, y + r, r, 1, h - 2 * r - 1, color);
	lcdFillCircleHelper(dev_data, x + r, y + r, r, 2, h - 2 * r - 1, color);
}

void lcdFillCircleHelper(struct ili9341_device *dev_data, int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	while (x < y){

		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		if (cornername & 0x1) {
			drawFastVLine(dev_data, x0 + x, y0 - y, y0 + y + 1 + delta, color);
			drawFastVLine(dev_data, x0 + y, y0 - x, y0 + x + 1 + delta, color);
		}
		if (cornername & 0x2) {
			drawFastVLine(dev_data, x0 - x, y0 - y, y0 + y + 1 + delta, color);
			drawFastVLine(dev_data, x0 - y, y0 - x, y0 + x + 1 + delta, color);
		}
	}
}

