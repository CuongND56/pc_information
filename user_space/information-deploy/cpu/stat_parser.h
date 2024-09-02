#ifndef __STAT_PARSER_H_
#define __STAT_PARSER_H_

#include "../common_lib.h"

/* Contains data aquired when reading the cpuN data from `/proc/stat` */
struct cpu_struct_t {

    unsigned long long int user;            /* Total time spent by user mode processes */
    unsigned long long int nice;            /* Total time spent by niced user mode processes */
    unsigned long long int system;          /* Total time spent by system mode processes */
    unsigned long long int idle;            /* Total time spent by idle processes */
    unsigned long long int io_wait;         /* Total time spent waiting for I/O to complete */
    unsigned long long int irq;             /* Total time spent servicing hardware interrupts */
    unsigned long long int soft_irq;        /* Total time spent servicing software interrupts */
    unsigned long long int steal;           /* Total time spent in other Operating Systems when running in a virtualized environment */
    unsigned long long int guest;           /* Total time spent running a virtual CPU for guest Operating Systems */
    unsigned long long int guest_nice;      /* Total time spent running a niced guest operating system */

    uint8_t percentage;
};


/* Contains data from a whole reading of `/proc/stat` file */
struct stat_struct_t {

    size_t cpu_num;
    struct cpu_struct_t *cpu;
};

/***Scans the contents of `/proc/stat` to determine number of CPUs 
    Return: `true` if successfully calculated number of CPU cores, else `false`***/
int scan_proc_stat(size_t *cpu_num);

/***Reads the most important contents of the `/proc/stat` file
    Return `true` if successfully scraped metrics from `/proc/stat`, else `false`***/
int read_proc_stat(struct stat_struct_t *stat);

void _print_proc_stat(struct stat_struct_t *stat);

#endif /* __STAT_PARSER_H_ */