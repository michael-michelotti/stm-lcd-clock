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

#define DS3231_AM_PM_BIT			5
#define DS3231_12_24_BIT			6
#define DS3231_CENTURY_BIT			7

typedef struct
{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t date;
	uint8_t month_century;
	uint8_t year;
	uint8_t alarm_1_secs;
	uint8_t alarm_1_mins;
	uint8_t alarm_1_hrs;
	uint8_t alarm_1_day;
	uint8_t alarm_1_date;
	uint8_t alarm_2_secs;
	uint8_t alarm_2_mins;
	uint8_t alarm_2_hrs;
	uint8_t alarm_2_day;
	uint8_t alarm_2_date;
	uint8_t control;
	uint8_t control_status;
	uint8_t aging_offset;
	uint8_t msb_temp;
	uint8_t lsb_temp;
} DS3231_Register_Map_t;

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

typedef struct
{
	DS3231_12_24_Hour_t 	hour_12_24;
	DS3231_AM_PM_t			am_pm;
	uint8_t					hour;
} DS3231_Hours_t;

typedef enum
{
	SUN = 1,
	MON,
	TUE,
	WED,
	THU,
	FRI,
	SAT
} DS3231_DOW_t;

typedef struct
{
	uint8_t month;
	uint8_t century;
} DS3231_Month_Century_t;

typedef struct
{
	uint8_t year;
	uint8_t month;
	uint8_t date;
	DS3231_DOW_t dow;
} DS3231_Full_Date_t;

typedef struct
{
	uint8_t seconds;
	uint8_t minutes;
	DS3231_Hours_t hours;
} DS3231_Full_Time_t;

typedef struct
{
	DS3231_Full_Date_t date;
	DS3231_Full_Time_t time;
} DS3231_Datetime_t;

uint8_t DS3231_Get_Seconds(I2C_Handle_t *p_i2c_handle);
uint8_t DS3231_Get_Minutes(I2C_Handle_t *p_i2c_handle);
DS3231_Hours_t DS3231_Get_Hours(I2C_Handle_t *p_i2c_handle);
DS3231_DOW_t DS3231_Get_Day_Of_Week(I2C_Handle_t *p_i2c_handle);
uint8_t DS3231_Get_Date(I2C_Handle_t *p_i2c_handle);
uint8_t DS3231_Get_Month(I2C_Handle_t *p_i2c_handle);
uint8_t DS3231_Get_Century(I2C_Handle_t *p_i2c_handle);
DS3231_Month_Century_t DS3231_Get_Month_Century(I2C_Handle_t *p_i2c_handle);
uint8_t DS3231_Get_Year(I2C_Handle_t *p_i2c_handle);
DS3231_Full_Date_t DS3231_Get_Full_Date(I2C_Handle_t *p_i2c_handle);
DS3231_Full_Time_t DS3231_Get_Full_Time(I2C_Handle_t *p_i2c_handle);
DS3231_Datetime_t DS3231_Get_Full_Datetime(I2C_Handle_t *p_i2c_handle);
float DS3231_Get_Temp(I2C_Handle_t *p_i2c_handle);

void DS3231_Set_12_24_Hour(I2C_Handle_t *p_i2c_handle, DS3231_12_24_Hour_t hour_mode);
void DS3231_Set_Seconds(I2C_Handle_t *p_i2c_handle, uint8_t seconds);
void DS3231_Set_Minutes(I2C_Handle_t *p_i2c_handle, uint8_t minutes);
void DS3231_Set_Hours();
void DS3231_Set_Day();
void DS3231_Set_Date();
void DS3231_Set_Month();
void DS3231_Set_Year();

void DS3231_Set_Full_Date();
void DS3231_Set_Full_Time();

#endif /* INC_DS3231_RTC_DRIVER_H_ */
