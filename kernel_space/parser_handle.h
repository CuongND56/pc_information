#ifndef __PARSER_HANDLE_H_
#define __PARSER_HANDLE_H_

#include "ili9341_lib.h"

#define MAX_LENGTH 100

#define RING_METER                  "RING_METER"
#define STRING_BOUNDED_OF_REACT     "STRING_BOUNDED_OF_REACT"
#define CLEAR_SCREEN			    "CLEAR_SCREEN"
#define DRAW_STRING			        "DRAW_STRING"

typedef enum {

    ERROR = -1,
    SUCCESS = 0,
    INVALID = 1

} PARSER_STATE;

struct PARSER_HANDLE_TABLE {
    // char *command[MAX_LENGTH];
    char *command;
    void (*draw)(struct ili9341_device *dev_data, char *str, char *result);
};

PARSER_STATE parser_handle(struct ili9341_device *dev_data, char *str);

#endif /* __PARSER_HANDLE_H_ */