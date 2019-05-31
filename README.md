# disastrOS_semaphores

### IMPLEMENTED FUNCTIONS: ###

- disastrOS_semOpen
- disastrOS_semClose
- disastrOS_semWait
- disastrOS_semPost

### DISASTROS_SEMOPEN ###  

int disastrOS_semOpen(int id, int count)

##### DESCRIPTION #####

This function creates a new semaphore or opens an existing semaphore. The semaphore is identified by id (taken by the first parameter of syscall_args) and count.

##### PARAMETERS #####
- `int id` : identifies the semaphore.
- `int count` : represents the number of processes that can enter in critical section.
##### RETURN VALUE #####
On success, `disastrOS_semOpen()` returns a non-negative integer. On error, `disastrOS_semOpen()` returns an error code (negative value). 
##### ERRORS ##### 
- DSOS_ESEMNEG : the id of the semaphore is negative.
- DSOS_ESEMALLOC : the semaphore was not allocated.
- DSOS_ESEMNOFD : invalid semDescriptor.

### DISASTROS_SEMCLOSE ###  

int disastrOS_semClose(int fd)

##### DESCRIPTION #####

This function closes the semaphore referred to by fd.

##### PARAMETERS #####
- `int fd` : the file descriptor of the semaphore.
##### RETURN VALUE #####
On success, `disastrOS_semClose()` returns 0. On error, `disastrOS_semClose()` returns an error code (negative value). 
##### ERRORS ##### 
- DSOS_ESEMNOFD : invalid semDescriptor.

### DISASTROS_SEMWAIT ###  

int disastrOS_semWait(int fd)

##### DESCRIPTION #####

This function decrements the semaphore pointed to by fd.

##### PARAMETERS #####
- `int fd` : the file descriptor of the semaphore.
##### RETURN VALUE #####
On success, `disastrOS_semWait()` returns 0. On error, `disastrOS_semWait()` returns an error code (negative value). 
##### ERRORS #####
- DSOS_ESEMNOFD : invalid semDescriptor.

### DISASTROS_SEMPOST ###  

int disastrOS_semPost(int fd)

##### DESCRIPTION #####

This function increments the semaphore pointed to by fd.

##### PARAMETERS #####
- `int fd` : the file descriptor of the semaphore.
##### RETURN VALUE #####
On success, `disastrOS_semPost()` returns 0. On error, `disastrOS_semPost()` returns an error code (negative value). 
##### ERRORS #####
- DSOS_ESEMNOFD : invalid semDescriptor.
