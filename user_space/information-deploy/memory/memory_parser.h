#ifndef __MEMORY_PARSER_H_
#define __MEMORY_PARSER_H_

#include "../common_lib.h"

struct proc_mem_t {

    unsigned long total;
    unsigned long avail;
    unsigned long free;
    char* buf;
    size_t allocated;
};

int proc_mem_read(struct proc_mem_t *info);

#endif /* __MEMORY_PARSER_H_ */
