#include "kernel/types.h"
#include "user/user.h"
#define STACK_DEPTH 512

// Saved registers for kernel context switches. (from kernel/proc.h)
struct context {
  uint64 ra;
  uint64 sp;

  // callee-saved
  uint64 s0;
  uint64 s1;
  uint64 s2;
  uint64 s3;
  uint64 s4;
  uint64 s5;
  uint64 s6;
  uint64 s7;
  uint64 s8;
  uint64 s9;
  uint64 s10;
  uint64 s11;
};

// swtch.S (from kernel/defs.h)
void swtch(struct context*, struct context*);

struct context_node {
    uint64 stack[STACK_DEPTH];
    int context_numth;    //何番目の子スレッドかを返す
    int waiting;          //waitingなどの状態を表す。
    void *waiting_for;
    struct context cnxt;
    struct context_node *next;
};

struct id_pool {
    int num;
    struct id_pool *next;
};

struct context parent;
struct context tmp;
struct context_node *head;
struct context_node *tail;
struct id_pool *id_head;
struct id_pool *id_tail;
int total_node;
int total_pool;

// help function
void head_goes2_tail(void) {
    struct context_node *tmp = head;
    if (total_node > 1) {
        head = tmp->next;
        tail->next = tmp;
        tail = tmp;
    }
}

int search_nonwaiting_head() {
    for (int i = 0; i < total_node; i++) {
        if (head->waiting < 1) {
            return 1;
        }
        head_goes2_tail();
    }
    //all child threads is waiting
    return -1;
}

void start_nextthread(int from_parent, int yield) {
    struct context trash;
    if (total_node == 0) {
        swtch(&trash, &parent);
    }
    if (from_parent > 0) {
        if (search_nonwaiting_head()) swtch(&parent, &(head->cnxt));
        //if all child threads is waiting do nothing
    } else if (yield > 0){
        if (total_node > 1) {
            struct context_node *tmp = head;
            head_goes2_tail(); //current context goes to last on queue
            int runnable = search_nonwaiting_head();
            if (runnable < 0 || tmp->context_numth == head->context_numth) {
                // other child threads are waiting so start parent
                swtch(&(head->cnxt), &parent);
            } else {
                swtch(&(tmp->cnxt), &(head->cnxt)); 
            }
        } else {
            //no other child thread to yield
            swtch(&(head->cnxt), &parent);
        }
    } else { //from uthread_exit()
        if (search_nonwaiting_head()) { 
            swtch(&trash, &(head->cnxt));
        } else { // if all child threads is waiting go back to parent thread
            swtch(&trash, &parent);
        }
    }
}

//Level 1
int make_uthread(void (*fun)()) {
    struct context_node *child = malloc(sizeof(struct context_node));

    child->cnxt.ra = (uint64)*fun;
    child->cnxt.sp = (uint64)(child->stack + STACK_DEPTH);
    child->waiting = 0;

    if (total_node > 0) {//child arleady exists
        //decide numbering of thread
        if (total_pool > 0) {
            struct id_pool *tmp;
            child->context_numth = id_head->num;

            tmp = id_head->next; // if next == NULL, id_head = NULL
            free(id_head);
            id_head = tmp;
            total_pool -= 1;
        } else {
            child->context_numth = total_node;
        }
        total_node += 1;

        //connect in unidirect list
        tail->next = child;
        tail = child;
    } else { //first child thread to run
        head = child;
        tail = child;
        child->context_numth = 0;
        total_node = 1;
        total_pool = 0;
    }
    return child->context_numth;
}

void start_uthreads() {
    start_nextthread(1, 0);
}

void yield() {
    start_nextthread(0, 1);
}

int mytid() {
    return head->context_numth;
}

//Level 2
void uthread_exit() {
    struct context_node *tmp;
    struct id_pool *id_child = malloc(sizeof(struct id_pool));

    total_node -= 1;
    if (total_node > 0) {
        tmp = head->next; //if next == NULL, head = NULL
    } else {
        tmp = head; // not going to be used 
    }

    //deal with new available tid
    if (total_node == 0) {
        // all child is run
        // delete all id_pool
        if (total_pool > 1) {
            struct id_pool *tmp1 = id_head->next;
            free(id_head);
            id_head = tmp1;
            while (id_tail->num != id_head->num) {
                tmp1 = id_head->next;
                free(id_head);
                id_head = tmp1;
            }
            free(id_head);
        } else if (total_pool == 1) {
            free(id_head);
        }
        free(id_child);
        total_pool = 0;
    } else {
        //put current id in id_pool
        id_child->num = head->context_numth;
        if(total_pool > 0) {
            id_tail->next = id_child;
            id_tail = id_child;
            total_pool += 1;
        } else {
            id_head = id_child;
            id_tail = id_child;
            total_pool = 1;
        }
    }
    free(head);
    if (total_node > 0) {
        head = tmp; //if next == NULL, head = NULL
    }
    start_nextthread(0, 0);
}

//Level 3
void uthread_wait(void *a) {
    head->waiting = 1;
    head->waiting_for = a;
    start_nextthread(0, 1);
}

void uthread_notify(int tid, void *a) {
    struct context_node *tmp = head;
    for (int i = 0; i < total_node; i++) {
        if (tmp->context_numth == tid) {
            if (tmp->waiting > 0) { //tmp->waiting_for may not be registered
                if (tmp->waiting_for == a) {
                    tmp->waiting = 0;
                }
            }
            break;
        }
        tmp = tmp->next;
    }
}

void uthread_notify_all(void *a) {
    struct context_node *tmp = head;
    for (int i = 0; i < total_node; i++) {
        if (tmp->waiting > 0) { //tmp->waiting_for may not be registered
            if (tmp->waiting_for == a) {
                tmp->waiting = 0;
            }
        }
        tmp = tmp->next;
    }
}