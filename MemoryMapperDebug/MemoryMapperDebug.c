/*
	Part of the R2A03 Recompiler Project.
*/

#include "MemoryMapperDebug.h"

uint8_t * memory;

void rrpMemWrite(uint16_t addr, uint8_t byte)
{
	memory[addr] = byte;
	printf("[RRP] Wrote %02hhX to %04hX\n", byte, addr);
}

uint_fast8_t rrpMemRead(uint16_t addr)
{
	uint8_t byte = memory[addr];
	printf("[RRP] Read %02hhX from %04hX\n", byte, addr);
	return byte;
}

void rrpInit(void)
{
	memory = (uint8_t *)malloc(sizeof(uint8_t)*0x10000);
}

void rrpExit(void)
{
	free(memory);
}
