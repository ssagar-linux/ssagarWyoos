
#include "mouse.h"

	MouseDriver::MouseDriver(InterruptManager * pInterruptManager)
	:InterruptHandler(pInterruptManager, 0x2C),
	dataPort(0x60),
	commandPort(0x64)
	{
		uint16_t* VideoMemory = (uint16_t*) 0xb8000;
		offset  = 0;
		buttons = 0;
		x = 40;
		y = 12;

		VideoMemory[80*y+x] =
			(VideoMemory[80*y+x] & 0x0F00) << 4
			| (VideoMemory[80*y+x] & 0xF000) >> 4
			| (VideoMemory[80*y+x] & 0x00FF);


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

	MouseDriver::~MouseDriver()
	{
	}

	uint32_t MouseDriver::HandleInterrupt(uint32_t esp)
	{
		uint8_t status= commandPort.Read();
		if(!(status & 0x20))
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
				static uint16_t* VideoMemory = (uint16_t*) 0xb8000;
				VideoMemory[80*y+x] =
					(VideoMemory[80*y+x] & 0x0F00) << 4
					| (VideoMemory[80*y+x] & 0xF000) >> 4
					| (VideoMemory[80*y+x] & 0x00FF);


				/* X- & Y- axix cursor movement, ++ hold cursor on boundary
				   when cursor movement goes out of screen */
				x += buffer[1];
				if(x >= 80) x = 79;
				if(x < 0) x = 0;
				y -= buffer[2];
				if(y >= 25) y = 24;
				if(y < 0) y = 0;

				// invert foreground & background color of current position
				// of cursor to visualize cursor movement on screen
				VideoMemory[80*y+x] =
					(VideoMemory[80*y+x] & 0x0F00) << 4
					| (VideoMemory[80*y+x] & 0xF000) >> 4
					| (VideoMemory[80*y+x] & 0x00FF);

/*
				for(uint8_t i = 0; i < 3; i++)
				{
					if((buffer[0] & (0x1<<i)) != buttons & (0x1<<i))
					{
						if(buttons & (0x1<<i))
							handler->OnMouseButtonReleased(i+1);
						else
							handler->OnMouseButtonPressed(i+1);
					}
				}
				buttons = buffer[0]; // update buttons with new state
*/
			}
		}
		return esp;
	}
