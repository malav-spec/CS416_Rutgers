// File:	mypthread.c

// List all group member's name:
// username of iLab:
// iLab Server:

#include "mypthread.h"

// INITAILIZE ALL YOUR VARIABLES HERE
// YOUR CODE HERE

uint count = 1;
struct Queue *queue;
struct scheduler* sched;

/*ENQUEUE */
void createScheduler(){

 
 queue = (struct Queue*)malloc(sizeof(struct Queue));
 queue->num_node = 0;
 sched = (struct scheduler*)malloc(sizeof(struct scheduler));
 sched->queue = queue;
 
}

void enqueue(struct Node* node){

node->next = NULL;

  if(node->thread_block->id == 1){
    createScheduler();
    sched->queue->head = node;
    sched->queue->tail = node;
    sched->queue->num_node++;
    return;
  }
  
  sched->queue->tail->next = node;
  sched->queue->tail = node;
  sched->queue->num_node++;
}

struct Node* dequeue(){

 struct Node* ptr = sched->queue->head;
 
 if(sched->queue->head->next == NULL){
   return sched->queue->head;
 }
 sched->queue->head = sched->queue->head->next;
 return ptr;
}

void printQueue(){
struct Node *ptr = sched->queue->head;
int i = 1;
 while(i <= sched->queue->num_node){
  printf("i: %d id: %d\n",i,ptr->thread_block->id);
  ptr = ptr->next;
 i++;
 }

}

/* create a new thread */
int mypthread_create(mypthread_t * thread, pthread_attr_t * attr,
                      void *(*function)(void*), void * arg) {
       // create Thread Control Block
       // create and initialize the context of this thread
       // allocate space of stack for this thread to run
       // after everything is all set, push this thread int
       // YOUR CODE HERE
       
       /*Create a tcb for new thread */
       tcb *new_tcb = initialize_tcb();
        
       void *stack=malloc(STACK_SIZE);
       new_tcb->tid = count;
       
       getcontext(&(new_tcb->tctx));
       new_tcb->tctx.uc_link = NULL;
       new_tcb->tctx.uc_stack.ss_sp = stack;
       new_tcb->tctx.uc_stack.ss_size = STACK_SIZE;
       new_tcb->tctx.uc_stack.ss_flags = 0;
       
       if(arg != NULL){
       makecontext(&(new_tcb->tctx), (void *)&function, 1, arg);
       }
       else{
       makecontext(&(new_tcb->tctx), (void *)&function, 0);
       }
       
       thread = &count;
       count++;
       
       struct Node *new_node = (struct Node*)malloc(sizeof(struct Node));
       new_node->thread_block = new_tcb;
       
       enqueue(new_node);
       

    return 0;
};

/* give CPU possession to other user-level threads voluntarily */
int mypthread_yield() {

	// change thread state from Running to Ready
	// save context of this thread to its thread control block
	// wwitch from thread context to scheduler context
        
        //basically set a timer prematurely to stop executing the current thread
        
	// YOUR CODE HERE
	return 0;
};

/* terminate a thread */
void mypthread_exit(void *value_ptr) {
	// Deallocated any dynamic memory created when starting this thread

	// YOUR CODE HERE
};


/* Wait for thread termination */
int mypthread_join(mypthread_t thread, void **value_ptr) {

	// wait for a specific thread to terminate
	// de-allocate any dynamic memory created by the joining thread
	

	// YOUR CODE HERE
	return 0;
};

/* initialize the mutex lock */
int mypthread_mutex_init(mypthread_mutex_t *mutex,
                          const pthread_mutexattr_t *mutexattr) {
	//initialize data structures for this mutex

	// YOUR CODE HERE
	return 0;
};

/* aquire the mutex lock */
int mypthread_mutex_lock(mypthread_mutex_t *mutex) {
        // use the built-in test-and-set atomic function to test the mutex
        // if the mutex is acquired successfully, enter the critical section
        // if acquiring mutex fails, push current thread into block list and //
        // context switch to the scheduler thread

        // YOUR CODE HERE
        return 0;
};

/* release the mutex lock */
int mypthread_mutex_unlock(mypthread_mutex_t *mutex) {
	// Release mutex and make it available again.
	// Put threads in block list to run queue
	// so that they could compete for mutex later.

	// YOUR CODE HERE
	return 0;
};


/* destroy the mutex */
int mypthread_mutex_destroy(mypthread_mutex_t *mutex) {
	// Deallocate dynamic memory created in mypthread_mutex_init

	return 0;
};

/* scheduler */
static void schedule() {
	// Every time when timer interrup happens, your thread library
	// should be contexted switched from thread context to this
	// schedule function

	// Invoke different actual scheduling algorithms
	// according to policy (STCF or MLFQ)

	// if (sched == STCF)
	//		sched_stcf();
	// else if (sched == MLFQ)
	// 		sched_mlfq();

	// YOUR CODE HERE

// schedule policy
#ifndef MLFQ
	// Choose STCF
#else
	// Choose MLFQ
#endif

}

/* Preemptive SJF (STCF) scheduling algorithm */
static void sched_stcf() {
	// Your own implementation of STCF
	// (feel free to modify arguments and return types)

	// YOUR CODE HERE
}

/* Preemptive MLFQ scheduling algorithm */
static void sched_mlfq() {
	// Your own implementation of MLFQ
	// (feel free to modify arguments and return types)

	// YOUR CODE HERE
}

void runScheduler(){

getcontext(&sched->curr);
struct Node* next_thread = dequeue();
swapcontext((&sched->curr), &(next_thread->thread_block->tctx));
printf("In scheduler\n");
}

// Feel free to add any other functions you need
void *func(){

printf("This is from thread\n");
setcontext(&(sched->curr));
}

//Delete main after testing
int main(int argc, char** argv){
mypthread_t * t;
mypthread_t * t1;
mypthread_t * t2;
int a = 10;
int x = mypthread_create(t, NULL,&func,NULL);
int y = mypthread_create(t1, NULL, &func, NULL);
int z = mypthread_create(t2, NULL, &func, NULL);
printQueue();
runScheduler();
return 0;
}
// YOUR CODE HERE
