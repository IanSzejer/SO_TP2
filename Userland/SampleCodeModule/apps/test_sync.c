#include <stdio.h>
#include "test_util.h"
#include "stdinout.h"

#define SEM_ID "sem"
#define TOTAL_PAIR_PROCESSES 2

int64_t global;  //shared memory

void slowInc(int64_t *p, int64_t inc){
  uint64_t aux = *p;
  my_yield(); //This makes the race condition highly probable
  aux += inc;
  *p = aux;
}

uint64_t my_process_inc(uint64_t argc, char *argv[]){
  uint64_t n;
  uint8_t inc;
  int8_t use_sem;

  if (argc != 3) return -1;

  if ((n = strToInt(argv[0])) <= 0) return -1;
  if ((inc = strToInt(argv[1])) == 0) return -1;
  if ((use_sem = strToInt(argv[2])) < 0) return -1;

  if (use_sem)
    if (!openSemaphore(SEM_ID, 1)){
      print("test_sync: ERROR opening semaphore\n");
      return -1;
    }

  uint64_t i;
  for (i = 0; i < n; i++){
    if (use_sem) waitSem(SEM_ID);
    slowInc(&global, inc);
    if (use_sem) postSem(SEM_ID);
  }

  if (use_sem) closeSemaphore(SEM_ID);
  
  return 0;
}

uint64_t test_sync(uint64_t argc, char *argv[]){ //{n, use_sem, 0}
  uint64_t pids[2 * TOTAL_PAIR_PROCESSES];

  if (argc != 2) return -1;

  char * argvDec[] = {argv[0], "-1", argv[1], NULL};
  char * argvInc[] = {argv[0], "1", argv[1], NULL};

  global = 0;

  uint64_t i;
  for(i = 0; i < TOTAL_PAIR_PROCESSES; i++){
    pids[i] = new_process("my_process_inc", 3, argvDec, SEM_ID);
    pids[i + TOTAL_PAIR_PROCESSES] = new_process("my_process_inc", 3, argvInc, SEM_ID);
  }

//los wait q se usan aca no se q onda
  for(i = 0; i < TOTAL_PAIR_PROCESSES; i++){
    my_wait(pids[i]);
    my_wait(pids[i + TOTAL_PAIR_PROCESSES]);
  }

  print("Final value: %d\n");

  return 0;
}
