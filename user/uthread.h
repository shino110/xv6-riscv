//uthread.c
//Level 1
int make_uthread(void (*fun)());
void start_uthreads();
void yield();
int mytid();

//Level 2
void uthread_exit();

//Level 3
void uthread_wait(void *a);
void uthread_notify(int tid, void *a);
void uthread_notify_all(void *a);