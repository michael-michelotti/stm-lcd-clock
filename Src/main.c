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
	// PB10 = SCL, PB11 = SDA
	GPIO_Pin_Config_t pb10 = { 10, GPIO_MODE_ALT, GPIO_SPEED_HIGH, GPIO_PUPD_NONE, GPIO_OUT_OD, 4 };
	GPIO_Handle_t scl_handle = { GPIOB, pb10 };
	GPIO_Init(&scl_handle);

	GPIO_Pin_Config_t pb11 = { 11, GPIO_MODE_ALT, GPIO_SPEED_HIGH, GPIO_PUPD_NONE, GPIO_OUT_OD, 4 };
	GPIO_Handle_t sda_handle = { GPIOB, pb11 };
	GPIO_Init(&sda_handle);

	// configure I2C SDA and SCL pins
	I2C_Config_t i2c2_conf = { I2C_SPEED_SM, 0x62, I2C_ACK_EN, I2C_FM_DUTY_2 };
	I2C_Handle_t i2c2_handle = { I2C2, i2c2_conf, 0, 0, 0, 0, 0, DS3231_SLAVE_ADDR, 0, 0 };
	I2C_Init(&i2c2_handle);

	DS3231_Hours_t hrs = { DS3231_12_HOUR, DS3231_PM, 6 };
	DS3231_Time_t time = { 45, 20, hrs };
	DS3231_Full_Date_t date = { DS3231_MON, 6, 2, 23 };
	DS3231_Datetime_t dt = { time, date };
	// DS3231_Set_Full_Datetime(&i2c2_handle, dt);
	DS3231_Datetime_t datetime = DS3231_Get_Full_Datetime(&i2c2_handle);

	LCD_Initialize();

	LCD_Update_Date_And_Time(datetime);

	LCD_Power_Switch(OFF);

	for(;;);
}
