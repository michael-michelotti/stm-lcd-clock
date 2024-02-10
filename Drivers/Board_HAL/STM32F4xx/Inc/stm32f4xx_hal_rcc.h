#ifndef BOARD_HAL_STM32F4XX_INC_STM32F4XX_HAL_RCC_H_
#define BOARD_HAL_STM32F4XX_INC_STM32F4XX_HAL_RCC_H_

typedef struct
{
	uint32_t Osc_Type;
	uint32_t HSE_State;
	uint32_t LSE_State;
	uint32_t HSI_State;
	uint32_t HSI_Calibration_Value;
	uint32_t LSI_State;

} RCC_Osc_Init_t;

typedef struct
{
	uint32_t PLL_State;
	uint32_t PLL_Source;
	uint32_t PLLM;
	uint32_t PLLN;
	uint32_t PLLP;
	uint32_t PLLQ;
} RCC_PLL_Init_t;

HAL_Status_t HAL_RCC_OscConfig(RCC_Osc_Init_t *RCC_OscInitStruct);

#endif /* BOARD_HAL_STM32F4XX_INC_STM32F4XX_HAL_RCC_H_ */
