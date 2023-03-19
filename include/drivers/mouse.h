
#ifndef __MYOS__DRIVERS__MOUSE_H
#define __MYOS__DRIVERS__MOUSE_H

#include <common/types.h>
#include <hardwarecommunication/interrupts.h>
#include <drivers/driver.h>
#include <hardwarecommunication/port.h>

namespace myos
{
    namespace drivers
    {
        class MouseEventHandler
        {
            public:
                MouseEventHandler();

                virtual void OnActivate();
                virtual void OnMouseDown(myos::common::uint8_t button);
                virtual void OnMouseUp(myos::common::uint8_t button);
                virtual void OnMouseMove(int x, int y);
        };

        class MouseDriver : public myos::hardwarecommunication::InterruptHandler, public Driver
        {
            myos::hardwarecommunication::Port8Bit dataPort;
            myos::hardwarecommunication::Port8Bit commandPort;

            myos::common::uint8_t buffer[3];
            myos::common::uint8_t offset;
            myos::common::uint8_t buttons;

            MouseEventHandler *pMouseEventHandler;
            public:
            MouseDriver(myos::hardwarecommunication::InterruptManager * pInterruptManager, MouseEventHandler *pMouseEventHandler);
            ~MouseDriver();
            virtual myos::common::uint32_t HandleInterrupt(myos::common::uint32_t esp);
            virtual void Activate();
        };
    }
}
#endif
