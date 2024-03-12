#ifndef STM32F407XX_I2C_DRIVER_H_
#define STM32F407XX_I2C_DRIVER_H_

#include "stm32f407xx.h"
#include "i2c.h"

#define I2C_REG							I2C1
#define I2C_OWN_ADDR					12
/* Using I2C1 peripheral SDA over GPIOB, pin 7 */
#define SDA_GPIO						GPIOB
#define	SDA_PIN_NUM						GPIO_PIN_7
#define SDA_ALT_FUN						4
/* Using I2C1 peripheral SCL over GPIOB, pin 6 */
#define SCL_GPIO						GPIOB
#define SCL_PIN_NUM						GPIO_PIN_6
#define SDA_ALT_FUN						4

typedef struct
{
	I2C_Register_Map_t					*p_i2c_x;
	I2C_Device_t						i2c_dev;
} I2C_Handle_t;

#define I2C1_EV_NVIC_POS				31
#define I2C1_ER_NVIC_POS				32
#define I2C2_EV_NVIC_POS				33
#define I2C2_ER_NVIC_POS				34
#define I2C3_EV_NVIC_POS				72
#define I2C3_ER_NVIC_POS				73

// 2:1 or 16:9 for Tlow:Thigh clock signal (only applicable to fast mode)
#define I2C_FM_DUTY_2        			0
#define I2C_FM_DUTY_16_9     			1

#define I2C_SR1_CHECK					0
#define I2C_SR2_CHECK					1

/*************** RELEVANT BIT POSITIONS FOR I2C PERIPHERAL REGISTERS *****************/
/*************** I2C_CR1 bit positions and masks - General control register *****************/
#define I2C_CR1_PE_POS					0	// Peripheral enable; basically on/off switch
#define I2C_CR1_SMBUS_POS				1	// Enables SMBus mode, "stricter" version of I2C
#define I2C_CR1_SMBUS_TYPE_POS			3	// Either device or host SMBus mode
#define I2C_CR1_EN_ARP_POS				4	// Address Resolution Protocol, resolves device addresses
#define I2C_CR1_EN_PEC_POS				5	// Packet Error Checking, part of SMBus mode
#define I2C_CR1_EN_GC_POS				6	// General call, allows master to address all slaves
#define I2C_CR1_NO_STRETCH_POS 			7	// Disable clock stretching (enabled by default)
#define I2C_CR1_START_POS				8	// Generate start condition
#define I2C_CR1_STOP_POS 			 	9	// Generate stop condition
#define I2C_CR1_ACK_POS 			 	10	// Enables acknowledge bits; can only set when PE is 0
#define I2C_CR1_POS_POS					11	// Set or unset depending on whether byte is PEC
#define I2C_CR1_PEC_POS					12	// Set when PEC is transfered
#define I2C_CR1_ALERT_POS				13	// Allows devices to signal they require attention
#define I2C_CR1_SWRST_POS			 	15	// Software reset

typedef enum
{
	I2C_CR1_PE_MASK						= (0x1U << I2C_CR1_PE_POS),
	I2C_CR1_SMBUS_MASK					= (0x1U << I2C_CR1_SMBUS_POS),
	I2C_CR1_SMBUS_TYPE_MASK				= (0x1U << I2C_CR1_SMBUS_TYPE_POS),
	I2C_CR1_EN_ARP_MASK					= (0x1U << I2C_CR1_EN_ARP_POS),
	I2C_CR1_EN_PEC_MASK					= (0x1U << I2C_CR1_EN_PEC_POS),
	I2C_CR1_EN_GC_MASK					= (0x1U << I2C_CR1_EN_GC_POS),
	I2C_CR1_NO_STRETCH_MASK				= (0x1U << I2C_CR1_NO_STRETCH_POS),
	I2C_CR1_START_MASK					= (0x1U << I2C_CR1_START_POS),
	I2C_CR1_STOP_MASK					= (0x1U << I2C_CR1_STOP_POS),
	I2C_CR1_ACK_MASK					= (0x1U << I2C_CR1_ACK_POS),
	I2C_CR1_POS_MASK					= (0x1U << I2C_CR1_POS_POS),
	I2C_CR1_PEC_MASK					= (0x1U << I2C_CR1_PEC_POS),
	I2C_CR1_ALERT_MASK					= (0x1U << I2C_CR1_ALERT_POS),
	I2C_CR1_SWRST_MASK					= (0x1U << I2C_CR1_SWRST_POS)
} I2C_CR1_Mask_t;

