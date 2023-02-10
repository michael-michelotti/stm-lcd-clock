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

/*************** RELEVANT BIT POSITIONS FOR I2C PERIPHERAL REGISTERS *****************/
// I2C_CR1 bit positions - General control register
#define I2C_CR1_PE						0		// Peripheral enable; basically on/off switch
#define I2C_CR1_NO_STRETCH  			7		// Disable clock stretching (enabled by default)
#define I2C_CR1_START 					8		// Generate start condition
#define I2C_CR1_STOP  				 	9		// Generate stop condition
#define I2C_CR1_ACK 				 	10		// Enables acknowledge bits; can only set when PE is 0
#define I2C_CR1_SWRST  				 	15		// Software reset

typedef enum
{
	I2C_CR1_PE_MASK							= 0x0001U,
	I2C_CR1_NO_STRETCH_MASK					= 0x0080U,
	I2C_CR1_START_MASK						= 0x0100U,
	I2C_CR1_STOP_MASK						= 0x0200U,
	I2C_CR1_ACK_MASK						= 0x0400U,
	I2C_CR1_SWRST_MASK						= 0x8000U
} I2C_CR1_Mask_t;

// I2C_CR2 bit positions - General control register
typedef enum
{
	I2C_CR2_FREQ_MASK						= 0x001FU,
	I2C_CR2_ITERREN_MASK					= 0x0100U,
	I2C_CR2_ITEVTEN_MASK					= 0x0200U,
	I2C_CR2_ITBUFEN_MASK					= 0x0400U
} I2C_CR2_Mask_t;

#define I2C_CR2_FREQ				 	0		// APB1 clock frequency value in MHz (6 bits, 0-5)
#define I2C_CR2_ITERREN				 	8		// Error interrupt enable
#define I2C_CR2_ITEVTEN				 	9		// Event interrupt enable
#define I2C_CR2_ITBUFEN 			    10		// Buffer interrupt enable; causes TxE and RxNE to generate interrupts

// I2C_OAR1 bit positions - defines STM board's own I2C address
typedef enum
{
	I2C_OAR1_ADD0_MASK						= 0x0001U,
	I2C_OAR1_ADD_1_7_MASK					= 0x00FEU,
	I2C_OAR1_ADD_8_9_MASK					= 0x0300U,
	I2C_OAR1_ADDMODE_MASK					= 0x8000U
} I2C_OAR1_Mask_t;

#define I2C_OAR1_ADD0    				 0		// Bit 0 of 10-bit address
#define I2C_OAR1_ADD_1_7 				 1		// Bits 1 through 7 of 7-bit address
#define I2C_OAR1_ADD_8_9  			 	 8		// Bits 8 and 9 of 10-bit address (not relevant for 7-bit)
#define I2C_OAR1_ADDMODE   			 	15		// Addressing mode; 0 for 7-bit, 1 for 10-bit

// Do not need OAR2, no dual addressing in this project

// I2C_SR1 bit positions - General status register
typedef enum
{
	I2C_SR1_SB_MASK						= 0x0001U,
	I2C_SR1_ADDR_MASK					= 0x0002U,
	I2C_SR1_BTF_MASK					= 0x0004U,
	I2C_SR1_ADD10_MASK					= 0x0008U,
	I2C_SR1_STOPF_MASK					= 0x0010U,
	I2C_SR1_RXNE_MASK					= 0x0040U,
	I2C_SR1_TXE_MASK					= 0x0080U,
	I2C_SR1_BERR_MASK					= 0x0100U,
	I2C_SR1_ARLO_MASK					= 0x0200U,
	I2C_SR1_AF_MASK						= 0x0400U,
	I2C_SR1_OVR_MASK					= 0x0800U,
	I2C_SR1_TIMEOUT_MASK				= 0x4000U
} I2C_SR1_Mask_t;

