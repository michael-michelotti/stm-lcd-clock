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

void I2C_Errata_Reset(I2C_Handle_t *p_i2c_handle)
{
	/**
	1. Disable the I2C peripheral by clearing the PE bit in I2Cx_CR1 register.
	2. Configure the SCL and SDA I/Os as General Purpose Output Open-Drain, High level
	(Write 1 to GPIOx_ODR).
	3. Check SCL and SDA High level in GPIOx_IDR.
	4. Configure the SDA I/O as General Purpose Output Open-Drain, Low level (Write 0 to
	GPIOx_ODR).
	5. Check SDA Low level in GPIOx_IDR.
	6. Configure the SCL I/O as General Purpose Output Open-Drain, Low level (Write 0 to
	GPIOx_ODR).
	7. Check SCL Low level in GPIOx_IDR.
	8. Configure the SCL I/O as General Purpose Output Open-Drain, High level (Write 1 to
	GPIOx_ODR).
	9. Check SCL High level in GPIOx_IDR.
	10. Configure the SDA I/O as General Purpose Output Open-Drain , High level (Write 1 to
	GPIOx_ODR).
	11. Check SDA High level in GPIOx_IDR.
	12. Configure the SCL and SDA I/Os as Alternate function Open-Drain.
	13. Set SWRST bit in I2Cx_CR1 register.
	14. Clear SWRST bit in I2Cx_CR1 register.
	15. Enable the I2C peripheral by setting the PE bit in I2Cx_CR1 register.
	**/
	// PB11 is SDA, PB10 is SCL
	I2C_Peripheral_Power_Switch(p_i2c_handle->p_i2c_x, OFF);
	uint8_t dummy_val = 0;

	GPIO_Pin_Config_t errata_config = {
			10,
			GPIO_MODE_OUT,
			GPIO_SPEED_HIGH,
			GPIO_PUPD_NONE,
			GPIO_OUT_OD,
			0
	};
	GPIO_Handle_t errata_handle = { GPIOB, errata_config };
	GPIO_Init(&errata_handle);
	errata_handle.gpio_pin_config.gpio_pin_num = 11;
	GPIO_Init(&errata_handle);
	GPIO_Write_To_Output_Pin(errata_handle.p_gpio_x, 10, 1);
	GPIO_Write_To_Output_Pin(errata_handle.p_gpio_x, 11, 1);
	dummy_val = GPIO_Read_From_Input_Pin(errata_handle.p_gpio_x, 10);
	dummy_val = GPIO_Read_From_Input_Pin(errata_handle.p_gpio_x, 11);

	GPIO_Write_To_Output_Pin(errata_handle.p_gpio_x, 11, 0);
	dummy_val = GPIO_Read_From_Input_Pin(errata_handle.p_gpio_x, 11);

	GPIO_Write_To_Output_Pin(errata_handle.p_gpio_x, 10, 0);
	dummy_val = GPIO_Read_From_Input_Pin(errata_handle.p_gpio_x, 10);

	GPIO_Write_To_Output_Pin(errata_handle.p_gpio_x, 10, 1);
	dummy_val = GPIO_Read_From_Input_Pin(errata_handle.p_gpio_x, 10);

	GPIO_Write_To_Output_Pin(errata_handle.p_gpio_x, 11, 1);
	dummy_val = GPIO_Read_From_Input_Pin(errata_handle.p_gpio_x, 11);

	errata_handle.gpio_pin_config.gpio_pin_mode = GPIO_MODE_ALT;
	errata_handle.gpio_pin_config.gpio_pin_alt_fun_mode = 4;
	GPIO_Init(&errata_handle);
	errata_handle.gpio_pin_config.gpio_pin_num = 10;
	GPIO_Init(&errata_handle);
	p_i2c_handle->p_i2c_x->CR1 |= (1 << I2C_CR1_SWRST);
	p_i2c_handle->p_i2c_x->CR1 &= ~(1 << I2C_CR1_SWRST);
	// I2C_Peripheral_Power_Switch(p_i2c_handle->p_i2c_x, ON);
}

void I2C_Init(I2C_Handle_t *p_i2c_handle)
{
	// CLOCK CONFIGURATION
	// enable peripheral clock
	I2C_Peri_Clk_Ctrl(p_i2c_handle->p_i2c_x, ENABLE);

	// I2C_Errata_Reset(p_i2c_handle);

	/*
	// try a SW reset - busy bit seems to stay set???
	p_i2c_handle->p_i2c_x->CR1 |= (1 << I2C_CR1_SWRST);
	while(p_i2c_handle->p_i2c_x->CR1 & (1 << I2C_CR1_SWRST));
	 */

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

static void I2C_Generate_Start_Condition(I2C_Handle_t *p_i2c_handle)
{
	p_i2c_handle->p_i2c_x->CR1 |= (1 << I2C_CR1_START);
}

static void I2C_Generate_Stop_Condition(I2C_Handle_t *p_i2c_handle)
{
	p_i2c_handle->p_i2c_x->CR1 |= (1 << I2C_CR1_STOP);
}

static uint8_t I2C_Check_Status_Flag(I2C_Handle_t *p_i2c_handle, uint8_t flag_num, uint8_t sr_1_or_2)
{
	// 0 = SR1, 1 = SR2
	if (!sr_1_or_2)
	{
		if (p_i2c_handle->p_i2c_x->SR1 & (1 << flag_num))
		{
			return SET;
		}
		else
		{
			return RESET;
		}
		// return ((p_i2c_handle->p_i2c_x->SR1 >> flag_num) & 1);
	}
	else
	{
		if (p_i2c_handle->p_i2c_x->SR2 & (1 << flag_num))
		{
			return SET;
		}
		else
		{
			return RESET;
		}
		// return (p_i2c_handle->p_i2c_x->SR2 >> flag_num) & 1;
	}
}

static void I2C_Write_Address_Byte(I2C_Handle_t *p_i2c_handle, uint8_t slave_addr, uint8_t read_or_write)
{
	slave_addr <<= 1;
	// 0 for write, 1 for read
	if (!read_or_write)
	{
		slave_addr &= ~1;
	}
	else
	{
		slave_addr |= 1;
	}
	p_i2c_handle->p_i2c_x->DR = slave_addr;
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
	I2C_Generate_Stop_Condition(p_i2c_handle);
}


/*
void I2C_Cleanup(I2C_Register_t *p_i2c_x);


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

	Set_I2C_CCR(p_i2c_handle, pclk_freq);

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

	// 14th bit must be kept high
	p_i2c_handle->p_i2c_x->OAR1 |= (1 << 14);
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

