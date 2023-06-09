#ifndef __MYOS__MULTITASKING_H
#define __MYOS__MULTITASKING_H

#include <common/types.h>
#include <gdt.h>

namespace myos
{
   struct CPUState
   {
      common::uint32_t eax;
      common::uint32_t ebx;
      common::uint32_t ecx;
      common::uint32_t edx;

      common::uint32_t esi;
      common::uint32_t edi;
      common::uint32_t ebp;

      /*
         common::uint32_t gs;
         common::uint32_t fs;
         common::uint32_t es;
         common::uint32_t ds;
       */
      common::uint32_t error;

      /* Below registers are pushed automatically by the CPU when an interrupt occurs */
      common::uint32_t eip;
      common::uint32_t cs;
      common::uint32_t eflags;
      common::uint32_t esp;
      common::uint32_t ss;
   }__attribute__((packed));



   class Task
   {
      friend class TaskManager;
      private:
         common::uint8_t stack[4096]; // 4KiB
         CPUState *pCpustate;

      public:
      Task(GlobalDescriptorTable *pGDT, void entrypoint());
      ~Task();
   };

   class TaskManager
   {
      private:
         Task *ppTasks[256];
         int numTasks;
         int currentTask;

      public:
         TaskManager();
         ~TaskManager();
         bool AddTask(Task *pTask);
         CPUState* Schedule(CPUState *pCpustate);
   };
}
#endif
