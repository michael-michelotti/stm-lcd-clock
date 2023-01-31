/*
 * stm32f407xx.h
 *
 *  Created on: Jan 27, 2023
 *      Author: Michael
 */

#ifndef STM32F407XX_H_
#define STM32F407XX_H_

// Includes (should only include files with include guards)
#include <stdint.h>

// Generic macros
#define __weak 			__attribute__((weak))
#define ENABLE 			1
#define DISABLE 		0
#define ON				1
#define OFF				0
#define SET				ENABLE
#define RESET			DISABLE
#define RISING			0
#define FALLING			1
#define NO_PRIORITY_BITS_IMPLEMENTED	4

#define HSI_CLK_SPEED	16000000u


/*************** MEMORY ADDRESSES *****************/
// Major memory segment addresses
#define FLASH_BASE_ADDR 		0x08000000u
#define SRAM1_BASE_ADDR 		0x20000000u
#define SRAM2_BASE_ADDR 		0x20001C00u
#define ROM						0x1FFF0000u
#define SRAM 					SRAM1_BASE_ADDR

// Nested Vector Interrupt Controller, Interrupt enabling (ISER) and disabling (ICER)
#define NVIC_ISER_0				((volatile uint32_t *)0xE000E100)
#define NVIC_ISER_1				((volatile uint32_t *)0xE000E104)
#define NVIC_ISER_2				((volatile uint32_t *)0xE000E108)
#define NVIC_ISER_3				((volatile uint32_t *)0xE000E10c)

#define NVIC_ICER_0				((volatile uint32_t *)0xE000E180)
#define NVIC_ICER_1				((volatile uint32_t *)0xE000E184)
#define NVIC_ICER_2				((volatile uint32_t *)0xE000E188)
#define NVIC_ICER_3				((volatile uint32_t *)0xE000E18c)

#define NVIC_IPR_0				((volatile uint32_t *)0xE000E400)

// Peripheral bus base addresses
#define PERIPH_BASE 			0x40000000u
#define APB1_PERIPH_BASE		PERIPH_BASE
#define APB2_PERIPH_BASE		0x40010000u
#define AHB1_PERIPH_BASE		0x40020000u
#define AHB2_PERIPH_BASE		0x50000000u

// Peripherals hanging on APB1
#define I2C1_BASE_ADDR			( APB1_PERIPH_BASE + 0x5400 )
#define I2C2_BASE_ADDR			( APB1_PERIPH_BASE + 0x5800 )
#define I2C3_BASE_ADDR			( APB1_PERIPH_BASE + 0x5C00 )

// Peripherals hanging on AHB1
#define GPIOA_BASE_ADDR			( AHB1_PERIPH_BASE + 0x0000 )
#define GPIOB_BASE_ADDR			( AHB1_PERIPH_BASE + 0x0400 )
#define GPIOC_BASE_ADDR			( AHB1_PERIPH_BASE + 0x0800 )
#define GPIOD_BASE_ADDR			( AHB1_PERIPH_BASE + 0x0C00 )
#define GPIOE_BASE_ADDR			( AHB1_PERIPH_BASE + 0x1000 )
#define GPIOF_BASE_ADDR			( AHB1_PERIPH_BASE + 0x1400 )
#define GPIOG_BASE_ADDR			( AHB1_PERIPH_BASE + 0x1800 )
#define GPIOH_BASE_ADDR			( AHB1_PERIPH_BASE + 0x1C00 )
#define GPIOI_BASE_ADDR			( AHB1_PERIPH_BASE + 0x2000 )
#define RCC_BASE_ADDR 			( AHB1_PERIPH_BASE + 0x3800 )

// Miscellaneous peripherals
#define EXTI_BASE_ADDR			( APB2_PERIPH_BASE + 0x3C00 )
#define SYSCFG_BASE_ADDR		( APB2_PERIPH_BASE + 0x3800 )

// Vector table position for various EXTI lines (tied to the GPIO pins)
#define IRQ_NO_EXTI0			6
#define IRQ_NO_EXTI1			7
#define IRQ_NO_EXTI2			8
#define IRQ_NO_EXTI3			9
#define IRQ_NO_EXTI4			10
#define IRQ_NO_EXTI5_9			23
#define IRQ_NO_EXTI10_15		40

