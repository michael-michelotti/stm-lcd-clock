#ifndef BOARD_HAL_STM32F4XX_INC_STM32F4XX_HAL_H_
#define BOARD_HAL_STM32F4XX_INC_STM32F4XX_HAL_H_


void HAL_Init(void);
void HAL_DeInit(void);
void HAL_MspInit(void);
void HAL_Msp_DeInit(void);

#ifdef HAL_I2C_MODULE_ENABLED
#	include "stm32f4xx_hal_i2c.h"
#endif

#ifdef HAL_GPIO_MODULE_ENABLED
#	include "stm32f4xx_hal_gpio.h"
#endif

#ifdef HAL_RCC_MODULE_ENABLED
#	include "stm32f4xx_hal_rcc.h"
#endif

#endif /* BOARD_HAL_STM32F4XX_INC_STM32F4XX_HAL_H_ */
