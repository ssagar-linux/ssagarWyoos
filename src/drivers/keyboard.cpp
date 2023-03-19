
#include <drivers/keyboard.h>
using namespace::myos::common;
using namespace::myos::drivers;
using namespace::myos::hardwarecommunication;

KeyboardEventHandler::KeyboardEventHandler()
{
}

void KeyboardEventHandler::OnKeyDown(char)
{
}

void KeyboardEventHandler::OnKeyUp(char)
{
}

KeyboardDriver::KeyboardDriver(InterruptManager *pInterruptManager, KeyboardEventHandler *pKbEventHandler)
	:InterruptHandler(pInterruptManager, 0x21),
	dataPort(0x60),
	commandPort(0x64)
{
	this->pKbEventHandler = pKbEventHandler;
}

KeyboardDriver::~KeyboardDriver()
{
}

void KeyboardDriver::Activate()
{
	while(commandPort.Read() & 0x1)		// waiting to empty dataBuffer
		dataPort.Read();

	commandPort.Write(0xae);	//activate interrupts i.e. enable 1st ps/2 port
	commandPort.Write(0x20);	// read controller command byte
	/* keyboard interrupt enable(i.e. send IRQ1 when keyboard output buffer is full*/
	uint8_t status = (dataPort.Read() | 1) & ~0x10;
	commandPort.Write(0x60);	// set controller command byte
	dataPort.Write(status);
	dataPort.Write(0xf4); // keyboard enable
}

void printf(char*);
void printfHex(uint8_t key);