/*************** REGISTER DEFINITIONS *****************/
typedef struct
{
	volatile uint32_t MODER;
	volatile uint32_t OTYPER;
	volatile uint32_t OSPEEDR;
	volatile uint32_t PUPDR;
	volatile uint32_t IDR;
	volatile uint32_t ODR;
	volatile uint32_t BSRR;
	volatile uint32_t LCKR;
	volatile uint32_t AFRL;
	volatile uint32_t AFRH;
} GPIO_Register_Map_t;

typedef struct
{
	volatile uint32_t CR;
	volatile uint32_t PLLCFG;
	volatile uint32_t CFGR;
	volatile uint32_t CIR;
	volatile uint32_t AHB1RSTR;
	volatile uint32_t AHB2RSTR;
	volatile uint32_t AHB3RSTR;
	volatile uint32_t RESERVED_1;
	volatile uint32_t APB1RSTR;
	volatile uint32_t APB2RSTR;
	volatile uint32_t RESERVED_2;
	volatile uint32_t RESERVED_3;
	volatile uint32_t AHB1ENR;
	volatile uint32_t AHB2ENR;
	volatile uint32_t AHB3ENR;
	volatile uint32_t RESERVED_4;
	volatile uint32_t APB1ENR;
	volatile uint32_t APB2ENR;
	volatile uint32_t RESERVED_5;
	volatile uint32_t RESERVED_6;
	volatile uint32_t AHB1LPENR;
	volatile uint32_t AHB2LPENR;
	volatile uint32_t AHB3LPENR;
	volatile uint32_t RESERVED_7;
	volatile uint32_t APB1LPENR;
	volatile uint32_t APB2LPENR;
	volatile uint32_t RESERVED_8;
	volatile uint32_t RESERVED_9;
	volatile uint32_t BDCR;
	volatile uint32_t CSR;
	volatile uint32_t RESERVED_10;
	volatile uint32_t RESERVED_11;
	volatile uint32_t SSCGR;
	volatile uint32_t PLLI2SCFGR;
	volatile uint32_t PLLSAICFGR;
} RCC_Register_Map_t;

typedef struct
{
	volatile uint32_t IMR; 					// Interrupt mask
	volatile uint32_t EMR;					// Event mask
	volatile uint32_t RTSR;					// Rising trigger selection
	volatile uint32_t FTSR;					// Falling trigger selection
	volatile uint32_t SWIEr;				// Software interrupt event
	volatile uint32_t PR;					// Pending register
} EXTI_Register_Map_t;

typedef struct
{
	volatile uint32_t MEMRMP;				// Memory remap
	volatile uint32_t PMC;					// Peripheral mode configuration
	volatile uint32_t EXTICR[4];			// External interrupt configuration registers 1-4
	volatile uint32_t RESERVED_1;
	volatile uint32_t RESERVED_2;
	volatile uint32_t CMPCR;				// Compensation cell control
} SYSCFG_Register_Map_t;

typedef struct
{
	volatile uint32_t CR1;					// Control register 1
	volatile uint32_t CR2;					// Control register 2
	volatile uint32_t OAR1;					// Own address register 1
	volatile uint32_t OAR2;					// Own address register 2
	volatile uint32_t DR;					// Data register
	volatile uint32_t SR1;					// Status register 1
	volatile uint32_t SR2;					// Status register 2
	volatile uint32_t CCR;					// Clock control register
	volatile uint32_t TRISE;				// TRISE register
	volatile uint32_t FLTR;					// FLTR register
} I2C_Register_Map_t;

/*************** POINTER MACROS *****************/
#define GPIOA 		( (GPIO_Register_Map_t*) GPIOA_BASE_ADDR )
#define GPIOB 		( (GPIO_Register_Map_t*) GPIOB_BASE_ADDR )
#define GPIOC 		( (GPIO_Register_Map_t*) GPIOC_BASE_ADDR )
#define GPIOD 		( (GPIO_Register_Map_t*) GPIOD_BASE_ADDR )
#define GPIOE 		( (GPIO_Register_Map_t*) GPIOE_BASE_ADDR )
#define GPIOF 		( (GPIO_Register_Map_t*) GPIOF_BASE_ADDR )
#define GPIOG 		( (GPIO_Register_Map_t*) GPIOG_BASE_ADDR )
#define GPIOH 		( (GPIO_Register_Map_t*) GPIOH_BASE_ADDR )
#define GPIOI 		( (GPIO_Register_Map_t*) GPIOI_BASE_ADDR )

