// File:	mypthread.c

// List all group member's name: Jishan Desai, Malav Doshi
// username of iLab:
// iLab Server:

#include "mypthread.h"

// INITAILIZE ALL YOUR VARIABLES HERE
// YOUR CODE HERE
uint count = 1;
int thread_id = 1;
struct Queue *queue;
struct scheduler* sched;
struct sigaction sa;
struct itimerval timer;
int first_sched = 1;
int first_c = 1;
int init = 1;

/*TCB initialization*/
tcb *initialize_tcb(){

 tcb* new_tcb = (tcb*)malloc(sizeof(tcb));
 new_tcb->id = thread_id++;
 new_tcb->status = 0;
 return new_tcb; 
}

/*TIMER FUNCTIONS*/

void Handler(int signum){

printf("From handler\n");


}

void setTimer(){
 memset (&sa, 0, sizeof (sa));
sa.sa_handler = &Handler;
sigaction (SIGPROF, &sa, NULL);
}

void startTimer(){

timer.it_interval.tv_usec = 1; 
timer.it_interval.tv_sec = 0;

timer.it_value.tv_usec = 1;
timer.it_value.tv_sec = 0;

setitimer(ITIMER_PROF, &timer, NULL);
}


/*SCHEDULER FUNCTIONS*/
void createScheduler(){
 
 queue = (struct Queue*)malloc(sizeof(struct Queue));
 queue->num_node = 0;
 sched = (struct scheduler*)malloc(sizeof(struct scheduler));
 sched->queue = queue;
 
}

void runScheduler(){
  struct Node * next_thread = dequeue();
  sched->current = next_thread->thread_block;
  /*Node* next_thread_tcb = NULL;
  struct Node *ptr = sched->queue->head
  while(ptr!=sched->queue->tail){
     if((ptr->tid != sched->current->tid)&&(ptr->status==0)){
         next_thread_tcb = dequeue();
         break;
     }
   ptr=ptr->next;
  }*/
 
getcontext(&(sched->schedcontext));
swapcontext(&(sched->schedcontext), &(next_thread->thread_block->tctx));
if(sched->current!=NULL){
   enqueue(next_thread);

}

}


/*ENQUEUE */

void enqueue(struct Node* node){

node->next = NULL;

  if(node->thread_block->id == 1){
    //createScheduler();
    sched->queue->head = node;
    sched->queue->tail = node;
    sched->queue->num_node++;
    sched->current = node->thread_block;
    return;
  }
  
  sched->queue->tail->next = node;
  sched->queue->tail = node;
  sched->current = node->thread_block;
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
       
       if(init){
       	  setTimer();
       	  createScheduler();
          init--;
       }
       /*Create a tcb for new thread */
       tcb *new_tcb = initialize_tcb();
        
       void *stack=malloc(STACK_SIZE);
       new_tcb->tid = count;
       
       getcontext(&(new_tcb->tctx));
       new_tcb->tctx.uc_link = NULL;
       new_tcb->tctx.uc_stack.ss_sp = stack;
       new_tcb->tctx.uc_stack.ss_size = STACK_SIZE;
       new_tcb->tctx.uc_stack.ss_flags = 0;
       new_tcb->status = 0;
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
           tcb * curr_thread_block = sched->current;
           curr_thread_block->status = 0;
          
	// save context of this thread to its thread control block
            if (getcontext(&(curr_thread_block->tctx))==-1){
               perror("getcontext failed");return 0;
             }
	// switch from thread context to scheduler context
            if(getcontext(&(sched->schedcontext))==-1){
               perror("getcontext failed"); return 0;
   
            }
            sched->schedcontext.uc_link = NULL;
            sched->schedcontext.uc_stack.ss_sp = stack
       sched->schedcontext.uc_stack.ss_size = STACK_SIZE;
       setcontext(&sched->schedcontext);
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


// Feel free to add any other functions you need


//Delete main after testing

// YOUR CODE HERE
