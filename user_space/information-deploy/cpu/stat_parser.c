#include "stat_parser.h"

#define BUFF_SIZE 20
#define TMP_SIZE 4
#define PROC_PATH "/proc/stat"

static char *trim(char *str);
static unsigned long long int cpu_work(struct cpu_struct_t *stat);
static unsigned long long int cpu_total(struct cpu_struct_t *stat);
static uint8_t usage_cpu(struct cpu_struct_t *prev_stat, struct cpu_struct_t *cur_stat);

static char *trim(char *str) {
    while (*str && isspace((unsigned char)*str)) {
        str++;
    }
    if (*str) {
        // Here it is safe to use `strlen - 1` because
        // `if (*str)` ensures that there is a character
        char *end = str + strlen(str) - 1;

        while (*end && isspace((unsigned char)*end)) {
            end--;
        }
        *(end + 1) = '\0';
    }
    return str;
}


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
            
            // printf("scan_proc_stat, buff: %s\n", buff);

            if (fscanf(proc_stat, "%*[^\n]\n") != 0) {
                return -1;
            }

            if (strlen(buff) > 3) {
                (*cpu_num)++;
            }
        }
        else {
            fclose(proc_stat);
            return 0;
        }
    }


    fclose(proc_stat);
    return 0;
}

static unsigned long long int cpu_work(struct cpu_struct_t *stat){

    return stat->user
         + stat->nice
         + stat->system
         // not entirely sure about these following two, whether or not they
         // count as the processor doing work
         + stat->guest
         + stat->guest_nice;
}

static unsigned long long int cpu_total(struct cpu_struct_t *stat) {

    return stat->user
         + stat->nice
         + stat->system
         + stat->idle
         + stat->io_wait
         + stat->irq
         + stat->soft_irq
         + stat->steal
         + stat->guest
         + stat->guest_nice;
}

// 1890455
// 1890556

static uint8_t usage_cpu(struct cpu_struct_t *prev_stat, struct cpu_struct_t *cur_stat) {
    
    uint8_t percentage = 0;
    unsigned long long int prev_work = 0, cur_work = 0, prev_total = 0, cur_total = 0;

    if (prev_stat == NULL || prev_stat->user == 0) {
        printf("stat_parser: pre_stat is invalid --> return\n");
        cur_stat->percentage = 0;
        memcpy(prev_stat, cur_stat, sizeof(struct cpu_struct_t));
        return -1;
    }

    prev_work   = cpu_work(prev_stat);
    cur_work    = cpu_work(cur_stat);
    prev_total  = cpu_total(prev_stat);
    cur_total   = cpu_total(cur_stat);

    percentage = (cur_work - prev_work) / (cur_total - prev_total);

    cur_stat->percentage = percentage;
    memcpy(prev_stat, cur_stat, sizeof(struct cpu_struct_t));

    return percentage;

}

int read_proc_stat(struct stat_struct_t *stat) {

    size_t i = 0;
    FILE *proc_stat = NULL;
    char buff[BUFF_SIZE], tmp[TMP_SIZE];
    static struct cpu_struct_t prev_cpu0_stat = { 0 };

    if ((proc_stat = fopen(PROC_PATH, "r")) == NULL) {
        return -1;
    }

    tmp[TMP_SIZE - 1] = '\0';
    rewind(proc_stat);

    while ((feof(proc_stat)) != -1) {
        if (fscanf(proc_stat, "%s", buff) != 1){
            return -1;
        }

        if (strcmp(strncpy(tmp, buff, 3), "cpu") == 0) {

            if (strlen(buff) > 3) {
                if (fscanf(proc_stat, "%llu %llu %llu %llu %llu %llu %llu %llu %llu %llu%*[^\n]\n", 
                    &stat->cpu[i].user, &stat->cpu[i].nice, &stat->cpu[i].system, &stat->cpu[i].idle, 
                    &stat->cpu[i].io_wait, &stat->cpu[i].irq, &stat->cpu[i].soft_irq, &stat->cpu[i].steal, 
                    &stat->cpu[i].guest, &stat->cpu[i].guest_nice) != 10) {
                        return -1;
                    }
                if (i == 0) {
                    usage_cpu(&prev_cpu0_stat, &(stat->cpu[i]));
                }
                i++;
            } else {
                if (fscanf(proc_stat, "%*[^\n]\n") != 0) {
                    return -1;
                }
            }
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