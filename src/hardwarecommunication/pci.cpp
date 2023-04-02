
#include <hardwarecommunication/pci.h>
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

void PeripheralComponentInterconnectController::SelectDrivers(DriverManager *pDriverManager, InterruptManager *pInterrupts)
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

                for(int barNum = 0; barNum < MAX_BAR; barNum++)
                {
                    BaseAddressRegister bar = GetBaseAddressRegister(bus, device, function, barNum);

                    if(bar.pAddress && (bar.type == BAR_Type_InputOutput))
                        dev.portBase = (uint32_t)bar.pAddress;

                    Driver *pDriver = GetDriver(dev, pInterrupts);
                    if(pDriver != 0)
                        pDriverManager->AddDriver(pDriver);
                }


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

BaseAddressRegister PeripheralComponentInterconnectController::
GetBaseAddressRegister(uint16_t bus, uint16_t device, uint16_t function, uint16_t bar)
{
    /* For HeaderType 0x00(Standard Header), 6 Bars & 0x01(PCI-to-PCI Header), 2 Bars are supported
     * 1st bit of BAR value defines type of BAR i.e. InputOutput(1) or MemoryMapping(0)
       InputOutput BAR address is 4-Byte Aligned & MemoryMapping BAR is 16-Byte Aligned
     * 1 & 2 bit of MemoryMapping BAR defines address space size
       i.e 0 -> 32 bit, 2 -> 64 bit, 1 -> reserved
     */
    BaseAddressRegister result;

    uint32_t headerType = Read(bus, device, function, 0x0E) & 0x7F;
    int maxBars = 6 - (4 * headerType);
    if(bar >= maxBars)
        return result;

    uint8_t barOffset = 4 * bar;
    uint32_t bar_value = Read(bus, device, function, 0x10 + barOffset);
    result.type = (bar_value & 0x01) ? BAR_Type_InputOutput : BAR_Type_MemoryMapping;

    if(result.type == BAR_Type_MemoryMapping)
    {
        switch((bar_value >> 1) & 0x03)
        {
            case 0: // 32 Bit Mode
            case 1: // 20 Bit Mode
            case 2: // 64 Bit Mode
                    break;
        }

    }
    else // BAR_Type_InputOutput
    {
       result.pAddress = (uint8_t *)(bar_value & ~0x03);
       result.prefetchable = false;
    }

    return result;
}

Driver* PeripheralComponentInterconnectController::
GetDriver(PeripheralComponentInterconnectDeviceDescriptor pciDevDesc,
          InterruptManager *pInterrupts)
{
   Driver *driver = 0;
    switch(pciDevDesc.vendor_id)
    {
        case 0x1022: // AMD
            switch(pciDevDesc.device_id)
            {
                case 0x2000: // am79c973
                    /*
                    driver = (amd_am79c973*)MemoryManager::pActiveMemoryManager->malloc(sizeof(amd_am79c973));
                    if(driver != 0)
                        new (driver) amd_am79c973(...);
                    */
                    printf("AMD am79c973 ");
                    break;
            }
            break;

        case 0x8086: // Intel
            break;
    }


    switch(pciDevDesc.class_id)
    {
        case 0x03: // Graphics
            switch(pciDevDesc.subclass_id)
            {
                case 0x00: // VGA
                    printf("VGA ");
                    break;
            }
            break;
    }

    return driver;
}