#define RCC 		( (RCC_Register_Map_t*) RCC_BASE_ADDR )
#define EXTI		( (EXTI_Register_Map_t*) EXTI_BASE_ADDR )
#define SYSCFG		( (SYSCFG_Register_Map_t*) SYSCFG_BASE_ADDR )

#define I2C1		( (I2C_Register_Map_t*) I2C1_BASE_ADDR )
#define I2C2		( (I2C_Register_Map_t*) I2C2_BASE_ADDR )
#define I2C3		( (I2C_Register_Map_t*) I2C3_BASE_ADDR )


/*************** RELEVANT BIT POSITIONS FOR I2C PERIPHERAL REGISTERS *****************/
// I2C_CR1 bit positions - General control register
#define I2C_CR1_PE						0		// Peripheral enable; basically on/off switch
#define I2C_CR1_NO_STRETCH  			7		// Disable clock stretching (enabled by default)
#define I2C_CR1_START 					8		// Generate start condition
#define I2C_CR1_STOP  				 	9		// Generate stop condition
#define I2C_CR1_ACK 				 	10		// Enables acknowledge bits; can only set when PE is 0
#define I2C_CR1_SWRST  				 	15		// Software reset

// I2C_CR2 bit positions - General control register
#define I2C_CR2_FREQ				 	0		// APB1 clock frequency value in MHz (6 bits, 0-5)
#define I2C_CR2_ITERREN				 	8		// Error interrupt enable
#define I2C_CR2_ITEVTEN				 	9		// Event interrupt enable
#define I2C_CR2_ITBUFEN 			    10		// Buffer interrupt enable; causes TxE and RxNE to generate interrupts

// I2C_OAR1 bit positions - defines STM board's own I2C address
#define I2C_OAR1_ADD0    				 0		// Bit 0 of 10-bit address
#define I2C_OAR1_ADD_1_7 				 1		// Bits 1 through 7 of 7-bit address
#define I2C_OAR1_ADD_8_9  			 	 8		// Bits 8 and 9 of 10-bit address (not relevant for 7-bit)
#define I2C_OAR1_ADDMODE   			 	15		// Addressing mode; 0 for 7-bit, 1 for 10-bit

// Do not need OAR2, no dual addressing in this project

// I2C_SR1 bit positions - General status register
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

// I2C_SR2 bit positions - General status register
#define I2C_SR2_MSL						0		// Whether board is in master or slave mode
#define I2C_SR2_BUSY 					1		// Bus busy
#define I2C_SR2_TRA 					2		// Whether board is in transmit or receive
#define I2C_SR2_GENCALL 				4		// General call received
#define I2C_SR2_DUALF 					7		// Received address match in dual address mode

// I2C_CCR - Clock control register
#define I2C_CCR_CCR 					0		// Controls SCL clock in master mode; 12 bits, 0 to 11
#define I2C_CCR_DUTY 					14		// Fast mode clock signal duty cycle
#define I2C_CCR_FS  				 	15		// Mode selection; 0 for standard, 1 for fast

// I2C_TRISE - Clock rise time register
#define I2C_TRISE_TRISE 				0		// Controls maximum clock rise time in master mode; 6 bits, 0 to 5

// Do not need FLTR register - will not alter digital noise filter

