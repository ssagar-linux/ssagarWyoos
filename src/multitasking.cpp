
#include <multitasking.h>
using namespace::myos;
using namespace::myos::common;



Task::Task(GlobalDescriptorTable *pGDT, void entrypoint())
{
   pCpustate = (CPUState*)(stack + 4096 - sizeof(CPUState));

   pCpustate -> eax = 0;
   pCpustate -> ebx = 0;
   pCpustate -> ecx = 0;
   pCpustate -> edx = 0;

   pCpustate -> esi = 0;
   pCpustate -> edi = 0;
   pCpustate -> ebp = 0;

   /*
      pCpustate -> gs = 0;
      pCpustate -> fs = 0;
      pCpustate -> es = 0;
      pCpustate -> ds = 0;
    */
   //pCpustate -> error = 0;                       // pushed by ISR routine

   //pCpustate -> esp = ;                          // used for userspace programs
   pCpustate -> eip = (uint32_t)entrypoint;
   pCpustate -> cs = pGDT->CodeSegmentSelector();
   //pCpustate -> ss = ;                           // used in case of security
   pCpustate -> eflags = 0x202;                    /* Bit 1 is always on in flags register & we set bit 9(IF: Interrupt enable Flag)*/
}

Task::~Task()
{
}



TaskManager::TaskManager()
{
   numTasks = 0;
   currentTask = -1;
}

TaskManager::~TaskManager()
{
}

bool TaskManager::AddTask(Task *pTask)
{
   if(numTasks >= 256)
      return false;

   ppTasks[numTasks++] = pTask;
   return true;
}

/* Round Robin scheduling */
CPUState* TaskManager::Schedule(CPUState *pCpustate)
{
   if(numTasks <= 0)
      return pCpustate;

   // add task to list after execution
   if(currentTask >= 0)
      ppTasks[currentTask]->pCpustate = pCpustate;

   if(++currentTask >= numTasks)
      currentTask %= numTasks;
   return ppTasks[currentTask]->pCpustate;
}

