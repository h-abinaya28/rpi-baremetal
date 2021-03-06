#include "rpi.h"
#include <stdio.h>
#include <stdlib.h>

THREADCTL threadctl;

void InitThread() {
  threadctl.length = 1;
  threadctl.thread[0].state = THREAD_RUNNING;
  threadctl.currentId = 0;

  threadctl.thread[0].stack = NULL;
  /* threadctl.thrad[0].stackSize = TBD; */
}

void CreateThread(void *thread_entry) {
  unsigned int id = threadctl.length;
  const unsigned int stackSize = 4096;  // 4096 * sizeof(int) allocated
  int *stackBase;

  threadctl.length++;
  threadctl.thread[id].state = THREAD_CREATED;
  stackBase = (int *)malloc(stackSize * sizeof(int));
  stackBase += stackSize - 1;
  threadctl.thread[id].stack = stackBase - 13;
  threadctl.thread[id].stackSize = stackSize;

  // push default registers into the stack which will be poped in
  // _context_switch
  *threadctl.thread[id].stack =
      (int)thread_entry;  // r14: lr (to be stored in pc)
  threadctl.thread[id].stack--;
  *threadctl.thread[id].stack = 0;  // r12
  threadctl.thread[id].stack--;
  *threadctl.thread[id].stack = 0;  // r11
  threadctl.thread[id].stack--;
  *threadctl.thread[id].stack = 0;  // r10
  threadctl.thread[id].stack--;
  *threadctl.thread[id].stack = 0;  // r9
  threadctl.thread[id].stack--;
  *threadctl.thread[id].stack = 0;  // r8
  threadctl.thread[id].stack--;
  *threadctl.thread[id].stack = 0;  // r7
  threadctl.thread[id].stack--;
  *threadctl.thread[id].stack = 0;  // r6
  threadctl.thread[id].stack--;
  *threadctl.thread[id].stack = 0;  // r5
  threadctl.thread[id].stack--;
  *threadctl.thread[id].stack = 0;  // r4
  threadctl.thread[id].stack--;
  *threadctl.thread[id].stack = 0;  // r3
  threadctl.thread[id].stack--;
  *threadctl.thread[id].stack = 0;  // r2
  threadctl.thread[id].stack--;
  *threadctl.thread[id].stack = 0;  // r1
  threadctl.thread[id].stack--;
  *threadctl.thread[id].stack = 0;  // r0
  // don't do stack--!
}

void ContextSwitch() {
  if (threadctl.length <= 1) {
    return;
  }

  unsigned int currentId = threadctl.currentId;
  unsigned int nextId = currentId + 1;

  if (nextId >= threadctl.length) {
    nextId = 0;
  }

  char message[512];
  if (NULL != threadctl.thread[currentId].stack &&
      NULL != threadctl.thread[nextId].stack) {
    sprintf(message, "%d jumping from th:%d@%p to th:%d@%p", timerctl.counter,
            currentId, *threadctl.thread[currentId].stack, nextId,
            *threadctl.thread[nextId].stack);
  } else {
    sprintf(message, "%d jumping from th:%d to th:%d", timerctl.counter,
            currentId, nextId);
  }
  FillRect(0, 16, kWidth, 16, 0);
  PrintStr(0, 16, message, 7);

  // get current sp
  threadctl.thread[currentId].stack = (int *)_get_stack_pointer();

  // do context switch
  threadctl.thread[nextId].state = THREAD_RUNNING;
  threadctl.thread[nextId].state = THREAD_WAITING;
  threadctl.currentId = nextId;
  _context_switch(&threadctl.thread[currentId].stack,
                  &threadctl.thread[nextId].stack);
}
