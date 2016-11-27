/*
	Part of the R2A03 Recompiler Project.
*/

#include "MemoryMapperDebug.h"

uint8_t * memory;
uint8_t ** assoc_table;

void rrpMemWrite(uint16_t addr, uint8_t byte)
{
	memory[addr] = byte;
	printf("[RRP] Wrote %02hhX to %04hX\n", byte, addr);
}

uint8_t rrpMemRead(uint16_t addr)
{
	uint8_t byte = memory[addr];
	printf("[RRP] Read %02hhX from %04hX\n", byte, addr);
	return byte;
}

uint8_t * rrpMemReadAssoc(uint16_t addr)
{
	uint8_t * ptr = assoc_table[addr];
	printf("[RRP] Read assoc %p from %04hX\n", ptr, addr);
	return ptr;
}

void rrpMemWriteAssoc(uint16_t addr, uint8_t * ptr)
{
	assoc_table[addr] = ptr;
	printf("[RRP] Wrote assoc %p to %04hX\n", ptr, addr);
}

void rrpInit(void)
{
	memory = (uint8_t *)malloc(sizeof(uint8_t)*0x10000);
	assoc_table = (uint8_t **)malloc(sizeof(uint8_t*)*0x10000);
}

void rrpExit(void)
{
	free(memory);
}
