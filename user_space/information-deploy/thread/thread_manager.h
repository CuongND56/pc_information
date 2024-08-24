#ifndef __THREAD_MANAGER_H_
#define __THREAD_MANAGER_H_

#include "../common_lib.h"
#include "../cpu/stat_parser.h"
#include "../memory/memory_parser.h"

#define THREAD_MAX 2
#define TAG_MAX 100

typedef enum {
    READ, READING,
    DREW, DRAWING,
    NONE
} Event_type;

struct Data {
    struct proc_mem_t       memory_stat;
    struct stat_struct_t    cpu_stat;
};

struct Thread_manager {
    struct Data inform;
    pthread_mutex_t lock;
    pthread_cond_t cond;
    Event_type event;
    unsigned int thread_num;
};

struct Thread_worker {
    pthread_t thread;
    char TAG[TAG_MAX];
    struct Thread_manager *mManager;
};

void *thread_display(void *args);
void *thread_read(void *args);

int initialize();
void deinitialize();

#endif /* __THREAD_MANAGER_H_ */