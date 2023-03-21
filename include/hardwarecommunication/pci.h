
#ifndef __MYOS__HARDWARECOMMUNICATION__PCI_H
#define __MYOS__HARDWARECOMMUNICATION__PCI_H

#ifndef MAX_PCI_BUS
#define MAX_PCI_BUS 8
#endif
#ifndef MAX_DEVICE_PER_PCI_BUS
#define MAX_DEVICE_PER_PCI_BUS 32
#endif
#ifndef MAX_FUNC_PER_DEVICE
#define MAX_FUNC_PER_DEVICE 8
#endif
#ifndef MAX_BAR
#define MAX_BAR 6
#endif

#include <common/types.h>
#include <drivers/driver.h>
#include <hardwarecommunication/port.h>
#include <hardwarecommunication/interrupts.h>


namespace myos
{
    namespace hardwarecommunication
    {
        enum BaseAddressRegisterType
        {
            BAR_Type_MemoryMapping = 0,
            BAR_Type_InputOutput = 1
        };

        class BaseAddressRegister
        {
            public:
                bool prefetchable;
                myos::common::uint8_t* pAddress;
                myos::common::uint32_t size;
                BaseAddressRegisterType type;
        };

        class PeripheralComponentInterconnectDeviceDescriptor
        /* PeripheralComponentInterconnectDeviceDescriptor is a descriptor
           for configuration address space */
        {
            public:
                myos::common::uint32_t portBase;
                myos::common::uint32_t interrupt;

                myos::common::uint16_t bus;
                myos::common::uint16_t device;
                myos::common::uint16_t function;

                myos::common::uint16_t vendor_id;
                myos::common::uint16_t device_id;

                myos::common::uint8_t class_id;
                myos::common::uint8_t subclass_id;
                myos::common::uint8_t interface_id;

                myos::common::uint8_t revision;

                PeripheralComponentInterconnectDeviceDescriptor();
                ~PeripheralComponentInterconnectDeviceDescriptor();
        };

        class PeripheralComponentInterconnectController
        {
            Port32Bit dataPort;
            Port32Bit commandPort;

            public:
                PeripheralComponentInterconnectController();
                ~PeripheralComponentInterconnectController();

                myos::common::
                uint32_t Read(myos::common::uint16_t bus,
                              myos::common::uint16_t device,
                              myos::common::uint16_t function,
                              myos::common::uint32_t registeroffset);
                void Write(myos::common::uint16_t bus,
                           myos::common::uint16_t device,
                           myos::common::uint16_t function,
                           myos::common::uint32_t registeroffset,
                           myos::common::uint32_t value);
                bool DeviceHasFunctions(myos::common::uint16_t bus,
                                        myos::common::uint16_t device);
                void SelectDrivers(myos::drivers::DriverManager *pDriverManager,
                                   InterruptManager *pInterrupts);
                myos::drivers::Driver* GetDriver(PeripheralComponentInterconnectDeviceDescriptor pciDevDesc,
                                                 InterruptManager *pInterrupts);

                PeripheralComponentInterconnectDeviceDescriptor
                    GetDeviceDescriptor(myos::common::uint16_t bus,
                                        myos::common::uint16_t device,
                                        myos::common::uint16_t function);
                BaseAddressRegister
                    GetBaseAddressRegister(myos::common::uint16_t bus,
                                           myos::common::uint16_t device,
                                           myos::common::uint16_t function,
                                           myos::common::uint16_t bar); // bar == Base Address Register
        };
    }
}
#endif
