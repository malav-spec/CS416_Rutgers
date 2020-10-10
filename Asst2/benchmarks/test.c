#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "../mypthread.h"
/* A scratch program template on which to call and
 * test mypthread library functions as you implement
 * them.
 *
 * You can modify and use this program as much as possible.
 * This will not be graded.
 */
void* func(){
printf("From thread\n");
} 

int main(int argc, char** argv){
pthread_t * t;
pthread_t * t1;
pthread_t * t2;
int a = 10;
int x = pthread_create(t, NULL,&func,&a);
int y = pthread_create(t1, NULL, &func, &a);
int z = pthread_create(t2, NULL, &func, &a);
printQueue();
runScheduler();
//printf("%d\n",sched->queue->head->thread_block->tid);
return 0;
}