/*************** CLOCK ENABLE/DISABLE/RESET MACROS *****************/
// Enable GPIO clocks
#define GPIOA_PCLK_EN() 		( RCC->AHB1ENR |= ( 1 << 0 ) )
#define GPIOB_PCLK_EN() 		( RCC->AHB1ENR |= ( 1 << 1 ) )
#define GPIOC_PCLK_EN() 		( RCC->AHB1ENR |= ( 1 << 2 ) )
#define GPIOD_PCLK_EN() 		( RCC->AHB1ENR |= ( 1 << 3 ) )
#define GPIOE_PCLK_EN() 		( RCC->AHB1ENR |= ( 1 << 4 ) )
#define GPIOF_PCLK_EN() 		( RCC->AHB1ENR |= ( 1 << 5 ) )
#define GPIOG_PCLK_EN() 		( RCC->AHB1ENR |= ( 1 << 6 ) )
#define GPIOH_PCLK_EN() 		( RCC->AHB1ENR |= ( 1 << 7 ) )
#define GPIOI_PCLK_EN() 		( RCC->AHB1ENR |= ( 1 << 8 ) )
// Disable GPIO clocks
#define GPIOA_PCLK_DI() 		( RCC->AHB1ENR &= ~( 1 << 0 ) )
#define GPIOB_PCLK_DI() 		( RCC->AHB1ENR &= ~( 1 << 1 ) )
#define GPIOC_PCLK_DI() 		( RCC->AHB1ENR &= ~( 1 << 2 ) )
#define GPIOD_PCLK_DI() 		( RCC->AHB1ENR &= ~( 1 << 3 ) )
#define GPIOE_PCLK_DI() 		( RCC->AHB1ENR &= ~( 1 << 4 ) )
#define GPIOF_PCLK_DI() 		( RCC->AHB1ENR &= ~( 1 << 5 ) )
#define GPIOG_PCLK_DI() 		( RCC->AHB1ENR &= ~( 1 << 6 ) )
#define GPIOH_PCLK_DI() 		( RCC->AHB1ENR &= ~( 1 << 7 ) )
#define GPIOI_PCLK_DI() 		( RCC->AHB1ENR &= ~( 1 << 8 ) )
// Enable I2C clocks
#define I2C1_PCLK_EN() 			( RCC->APB1ENR |= ( 1 << 21 ) )
#define I2C2_PCLK_EN() 			( RCC->APB1ENR |= ( 1 << 22 ) )
#define I2C3_PCLK_EN() 			( RCC->APB1ENR |= ( 1 << 23 ) )
// Disable I2C clocks
#define I2C1_PCLK_DI() 			( RCC->APB1ENR &= ~( 1 << 21 ) )
#define I2C2_PCLK_DI() 			( RCC->APB1ENR &= ~( 1 << 22 ) )
#define I2C3_PCLK_DI() 			( RCC->APB1ENR &= ~( 1 << 23 ) )
// Reset I2C peripherals
#define I2C1_PCLK_RST() 		( RCC->APB1RSTR |= ( 1 << 21 ) )
#define I2C2_PCLK_RST() 		( RCC->APB1RSTR |= ( 1 << 22 ) )
#define I2C3_PCLK_RST() 		( RCC->APB1RSTR |= ( 1 << 23 ) )

#define SYSCFG_PCLK_EN()		( RCC->APB2ENR |= ( 1 << 14 ) )

/* Macros to reset GPIO peripherals */
#define GPIOA_RESET()			do { (RCC->AHB1RSTR |= (1 << 0)); (RCC->AHB1RSTR &= ~(1 << 0)); } while(0)
#define GPIOB_RESET()			do { (RCC->AHB1RSTR |= (1 << 1)); (RCC->AHB1RSTR &= ~(1 << 1)); } while(0)
#define GPIOC_RESET()			do { (RCC->AHB1RSTR |= (1 << 2)); (RCC->AHB1RSTR &= ~(1 << 2)); } while(0)
#define GPIOD_RESET()			do { (RCC->AHB1RSTR |= (1 << 3)); (RCC->AHB1RSTR &= ~(1 << 3)); } while(0)
#define GPIOE_RESET()			do { (RCC->AHB1RSTR |= (1 << 4)); (RCC->AHB1RSTR &= ~(1 << 4)); } while(0)
#define GPIOF_RESET()			do { (RCC->AHB1RSTR |= (1 << 5)); (RCC->AHB1RSTR &= ~(1 << 5)); } while(0)
#define GPIOG_RESET()			do { (RCC->AHB1RSTR |= (1 << 6)); (RCC->AHB1RSTR &= ~(1 << 6)); } while(0)
#define GPIOH_RESET()			do { (RCC->AHB1RSTR |= (1 << 7)); (RCC->AHB1RSTR &= ~(1 << 7)); } while(0)
#define GPIOI_RESET()			do { (RCC->AHB1RSTR |= (1 << 8)); (RCC->AHB1RSTR &= ~(1 << 8)); } while(0)


// Includes for protocol-specific header files
#include "stm32f407xx_i2c_driver.h"
#include "stm32f407xx_rcc_driver.h"

#endif /* STM32F407XX_H_ */
