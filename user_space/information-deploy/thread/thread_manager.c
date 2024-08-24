#include "thread_manager.h"

static struct Thread_manager mThread_manager = {
    .inform = { 0 },
    .lock = PTHREAD_MUTEX_INITIALIZER,
    .cond = PTHREAD_COND_INITIALIZER,
    .event = NONE,
};

static struct Thread_worker *thread_worker_initialize(char *tag, void *(*start_routine) (void *arg)) {

    int status;
    struct Thread_worker *mWorker = NULL;
    
    mWorker = (struct Thread_worker *)malloc(sizeof(struct Thread_worker));

    if (mWorker == NULL) {
        printf("%s: cannot allocate memory!!", tag);
        return NULL;
    }

    if (status = pthread_create(&mWorker->thread, NULL, start_routine, mWorker)) {
        printf("%s: pthread_create() error number = %d\n", tag, status);
        return NULL;
    }

    mWorker->mManager = &mThread_manager;
    strcpy(mWorker->TAG, tag);

    return mWorker;
}

int initialize() {

    int status;
    struct Thread_worker *tid_read, *tid_display = { 0 };

    if ((tid_read = thread_worker_initialize("THREAD_READ", thread_read)) == NULL) {
        return -1;
    }

    if ((tid_display = thread_worker_initialize("THREAD_DISPLAY", thread_display)) == NULL) {
        return -1;
    }

    return 0;
}

void deinitialize() {

}
