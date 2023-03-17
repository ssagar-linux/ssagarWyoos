
#ifndef __KEYBOARD_H
#define __KEYBOARD_H

	#include "types.h"
	#include "interrupts.h"
	#include "driver.h"
	#include "port.h"

	class KeyboardEventHandler
	{
		public:
			KeyboardEventHandler();

			virtual void OnKeyDown(char);
			virtual void OnKeyUp(char);
	};

	class KeyboardDriver : public InterruptHandler, public Driver
	{
		Port8Bit dataPort;
		Port8Bit commandPort;

		KeyboardEventHandler *pKbEventHandler;
	public:
		KeyboardDriver(InterruptManager *pInterruptManager, KeyboardEventHandler *pKbEventHandler);
		~KeyboardDriver();
		virtual uint32_t HandleInterrupt(uint32_t esp);
		virtual void Activate();
	};

#endif
