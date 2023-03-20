
#include<hardwarecommunication/pci.h>
using namespace::myos::common;
using namespace::myos::hardwarecommunication;
using namespace::myos::drivers;

void printf(char* str);
void printfHex(uint8_t key);


PeripheralComponentInterconnectDeviceDescriptor::PeripheralComponentInterconnectDeviceDescriptor()
{
}

PeripheralComponentInterconnectDeviceDescriptor::~PeripheralComponentInterconnectDeviceDescriptor()
{
}



PeripheralComponentInterconnectController::PeripheralComponentInterconnectController()
    :dataPort(0xCFC),
    commandPort(0xCF8)
{
}

PeripheralComponentInterconnectController::~PeripheralComponentInterconnectController()
{
}


uint32_t PeripheralComponentInterconnectController::Read(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset)
{
    /* reigsteroffset points to consecutive DWORD in PCI configuration
       address space whose bit 0 & 1 are always 0
     * result is 32 bit value, out of 32 only couple of bits are significant to current
       read so its been shifted to clean unwanted bits using '8* (registeroffset % 4)'
       i.e. registeroffset for 'class code' is 0x0B, so result is shifted by 24 bits to
       read correct class code
     */
    uint32_t id =
        0x01 << 31
        | ((bus & 0xFF) << 16)
        | ((device & 0x1F) << 11)
        | ((function & 0x07) << 8)
        | (registeroffset & 0xFC);
    commandPort.Write(id);
    uint32_t result = dataPort.Read();
    return result >> (8* (registeroffset % 4));
}

void PeripheralComponentInterconnectController::Write(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset, uint32_t value)
{
    uint32_t id =
        0x01 << 31
        | ((bus & 0xFF) << 16)
        | ((device & 0x1F) << 11)
        | ((function & 0x07) << 8)
        | (registeroffset & 0xFC);
    commandPort.Write(id);
    dataPort.Write(value);
}

bool PeripheralComponentInterconnectController::DeviceHasFunctions(uint16_t bus, uint16_t device)
{
    /* 0x0E is headerType of Common Header, if bit 7 is set then
       multifunction device otherwise single function device */
    return Read(bus, device, 0, 0x0E) & (1<<7);
}

void PeripheralComponentInterconnectController::SelectDrivers(DriverManager *pDriverManager)
{
    /* since there are no vendor with vendor_id 0x0000,
       0xFFFF. It must be a non-existent device*/
    for(int bus = 0; bus < MAX_PCI_BUS; bus++)
    {
        for(int device = 0; device < MAX_DEVICE_PER_PCI_BUS; device++)
        {
            int numFunctions = DeviceHasFunctions(bus, device) ? MAX_FUNC_PER_DEVICE : 1;
            for(int function = 0; function < numFunctions; function++)
            {
                PeripheralComponentInterconnectDeviceDescriptor
                    dev = GetDeviceDescriptor(bus, device, function);

                if(dev.vendor_id == 0x0000 || dev.vendor_id == 0xFFFF)
                    continue;

                printf("PCI BUS ");
                printfHex(bus & 0xFF);

                printf(", DEVICE ");
                printfHex(device & 0xFF);

                printf(", FUNCTION ");
                printfHex(function & 0xFF);

                printf(" = VENDOR ");
                printfHex((dev.vendor_id & 0xFF00) >> 8);
                printfHex(dev.vendor_id & 0xFF);
                printf(", DEVICE ");
                printfHex((dev.device_id & 0xFF00) >> 8);
                printfHex(dev.device_id & 0xFF);
                printf("\n");
            }
        }
    }
}

PeripheralComponentInterconnectDeviceDescriptor PeripheralComponentInterconnectController::
GetDeviceDescriptor(uint16_t bus, uint16_t device, uint16_t function)
{
   PeripheralComponentInterconnectDeviceDescriptor result;

   result.bus = bus;
   result.device = device;
   result.function = function;

   result.vendor_id = Read(bus, device, function, 0x00);
   result.device_id = Read(bus, device, function, 0x02);

   result.class_id = Read(bus, device, function, 0x0b);
   result.subclass_id = Read(bus, device, function, 0x0a);
   result.interface_id = Read(bus, device, function, 0x09);

   result.revision = Read(bus, device, function, 0x08);
   result.interrupt = Read(bus, device, function, 0x3c);

   return result;
}
