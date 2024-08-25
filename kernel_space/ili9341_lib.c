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

static uint16_t rainbow(uint16_t value);

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

int ringMeter1(struct ili9341_device *dev_data, int value, int vmin, int vmax, int x, int y, int r, int w, uint16_t bcolor, uint16_t scheme) {
	
	// Minimum value of r is about 52 before value text intrudes on ring
	// drawing the text first is an option
	x += r;
	y += r;										   // Calculate coords of centre of ring
												   //  int w = r / 8;    // Width of outer ring is 1/4 of radius
	int angle = 150;							   // Half the sweep angle of meter (300 degrees)
												   //  int text_colour = 0; // To hold the text colour
	int v = map(value, vmin, vmax, -angle, angle); // Map the value to an angle v
	uint16_t seg = 5;							   // Segments are 5 degrees wide = 60 segments for 300 degrees
	uint16_t inc = 5;							   // Draw segments every 5 degrees, increase to 10 for segmented ring

	// Draw colour blocks every inc degrees
	for (int i = -angle; i < angle; i += inc) {
		// Choose colour from scheme
		int colour = 0;
		switch (scheme) {
			case 0:
				colour = RED;
				break; // Fixed colour
			case 1:
				colour = GREEN;
				break; // Fixed colour
			case 2:
				colour = BLUE;
				break; // Fixed colour
			case 3:
				colour = rainbow(map(i, -angle, angle, 0, 127));
				break; // Full spectrum blue to red
			case 4:
				colour = rainbow(map(i, -angle, angle, 63, 127));
				break; // Green to red (high temperature etc)
			case 5:
				colour = rainbow(map(i, -angle, angle, 127, 63));
				break; // Red to green (low battery etc)
			case 6:
				colour = BLACK;
				break;
			case 7:
				colour = PINK;
				break;
			default:
				colour = BLUE;
				break; // Fixed colour
		}

		// Calculate pair of coordinates for segment start
		float sx = cos((i - 90) * 0.0174532925);
		float sy = sin((i - 90) * 0.0174532925);
		uint16_t x0 = sx * (r - w) + x;
		uint16_t y0 = sy * (r - w) + y;
		uint16_t x1 = sx * r + x;
		uint16_t y1 = sy * r + y;

		// Calculate pair of coordinates for segment end
		float sx2 = cos((i + seg - 90) * 0.0174532925);
		float sy2 = sin((i + seg - 90) * 0.0174532925);
		int x2 = sx2 * (r - w) + x;
		int y2 = sy2 * (r - w) + y;
		int x3 = sx2 * r + x;
		int y3 = sy2 * r + y;

		if (i < v) {	
			// Fill in coloured segments with 2 triangles
			//      my_lcd.Set_Draw_color(colour);
			//      my_lcd.Fill_Triangle(x0, y0, x1, y1, x2, y2);
			//      my_lcd.Fill_Triangle(x1, y1, x2, y2, x3, y3);
			//      text_colour = colour; // Save the last colour drawn

			drawFillTriangle(dev_data, x0, y0, x1, y1, x2, y2, colour);
			drawFillTriangle(dev_data, x1, y1, x2, y2, x3, y3, colour);
		} else {
			// Fill in blank segments
			//      my_lcd.Set_Draw_color(GRAY);
			//      my_lcd.Fill_Triangle(x0, y0, x1, y1, x2, y2);
			//      my_lcd.Fill_Triangle(x1, y1, x2, y2, x3, y3);

			drawFillTriangle(dev_data, x0, y0, x1, y1, x2, y2, bcolor);
			drawFillTriangle(dev_data, x1, y1, x2, y2, x3, y3, bcolor);
		}
	}
	return x + r;
}

static uint16_t rainbow(uint16_t value) {

	// Value is expected to be in range 0-127
	// The value is converted to a spectrum colour from 0 = blue through to 127 = red

	uint16_t red = 0; // Red is the top 5 bits of a 16 bit colour value
	uint16_t green = 0;// Green is the middle 6 bits
	uint16_t blue = 0; // Blue is the bottom 5 bits

	uint16_t quadrant = value / 32;

	switch (quadrant) {

		case 0:
			blue = 31;
			green = 2 * (value % 32);
			red = 0;
			break;

		case 1:
			blue = 31 - (value % 32);
			green = 63;
			red = 0;
			break;
		
		case 2:
			blue = 0;
			green = 63;
			red = value % 32;
			break;
		
		case 3:
			blue = 0;
			green = 63 - 2 * (value % 32);
			red = 31;
			break;
		
		default:
			break;

	}

	return (red << 11) + (green << 5) + blue;
}

/**************************************************************************/
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
		if (x1 < a) {
			a = x1;
		} else if (x1 > b){
			b = x1;
		}
		if (x2 < a) {
			a = x2;
		} else if (x2 > b) {
			b = x2;
		}

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