/*************** I2C_CR2 bit positions and masks - General control register *****************/
#define I2C_CR2_FREQ_POS			 	0	// APB1 clock frequency value in MHz (6 bits, 0-5)
#define I2C_CR2_ITERREN_POS			 	8	// Error interrupt enable
#define I2C_CR2_ITEVTEN_POS			 	9	// Event interrupt enable
#define I2C_CR2_ITBUFEN_POS			    10	// Buffer interrupt enable; causes TxE and RxNE to generate interrupts
#define I2C_CR2_DMAEN_POS				11	// Enables Direct Memory Access requests
#define I2C_CR2_LAST_POS				12	// Whether the next DMA EOT is the last transfer

typedef enum
{
	I2C_CR2_FREQ_MASK					= (0x1FU << I2C_CR2_FREQ_POS),
	I2C_CR2_ITERREN_MASK				= (0x1U << I2C_CR2_ITERREN_POS),
	I2C_CR2_ITEVTEN_MASK				= (0x1U << I2C_CR2_ITEVTEN_POS),
	I2C_CR2_ITBUFEN_MASK				= (0x1U << I2C_CR2_ITBUFEN_POS),
	I2C_CR2_DMAEN_MASK					= (0x1U << I2C_CR2_DMAEN_POS),
	I2C_CR2_LAST_MASK					= (0x1U << I2C_CR2_LAST_POS)
} I2C_CR2_Mask_t;

/*************** I2C_OAR1 bit positions and masks - defines STM board's own I2C address *****************/
#define I2C_OAR1_ADD0_POS    		 	0	// Bit 0 of 10-bit address
#define I2C_OAR1_ADD_1_7_POS 		 	1	// Bits 1 through 7 of 7-bit address
#define I2C_OAR1_ADD_8_9_POS  		 	8	// Bits 8 and 9 of 10-bit address (not relevant for 7-bit)
#define I2C_OAR1_ADDMODE_POS   		 	15	// Addressing mode; 0 for 7-bit, 1 for 10-bit

typedef enum
{
	I2C_OAR1_ADD0_MASK					= (0x1U << I2C_OAR1_ADD0_POS),
	I2C_OAR1_ADD_1_7_MASK				= (0xEFU << I2C_OAR1_ADD_1_7_POS),
	I2C_OAR1_ADD_8_9_MASK				= (0x3U << I2C_OAR1_ADD_8_9_POS),
	I2C_OAR1_ADDMODE_MASK				= (0x1U << I2C_OAR1_ADDMODE_POS)
} I2C_OAR1_Mask_t;

/*************** I2C_OAR2 bit positions and masks - used for dual addressing mode *****************/
#define I2C_OAR2_ENDUAL_POS				0	// Enables dual addressing mode
#define I2C_OAR2_ADD2_POS				1	// Second 7-bit address

typedef enum
{
	I2C_OAR2_ENDUAL_MASK				= (0x1U << I2C_OAR2_ENDUAL_POS),
	I2C_OAR2_ADD2_MASK					= (0xEFU << I2C_OAR2_ADD2_POS)
} I2C_OAR2_Mask_t;

/*************** I2C_SR1 bit positions and masks - General status register *****************/
#define I2C_SR1_SB_POS 				 	0	// Start condition detected
#define I2C_SR1_ADDR_POS 			 	1	// Address sent (master) or matched (slave)
#define I2C_SR1_BTF_POS 				2	// Byte transfer finished
#define I2C_SR1_ADD10_POS 				3	// 10-bit header was sent
#define I2C_SR1_STOPF_POS 				4	// Stop condition detected
#define I2C_SR1_RXNE_POS 				6	// Receive not empty; received byte in DR
#define I2C_SR1_TXE_POS 				7	// Transmit empty; DR is empty during transmission
#define I2C_SR1_BERR_POS 				8	// Bus error; start or stop condition in invalid position
#define I2C_SR1_ARLO_POS 				9	// Arbitration lost; another master on bus
#define I2C_SR1_AF_POS 				 	10	// Acknowledge failure; no acknowledge received
#define I2C_SR1_OVR_POS 				11	// Over/underrun; incoming byte was lost or duplicate byte was sent
#define I2C_SR1_PEC_ERR_POS				12	// PEC error in reception
#define I2C_SR1_TIMEOUT_POS 			14	// Clock line was stretched low too long; master will generate stop condition
#define I2C_SR1_SMB_ALERT_POS			15	// SMB alert signal detected

