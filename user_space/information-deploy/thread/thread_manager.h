#ifndef __THREAD_MANAGER_H_
#define __THREAD_MANAGER_H_

#include "../common_lib.h"
#include "../cpu/stat_parser.h"
#include "../memory/memory_parser.h"

#define THREAD_MAX 2
#define TAG_MAX 100


typedef uint16_t colour_t;

#define BLACK 						((colour_t)0x0000)
#define YELLOW 						((colour_t)0xffe0)
#define RED 						((colour_t)0xf800)
#define GREEN 						((colour_t)0x07e0)
#define BLUE 						((colour_t)0x001f)
#define WHITE 						((colour_t)0xffff)
#define PINK 						((colour_t)0xf80e)
#define PURPLE 						((colour_t)0xf83f)
#define	GREY15						((colour_t)0x1082)
#define	GREY14						((colour_t)0x2104)
#define	GREY13						((colour_t)0x3186)
#define	GREY12						((colour_t)0x4208)
#define	GREY11						((colour_t)0x528a)
#define	GREY10						((colour_t)0x630c)
#define	GREY9						((colour_t)0x738e)
#define	GREY8						((colour_t)0x8410)
#define	GREY7						((colour_t)0x9492)
#define	GREY6						((colour_t)0xa514)
#define	GREY5						((colour_t)0xb596)
#define	GREY4						((colour_t)0xc618)
#define	GREY3						((colour_t)0xd69a)
#define	GREY2						((colour_t)0xe71c)
#define	GREY1						((colour_t)0xf79e)
#define ORANGE 						((colour_t)0xfb80)
#define CYAN						((colour_t)0x07ff)
#define DARK_CYAN					((colour_t)0x0492)
#define LIGHT_ORANGE				((colour_t)0xfe20)
#define BRICK_RED					((colour_t)0xb104)
#define GREEN_D					    ((colour_t)0x0400)


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