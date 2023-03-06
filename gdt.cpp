
#include "gdt.h"

GlobalDescriptorTable::GlobalDescriptorTable()
	: nullSegmentSelector(0,0,0),
	unusedSegmentSelector(0,0,0),
	codeSegmentSelector(0,64*1024*1024, 0x9A),
	dataSegmentSelector(0,64*1024*1024, 0x92)
{
	// Now need to tell cpu the address of GDT
	uint32_t i[2];
	i[0] = (uint32_t)this;
	i[1] = sizeof(GlobalDescriptorTable) << 16;

	asm volatile("lgdt (%0)": :"p" (((uint8_t*)i)+2));
}


GlobalDescriptorTable::~GlobalDescriptorTable()
{
}

uint16_t GlobalDescriptorTable::DataSegmentSelector()
{
	return (uint8_t*)&dataSegmentSelector - (uint8_t*) this;
}

uint16_t GlobalDescriptorTable::CodeSegmentSelector()
{
	return (uint8_t*)&codeSegmentSelector - (uint8_t*) this;
}

GlobalDescriptorTable::SegmentDescriptor::SegmentDescriptor(uint32_t limit, uint32_t base , uint8_t accessFlags)
{
	uint8_t* target = (uint8_t*)this;

	if(limit <= 65536)
	// filing GDT flags in case of 16 bit protected mode
	{
		target[6] = 0x40;
	}
	else
	// filing GDT flags in case of 32 bit protected mode
	{
		// limit is greater than 2^16
		if((limit & 0xFFF) != 0xFFF)
			limit = (limit >> 12) - 1;
		else// due to 12 imaginary bits in order to address 2^32 size in 20 bits
			limit = limit >> 12;

		target[6] = 0xC0;
	}

	// fill limit
	target[0] = limit & 0xFF;
	target[1] = (limit >> 8) & 0xFF;
	target[6] |= (limit >> 16) & 0xF;

	// file base pointer 
	target[2] = base & 0xFF;
	target[3] = (base >> 8) & 0xFF;
	target[4] = (base >> 16) & 0xFF;
	target[7] = (base >> 24) & 0xFF;
	
	// fill accessFlags
	target[5] = accessFlags;
}


uint32_t GlobalDescriptorTable::SegmentDescriptor::Base()
{
	uint8_t* target = (uint8_t*)this;
	uint32_t result = target[7];
	result = (result << 8) + target[4];
	result = (result << 8) + target[3];
	result = (result << 8) + target[2];

	return result;
}

uint32_t GlobalDescriptorTable::SegmentDescriptor::Limit()
{
	uint8_t* target = (uint8_t*)this;
	uint32_t result = target[6] & 0xF;
	result = (result << 8) + target[1];
	result = (result << 8) + target[0];

	if((target[6] & 0xC0) == 0)
		result = (result << 12) + 0xFFF;

	return result;
}
