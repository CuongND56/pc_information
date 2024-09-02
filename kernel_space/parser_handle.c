#include "parser_handle.h"
#include "font/024_Open_Sans_Bold.h"
#include "font/020_Open_Sans_Bold.h"

#define TAG         "parser_handle"
#define DELIMITER   "," 
#define STR_MAX     100

#define _Open_Sans_Bold_24      &Open_Sans_Bold_24
#define _Open_Sans_Bold_20      &Open_Sans_Bold_20

static void draw_ring_meter(struct ili9341_device *dev_data, char *str, char *result);
static void draw_string_bounded_react(struct ili9341_device *dev_data, char *str, char *result);
static void draw_string(struct ili9341_device *dev_data, char *str, char *result);

struct PARSER_HANDLE_TABLE handle_table[] = {
    { RING_METER, draw_ring_meter },
    { STRING_BOUNDED_OF_REACT, draw_string_bounded_react },
    { DRAW_STRING, draw_string },
    { NULL, NULL}
};


PARSER_STATE parser_handle(struct ili9341_device *dev_data, char *str) {

    int i = 0;
    char result[STR_MAX] = {0};
    PARSER_STATE status = ERROR;
    
    if (str == NULL) {
        pr_info("%s: string is null\n", TAG);
        return status;
    }

    for (; handle_table[i].command != NULL; i ++) {
        // pr_info("%s: len: %d, command: %s, string: %s\n", TAG, strlen(handle_table[i].command), handle_table[i].command, str);
        if (strncmp(handle_table[i].command, str, strlen(handle_table[i].command)) == 0) {
            handle_table[i].draw(dev_data, str, result);
            status = SUCCESS;
        }
    }

    if (status != 0) {
        status = INVALID;
    }

    return status;
}

static void draw_ring_meter(struct ili9341_device *dev_data, char *str, char *result) {
    char command[50] = {0};
    uint16_t x0, y0, x1, y1, x2, y2, color;
    sscanf(str, "%s %hd %hd %hd %hd %hd %hd %hd", command, &x0, &y0, &x1, &y1, &x2, &y2, &color);
    drawFillTriangle(dev_data, x0, y0, x1, y1, x2, y2, color);
}

static void draw_string(struct ili9341_device *dev_data, char *str, char *result) {

    char *message;
    char *token;
    int count = 0;
    int status;
    uint16_t x = 0, y = 0, color = WHITE;

    while ((token = strsep(&str, DELIMITER)) != NULL) {
        status = -1;
        switch (count) {
            case 0: // it is command --> ignore
                break;
            case 1:
                message = token;
                break;
            case 2:
                status = kstrtou16(token, 10, &x);
                if (status != 0) {
                    pr_err("Conversion failed with error code: %d\n", status);
                }
                break;
            case 3:
                status = kstrtou16(token, 10, &y);
                if (status != 0) {
                    pr_err("Conversion failed with error code: %d\n", status);
                }
                break;
            case 4:
                status = kstrtou16(token, 10, &color);
                if (status != 0) {
                    pr_err("Conversion failed with error code: %d\n", status);
                }
                break;
            default:
                break;
        }
        count ++;
    }

    sprintf(result, "%s,%hd,%hd", message, x, y);

    LCD_Font(dev_data, x, y, message, _Open_Sans_Bold_20, 1, color);
}

static void draw_string_bounded_react(struct ili9341_device *dev_data, char *str, char *result) {

    char *message;
    char character;
    uint16_t x = 0, y = 0;
    int count = 0, length_msg = 0, h_temp = 0;
    int x_bound = 0, y_bound = 0, w_bound = 0, h_bound = 0, color_bound;

    draw_string(dev_data, str, result);
    sscanf(result, "%[^,],%hd,%hd", message, &x, &y);

    length_msg = strlen(message);

    w_bound = 5; // offset

    for (; count < length_msg; count ++) {

        character = message[count];
        if (character < ' ') {
            character = 0;
        } else {
            character -= 32;
        }

        w_bound += Open_Sans_Bold_20.glyph[(uint8_t)character].width + Open_Sans_Bold_20.glyph[(uint8_t)character].xOffset;
        h_temp = Open_Sans_Bold_20.glyph[(uint8_t)character].height;

        if (h_bound < h_temp) {
            h_bound = h_temp;
        }

    }

    x_bound = (x > 5) ? (x - 5) : x;
    y_bound = (y > (h_bound + 5)) ? (y - (h_bound + 5)) : y;

    w_bound += 10;
    h_bound += 10;

    lcdFillRoundRect(dev_data, x_bound, y_bound, w_bound, h_bound, 5, GREEN_D);
}

