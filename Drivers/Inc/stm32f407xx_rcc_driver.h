/*
 * stm32f407xx_rcc_driver.h
 *
 *  Created on: Jan 27, 2023
 *      Author: Michael
 */

#ifndef INC_STM32F407XX_RCC_DRIVER_H_
#define INC_STM32F407XX_RCC_DRIVER_H_

/*************** RELEVANT BIT POSITIONS FOR RCC PERIPHERAL REGISTERS *****************/
// don't plan on using any Phased Lock Loop (PLL) functionality

// RCC_CFGR - Configuration register; determines all clock prescalars
#define RCC_CFGR_SW			0		// Clock select; 2 bits 0:1, set by SW
#define RCC_CFGR_SWS		2		// Clock status; 2 bits 2:3, set by HW as status
#define RCC_CFGR_HPRE		4		// AHB prescaler; system clock division factor for AHB
#define RCC_CFGR_PPRE1		10		// APB1 prescaler; division from AHB clock to APB1 clock
#define RCC_CFGR_PPRE2		13		// APB2 prescaler; division from AHB clock to APB2 clock


#endif /* INC_STM32F407XX_RCC_DRIVER_H_ */
