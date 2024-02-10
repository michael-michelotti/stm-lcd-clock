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
#define HIGH			1
#define LOW				0
#define SET				ENABLE
#define RESET			DISABLE
#define I2C_WRITE		0
#define I2C_READ		1
#define RISING			0
#define FALLING			1
#define NO_PRIORITY_BITS_IMPLEMENTED	4

#define HSI_CLK_SPEED	16000000u

// Given a mask for any field in a register, and a value for that field, set the value
#define SET_FIELD(ADDR, MASK, VALUE) do { 																			\
	uint32_t value = (VALUE);																						\
	uint32_t curr_reg = *(ADDR);				/* Get current register value */									\
	value *= ((MASK) & ~((MASK) << 1));			/* Shift field value by multiplying by first bit of mask */			\
	*(ADDR) = ((curr_reg & ~(MASK)) | value); } /* Set register to old register, cleared by mask, set by value */	\
	while (0)
// Given a mask for any field in a register, clear that field
#define CLEAR_FIELD(ADDR, MASK) *(ADDR) &= ~(MASK)
// Given an address and an offset, set the bit at that offset
// #define SET_BIT(ADDR, BIT) *(ADDR) |= (1 << (BIT))
#define SET_BIT(ADDR, MASK) *(ADDR) |= (MASK)
// Given an address and an offset, clear the bit at that offset
// #define CLEAR_BIT(ADDR, BIT) *(ADDR) &= ~(1 << (BIT))
#define CLEAR_BIT(ADDR, MASK) *(ADDR) &= ~(MASK)
#define CHECK_BIT(ADDR, MASK) *(ADDR) & (MASK)
uint32_t GET_FIELD(uint32_t *ADDR, uint32_t MASK);
uint8_t GET_BIT(uint32_t *ADDR, uint32_t MASK);

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
#include "stm32f407xx_gpio_driver.h"

#endif /* STM32F407XX_H_ */
