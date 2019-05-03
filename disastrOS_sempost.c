#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semPost(){
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
  
  // increase the counter of the semaphore
  sem->count++;
  
  if(sem->count <= 0){
    // puts the process in the ready list
    List_insert(&ready_list, ready_list.last, (ListItem*)running);

    // removes the first sem_desc from the list of waiting_descriptors
    SemDescriptorPtr* sem_desc_ptr = (SemDescriptorPtr*)List_detach(&sem->waiting_descriptors, (ListItem*)sem->waiting_descriptors.first);
    List_insert(&sem->descriptors, sem->descriptors.last, (ListItem*)sem_desc_ptr);
    // removes the process from the waiting_list with its sem_desc_ptr
    List_detach(&waiting_list, (ListItem*)sem_desc_ptr->descriptor->pcb);

    // changes the status of the process
    running->status = Ready;
    // runs the process removed from the waiting_list
    running = sem_desc_ptr->descriptor->pcb;
  }
  
  running->syscall_retvalue = 0;
  
  return;
}
