#ifndef INC_DS3231_RTC_DRIVER_H_
#define INC_DS3231_RTC_DRIVER_H_

#include "clock.h"
#include "stm32f407xx_i2c_driver.h"

typedef enum
{
	DS3231_STATE_IDLE,
	DS3231_STATE_POINTER_WRITE_FOR_READ,
	DS3231_STATE_DATA_READ,
	DS3231_STATE_DATA_WRITE,
} DS3231_State_t;

typedef enum
{
	DS3231_UNIT_NONE,
	DS3231_UNIT_SECONDS,
	DS3231_UNIT_MINUTES,
} DS3231_Unit_t;

typedef struct
{
	Clock_Device_t 		clock_dev;
	DS3231_State_t		state;
	DS3231_Unit_t		curr_unit;
	I2C_Interface_t		*i2c_interface;
} DS3231_Handle_t;

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
#define DS3231_MINIMUM_SECONDS		0
#define DS3231_MAXIMUM_SECONDS		59
#define DS3231_MINIMUM_MINUTES		0
#define DS3231_MAXIMUM_MINUTES		59
#define DS3231_MINIMUM_HOURS		0
#define DS3231_MAXIMUM_HOURS		23
#define DS3231_MINIMUM_DATE			1
#define DS3231_MAXIMUM_DATE			31
#define DS3231_MINIMUM_MONTH		1
#define DS3231_MAXIMUM_MONTH		12
#define DS3231_MINIMUM_YEAR			0
#define DS3231_MAXIMUM_YEAR			99

#define DS3231_BLOCKING_CALL		0
#define DS3231_INTERRUPT_CALL		1

void DS3231_Initialize(Clock_Device_t ds3231_dev);

// Functions which retrieve data from the DS3231 clock module
seconds_t DS3231_Get_Seconds(void);
void DS3231_Get_Seconds_IT(void);
void DS3231_Get_Minutes_IT(void);
minutes_t DS3231_Get_Minutes(void);
hours_t DS3231_Get_Hours(void);
day_of_week_t DS3231_Get_Day_Of_Week(void);
date_t DS3231_Get_Date(void);
month_t DS3231_Get_Month(void);

//uint8_t DS3231_Get_Century(void);
//DS3231_Month_Century_t DS3231_Get_Month_Century(void);

year_t DS3231_Get_Year(void);
century_t DS3231_Get_Century(void);
full_date_t DS3231_Get_Full_Date(void);
full_time_t DS3231_Get_Full_Time(void);
full_datetime_t DS3231_Get_Full_Datetime(void);
float DS3231_Get_Temp(void);

// Functions which set data in the DS3231 clock module
void DS3231_Convert_Hour_Format(I2C_Handle_t *p_i2c_handle, hour_format_t hour_mode);
void DS3231_Set_Seconds(seconds_t seconds);
void DS3231_Set_Seconds_IT(seconds_t seconds);
void DS3231_Set_Minutes(minutes_t mins);
void DS3231_Set_Hours(hours_t hours);
void DS3231_Set_Day_Of_Week(day_of_week_t dow);
void DS3231_Set_Date(date_t date);
void DS3231_Set_Month(month_t month);
void DS3231_Set_Year(year_t year);

void DS3231_Set_Full_Date(full_date_t full_date);
void DS3231_Set_Full_Time(full_time_t full_time);
void DS3231_Set_Full_Datetime(full_datetime_t full_datetime);

#endif /* INC_DS3231_RTC_DRIVER_H_ */
