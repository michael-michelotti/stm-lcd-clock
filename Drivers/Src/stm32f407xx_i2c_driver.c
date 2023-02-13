/*
 * stm32f407xx_i2c_driver.c
 *
 *  Created on: Jan 27, 2023
 *      Author: Michael
 */

#include "stm32f407xx_i2c_driver.h"
#include "stm32f407xx_rcc_driver.h"
#include "stm32f407xx_gpio_driver.h"
#include <stdio.h>

/*************** PRIVATE UTILITY FUNCTION DECLARATIONS*****************/
static uint8_t I2C_Check_Status_Flag(I2C_Handle_t *p_i2c_handle, uint8_t flag_num, uint8_t sr_1_or_2);
static void I2C_Write_Address_Byte(I2C_Handle_t *p_i2c_handle, uint8_t slave_addr, uint8_t read_or_write);
static void I2C_Ack_Control(I2C_Register_Map_t *p_i2c_x, uint8_t enable);
static void I2C_Configure_Clock_Registers(I2C_Handle_t *p_i2c_handle);
static void I2C_Set_CR2_Freq(I2C_Register_Map_t *p_i2c_x, uint32_t sys_clk_freq);
static void I2C_Set_CCR(I2C_Handle_t *p_i2c_handle, uint32_t sys_clk_freq);
static void I2C_Configure_TRISE(I2C_Handle_t *p_i2c_handle, uint32_t sys_clk_freq);
static void I2C_Set_Own_Address(I2C_Handle_t *p_i2c_handle);

void I2C_Peri_Clk_Ctrl(I2C_Register_Map_t *p_i2c_x, uint8_t enable)
{
	if (enable == ENABLE)
	{
		switch ((uint32_t) p_i2c_x)
		{
		case I2C1_BASE_ADDR:
			I2C1_PCLK_EN();
			break;
		case I2C2_BASE_ADDR:
			I2C2_PCLK_EN();
			break;
		case I2C3_BASE_ADDR:
			I2C3_PCLK_EN();
			break;
		}
	}
	if (enable == DISABLE)
	{
		switch ((uint32_t) p_i2c_x)
		{
		case I2C1_BASE_ADDR:
			I2C1_PCLK_DI();
			break;
		case I2C2_BASE_ADDR:
			I2C2_PCLK_DI();
			break;
		case I2C3_BASE_ADDR:
			I2C3_PCLK_DI();
			break;
		}
	}
}

void I2C_Init(I2C_Handle_t *p_i2c_handle)
{
	// enable peripheral clock
	I2C_Peri_Clk_Ctrl(p_i2c_handle->p_i2c_x, ENABLE);

	// configure frequency and CCR
	I2C_Configure_Clock_Registers(p_i2c_handle);

	// CR1 AND CR2 CONFIGURATION
	I2C_Set_Own_Address(p_i2c_handle);

	// Enable peripheral
	I2C_Peripheral_Power_Switch(p_i2c_handle->p_i2c_x, ON);

	// Ack bit must be set after peripheral is enabled
	if (p_i2c_handle->i2c_config.ack_ctrl == I2C_ACK_EN)
	{
		I2C_Ack_Control(p_i2c_handle->p_i2c_x, ENABLE);
	}
	else
	{
		I2C_Ack_Control(p_i2c_handle->p_i2c_x, DISABLE);
	}
}

void I2C_Peripheral_Power_Switch(I2C_Register_Map_t *p_i2c_x, uint8_t on_or_off)
{
	SET_BIT(&p_i2c_x->CR1, I2C_CR1_PE_MASK);
}

void I2C_Master_Send(I2C_Handle_t *p_i2c_handle, uint8_t *p_tx_buffer, uint32_t len, uint8_t slave_addr, uint8_t sr)
{
	// MASTER TRANSMISSION
	// sequence diagram for master transmission is on page 849 of the board reference manual
	// 1) generate start condition
	I2C_Generate_Start_Condition(p_i2c_handle);

	// 2) EV5 (not an interrupt in this blocking API). Start Bit (SB) in SR1
	// check SB flag in SR1 to clear EV5
	while (!I2C_Check_Status_Flag(p_i2c_handle, I2C_SR1_SB, I2C_SR1_CHECK));

	// 3) After receiving ACK from slave, event EV6. ADDR bit set high (meaning address was matched)
	// check ADDR flag in SR1, then check TRA flag in SR2 to verify we are configured as transmitter
	I2C_Write_Address_Byte(p_i2c_handle, slave_addr, I2C_WRITE);
	while (!I2C_Check_Status_Flag(p_i2c_handle, I2C_SR1_ADDR, I2C_SR1_CHECK));

	if (!I2C_Check_Status_Flag(p_i2c_handle, I2C_SR2_TRA, I2C_SR2_CHECK))
	{
		// we are configured as receiver, which is wrong. just return
		// return;
	}

	while (len > 0)
	{
		// 4) After EV6 event cleared, EV8_1 event triggers immediately, meaning TxE = 1, transmit buffer is empty
		// wait for TxE to be set to 1, indicating EV8_1 event has triggered. write data to DR
		while(!I2C_Check_Status_Flag(p_i2c_handle, I2C_SR1_TXE, I2C_SR1_CHECK));
		p_i2c_handle->p_i2c_x->DR = *p_tx_buffer;
		p_tx_buffer++;
		len--;
	}

	// 6) After every byte has been sent, generate the stop condition
	// once length becomes 0, wait for txe=1 and btf=1, then generate stop condition
	while (!I2C_Check_Status_Flag(p_i2c_handle, I2C_SR1_TXE, I2C_SR1_CHECK));
	while (!I2C_Check_Status_Flag(p_i2c_handle, I2C_SR1_BTF, I2C_SR1_CHECK));
	if (sr == I2C_DISABLE_SR)
		I2C_Generate_Stop_Condition(p_i2c_handle);
}


