
#ifndef __MOUSE_H
#define __MOUSE_H

	#include "types.h"
	#include "interrupts.h"
	#include "port.h"

	class MouseDriver : public InterruptHandler
	{
		Port8Bit dataPort;
		Port8Bit commandPort;

		uint8_t buffer[3];
		uint8_t offset;
		uint8_t buttons;

		int8_t x,y;
	public:
		MouseDriver(InterruptManager * pInterruptManager);
		~MouseDriver();
		virtual uint32_t HandleInterrupt(uint32_t esp);
	};

#endif
