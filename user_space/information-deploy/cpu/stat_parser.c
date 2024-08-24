#include "stat_parser.h"

#define BUFF_SIZE 20
#define TMP_SIZE 4
#define PROC_PATH "/proc/stat"

int scan_proc_stat(size_t *cpu_num) {

    FILE *proc_stat;
    *cpu_num = 0;
    char buff[25] = "";
    char tmp[4] = "";
    tmp[3] = '\0';

    if ((proc_stat = fopen(PROC_PATH, "r")) == NULL) {
        return -1;
    }
    rewind(proc_stat);

    while ((feof(proc_stat)) != -1) {

        if (fscanf(proc_stat, "%s", buff) != 1) {
            return -1;
        }
        if (strcmp(strncpy(tmp, buff, 3), "cpu") == 0) {
            if (fscanf(proc_stat, "%*[^\n]\n") != 0) {
                return -1;
            }
            (*cpu_num)++;
        }
        else {
            fclose(proc_stat);
            return 0;
        }
    }

    fclose(proc_stat);
    return 0;
}

int read_proc_stat(struct stat_struct_t *stat) {

    FILE *proc_stat = NULL;
    char buff[BUFF_SIZE];
    char tmp[TMP_SIZE];
    tmp[TMP_SIZE - 1] = '\0';

    if ((proc_stat = fopen(PROC_PATH, "r")) == NULL) {
        return -1;
    }

    size_t i = 0;
    rewind(proc_stat);

    while ((feof(proc_stat)) != -1) {
        if (fscanf(proc_stat, "%s", buff) != 1){
            return -1;
        }

        if (strcmp(strncpy(tmp, buff, 3), "cpu") == 0) {
            if (fscanf(proc_stat, "%llu %llu %llu %llu %llu %llu %llu %llu %llu %llu%*[^\n]\n", &stat->cpu[i].user, &stat->cpu[i].nice, &stat->cpu[i].system, &stat->cpu[i].idle, &stat->cpu[i].io_wait, &stat->cpu[i].irq, &stat->cpu[i].soft_irq, &stat->cpu[i].steal, &stat->cpu[i].guest, &stat->cpu[i].guest_nice) != 10) {
                return -1;
            }
            i++;
        }
        else {
            fclose(proc_stat);
            return 0;
        }
    }

    fclose(proc_stat);
    
    return 0;
}

void _print_proc_stat(struct stat_struct_t *stat) {
    for (size_t i = 0; i < stat->cpu_num; ++i) {
        printf("CPU%lu: %llu, %llu, %llu, %llu, %llu, %llu, %llu, %llu, %llu, %llu \n", 
            i, stat->cpu[i].user, stat->cpu[i].nice, stat->cpu[i].system, stat->cpu[i].idle, 
            stat->cpu[i].io_wait, stat->cpu[i].irq, stat->cpu[i].soft_irq, stat->cpu[i].steal, 
            stat->cpu[i].guest, stat->cpu[i].guest_nice);
    }
}