/*
 * stm32f407xx_i2c_driver.c
 *
 *  Created on: Jan 27, 2023
 *      Author: Michael
 */

#include "stm32f407xx_i2c_driver.h"
#include "stm32f407xx_rcc_driver.h"

static void Manage_Acking(I2C_Register_Map_t *p_i2c_x, uint8_t enable);
static void Configure_I2C_Clock_Mode(I2C_Handle_t *p_i2c_handle);
static void Set_I2C_CR2_Freq(I2C_Register_Map_t *p_i2c_x, uint32_t freq);
static void Set_I2C_CCR(I2C_Handle_t *p_i2c_handle, uint32_t pclk_freq);
static void Configure_I2C_TRISE(I2C_Handle_t *p_i2c_handle, uint32_t pclk_freq);
static void I2C_Set_Own_Address(I2C_Handle_t *p_i2c_handle);
static uint32_t Get_Peri_Clk_Frequency();
static uint32_t Translate_AHB_Prescaler(uint8_t bit_value);
static uint32_t Translate_APB_Prescaler(uint8_t bit_value);

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

void I2C_Init(I2C_Handle_t *p_i2c_handle)
{
	// CLOCK CONFIGURATION
	// enable peripheral clock
	I2C_Peri_Clk_Ctrl(p_i2c_handle->p_i2c_x, ENABLE);

	// configure frequency and CCR
	Configure_I2C_Clock_Mode(p_i2c_handle);

	// CR1 AND CR2 CONFIGURATION
	I2C_Set_Own_Address(p_i2c_handle);

	// Enable peripheral
	I2C_Peripheral_Power_Switch(p_i2c_handle->p_i2c_x, ON);

	// Ack bit must be set after peripheral is enabled
	if (p_i2c_handle->i2c_config.ack_ctrl == I2C_ACK_EN)
	{
		Manage_Acking(p_i2c_handle->p_i2c_x, ENABLE);
	}
	else
	{
		Manage_Acking(p_i2c_handle->p_i2c_x, DISABLE);
	}
}

void I2C_Peripheral_Power_Switch(I2C_Register_Map_t *p_i2c_x, uint8_t on_or_off)
{
	p_i2c_x->CR1 |= (1 << I2C_CR1_PE);
}

void I2C_Master_Send(I2C_Handle_t *p_i2c_handle, uint8_t *p_tx_buffer, uint32_t len, uint8_t slave_addr,uint8_t sr)
{

}


/*
void I2C_Cleanup(I2C_Register_t *p_i2c_x);


uint8_t I2C_Get_Flag_Status(I2C_Register_t *p_i2c_x, uint32_t flag_name);
void I2C_Manage_Acking(I2C_Register_t *p_i2c_x, uint8_t enable);

void I2C_MasterSend(I2C_Handle_t *p_i2c_handle, uint8_t *p_tx_buffer, uint32_t len, uint8_t slave_addr,uint8_t sr);
void I2C_MasterReceive(I2C_Handle_t *p_i2c_handle, uint8_t *p_rx_buffer, uint32_t len, uint8_t slave_addr);
*/

/*************** PRIVATE UTILITY FUNCTIONS *****************/
static void Manage_Acking(I2C_Register_Map_t *p_i2c_x, uint8_t enable)
{
	if (enable == ENABLE)
	{
		p_i2c_x->CR1 |= (1 << I2C_CR1_ACK);
	}
	else
	{
		p_i2c_x->CR1 &= ~(1 << I2C_CR1_ACK);
	}
}