void I2C_Master_Receive(I2C_Handle_t *p_i2c_handle, uint8_t *p_rx_buffer, uint32_t len, uint8_t slave_addr, uint8_t sr)
{
	// MASTER RECEIVER
	// 1) Generate start condition (same as master send..)
	I2C_Generate_Start_Condition(p_i2c_handle);

	// 2) Wait for SB to indicate start condition created
	while (!I2C_Check_Status_Flag(p_i2c_handle, I2C_SR1_SB, I2C_SR1_CHECK));

	// 3) Write slave address to DR with read bit (last bit 1)
	I2C_Write_Address_Byte(p_i2c_handle, slave_addr, I2C_READ);
	// wait for ADDR bit to set high. need to check SR2 as well (ADDR is in SR1)
	while (!I2C_Check_Status_Flag(p_i2c_handle, I2C_SR1_ADDR, I2C_SR1_CHECK));
	if (!I2C_Check_Status_Flag(p_i2c_handle, I2C_SR2_TRA, I2C_SR2_CHECK)) {}

	if (len == 1)
	{
		// NACK must be sent on first byte
		I2C_Ack_Control(p_i2c_handle->p_i2c_x, DISABLE);
		while (!I2C_Check_Status_Flag(p_i2c_handle, I2C_SR1_RXNE, I2C_SR1_CHECK));
		I2C_Generate_Stop_Condition(p_i2c_handle);
		*p_rx_buffer = p_i2c_handle->p_i2c_x->DR;
	}
	else
	{
		// 4) Wait for RxNE equal 1, meaning DR is full
		while (len > 0)
		{
			while (!I2C_Check_Status_Flag(p_i2c_handle, I2C_SR1_RXNE, I2C_SR1_CHECK));
			if (len == 2)
			{
				// last byte when len = 1 must be NACK'd, so ACK must be disabled
				I2C_Ack_Control(p_i2c_handle->p_i2c_x, DISABLE);
				I2C_Generate_Stop_Condition(p_i2c_handle);
			}
			*p_rx_buffer = p_i2c_handle->p_i2c_x->DR;
			p_rx_buffer++;
			len--;
		}
	}

	if (p_i2c_handle->i2c_config.ack_ctrl == I2C_ACK_EN)
	{
		I2C_Ack_Control(p_i2c_handle->p_i2c_x, ENABLE);
	}
}

void I2C_Generate_Start_Condition(I2C_Handle_t *p_i2c_handle)
{
	SET_BIT(&p_i2c_handle->p_i2c_x->CR1, I2C_CR1_START_MASK);
}

void I2C_Generate_Stop_Condition(I2C_Handle_t *p_i2c_handle)
{
	SET_BIT(&p_i2c_handle->p_i2c_x->CR1, I2C_CR1_STOP_MASK);
}

void I2C_Enable_Interrupts(I2C_Handle_t *p_i2c_handle)
{
	// determine which ISER register (0-7) will be used
	uint8_t iser_num;
	uint8_t bit_pos;

	switch ((uint32_t) p_i2c_handle->p_i2c_x)
	{
	case I2C1_BASE_ADDR:
		iser_num = I2C1_EV_NVIC_POS / 32;
		bit_pos = I2C1_EV_NVIC_POS % 32;
		break;
	case I2C2_BASE_ADDR:
		iser_num = I2C2_EV_NVIC_POS / 32;
		bit_pos = I2C2_EV_NVIC_POS % 32;
		break;
	case I2C3_BASE_ADDR:
		iser_num = I2C3_EV_NVIC_POS / 32;
		bit_pos = I2C3_EV_NVIC_POS % 32;
		break;
	}

	switch (iser_num)
	{
	case 0:
		NVIC_ISER_0 |= (1 << bit_pos);
		break;
	case 1:
		NVIC_ISER_1 |= (1 << bit_pos);
		break;
	case 2:
		NVIC_ISER_2 |= (1 << bit_pos);
		break;
	case 3:
		NVIC_ISER_3 |= (1 << bit_pos);
		break;
	}
}

/*
void I2C_Cleanup(I2C_Register_t *p_i2c_x);
*/

/*************** PRIVATE UTILITY FUNCTIONS *****************/


