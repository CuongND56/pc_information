#include "thread_manager.h"

void *thread_display(void *args) {

    int status;
    struct Data mData = {0};
    struct Thread_worker *mWorker = (struct Thread_worker *)args;

    while (1) {
        
        pthread_mutex_lock(&(mWorker->mManager->lock));
        while (mWorker->mManager->event != READ) {
            pthread_cond_wait(&(mWorker->mManager->cond), &(mWorker->mManager->lock));
        }

        mWorker->mManager->event = DRAWING;

        if (mData.cpu_stat.cpu_num == 0 && mWorker->mManager->inform.cpu_stat.cpu_num != 0) {
            mData.cpu_stat.cpu = (struct cpu_struct_t *)malloc(mWorker->mManager->inform.cpu_stat.cpu_num * sizeof(struct cpu_struct_t));
        }

        mData = mWorker->mManager->inform;
        memcpy(mData.cpu_stat.cpu, mWorker->mManager->inform.cpu_stat.cpu, mData.cpu_stat.cpu_num * sizeof(struct cpu_struct_t));

        _print_proc_stat(&mData.cpu_stat);

        pthread_mutex_unlock(&(mWorker->mManager->lock));

        // display 
    }
}