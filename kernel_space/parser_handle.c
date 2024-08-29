#include "parser_handle.h"

static void draw_ring_meter(struct ili9341_device *dev_data, char *str);
static void draw_string_bounded_react(struct ili9341_device *dev_data, char *str);

#define TAG "parser_handle"

struct PARSER_HANDLE_TABLE handle_table[] = {
    { RING_METER, draw_ring_meter },
    { STRING_BOUNDED_OF_REACT, draw_string_bounded_react },
    { NULL, NULL}
};


PARSER_STATE parser_handle(struct ili9341_device *dev_data, char *str) {

    PARSER_STATE status = ERROR;
    int i = 0;
    
    if (str == NULL) {
        pr_info("%s: string is null\n", TAG);
        return status;
    }

    for (; handle_table[i].command != NULL; i ++) {
        // pr_info("%s: len: %d, command: %s, string: %s\n", TAG, strlen(handle_table[i].command), handle_table[i].command, str);
        if (strncmp(handle_table[i].command, str, strlen(handle_table[i].command)) == 0) {
            handle_table[i].draw(dev_data, str);
            status = SUCCESS;
        }
    }

    if (status != 0) {
        status = NOT_CONTAIN;
    }

    return status;
}

static void draw_ring_meter(struct ili9341_device *dev_data, char *str) {
    char command[50] = {0};
    uint16_t x0, y0, x1, y1, x2, y2, color;
    sscanf(str, "%s %hd %hd %hd %hd %hd %hd %hd", command, &x0, &y0, &x1, &y1, &x2, &y2, &color);
    drawFillTriangle(dev_data, x0, y0, x1, y1, x2, y2, color);
}

static void draw_string_bounded_react(struct ili9341_device *dev_data, char *str) {

}