typedef enum
{
	I2C_SR1_SB_MASK						= (0x1U << I2C_SR1_SB_POS),
	I2C_SR1_ADDR_MASK					= (0x1U << I2C_SR1_ADDR_POS),
	I2C_SR1_BTF_MASK					= (0x1U << I2C_SR1_BTF_POS),
	I2C_SR1_ADD10_MASK					= (0x1U << I2C_SR1_ADD10_POS),
	I2C_SR1_STOPF_MASK					= (0x1U << I2C_SR1_STOPF_POS),
	I2C_SR1_RXNE_MASK					= (0x1U << I2C_SR1_RXNE_POS),
	I2C_SR1_TXE_MASK					= (0x1U << I2C_SR1_TXE_POS),
	I2C_SR1_BERR_MASK					= (0x1U << I2C_SR1_BERR_POS),
	I2C_SR1_ARLO_MASK					= (0x1U << I2C_SR1_ARLO_POS),
	I2C_SR1_AF_MASK						= (0x1U << I2C_SR1_AF_POS),
	I2C_SR1_OVR_MASK					= (0x1U << I2C_SR1_OVR_POS),
	I2C_SR1_PEC_ERR_MASK				= (0x1U << I2C_SR1_PEC_ERR_POS),
	I2C_SR1_TIMEOUT_MASK				= (0x1U << I2C_SR1_TIMEOUT_POS),
	I2C_SR1_SMB_ALERT_MASK				= (0x1U << I2C_SR1_SMB_ALERT_POS),
} I2C_SR1_Mask_t;

/*************** I2C_SR2 bit positions and masks - General status register *****************/
#define I2C_SR2_MSL_POS					0	// Whether board is in master or slave mode
#define I2C_SR2_BUSY_POS 				1	// Bus busy
#define I2C_SR2_TRA_POS 				2	// Whether board is in transmit or receive
#define I2C_SR2_GENCALL_POS 			4	// General call received
#define I2C_SR2_SMB_DEF_POS				5	// SMBus device default address for slave mode
#define I2C_SR2_SMB_HOST_POS			6	// SMBus host address received for slave mode
#define I2C_SR2_DUALF_POS				7	// Received address match in dual address mode
#define I2C_SR2_PEC_POS					8	// Packet error checking register

typedef enum
{
	I2C_SR2_MSL_MASK					= (0x1 << I2C_SR2_MSL_POS),
	I2C_SR2_BUSY_MASK					= (0x1 << I2C_SR2_BUSY_POS),
	I2C_SR2_TRA_MASK					= (0x1 << I2C_SR2_TRA_POS),
	I2C_SR2_GENCALL_MASK				= (0x1 << I2C_SR2_GENCALL_POS),
	I2C_SR2_SMB_DEF_MASK				= (0x1 << I2C_SR2_SMB_DEF_POS),
	I2C_SR2_SMB_HOST_MASK				= (0x1 << I2C_SR2_SMB_HOST_POS),
	I2C_SR2_DUALF_MASK					= (0x1 << I2C_SR2_DUALF_POS),
	I2C_SR2_PEC_MASK					= (0xEF << I2C_SR2_PEC_POS)
} I2C_SR2_Mask_t;

/*************** I2C_CCR bit positions and masks - Clock control register *****************/
#define I2C_CCR_CCR_POS 				0	// Controls SCL clock in master mode; 12 bits, 0 to 11
#define I2C_CCR_DUTY_POS				14	// Fast mode clock signal duty cycle
#define I2C_CCR_FS_POS  			 	15	// Mode selection; 0 for standard, 1 for fast

typedef enum
{
	I2C_CCR_CCR_MASK					= (0xFFFU << I2C_CCR_CCR_POS),
	I2C_CCR_DUTY_MASK					= (0x1U << I2C_CCR_DUTY_POS),
	I2C_CCR_FS_MASK						= (0x1U << I2C_CCR_FS_POS)
} I2C_CCR_Mask_t;

/*************** I2C_TRISE bit positions and masks - Clock rise time register *****************/
#define I2C_TRISE_TRISE_POS				0	// Controls maximum clock rise time in master mode; 6 bits, 0 to 5

typedef enum
{
	I2C_TRISE_TRISE_MASK				= (0x1FU << I2C_TRISE_TRISE_POS),
} I2C_TRISE_Mask_t;

#endif /* STM32F407XX_I2C_DRIVER_H_ */