uint32_t KeyboardDriver::HandleInterrupt(uint32_t esp)
{
	uint8_t key = dataPort.Read();
	static bool Shift = false;

	if(pKbEventHandler == 0)
		return esp;

	switch(key)
	{
		case 0x02: if(Shift) pKbEventHandler->OnKeyDown('!');
					else	 pKbEventHandler->OnKeyDown('1'); break;
		case 0x03: if(Shift) pKbEventHandler->OnKeyDown('@');
					else	 pKbEventHandler->OnKeyDown('2'); break;
		case 0x04: if(Shift) pKbEventHandler->OnKeyDown('#');
					else	 pKbEventHandler->OnKeyDown('3'); break;
		case 0x05: if(Shift) pKbEventHandler->OnKeyDown('$');
					else	 pKbEventHandler->OnKeyDown('4'); break;
		case 0x06: if(Shift) pKbEventHandler->OnKeyDown('%');
					else	 pKbEventHandler->OnKeyDown('5'); break;
		case 0x07: if(Shift) pKbEventHandler->OnKeyDown('^');
					else	 pKbEventHandler->OnKeyDown('6'); break;
		case 0x08: if(Shift) pKbEventHandler->OnKeyDown('&');
					else	 pKbEventHandler->OnKeyDown('7'); break;
		case 0x09: if(Shift) pKbEventHandler->OnKeyDown('*');
					else	 pKbEventHandler->OnKeyDown('8'); break;
		case 0x0A: if(Shift) pKbEventHandler->OnKeyDown('(');
					else	 pKbEventHandler->OnKeyDown('9'); break;
		case 0x0B: if(Shift) pKbEventHandler->OnKeyDown(')');
					else	 pKbEventHandler->OnKeyDown('0'); break;

		case 0x10: if(Shift) pKbEventHandler->OnKeyDown('Q');
					else	 pKbEventHandler->OnKeyDown('q'); break;
		case 0x11: if(Shift) pKbEventHandler->OnKeyDown('W');
					else	 pKbEventHandler->OnKeyDown('w'); break;
		case 0x12: if(Shift) pKbEventHandler->OnKeyDown('E');
					else	 pKbEventHandler->OnKeyDown('e'); break;
		case 0x13: if(Shift) pKbEventHandler->OnKeyDown('R');
					else	 pKbEventHandler->OnKeyDown('r'); break;
		case 0x14: if(Shift) pKbEventHandler->OnKeyDown('T');
					else	 pKbEventHandler->OnKeyDown('t'); break;
		case 0x15: if(Shift) pKbEventHandler->OnKeyDown('Y');
					else	 pKbEventHandler->OnKeyDown('y'); break;
		case 0x16: if(Shift) pKbEventHandler->OnKeyDown('U');
					else	 pKbEventHandler->OnKeyDown('u'); break;
		case 0x17: if(Shift) pKbEventHandler->OnKeyDown('I');
					else	 pKbEventHandler->OnKeyDown('i'); break;
		case 0x18: if(Shift) pKbEventHandler->OnKeyDown('O');
					else	 pKbEventHandler->OnKeyDown('o'); break;
		case 0x19: if(Shift) pKbEventHandler->OnKeyDown('P');
					else	 pKbEventHandler->OnKeyDown('p'); break;

		case 0x1E: if(Shift) pKbEventHandler->OnKeyDown('A');
					else	 pKbEventHandler->OnKeyDown('a'); break;
		case 0x1F: if(Shift) pKbEventHandler->OnKeyDown('S');
					else	 pKbEventHandler->OnKeyDown('s'); break;
		case 0x20: if(Shift) pKbEventHandler->OnKeyDown('D');
					else	 pKbEventHandler->OnKeyDown('d'); break;
		case 0x21: if(Shift) pKbEventHandler->OnKeyDown('F');
					else	 pKbEventHandler->OnKeyDown('f'); break;
		case 0x22: if(Shift) pKbEventHandler->OnKeyDown('G');
					else	 pKbEventHandler->OnKeyDown('g'); break;
		case 0x23: if(Shift) pKbEventHandler->OnKeyDown('H');
					else	 pKbEventHandler->OnKeyDown('h'); break;
		case 0x24: if(Shift) pKbEventHandler->OnKeyDown('J');
					else	 pKbEventHandler->OnKeyDown('j'); break;
		case 0x25: if(Shift) pKbEventHandler->OnKeyDown('K');
					else	 pKbEventHandler->OnKeyDown('k'); break;
		case 0x26: if(Shift) pKbEventHandler->OnKeyDown('L');
					else	 pKbEventHandler->OnKeyDown('l'); break;

		case 0x2C: if(Shift) pKbEventHandler->OnKeyDown('Z');
					else	 pKbEventHandler->OnKeyDown('z'); break;
		case 0x2D: if(Shift) pKbEventHandler->OnKeyDown('X');
					else	 pKbEventHandler->OnKeyDown('x'); break;
		case 0x2E: if(Shift) pKbEventHandler->OnKeyDown('C');
					else	 pKbEventHandler->OnKeyDown('c'); break;
		case 0x2F: if(Shift) pKbEventHandler->OnKeyDown('V');
					else	 pKbEventHandler->OnKeyDown('v'); break;
		case 0x30: if(Shift) pKbEventHandler->OnKeyDown('B');
					else	 pKbEventHandler->OnKeyDown('b'); break;
		case 0x31: if(Shift) pKbEventHandler->OnKeyDown('N');
					else	 pKbEventHandler->OnKeyDown('n'); break;
		case 0x32: if(Shift) pKbEventHandler->OnKeyDown('M');
					else	 pKbEventHandler->OnKeyDown('m'); break;
		case 0x33: if(Shift) pKbEventHandler->OnKeyDown('<');
					else	 pKbEventHandler->OnKeyDown(','); break;
		case 0x34: if(Shift) pKbEventHandler->OnKeyDown('>');
					else	 pKbEventHandler->OnKeyDown('.'); break;
		case 0x35: if(Shift) pKbEventHandler->OnKeyDown('?');
					else	 pKbEventHandler->OnKeyDown('/'); break;

		case 0x39: pKbEventHandler->OnKeyDown(' '); break;
		case 0x2A: case 0x36: Shift = true; break;
		case 0xAA: case 0xB6: Shift = false; break;
		default:
			/* Each key on keyboard associated with 2 code i.e mark & release scancode
			* currently we are interested only in mark scancode*/
			if(key < 0x80)
			{
				printf("KEYBOARD 0x");
				printfHex(key);
			}
			break;
	}

	return esp;
}
