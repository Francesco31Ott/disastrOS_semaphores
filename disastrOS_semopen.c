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
    //error 
    return;
  }

  Semaphore* sem = SemaphoreList_byId(&semaphores_list, id);

  if(!sem){
    sem = Semaphore_alloc(id, 1);
    if(!sem){
      //error 
      return;
    }

    List_insert(&semaphores_list, semaphores_list.last, (ListItem*)sem);
  }

  SemDescriptor* sem_desc = SemDescriptor_alloc(running->last_fd, sem, running);

  if (!sem_desc){
		//error    
		return;
  }

  running->last_fd++; 

  SemDescriptorPtr* sem_desc_ptr = SemDescriptorPtr_alloc(sem_desc);

  List_insert(&running->sem_descriptors, running->sem_descriptors.last, (ListItem*) sem_desc);

  sem_desc->ptr = sem_desc_ptr;

  List_insert(&sem->descriptors, sem->descriptors.last, (ListItem*) sem_desc_ptr);

  running->syscall_retvalue = sem_desc->fd;

	return;
}
