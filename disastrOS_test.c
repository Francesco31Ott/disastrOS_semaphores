#include <stdio.h>
#include <unistd.h>
#include <poll.h>

#include "disastrOS.h"

#define BUFFER_LENGTH 50
#define ITERATIONS 10

int filled_sem, empty_sem, read_sem, write_sem;
int buf[BUFFER_LENGTH];
int write_index = 0;
int read_index = 0;
int shared_variable;

int buffer_length = BUFFER_LENGTH;

/*   PRODUCER-CONSUMER PARADIGM   */

void producer(){
  printf("[PROCESS %d] I am the PRODUCER\n\n", disastrOS_getpid());

  //printf("[PROCESS %d] Started semWait\n", disastrOS_getpid());    
  disastrOS_semWait(empty_sem);
  disastrOS_semWait(write_sem);
  
  buf[write_index] = shared_variable;
  printf("[PROCESS %d] Write %d in buf[%d]\n", disastrOS_getpid(), shared_variable, write_index);
  write_index = (write_index+1) % buffer_length;
  shared_variable++;
    
  //printf("[PROCESS %d] Started semPost\n\n", disastrOS_getpid());    
  disastrOS_semPost(write_sem);
  disastrOS_semPost(filled_sem);
}

void consumer(){
  printf("[PROCESS %d] I am the CONSUMER\n\n", disastrOS_getpid());
    
  //printf("[PROCESS %d] Started semWait\n", disastrOS_getpid());    
  disastrOS_semWait(filled_sem);
  disastrOS_semWait(read_sem);
  
  printf("[PROCESS %d] Read %d from buf[%d]\n", disastrOS_getpid(), buf[read_index], read_index);
  read_index = (read_index+1) % buffer_length;
    
  //printf("[PROCESS %d] Started semPost\n\n", disastrOS_getpid());    
  disastrOS_semPost(read_sem);
  disastrOS_semPost(empty_sem);
}

/* ------------------------------ */

// we need this to handle the sleep state
void sleeperFunction(void* args){
  printf("Hello, I am the sleeper, and I sleep %d\n",disastrOS_getpid());
  while(1) {
    getc(stdin);
    disastrOS_printStatus();
  }
}

void childFunction(void* args){
  
  printf("Hello, I am the child with PID %d\n", disastrOS_getpid());

  printf("[PROCESS %d] Opening semaphores\n\n", disastrOS_getpid());
  filled_sem = disastrOS_semOpen(1, 0);
  empty_sem = disastrOS_semOpen(2, buffer_length);
  read_sem = disastrOS_semOpen(3, 1);
  write_sem = disastrOS_semOpen(4,1);

  disastrOS_printStatus();

  for(int i=0; i < ITERATIONS; i++){
    if (disastrOS_getpid() % 2 == 0)
      producer();
    else
      consumer();
  }

  disastrOS_printStatus();


  printf("[PROCESSO %d] Ending...\n", disastrOS_getpid());
  
  
  printf("[PROCESSO %d] Closing semaphores\n", disastrOS_getpid());
  disastrOS_semClose(filled_sem);
  disastrOS_semClose(empty_sem);
  disastrOS_semClose(read_sem);
  disastrOS_semClose(write_sem);

  disastrOS_exit(disastrOS_getpid());
}


void initFunction(void* args) {
  disastrOS_printStatus();
  printf("[INIT] Hello, I am init and I just started\n");
  //disastrOS_spawn(sleeperFunction, 0);
  
  printf("\n*** BUFFER AT THE BEGINNING ***\n[ ");
  for(int i=0; i<buffer_length; i++){
    printf("%d ", buf[i]);
  }
  printf("]\n\n");

  shared_variable=1;

  printf("I feel like to spawn 10 nice threads\n");
  int alive_children=0;
  
  for (int i=0; i<10; ++i) { 
    disastrOS_spawn(childFunction, 0);
    printf("[INFO] Thread %d created\n", i+1);
    alive_children++;
  }

  disastrOS_printStatus();
  int retval;
  int pid;
  while(alive_children>0 && (pid=disastrOS_wait(0, &retval))>=0){
    printf("initFunction, child: %d terminated, retval:%d, alive: %d \n", pid, retval, alive_children);
    alive_children--;
  }
  
  printf("\n*** BUFFER AT THE END ***\n[ ");
  for(int i=0; i<buffer_length; i++){
    printf("%d ", buf[i]);
  }
  printf("]\n\n");
  
  printf("shutdown!\n");
  disastrOS_shutdown();
}

int main(int argc, char** argv){
  char* logfilename=0;
  if (argc>1) {
    logfilename=argv[1];
  }
  // we create the init process processes
  // the first is in the running variable
  // the others are in the ready queue
  printf("the function pointer is: %p", childFunction);
  // spawn an init process
  printf("start\n");
  disastrOS_start(initFunction, 0, logfilename);
  return 0;
}
