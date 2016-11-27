/*
	Part of the R2A03 Recompiler Project.

	Contains the definitions for the simplest possible
	memory mapper; 64K of contiguous memory malloc'd on init,
	and free'd on exit. rrpMemWrite and rrpMemRead provide direct
	access to this memory.
*/

#pragma once

#include <stdint.h>
#include <stdlib.h>

void rrpMemWrite(uint16_t, uint8_t);
uint8_t rrpMemRead(uint16_t);

void rrpInit(void);
void rrpExit(void);
