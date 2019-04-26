#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semOpen(){
  
  int id = running->syscall_args[0];

  if(id < 0){
    // invalid id
    running->syscall_retvalue = DSOS_ESEMNEG; 
    return;
  }

  Semaphore* sem = SemaphoreList_byId(&semaphores_list, id);

  if(!sem){ // checks if the semaphore exists
    // the semaphore must be allocated
    sem = Semaphore_alloc(id, 1);
    
    if(!sem){ // checks if the semaphore is allocated
      running->syscall_retvalue = DSOS_ESEMALLOC;
      return;
    }

    List_insert(&semaphores_list, semaphores_list.last, (ListItem*)sem);
  }
  // allocates the sem_desc for the current process
  SemDescriptor* sem_desc = SemDescriptor_alloc(running->last_sem_fd, sem, running);

  if (!sem_desc){
    // invalid sem_desc
    running->syscall_retvalue = DSOS_ESEMNOFD;   
    return;
  }

  // increase the counter of the last sem_desc
  running->last_sem_fd++; 

  // allocates the pointer to the sem_desc
  SemDescriptorPtr* sem_desc_ptr = SemDescriptorPtr_alloc(sem_desc);

  List_insert(&running->sem_descriptors, running->sem_descriptors.last, (ListItem*) sem_desc);

  // adds the descriptor to the semaphore struct
  sem_desc->ptr = sem_desc_ptr;

  List_insert(&sem->descriptors, sem->descriptors.last, (ListItem*) sem_desc_ptr);

  running->syscall_retvalue = sem_desc->fd;

  return;
}
