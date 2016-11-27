/*
	Part of the R2A03 Recompiler Project.
*/

#include "MemoryMapperSimple.h"

uint8_t * memory;

void rrpMemWrite(uint16_t addr, uint8_t byte)
{
	memory[addr] = byte;
}

uint8_t rrpMemRead(uint16_t addr)
{
	return memory[addr];
}

void rrpInit(void)
{
	memory = (uint8_t *)malloc(sizeof(uint8_t)*0x10000);
}

void rrpExit(void)
{
	free(memory);
}
