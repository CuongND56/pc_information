#include "thread_manager.h"

void *thread_read(void *args) {

    int status;
    struct Thread_worker *mWorker = (struct Thread_worker *)args;

    pthread_mutex_lock(&(mWorker->mManager->lock));

    status = scan_proc_stat(&(mWorker->mManager->inform.cpu_stat.cpu_num));
    printf("%s: thread_read, status: %d, number of cpu: %d\n", mWorker->TAG, status, mWorker->mManager->inform.cpu_stat.cpu_num);
    if (status == -1) {
        printf("%s: scan_proc_stat failed --> exit\n", mWorker->TAG);
        pthread_exit(NULL);
    } 
    mWorker->mManager->inform.cpu_stat.cpu = (struct cpu_struct_t *)malloc(mWorker->mManager->inform.cpu_stat.cpu_num * sizeof(struct cpu_struct_t));

    pthread_mutex_unlock(&(mWorker->mManager->lock));

    while (1) {

        pthread_mutex_lock(&(mWorker->mManager->lock));

        mWorker->mManager->event = READING;
        status = read_proc_stat(&(mWorker->mManager->inform.cpu_stat));
        if (status == -1) {
            printf("%s: read_proc_stat failed\n", mWorker->TAG);
        }
        status = proc_mem_read(&(mWorker->mManager->inform.memory_stat));
        if (status == -1) {
            printf("%s: proc_mem_read failed\n", mWorker->TAG);
        } 
        mWorker->mManager->event = READ;

        pthread_cond_signal(&(mWorker->mManager->cond));
        pthread_mutex_unlock(&(mWorker->mManager->lock));
        sleep(1);
    }

    pthread_exit(NULL);
}