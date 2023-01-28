/*
 * stm32f407xx_i2c_driver.h
 *
 *  Created on: Jan 27, 2023
 *      Author: Michael
 */

#ifndef STM32F407XX_I2C_DRIVER_H_
#define STM32F407XX_I2C_DRIVER_H_

#include "stm32f407xx.h"

typedef struct
{
	uint32_t		scl_speed;		// Clock speed
	uint8_t			dev_addr;		// Boards own device address
	uint8_t			ack_ctrl;		// Whether acknowledgment bit is enabled or disabled
	uint8_t			fm_duty_cycle;	//
} I2C_Config_t;

typedef struct
{
	I2C_Register_Map_t	*p_i2c_x;
	I2C_Config_t		i2c_config;
	uint8_t 			*p_tx_buffer; 	// Pointer to data being transmitted
	uint8_t 			*p_rx_buffer;	// Pointer to data to be received
	uint32_t 			tx_len;			// Length of data to be transmitted
	uint32_t 			rx_len;			// Length of data to be received
	uint8_t 			tx_rx_state;	// Whether board is transmitting or receiving
	uint8_t 			dev_addr;		// Slave device address
    uint32_t        	rx_size;		//
    uint8_t         	sr;				// Repeated start value
} I2C_Handle_t;

// Clock speed options
#define I2C_SPEED_SM				1000000		// 100kHz clock for standard mode
#define I2C_SPEED_FM				4000000		// 400kHz clock for fast mode

// Enable or disable acknowledge bit after data bit
#define I2C_ACK_EN        			1
#define I2C_ACK_DI			    	0

// 2:1 or 16:9 for Tlow:Thigh clock signal (only applicable to fast mode)
#define I2C_FM_DUTY_2        		0
#define I2C_FM_DUTY_16_9     		1

// Macros to check flags in status register 1
#define I2C_FLAG_TXE   				(1 << I2C_SR1_TXE)
#define I2C_FLAG_RXNE   			(1 << I2C_SR1_RXNE)
#define I2C_FLAG_SB					(1 << I2C_SR1_SB)
#define I2C_FLAG_OVR  				(1 << I2C_SR1_OVR)
#define I2C_FLAG_AF   				(1 << I2C_SR1_AF)
#define I2C_FLAG_ARLO 				(1 << I2C_SR1_ARLO)
#define I2C_FLAG_BERR 				(1 << I2C_SR1_BERR)
#define I2C_FLAG_STOPF 				(1 << I2C_SR1_STOPF)
#define I2C_FLAG_ADD10 				(1 << I2C_SR1_ADD10)
#define I2C_FLAG_BTF  				(1 << I2C_SR1_BTF)
#define I2C_FLAG_ADDR 				(1 << I2C_SR1_ADDR)
#define I2C_FLAG_TIMEOUT 			(1 << I2C_SR1_TIMEOUT)

typedef enum
{
	SYS_CLK_HSI,
	SYS_CLK_HSE,
	SYS_CLK_PLL,
	SYS_CLK_NA
} System_Clock_t;

void I2C_Peri_Clk_Ctrl(I2C_Register_Map_t *p_i2c_x, uint8_t enable);

void I2C_Init(I2C_Handle_t *p_i2c_handle);
/*
void I2C_Cleanup(I2C_Register_t *p_i2c_x);

void I2C_Peripheral_Power_Switch(I2C_Register_t *p_i2c_x, uint8_t on_or_off);
uint8_t I2C_Get_Flag_Status(I2C_Register_t *p_i2c_x, uint32_t flag_name);
void I2C_Manage_Acking(I2C_Register_t *p_i2c_x, uint8_t enable);

void I2C_MasterSend(I2C_Handle_t *p_i2c_handle, uint8_t *p_tx_buffer, uint32_t len, uint8_t slave_addr,uint8_t sr);
void I2C_MasterReceive(I2C_Handle_t *p_i2c_handle, uint8_t *p_rx_buffer, uint32_t len, uint8_t slave_addr);
*/
#endif /* STM32F407XX_I2C_DRIVER_H_ */
