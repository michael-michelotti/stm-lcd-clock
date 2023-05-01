/*
 * stm32f407xx.c
 *
 *  Created on: Feb 9, 2023
 *      Author: Michael
 */

#include "stm32f407xx.h"

uint32_t GET_FIELD(uint32_t *ADDR, uint32_t MASK)
{
	uint32_t value = *ADDR & MASK;
	value /= MASK & ~(MASK << 1);
	return value;
}

uint8_t GET_BIT(uint32_t *ADDR, uint32_t MASK)
{
	if (*ADDR & MASK)
		return 1;
	return 0;
}