static uint8_t I2C_Check_Status_Flag(I2C_Handle_t *p_i2c_handle, uint8_t flag_num, uint8_t sr_1_or_2)
{
	// 0 = SR1, 1 = SR2
	if (!sr_1_or_2)
		return GET_BIT(&p_i2c_handle->p_i2c_x->SR1, (1 << flag_num));
	else
		return GET_BIT(&p_i2c_handle->p_i2c_x->SR2, (1 << flag_num));
}

static void I2C_Write_Address_Byte(I2C_Handle_t *p_i2c_handle, uint8_t slave_addr, uint8_t read_or_write)
{
	slave_addr <<= 1;
	// 0 for write, 1 for read
	if (!read_or_write)
		slave_addr &= ~1;
	else
		slave_addr |= 1;
	p_i2c_handle->p_i2c_x->DR = slave_addr;
}

static void I2C_Ack_Control(I2C_Register_Map_t *p_i2c_x, uint8_t enable)
{
	if (enable == ENABLE)
		SET_BIT(&p_i2c_x->CR1, I2C_CR1_ACK_MASK);
	else
		CLEAR_BIT(&p_i2c_x->CR1, I2C_CR1_ACK_MASK);
}

static void I2C_Configure_Clock_Registers(I2C_Handle_t *p_i2c_handle)
{
	uint32_t sys_clk_freq = RCC_Get_Sys_Clk_Frequency();

	// set FREQ field in I2C_CR2
	I2C_Set_CR2_Freq(p_i2c_handle->p_i2c_x, sys_clk_freq);

	if (p_i2c_handle->i2c_config.scl_speed <= I2C_SPEED_SM)
	{
		// cleared F/S bit means Standard Mode I2C
		CLEAR_BIT(&p_i2c_handle->p_i2c_x->CCR, (1 << I2C_CCR_FS));
		I2C_Set_CCR(p_i2c_handle, sys_clk_freq);
	}
	else
	{
		// Set F/S bit means Fast Mode
		SET_BIT(&p_i2c_handle->p_i2c_x->CCR, (1 << I2C_CCR_FS));
		I2C_Set_CCR(p_i2c_handle, sys_clk_freq);
	}

	I2C_Set_CCR(p_i2c_handle, sys_clk_freq);

	I2C_Configure_TRISE(p_i2c_handle, sys_clk_freq);
}

static void I2C_Set_CR2_Freq(I2C_Register_Map_t *p_i2c_x, uint32_t sys_clk_freq)
{
	// convert frequency from Hz to MHz
	sys_clk_freq /= 1000000u;
	// 5 bit frequency means maximum of 31
	if (sys_clk_freq > 31)
		return;
	// clear first 5 bits of CR2, then apply freq value
	SET_FIELD(&p_i2c_x->CR2, I2C_CR2_FREQ_MASK, sys_clk_freq);
}

static void I2C_Set_CCR(I2C_Handle_t *p_i2c_handle, uint32_t sys_clk_freq)
{
	// all I2C peripherals on APB1 - calculate APB1 frequency
	uint32_t ccr;
	uint32_t ahb_prescaler = RCC_Get_AHB_Prescaler();
	uint32_t apb1_prescaler = RCC_Get_APB_Prescaler();
	uint32_t apb1_clk = sys_clk_freq / ahb_prescaler / apb1_prescaler;

	if (p_i2c_handle->i2c_config.scl_speed <= I2C_SPEED_SM)
		ccr = apb1_clk / (I2C_SPEED_SM * 2);
	else
		ccr = apb1_clk / (I2C_SPEED_FM * 3);

	// clear bottom 12 bits, then set bottom 12 to calculated CCR
	SET_FIELD(&p_i2c_handle->p_i2c_x->CCR, I2C_CCR_CCR_MASK, ccr);
}

static void I2C_Configure_TRISE(I2C_Handle_t *p_i2c_handle, uint32_t sys_clk_freq)
{
	uint8_t trise;

	if (p_i2c_handle->i2c_config.scl_speed <= I2C_SPEED_SM)
	{
		// mode is standard - TRISE is (max rise time * apb1 clock) + 1
		// max rise time is 1000ns = 1e-6, simplifies to (apb1 clock / 1e6) + 1
		trise = (sys_clk_freq / 1000000u) + 1;
	}
	else
	{
		// mode is fast - max rise time is 300ns - simplifies to (apb1 clock * 3 / 1e7) + 1
		trise = ( (sys_clk_freq * 3) / 10000000u ) + 1;
	}

	// clear bottom 5 bits of I2C_TRISE register, then set to calculated value
	SET_FIELD(&p_i2c_handle->p_i2c_x->TRISE, I2C_TRISE_TRISE_MASK, trise);
}

static void I2C_Set_Own_Address(I2C_Handle_t *p_i2c_handle)
{
	// clear top bit of own address (in case user sent 8 bit address)
	p_i2c_handle->i2c_config.dev_addr &= ~(1 << 8);
	// clear 7-bit address field, then set address
	SET_FIELD(&p_i2c_handle->p_i2c_x->OAR1, I2C_OAR1_ADD_1_7_MASK, p_i2c_handle->i2c_config.dev_addr);
	// 14th bit must be kept high (unsure why)
	SET_BIT(&p_i2c_handle->p_i2c_x->OAR1, (1 << 14));
}

