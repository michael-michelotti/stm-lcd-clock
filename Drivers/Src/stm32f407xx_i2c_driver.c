/*
 * stm32f407xx_i2c_driver.c
 *
 *  Created on: Jan 27, 2023
 *      Author: Michael
 */

#include "stm32f407xx_i2c_driver.h"
#include "stm32f407xx_rcc_driver.h"

static uint32_t Get_Peri_Clk_Frequency();
static void Set_I2C_CR2_Freq(I2C_Register_Map_t *p_i2c_x, uint32_t freq);
static void Set_I2C_CCR(I2C_Handle_t *p_i2c_handle, uint32_t pclk_freq);
static void Configure_I2C_Clock_Mode(I2C_Handle_t *p_i2c_handle);

void I2C_Peri_Clk_Ctrl(I2C_Register_Map_t *p_i2c_x, uint8_t enable)
{
	if (enable == ENABLE)
	{
		if (p_i2c_x == I2C1)
		{
			I2C1_PCLK_EN();
		}
		if (p_i2c_x == I2C2)
		{
			I2C2_PCLK_EN();
		}
		if (p_i2c_x == I2C3)
		{
			I2C3_PCLK_EN();
		}
	}
	if (enable == DISABLE)
	{
		if (p_i2c_x == I2C1)
		{
			I2C1_PCLK_DI();
		}
		if (p_i2c_x == I2C2)
		{
			I2C2_PCLK_DI();
		}
		if (p_i2c_x == I2C3)
		{
			I2C3_PCLK_DI();
		}
	}
}

static uint32_t Get_Peri_Clk_Frequency()
{
	System_Clock_t sys_clk = RCC->CFGR &= (0b11 << RCC_CFGR_SWS);
	switch (sys_clk)
	{
	case SYS_CLK_HSI:
		return HSI_CLK_SPEED;
	case SYS_CLK_HSE:
		// TODO: implement logic for HSE and PLL
		return 0;
	case SYS_CLK_PLL:
		return 0;
	default:
		return 0;
	}
}

static void Set_I2C_CR2_Freq(I2C_Register_Map_t *p_i2c_x, uint32_t freq)
{
	// convert frequency from Hz to MHz
	freq /= 1000000u;
	// 5 bit frequency means maximum of 31 - how do i inform app?
	if (freq > 31)
		return;
	// create mask for first 5 bits (freq field)
	uint32_t mask = 0x001F;
	// mask out all but bottom 5 bits of freq
	freq &= mask;
	// clear first 5 bits of CR2, then apply freq value
	p_i2c_x->CR2 &= ~mask;
	p_i2c_x->CR2 |= freq;
}

static void Set_I2C_CCR(I2C_Handle_t *p_i2c_handle, uint32_t pclk_freq)
{
	uint32_t ccr;
	uint32_t apb1_clk;
	// look up active clock, ahb prescaler, apb1 prescaler
	uint8_t ahb_prescaler = RCC->CFGR &= (0b11 << RCC_CFGR_HPRE);
	uint8_t apb1_prescaler = RCC->CFGR &= (0b11 << RCC_CFGR_PPRE1);

	if (p_i2c_handle->i2c_config.scl_speed == I2C_SPEED_SM)
	{
		// calculate ccr for standard mode
		apb1_clk = HSI_CLK_SPEED / ahb_prescaler / apb1_prescaler;
		ccr = apb1_clk / (I2C_SPEED_SM * 2);
	}
	else
	{
		apb1_clk = HSI_CLK_SPEED / ahb_prescaler / apb1_prescaler;
		ccr = apb1_clk / (I2C_SPEED_FM * 3);
	}

	// clear bottom 12 bits
	p_i2c_handle->p_i2c_x->CCR &= ~(0x0FFF);
	// set bottom 12 bits to ccr
	p_i2c_handle->p_i2c_x->CCR |= ccr;
}

static void Configure_I2C_Clock_Mode(I2C_Handle_t *p_i2c_handle)
{
	uint32_t pclk_freq = Get_Peri_Clk_Frequency();

	if (p_i2c_handle->i2c_config.scl_speed == I2C_SPEED_SM)
	{
		// clear F/S bit
		p_i2c_handle->p_i2c_x->CCR &= ~(1 << I2C_CCR_FS);
		// set FREQ in I2C_CR2 to 16
		Set_I2C_CR2_Freq(p_i2c_handle->p_i2c_x, pclk_freq);
		// set I2C_CCR to 80 (value for standard mode)
		Set_I2C_CCR(p_i2c_handle, pclk_freq);
	}
	else
	{
		// set F/S bit
		p_i2c_handle->p_i2c_x->CCR |= (1 << I2C_CCR_FS);
		// set FREQ in I2C_CR2 to 16
		Set_I2C_CR2_Freq(p_i2c_handle->p_i2c_x, pclk_freq);
		// set I2C_CCR to 80 (value for standard mode)
		Set_I2C_CCR(p_i2c_handle, pclk_freq);
	}
}

void I2C_Init(I2C_Handle_t *p_i2c_handle)
{
	// CLOCK CONFIGURATION
	// enable peripheral clock
	I2C_Peri_Clk_Ctrl(p_i2c_handle->p_i2c_x, ENABLE);

	// configure frequency and CCR
	Configure_I2C_Clock_Mode(p_i2c_handle);

	// CR1 AND CR2 CONFIGURATION

	// ENABLE PERIPHERAL
}

/*
void I2C_Cleanup(I2C_Register_t *p_i2c_x);

void I2C_Peripheral_Power_Switch(I2C_Register_t *p_i2c_x, uint8_t on_or_off);
uint8_t I2C_Get_Flag_Status(I2C_Register_t *p_i2c_x, uint32_t flag_name);
void I2C_Manage_Acking(I2C_Register_t *p_i2c_x, uint8_t enable);

void I2C_MasterSend(I2C_Handle_t *p_i2c_handle, uint8_t *p_tx_buffer, uint32_t len, uint8_t slave_addr,uint8_t sr);
void I2C_MasterReceive(I2C_Handle_t *p_i2c_handle, uint8_t *p_rx_buffer, uint32_t len, uint8_t slave_addr);
*/
