#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semWait(){
  // takes the fd of the semaphore
  int fd = running->syscall_args[0];

  // sem_desc from the fd
  SemDescriptor* sem_desc = SemDescriptorList_byFd(&running->sem_descriptors, fd);
  if(!sem_desc){
    // invalid sem_desc
    running->syscall_retvalue = DSOS_ESEMNOFD;
    printf("Cannot open semaphore with the given fd!\n");
    return;
  }

  // takes the semaphore
  Semaphore* sem = sem_desc->semaphore;
  
  // allocates the pointer to the sem_desc
  SemDescriptorPtr* sem_desc_ptr = sem_desc->ptr;

  // decrease the counter of the semaphore
  sem->count--;

  if (sem->count < 0){
    // removes the descriptor of the process from the desc_list
    List_detach(&sem->descriptors, (ListItem*)sem_desc_ptr);
    // puts this in the waiting list
    List_insert(&sem->waiting_descriptors, sem->waiting_descriptors.last, (ListItem*)sem_desc->ptr);
    // changes the status of the process
    running->status = Waiting;

    // puts the current process in the waiting list
    List_insert(&waiting_list, waiting_list.last, (ListItem*) running);

    // takes the first element of the ready list
    PCB* pcb = (PCB*)List_detach(&ready_list, (ListItem*)ready_list.first);
    // runs the the first element of the ready list
    running = (PCB*)pcb;
  }

  running->syscall_retvalue = 0;

  return;
}
