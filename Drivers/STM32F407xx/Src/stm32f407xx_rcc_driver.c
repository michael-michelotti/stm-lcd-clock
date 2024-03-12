#include "stm32f407xx.h"
#include "stm32f407xx_rcc_driver.h"

static uint32_t Translate_APB_Prescaler(uint8_t bit_value);
static uint32_t Translate_AHB_Prescaler(uint8_t bit_value);

uint32_t RCC_Get_Sys_Clk_Frequency()
{
	System_Clock_t sys_clk = RCC->CFGR &= (0b11 << RCC_CFGR_SWS);
	switch (sys_clk)
	{
		case SYS_CLK_HSI:
			return HSI_CLK_SPEED;
		case SYS_CLK_HSE:
			/* TODO: implement logic for HSE and PLL */
			return 0;
		case SYS_CLK_PLL:
			return 0;
		default:
			return 0;
	}
}

uint32_t RCC_Get_AHB_Prescaler()
{
	uint32_t ahb_prescaler_bit_val = (RCC->CFGR >> RCC_CFGR_HPRE) & 0b1111;
	return Translate_AHB_Prescaler(ahb_prescaler_bit_val);
}

uint32_t RCC_Get_APB_Prescaler()
{
	uint32_t apb_prescaler_bit_val = (RCC->CFGR >> RCC_CFGR_PPRE1) & 0b111;
	return Translate_APB_Prescaler(apb_prescaler_bit_val);
}

static uint32_t Translate_AHB_Prescaler(uint8_t bit_value)
{
	/* AHB PRESCALER */
	switch (bit_value)
	{
		case 0b0000 ... 0b0111: return 1;
		case 0b1000: return 2;
		case 0b1001: return 4;
		case 0b1010: return 8;
		case 0b1011: return 16;
		case 0b1100: return 64;
		case 0b1101: return 128;
		case 0b1110: return 256;
		case 0b1111: return 512;
		default: return -1;
	}
}

static uint32_t Translate_APB_Prescaler(uint8_t bit_value)
{
	/* APB1 AND APB2 PRESCALER */
	switch (bit_value)
	{
		case 0b000 ... 0b011: return 1;
		case 0b100: return 2;
		case 0b101: return 4;
		case 0b110: return 8;
		case 0b111: return 16;
		default: return -1;
	}

}
