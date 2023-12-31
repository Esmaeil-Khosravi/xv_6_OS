#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"


uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int
sys_history(int id, char *buf, int bufsize)
{
    int n;
    if (argint(0, &n) < 0)
        return -1;

    int index = n;
    if (index < 0 || index >= historyBufferArray.numOfCommandsInMem) {

        // Invalid index. Return an error code.
        return -1;

    //int i = historyBufferArray.currentHistory;
    int m = historyBufferArray.lastCommandIndex;
    int x = historyBufferArray.numOfCommandsInMem;
    int i;
    while(x> 0){
         // this is for showing command histories.
        i = (m - 1 + MAX_HISTORY)% MAX_HISTORY;
        consputc('\n');
        consolewrite(1, (uint64_t)historyBufferArray.bufferArr[i], historyBufferArray.lengthsArr[i]);
        consputc('\n');
        x --;
        m = i;
    }

    if (index >= 0 && index < historyBufferArray.numOfCommandsInMem){ // this is for showing the specific command that used has requested.
        consolewrite(1, (uint64_t)historyBufferArray.bufferArr[index], historyBufferArray.lengthsArr[index]);
    }
    return 0;
}
