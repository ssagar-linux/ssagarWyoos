
#ifndef __MYOS_DRIVERS__KEYBOARD_H
#define __MYOS_DRIVERS__KEYBOARD_H

#include <common/types.h>
#include <hardwarecommunication/interrupts.h>
#include <drivers/driver.h>
#include <hardwarecommunication/port.h>

namespace myos
{
    namespace drivers
    {
        class KeyboardEventHandler
        {
            public:
                KeyboardEventHandler();

                virtual void OnKeyDown(char);
                virtual void OnKeyUp(char);
        };

        class KeyboardDriver : public myos::hardwarecommunication::InterruptHandler, public Driver
        {
            myos::hardwarecommunication::Port8Bit dataPort;
            myos::hardwarecommunication::Port8Bit commandPort;

            KeyboardEventHandler *pKbEventHandler;
            public:
                KeyboardDriver(myos::hardwarecommunication::InterruptManager *pInterruptManager, KeyboardEventHandler *pKbEventHandler);
                ~KeyboardDriver();
                virtual myos::common::uint32_t HandleInterrupt(myos::common::uint32_t esp);
                virtual void Activate();
        };
    }
}
#endif
