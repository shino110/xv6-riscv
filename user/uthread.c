#include "user/uthread.h"
#include "user/user.h"
#include "kernel/defs.h"
#include "kernel/param.h"
#include "kernel/proc.h"
#include "kernel/spinlock.h"
#include "kernel/types.h"
#include "kernel/riscv.h"

//Level 1
int make_uthread(void (*fun)()) {
    struct context parent = myproc().context;
    struct context_node *child = malloc(struct context_node);

    child->cnxt.ra = (uint64)fun;
    child->cnxt.sp = (uint64)(child->stack + STACK_DEPTH);
    child->next = NULL;

    if (total_node > 0) {
        //decide numbering of thread
        if (id_head != NULL) {
            child->context_numth = id_head->num;
            id_head = id_head->next; // if next == NULL, id_head = NULL
        } else {
            child->context_numth = total_node
        }
        total_node += 1;

        //connect in unidirect list
        tail->next = child;
        tail = child;
    } else {
        head = child;
        tail = child;
        child->context_numth = 0;
        total_node = 1;
        id_head = NULL;
    }
}

void start_uthreads() {
    while (head!= NULL) {
        swtch(&parent, &(head->cnxt));
    }
}

void yield() {
    struct context_node *tmp = head;

    //current context goes to last on queue
    head = tmp->next;
    tail->next = tmp;
    tail = tmp;
    swtch(&(tmp->cnxt), &(head->cnxt))
}

int mytid() {
    return head->context_numth;
}

//Level 2
void uthread_exit() {
    struct context_node *tmp = head;
    struct id_pool *id_child = malloc(sizeof(id_pool));

    head = head->next; //if next == NULL, head = NULL
    total_node -= 1;

    if (total_node == 0) {
        // delete all id_pool
        struct id_pool *tmp1 = id_head;
        struct id_pool *tmp2 = id_head->next;
        while (id_tail->num != tmp1->num) {
            free(tmp1);
            tmp1 = tmp2;
            tmp2 = tmp2->next;
        }
        free(tmp1);
        free(id_child);
        id_head = NULL;
        id_tail = NULL;
    } else {
        //put current id in id_pool
        id_child->num = tmp->context_numth;
        id_child->next = NULL;
        if(id_head == NULL) {
            id_head = id_child;
            id_tail = id_child;
        } else {
            id_tail->next = id_child;
            id_tail = id_child;
        }
    }
    free(tmp);
}

//Level 3
void uthread_wait(void *a);
void uthread_notify(int tid, void *a);
void uthread_notify_all(void *a);