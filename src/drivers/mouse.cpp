
#include <drivers/mouse.h>
using namespace::myos::common;
using namespace::myos::drivers;
using namespace::myos::hardwarecommunication;


    MouseEventHandler::MouseEventHandler()
    {
    }

    void MouseEventHandler::OnActivate()
    {
    }

    void MouseEventHandler::OnMouseDown(uint8_t button)
    {
    }

    void MouseEventHandler::OnMouseUp(uint8_t button)
    {
    }

    void MouseEventHandler::OnMouseMove(int x, int y)
    {
    }


	MouseDriver::MouseDriver(InterruptManager * pInterruptManager, MouseEventHandler *pMouseEventHandler)
	:InterruptHandler(pInterruptManager, 0x2C),
	dataPort(0x60),
	commandPort(0x64)
	{
        this->pMouseEventHandler = pMouseEventHandler;
	}

	MouseDriver::~MouseDriver()
	{
	}

	void MouseDriver::Activate()
	{
		offset  = 0;
		buttons = 0;

        if(pMouseEventHandler != 0)
            pMouseEventHandler->OnActivate();

		commandPort.Write(0xA8);	//activate interrupts i.e. enable 2nd ps/2 port
		commandPort.Write(0x20);	// get current state
		/* Mouse interrupt enable(i.e. send IRQ12 when output buffer is full)*/
		uint8_t status = dataPort.Read() | 2;
		commandPort.Write(0x60);	// set state
		dataPort.Write(status);

		commandPort.Write(0xD4);	// write next byte to 2nd ps/2 port input buffer
		dataPort.Write(0xF4); 		// enable data reporting
		dataPort.Read();
	}

	uint32_t MouseDriver::HandleInterrupt(uint32_t esp)
	{
		uint8_t status= commandPort.Read();
		if((!(status & 0x20)) ||
            (pMouseEventHandler == 0))
			// checking whether we have data to process for mouse or not
			// i.e. output buffer of ps/2 mouse full
            return esp;

		buffer[offset] = dataPort.Read();
		offset = (offset + 1) % 3;

		if(offset == 0)
		{
			/* Every mouse movement correspond to 3 byte data buffer
			 * so we will fetch them first before making a change in cursor position
			 * In our case 3 byte read completion happend when (offset + 1)%3 == 2
			 */
			if(buffer[1] != 0 || buffer[2] != 0)
			{
                pMouseEventHandler->OnMouseMove(buffer[1], -buffer[2]);

/*
				for(uint8_t i = 0; i < 3; i++)
				{
					if((buffer[0] & (0x1<<i)) != buttons & (0x1<<i))
					{
						if(buttons & (0x1<<i))
							pMouseEventHandler->OnMouseUp(i+1);
						else
							pMouseEventHandler->OnMouseDown(i+1);
					}
				}
				buttons = buffer[0]; // update buttons with new state
*/
			}
		}
		return esp;
	}
