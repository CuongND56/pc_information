#include "memory_parser.h"

#define IS_PREFIX(str, pre) (strncmp(line, pre, sizeof(pre) - 1) == 0)

static int parse_data(const char* str, unsigned long* num) {
    char* end;
    // skip whitespace
    while (isspace(str[0]))
        ++str;
    // parse number
    *num = strtoull(str, &end, 10);
    if ( end == str )
        return -1;
    // TODO: parse unit
    return 0;
}

int proc_mem_read(struct proc_mem_t* info) {

    FILE* file = fopen("/proc/meminfo", "r");

    if (ferror(file)) {
        fclose(file);
        return -1;
    }

    int i = 0;

    for (; !feof(file) && i < 3; ++i) {
        // -1 cause of \n
        size_t len = getline(&info->buf, &info->allocated, file) - 1;
        // remove newline
        info->buf[len] = '\0';

        char *line = info->buf;
        unsigned long num = 0;

        if (IS_PREFIX(line, "MemTotal:")) {
            line += sizeof("MemTotal:");
            parse_data(line, &num);
            info->total = num;
        } else if (IS_PREFIX(line, "MemFree:")) {
            line += sizeof("MemFree:");
            parse_data(line, &num);
            info->free = num;
        } else if (IS_PREFIX(line, "MemAvailable:")) {
            line += sizeof("MemAvailable:");
            parse_data(line, &num);
            info->avail = num;
        } else {
            // we expect the first three lines to contain the three
            // if one of them doesn't, return with error
            return -1;
        }
    }
    fclose(file);
    return 0;
}