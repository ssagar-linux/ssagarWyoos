#include <drivers/ata.h>
using namespace myos;
using namespace myos::common;
using namespace myos::drivers;
using namespace myos::hardwarecommunication;

void printf(char*);

AdvancedTechnologyAttachment::AdvancedTechnologyAttachment(uint16_t portBase, bool master)
:  dataPort(portBase),
   errorPort(portBase        + 0x1),
   sectoreCountPort(portBase + 0x2),
   lbaLowPort(portBase       + 0x3),
   lbaMidPort(portBase       + 0x4),
   lbaHiPort(portBase        + 0x5),
   devicePort(portBase       + 0x6),
   commandPort(portBase      + 0x7),
   controlPort(portBase    + 0x206)
{
   bytesPerSector = 512;
   this->master = master;
}

AdvancedTechnologyAttachment::~AdvancedTechnologyAttachment()
{
}




void AdvancedTechnologyAttachment::Identify()
{
   /*
    * Clear HOB bit, which is used to Read back the High Order Byte of the
                     last LBA48 value sent to IO port
    * Floating bus check: i)  select the master
                          ii) then compare status register value with 0xFF
                              (i.e. no device on bus return from IDENTIFY)
    * Read status port(0x1F7) again, if 0 then drive doesn't exists
    * poll out status Register as soon as device is get freed or error is hit
    */
   devicePort.Write(master ? 0xA0 : 0xB0);
   controlPort.Write(0);

   devicePort.Write(0xA0);
   uint8_t status = commandPort.Read();
   if(status == 0xFF)
      return;

   devicePort.Write(master ? 0xA0 : 0xB0);
   sectoreCountPort.Write(0);
   lbaLowPort.Write(0);
   lbaMidPort.Write(0);
   lbaHiPort.Write(0);
   commandPort.Write(0xEC); // identify

   status = commandPort.Read();
   if(status == 0x00)
      return;

   while(((status & 0x80) == 0x80)
      && ((status & 0x01) != 0x01))
         status = commandPort.Read();

   if(status & 0x01)
   {
      printf("ERROR");
      return;
   }

   for(int i = 0; i < 256; i++)
   {
      uint16_t data = dataPort.Read();
      char *text = "  \0";
      text[0] = (data >> 8) & 0xFF;
      text[1] = data & 0xFF;
      printf(text);
   }
   printf("\n");
}

void AdvancedTechnologyAttachment::Read28(uint32_t sector, uint8_t *pData, int count)
{
   /*
    * check base condition of sector, bytesPerSector
    * Prepare sector for Read command:
         i)   erase previous errors by sending 0 to error port(0x1F1)
         ii)  update sector number
         iii) write 28 bit LBA address
         iv) send write command to port 0x1F7
    * read data from sector and then print
    */

   if(sector > 0x0FFFFFFF)
      return;

   devicePort.Write( (master ? 0xE0 : 0xF0) |
                     ((sector & 0x0F000000) >> 24) );
   errorPort.Write(0);
   sectoreCountPort.Write(1);
   lbaLowPort.Write(sector  & 0x000000FF);
   lbaMidPort.Write((sector & 0x0000FF00) >> 8);
   lbaHiPort.Write((sector  & 0x00FF0000) >> 16);
   commandPort.Write(0x20); // Read

   uint8_t status = commandPort.Read();
   while(((status & 0x80) == 0x80)
      && ((status & 0x01) != 0x01))
         status = commandPort.Read();

   if(status & 0x01)
   {
      printf("ERROR");
      return;
   }

   printf("\nReading from S-ATA Drive: ");

   for(int i = 0; i < count; i += 2)
   {
      uint16_t wdata = dataPort.Read();

      char *text = "  \0";
      if(i+1 < count)
         text[1] = (wdata >> 8) & 0x00FF;
      text[0] = wdata & 0x00FF;
      printf(text);


      pData[i] = wdata & 0x00FF;
         pData[i+1] = (wdata >> 8) & 0x00FF;
   }

   for(int i = count + (count % 2); i < bytesPerSector; i += 2)
      dataPort.Read();
}

void AdvancedTechnologyAttachment::Write28(uint32_t sector, uint8_t *pData, int count)
{
   /*
    * check base condition of sector, bytesPerSector
    * Select sector for write command:
         i)   erase previous errors by sending 0 to error port(0x1F1)
         ii)  update sector number
         iii) write 28 bit LBA address
         iv) send write command to port 0x1F7
    * write user data to sector in little endian 
      and then fill remaining sector with zero
    */

   if(sector > 0x0FFFFFFF)
      return;
   if(count > bytesPerSector)
      return;

   devicePort.Write( (master ? 0xE0 : 0xF0) |
                     ((sector & 0x0F000000) >> 24) );
   errorPort.Write(0);
   sectoreCountPort.Write(1);
   lbaLowPort.Write(sector  & 0x000000FF);
   lbaMidPort.Write((sector & 0x0000FF00) >> 8);
   lbaHiPort.Write((sector  & 0x00FF0000) >> 16);
   commandPort.Write(0x30); // Write

   printf("Writing to S-ATA Drive: ");

   for(int i = 0; i < count; i += 2)
   {
      uint16_t wdata = pData[i];
      if(i+1 < count)
         wdata |= ((uint16_t)pData[i+1]) << 8;
      dataPort.Write(wdata);

      char *text = "  \0";
      text[1] = (wdata >> 8) & 0x00FF;
      text[0] = wdata & 0x00FF;
      printf(text);
   }

   for(int i = count + (count % 2); i < bytesPerSector; i += 2)
      dataPort.Write(0x0000);
}

void AdvancedTechnologyAttachment::Flush()
{
   /*
    * Based on master/slave update device port(0x1F6),
      then send flush command to control port(0x1F7)
    * poll out status Register as soon as device is get freed or error is hit
    */
   devicePort.Write(master ? 0xE0 : 0xF0);
   commandPort.Write(0xE7); // flush

   uint8_t status = commandPort.Read();
   if(status == 0x00)
      return;

   while(((status & 0x80) == 0x80)
      && ((status & 0x01) != 0x01))
         status = commandPort.Read();

   if(status & 0x01)
   {
      printf("ERROR");
      return;
   }
}

