/*
 * ds3231_rtc_driver.h
 *
 *  Created on: Feb 1, 2023
 *      Author: Michael
 */

#ifndef INC_DS3231_RTC_DRIVER_H_
#define INC_DS3231_RTC_DRIVER_H_

#include "stm32f407xx.h"

// TODO: Decide if this should be an enum or not
#define DS3231_BASE_ADDR			0x00
#define DS3231_SECONDS				0x00
#define DS3231_MINUTES				0x01
#define DS3231_HOURS				0x02
#define DS3231_DAY					0x03
#define DS3231_DATE					0x04
#define DS3231_MONTH_CENTURY		0x05
#define DS3231_YEAR					0x06
#define DS3231_ALARM_1_SECS			0x07
#define DS3231_ALARM_1_MINS			0x08
#define DS3231_ALARM_1_HRS			0x09
#define DS3231_ALARM_1_DAY_DATE		0x0A
#define DS3231_ALARM_2_MINS			0x0B
#define DS3231_ALARM_2_HRS			0x0C
#define DS3231_ALARM_2_DAY_DATE		0x0D
#define DS3231_CONTROL				0x0E
#define DS3231_CONTROL_STATUS		0x0F
#define DS3231_AGING_OFFSET			0x10
#define DS3231_MSB_TEMP				0x11
#define DS3231_LSB_TEMP				0x12

#define DS3231_SLAVE_ADDR			0b1101000

// Bit positions in various registers
#define DS3231_AM_PM_BIT			5
#define DS3231_12_24_BIT			6
#define DS3231_CENTURY_BIT			7

// How many registers date and time span in RTC
#define FULL_DATE_LEN				4
#define FULL_TIME_LEN				3
#define FULL_DATETIME_LEN			( (FULL_DATE_LEN) + (FULL_TIME_LEN) )

// Minimum and maximum values for various fields, for validation purposes
#define DS3231_MINIMUM_SECONDS				0
#define DS3231_MAXIMUM_SECONDS				59
#define DS3231_MINIMUM_MINUTES				0
#define DS3231_MAXIMUM_MINUTES				59
#define DS3231_MINIMUM_HOURS				0
#define DS3231_MAXIMUM_HOURS				23
#define DS3231_MINIMUM_DATE					1
#define DS3231_MAXIMUM_DATE					31
#define DS3231_MINIMUM_MONTH				1
#define DS3231_MAXIMUM_MONTH				12
#define DS3231_MINIMUM_YEAR					0
#define DS3231_MAXIMUM_YEAR					99

#define DS3231_BLOCKING_CALL				0
#define DS3231_INTERRUPT_CALL				1

typedef enum
{
	DS3231_24_HOUR,
	DS3231_12_HOUR
} DS3231_12_24_Hour_t;

typedef enum
{
	DS3231_AM,
	DS3231_PM,
	DS3231_NO_AM_PM
} DS3231_AM_PM_t;

typedef enum
{
	DS3231_SUN = 1,
	DS3231_MON,
	DS3231_TUE,
	DS3231_WED,
	DS3231_THU,
	DS3231_FRI,
	DS3231_SAT
} DS3231_Day_t;

typedef struct
{
	uint8_t month;
	uint8_t century;
} DS3231_Month_Century_t;

typedef struct
{
	DS3231_12_24_Hour_t 	hour_12_24;
	DS3231_AM_PM_t			am_pm;
	uint8_t					hour;
} DS3231_Hours_t;

typedef struct
{
	uint8_t seconds;
	uint8_t minutes;
	DS3231_Hours_t hours;
} DS3231_Time_t;

typedef struct
{
	DS3231_Day_t day;
	uint8_t date;
	uint8_t month;
	uint8_t year;
} DS3231_Full_Date_t;

typedef struct
{
	DS3231_Time_t time;
	DS3231_Full_Date_t date;
} DS3231_Datetime_t;

// Functions which retrieve data from the DS3231 clock module
uint8_t DS3231_Get_Seconds_IT();
uint8_t DS3231_Get_Seconds(I2C_Handle_t *p_i2c_handle);
uint8_t DS3231_Get_Minutes(I2C_Handle_t *p_i2c_handle);
DS3231_Hours_t DS3231_Get_Hours(I2C_Handle_t *p_i2c_handle);
DS3231_Day_t DS3231_Get_Day_Of_Week(I2C_Handle_t *p_i2c_handle);
uint8_t DS3231_Get_Date(I2C_Handle_t *p_i2c_handle);
uint8_t DS3231_Get_Month(I2C_Handle_t *p_i2c_handle);
uint8_t DS3231_Get_Century(I2C_Handle_t *p_i2c_handle);
DS3231_Month_Century_t DS3231_Get_Month_Century(I2C_Handle_t *p_i2c_handle);
uint8_t DS3231_Get_Year(I2C_Handle_t *p_i2c_handle);
DS3231_Full_Date_t DS3231_Get_Full_Date(I2C_Handle_t *p_i2c_handle);
DS3231_Time_t DS3231_Get_Full_Time(I2C_Handle_t *p_i2c_handle);
DS3231_Datetime_t DS3231_Get_Full_Datetime(I2C_Handle_t *p_i2c_handle);
float DS3231_Get_Temp(I2C_Handle_t *p_i2c_handle);

// Functions which set data in the DS3231 clock module
void DS3231_Convert_Hour_Format(I2C_Handle_t *p_i2c_handle, DS3231_12_24_Hour_t hour_mode);
void DS3231_Set_Seconds(I2C_Handle_t *p_i2c_handle, uint8_t seconds, uint8_t blocking);
void DS3231_Set_Minutes(I2C_Handle_t *p_i2c_handle, uint8_t minutes);
void DS3231_Set_Hours(I2C_Handle_t *p_i2c_handle, DS3231_Hours_t hours);
void DS3231_Set_Day(I2C_Handle_t *p_i2c_handle, DS3231_Day_t day);
void DS3231_Set_Date(I2C_Handle_t *p_i2c_handle, uint8_t date);
void DS3231_Set_Month(I2C_Handle_t *p_i2c_handle, uint8_t month);
void DS3231_Set_Year(I2C_Handle_t *p_i2c_handle, uint8_t year);

void DS3231_Set_Full_Date(I2C_Handle_t *p_i2c_handle, DS3231_Full_Date_t full_date, uint8_t blocking);
void DS3231_Set_Full_Time(I2C_Handle_t *p_i2c_handle, DS3231_Time_t full_time);
void DS3231_Set_Full_Datetime(I2C_Handle_t *p_i2c_handle, DS3231_Datetime_t datetime);

#endif /* INC_DS3231_RTC_DRIVER_H_ */