#define I2C_SR1_SB 					 	0		// Start condition detected
#define I2C_SR1_ADDR 				 	1		// Address sent (master) or matched (slave)
#define I2C_SR1_BTF 					2		// Byte transfer finished
#define I2C_SR1_ADD10 					3		// 10-bit header was sent
#define I2C_SR1_STOPF 					4		// Stop condition detected
#define I2C_SR1_RXNE 					6		// Receive not empty; received byte in DR
#define I2C_SR1_TXE 					7		// Transmit empty; DR is empty during transmission
#define I2C_SR1_BERR 					8		// Bus error; start or stop condition in invalid position
#define I2C_SR1_ARLO 					9		// Arbitration lost; another master on bus
#define I2C_SR1_AF 					 	10		// Acknowledge failure; no acknowledge received
#define I2C_SR1_OVR 					11		// Over/underrun; incoming byte was lost or duplicate byte was sent
#define I2C_SR1_TIMEOUT 				14		// Clock line was stretched low too long; master will generate stop condition

#define I2C_SR1_CHECK					0
#define I2C_SR2_CHECK					1

// I2C_SR2 bit positions - General status register
typedef enum
{
	I2C_SR2_MSL_MASK					= 0x0001U,
	I2C_SR2_BUSY_MASK					= 0x0002U,
	I2C_SR2_TRA_MASK					= 0x0004U,
	I2C_SR2_GENCALL_MASK				= 0x0010U,
	I2C_SR2_DUALF_MASK					= 0x0080U
} I2C_SR2_Mask_t;

#define I2C_SR2_MSL						0		// Whether board is in master or slave mode
#define I2C_SR2_BUSY 					1		// Bus busy
#define I2C_SR2_TRA 					2		// Whether board is in transmit or receive
#define I2C_SR2_GENCALL 				4		// General call received
#define I2C_SR2_DUALF 					7		// Received address match in dual address mode

// I2C_CCR - Clock control register
typedef enum
{
	I2C_CCR_CCR_MASK					= 0x0FFFU,
	I2C_CCR_DUTY_MASK					= 0x4000U,
	I2C_CCR_FS_MASK						= 0x8000U
} I2C_CCR_Mask_t;

#define I2C_CCR_CCR 					0		// Controls SCL clock in master mode; 12 bits, 0 to 11
#define I2C_CCR_DUTY 					14		// Fast mode clock signal duty cycle
#define I2C_CCR_FS  				 	15		// Mode selection; 0 for standard, 1 for fast

// I2C_TRISE - Clock rise time register
typedef enum
{
	I2C_TRISE_TRISE_MASK				= 0x001FU,
} I2C_TRISE_Mask_t;

#define I2C_TRISE_TRISE 				0		// Controls maximum clock rise time in master mode; 6 bits, 0 to 5

// Clock speed options
#define I2C_SPEED_SM				100000		// 100kHz clock for standard mode
#define I2C_SPEED_FM				400000		// 400kHz clock for fast mode

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
	I2C_DISABLE_SR,
	I2C_ENABLE_SR
} Repeated_Start_Enable_t;

typedef enum
{
	SYS_CLK_HSI,
	SYS_CLK_HSE,
	SYS_CLK_PLL,
	SYS_CLK_NA
} System_Clock_t;

void I2C_Peri_Clk_Ctrl(I2C_Register_Map_t *p_i2c_x, uint8_t enable);
void I2C_Init(I2C_Handle_t *p_i2c_handle);
void I2C_Peripheral_Power_Switch(I2C_Register_Map_t *p_i2c_x, uint8_t on_or_off);
void I2C_Master_Send(I2C_Handle_t *p_i2c_handle, uint8_t *p_tx_buffer, uint32_t len, uint8_t slave_addr,uint8_t sr);
void I2C_Master_Receive(I2C_Handle_t *p_i2c_handle, uint8_t *p_rx_buffer, uint32_t len, uint8_t slave_addr, uint8_t sr);

void I2C_Generate_Start_Condition(I2C_Handle_t *p_i2c_handle);
void I2C_Generate_Stop_Condition(I2C_Handle_t *p_i2c_handle);
/*
void I2C_Cleanup(I2C_Register_t *p_i2c_x);
*/
#endif /* STM32F407XX_I2C_DRIVER_H_ */
