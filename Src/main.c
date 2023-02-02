/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include "stm32f407xx.h"
#include "ds3231_rtc_driver.h"

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

int main(void)
{
	// configure I2C1 GPIO pins as SCL
	GPIO_Pin_Config_t scl_config = {
			10,					// PB8 is I2C1 SCL
			GPIO_MODE_ALT,
			GPIO_SPEED_HIGH,
			GPIO_PUPD_NONE,
			GPIO_OUT_OD,		// I2C pins must be configured open drain with a pull-up
			4					// Alt function 4 is SCL on PB6
	};

	GPIO_Handle_t scl_handle = { GPIOB, scl_config };

	// configure I2C1 GPIO pins as SDA
	GPIO_Pin_Config_t sda_config = {
			11,					// PB7 is I2C1 SDA
			GPIO_MODE_ALT,
			GPIO_SPEED_HIGH,
			GPIO_PUPD_NONE,
			GPIO_OUT_OD,		// I2C pins must be configured open drain with a pull-up
			4					// Alt function 4 is SDA on PB7
	};
	GPIO_Handle_t sda_handle = { GPIOB, sda_config };

	GPIO_Init(&scl_handle);
	GPIO_Init(&sda_handle);

	I2C_Config_t config = {I2C_SPEED_SM, 62, I2C_ACK_EN, I2C_FM_DUTY_2};
	I2C_Handle_t p_i2c_handle = {I2C2, config, NULL, NULL, 0, 0, 0, 21, 0, 0};
	I2C_Init(&p_i2c_handle);

	DS3231_Hours_t hours_12 = DS3231_Get_Hours(&p_i2c_handle);

	DS3231_Set_12_24_Hour(&p_i2c_handle, DS3231_12_HOUR);

	DS3231_Hours_t hours_24 = DS3231_Get_Hours(&p_i2c_handle);

	for(;;);
}