static void Configure_I2C_Clock_Mode(I2C_Handle_t *p_i2c_handle)
{
	uint32_t pclk_freq = Get_Peri_Clk_Frequency();

	// set FREQ field in I2C_CR2
	Set_I2C_CR2_Freq(p_i2c_handle->p_i2c_x, pclk_freq);

	if (p_i2c_handle->i2c_config.scl_speed <= I2C_SPEED_SM)
	{
		// clear F/S bit
		p_i2c_handle->p_i2c_x->CCR &= ~(1 << I2C_CCR_FS);
		// set I2C_CCR to 80 (value for standard mode)
		Set_I2C_CCR(p_i2c_handle, pclk_freq);
	}
	else
	{
		// set F/S bit
		p_i2c_handle->p_i2c_x->CCR |= (1 << I2C_CCR_FS);
		// set I2C_CCR to 80 (value for standard mode)
		Set_I2C_CCR(p_i2c_handle, pclk_freq);
	}

	Configure_I2C_TRISE(p_i2c_handle, pclk_freq);
}

static void Set_I2C_CR2_Freq(I2C_Register_Map_t *p_i2c_x, uint32_t freq)
{
	// convert frequency from Hz to MHz
	freq /= 1000000u;
	// 5 bit frequency means maximum of 31
	if (freq > 31)
		return;
	// clear first 5 bits of CR2, then apply freq value
	p_i2c_x->CR2 &= ~0x1F;
	p_i2c_x->CR2 |= freq;
}

static void Set_I2C_CCR(I2C_Handle_t *p_i2c_handle, uint32_t pclk_freq)
{
	uint32_t ccr;
	// look up values in apb and ahb prescaler fields, needs to be translated
	uint32_t ahb_prescaler = (RCC->CFGR >> RCC_CFGR_HPRE) & 0b1111;
	uint32_t apb1_prescaler = (RCC->CFGR >> RCC_CFGR_PPRE1) & 0b111;
	ahb_prescaler = Translate_AHB_Prescaler(ahb_prescaler);
	apb1_prescaler = Translate_APB_Prescaler(apb1_prescaler);

	// do clock division
	uint32_t apb1_clk = pclk_freq / ahb_prescaler / apb1_prescaler;

	if (p_i2c_handle->i2c_config.scl_speed <= I2C_SPEED_SM)
	{
		// calculate ccr for standard mode
		ccr = apb1_clk / (I2C_SPEED_SM * 2);
	}
	else
	{
		// calculate ccr for fast mode
		ccr = apb1_clk / (I2C_SPEED_FM * 3);
	}

	// clear bottom 12 bits, then set bottom 12 to CCR
	p_i2c_handle->p_i2c_x->CCR &= ~(0x0FFF);
	p_i2c_handle->p_i2c_x->CCR |= ccr;
}

static void Configure_I2C_TRISE(I2C_Handle_t *p_i2c_handle, uint32_t pclk_freq)
{
	uint8_t trise;

	if (p_i2c_handle->i2c_config.scl_speed <= I2C_SPEED_SM)
	{
		// mode is standard - TRISE is (max rise time * apb1 clock) + 1
		// max rise time is 1000ns = 1e-6, simplifies to (apb1 clock / 1e6) + 1
		trise = (pclk_freq / 1000000u) + 1;
	}
	else
	{
		// mode is fast - max rise time is 300ns
		// simplifies to (apb1 clock * 3 / 1e7) + 1
		trise = ( (pclk_freq * 3) / 10000000u ) + 1;
	}

	// set TRISE in I2C_TRISE register - clear bits first
	p_i2c_handle->p_i2c_x->TRISE &= ~(0x1F);
	p_i2c_handle->p_i2c_x->TRISE |= trise;
}

static void I2C_Set_Own_Address(I2C_Handle_t *p_i2c_handle)
{
	// clear top bit of own address (in case user sent 8 bit address)
	p_i2c_handle->i2c_config.dev_addr &= ~(1 << 8);
	// clear 7-bit address field, then set address
	p_i2c_handle->p_i2c_x->OAR1 &= ~(0x7F << 1);
	p_i2c_handle->p_i2c_x->OAR1 |= (p_i2c_handle->i2c_config.dev_addr << 1);
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

static uint32_t Translate_APB_Prescaler(uint8_t bit_value)
{
	// APB1 AND APB2 PRESCALER
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

static uint32_t Translate_AHB_Prescaler(uint8_t bit_value)
{
	// AHB PRESCALER
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

