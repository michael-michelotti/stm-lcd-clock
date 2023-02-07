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
#include "lcd1602a_driver.h"

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

int main(void)
{
	LCD_Initialize();

	DS3231_Hours_t hrs = { DS3231_12_HOUR, DS3231_PM, 6 };
	DS3231_Time_t time = { 45, 20, hrs };
	char *my_str = "Hello, world!\0";

	LCD_Update_Time(time);

	// LCD_Power_Switch(OFF);

	for(;;);
}
