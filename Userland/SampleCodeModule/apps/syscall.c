#include <stdint.h>
#include "stdinout.h"

void loop2(){
  while(1){

  }
}

int64_t my_getpid(){
  return get_pid_sys();
}

int64_t my_create_process(char * name, uint64_t argc, char *argv[]){
  return new_process((void *(*)(void*))&loop2,argv,argc,name);
}

int64_t my_nice(uint64_t pid, uint64_t newPrio){
  return nice(pid,newPrio);
}

int64_t my_kill(uint64_t pid){
   kill(pid);
   return(1);
}

int64_t my_block(uint64_t pid){
 change_state(pid,1);
 return 1;
}

int64_t my_unblock(uint64_t pid){
  change_state(pid,0);
 return 1;
}

int64_t my_sem_open(char *sem_id, uint64_t initialValue){
  return openSemaphore(sem_id,initialValue);
}

int64_t my_sem_wait(char *sem_id){
  return waitSem(sem_id);
}

int64_t my_sem_post(char *sem_id){
  return postSem(sem_id);
}

int64_t my_sem_close(char *sem_id){
  return closeSemaphore(sem_id);
}

int64_t my_yield(){
  
  return 0;
}

int64_t my_wait(int64_t pid){

  return 0;
}


