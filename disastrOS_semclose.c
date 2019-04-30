#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semClose(){
  // takes the fd of the semaphore
  int fd = running->syscall_args[0];
  
  // sem_desc from the fd
  SemDescriptor* sem_desc = SemDescriptorList_byFd(&running->sem_descriptors, fd);

  if(!sem_desc){
    // invalid sem_desc
    running->syscall_retvalue = DSOS_ESEMNOFD;
    printf("Cannot close semaphore with fd: %d\n", fd);
    return;
  }

  // removes the sem_desc from the process list
  sem_desc = (SemDescriptor*)List_detach(&running->sem_descriptors, (ListItem*)sem_desc);
  assert(sem_desc);

  // takes the semaphore
  Semaphore* sem = sem_desc->semaphore;

  // removes the descriptor pointer from the semaphore list
  SemDescriptorPtr* sem_desc_ptr = (SemDescriptorPtr*)List_detach(&sem->descriptors, (ListItem*)(sem_desc->ptr));
  assert(sem_desc_ptr);

  // freeing allocated memory
  SemDescriptor_free(sem_desc);
  SemDescriptorPtr_free(sem_desc_ptr);
  
  // checks if i have no descriptors 
  if(sem->descriptors.size == 0){
    // removes the semaphore from the list
    List_detach(&semaphores_list, (ListItem*)sem);
    Semaphore_free(sem);
  }

  running->syscall_retvalue = 0;
  
  printf("Semaphore with fd: %d has been deallocated!\n", fd);
  
  return;
}
