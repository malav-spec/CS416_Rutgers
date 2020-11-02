// File:	mypthread_t.h

// List all group member's name: Jishan Desai, Malav Doshi
// username of iLab:
// iLab Server:

#ifndef MYTHREAD_T_H
#define MYTHREAD_T_H

#define _GNU_SOURCE

/* To use Linux pthread Library in Benchmark, you have to comment the USE_MYTHREAD macro */
#define USE_MYTHREAD 1

/* include lib header files that you need here: */
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <ucontext.h>
#include <sys/time.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define STACK_SIZE SIGSTKSZ

typedef uint mypthread_t;



typedef struct threadControlBlock {
	/* add important states in a thread control block */
	// thread Id
	mypthread_t tid;
	int id;
	// thread status
	int status;
	int finished;
	// thread context
	ucontext_t tctx;
	// thread stack
	// thread priority
	// And more ...

	// YOUR CODE HERE
} tcb;

/*Staus codes:
  0 = Ready
  1 = Running
  2 = Wait */


/* mutex struct definition */
typedef struct mypthread_mutex_t {
	/* add something here */

	// YOUR CODE HERE
} mypthread_mutex_t;

/* define your data structures here: */
 struct Node{
 tcb *thread_block;
 struct Node *next;
};

 struct Queue{
  struct Node* head;
  struct Node* tail;
 int num_node;
 
};

struct scheduler{
 struct Queue *queue;
 //ucontext_t curr;
 tcb* current;
};

struct Queue *queue;
struct scheduler* sched;
// Feel free to add your own auxiliary data structures (linked list or queue etc...)

// YOUR CODE HERE


/* Function Declarations: */


/*Initialize a tcb*/
tcb *initialize_tcb();

 

/* create a new thread */
int mypthread_create(mypthread_t * thread, pthread_attr_t * attr, void
    *(*function)(void*), void * arg);

/* give CPU pocession to other user level threads voluntarily */
int mypthread_yield();

/* terminate a thread */
void mypthread_exit(void *value_ptr);

/* wait for thread termination */
int mypthread_join(mypthread_t thread, void **value_ptr);

/* initial the mutex lock */
int mypthread_mutex_init(mypthread_mutex_t *mutex, const pthread_mutexattr_t
    *mutexattr);

/* aquire the mutex lock */
int mypthread_mutex_lock(mypthread_mutex_t *mutex);

/* release the mutex lock */
int mypthread_mutex_unlock(mypthread_mutex_t *mutex);

/* destroy the mutex */
int mypthread_mutex_destroy(mypthread_mutex_t *mutex);

void createScheduler();
void enqueue(struct Node* node);
struct Node* dequeue();
void printQueue();
void *func();
void runScheduler();
void Handler(int signum);
void setTimer();

#ifdef USE_MYTHREAD
#define pthread_t mypthread_t
#define pthread_mutex_t mypthread_mutex_t
#define pthread_create mypthread_create
#define pthread_exit mypthread_exit
#define pthread_join mypthread_join
#define pthread_mutex_init mypthread_mutex_init
#define pthread_mutex_lock mypthread_mutex_lock
#define pthread_mutex_unlock mypthread_mutex_unlock
#define pthread_mutex_destroy mypthread_mutex_destroy
#endif

#endif