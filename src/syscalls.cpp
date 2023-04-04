
#include <syscalls.h>
using namespace myos;
using namespace myos::common;
using namespace myos::hardwarecommunication;

void printf(char*);

SyscallHandler::SyscallHandler(InterruptManager* pInterruptManager, uint8_t InterruptNumber)
:  InterruptHandler(pInterruptManager, InterruptNumber + pInterruptManager->HardwareInterruptOffset())
{
}

SyscallHandler::~SyscallHandler()
{
}


uint32_t SyscallHandler::HandleInterrupt(uint32_t esp)
{
   CPUState* pCpu = (CPUState*)esp;

   switch(pCpu->eax)
   {
      case 4:  // Generally for Write()
            printf((char*)pCpu->ebx);
            break;

      default:
            break;
   }

   return esp;
}

