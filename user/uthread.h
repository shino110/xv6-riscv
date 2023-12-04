#define STACK_DEPTH 512

struct context_node {
    uint64 stack[STACK_DEPTH];
    int context_numth;    //何番目の子スレッドかを返す
    struct context cnxt;
    struct context_node *next;
};
struct id_pool {
    int num;
    struct id_pool *next;
};

struct context parent;
struct context_node *head;
struct context_node *tail;
struct id_pool *id_head;
struct id_pool *id_tail;
int total_node;

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