#include "kernel/types.h"
#include "user/user.h"
#include <stddef.h>
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
    struct context *cnxt;
    struct context_node *next;
};

struct id_pool {
    int num;
    struct id_pool *next;
};

struct context *parent;
struct context_node *head;
struct context_node *tail;
struct id_pool *id_head;
struct id_pool *id_tail;
int total_node;

//Level 1
int make_uthread(void (*fun)()) {
    struct context *child_cnxt = malloc(sizeof(struct context));
    struct context_node *child = malloc(sizeof(struct context_node));
    if (child_cnxt == NULL || child == NULL) {
        return -1;
    }

    child->cnxt = child_cnxt;
    child->cnxt->ra = (uint64)(fun);
    child->cnxt->sp = (uint64)(child->stack + STACK_DEPTH);
    child->next = NULL;

    if (total_node > 0) {
        //decide numbering of thread
        if (id_head != NULL) {
            struct id_pool *tmp;
            
            child->context_numth = id_head->num;
            tmp = id_head;
            id_head = id_head->next; // if next == NULL, id_head = NULL
            free(tmp);
        } else {
            child->context_numth = total_node;
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
    return child->context_numth;
}

void start_uthreads() {
    while (head!= NULL) {
        swtch(parent, head->cnxt);
    }
}

void yield() {
    struct context_node *tmp = head;

    //current context goes to last on queue
    head = tmp->next;
    tail->next = tmp;
    tail = tmp;
    swtch(tmp->cnxt, head->cnxt);
}

int mytid() {
    return head->context_numth;
}

//Level 2
void uthread_exit() {
    struct context_node *tmp = head;
    struct id_pool *id_child = malloc(sizeof(struct id_pool));
    if (id_child == NULL) {
        return;
    }

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
    free(tmp->cnxt);
    free(tmp);
}

//Level 3
void uthread_wait(void *a);
void uthread_notify(int tid, void *a);
void uthread_notify_all(void *a);