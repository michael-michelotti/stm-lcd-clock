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
	DS3231_UNIT_HOURS,
	DS3231_UNIT_FULL_TIME,
	DS3231_UNIT_DATE,
	DS3231_UNIT_DOW,
	DS3231_UNIT_MONTHS,
	DS3231_UNIT_YEAR
} DS3231_Unit_t;

typedef struct
{
	Clock_Device_t 		clock_dev;
	DS3231_State_t		state;
	DS3231_Unit_t		curr_unit;
	I2C_Interface_t		*i2c_interface;
} DS3231_Handle_t;

// TODO: Decide if this should be an enum or not
#define DS3231_ADDR_BASE					0x00
#define DS3231_ADDR_SECONDS					0x00
#define DS3231_ADDR_MINUTES					0x01
#define DS3231_ADDR_HOURS					0x02
#define DS3231_ADDR_DAY						0x03
#define DS3231_ADDR_DATE					0x04
#define DS3231_ADDR_MONTH_CENTURY			0x05
#define DS3231_ADDR_YEAR					0x06
#define DS3231_ADDR_ALARM_1_SECS			0x07
#define DS3231_ADDR_ALARM_1_MINS			0x08
#define DS3231_ADDR_ALARM_1_HRS				0x09
#define DS3231_ADDR_ALARM_1_DAY_DATE		0x0A
#define DS3231_ADDR_ALARM_2_MINS			0x0B
#define DS3231_ADDR_ALARM_2_HRS				0x0C
#define DS3231_ADDR_ALARM_2_DAY_DATE		0x0D
#define DS3231_ADDR_CONTROL					0x0E
#define DS3231_ADDR_CONTROL_STATUS			0x0F
#define DS3231_ADDR_AGING_OFFSET			0x10
#define DS3231_ADDR_MSB_TEMP				0x11
#define DS3231_ADDR_LSB_TEMP				0x12

/* Byte length of each DS3231 unit in timekeeping registers */
#define DS3231_PTR_LEN						1
#define DS3231_LEN_SECONDS					1
#define DS3231_LEN_MINUTES					1
#define DS3231_LEN_HOURS					1
#define DS3231_LEN_FULL_TIME				(DS3231_LEN_SECONDS + DS3231_LEN_MINUTES + DS3231_LEN_HOURS)
#define DS3231_LEN_DOW						1
#define DS3231_LEN_DATE						1
#define DS3231_LEN_MONTH_CENTURY			1
#define DS3231_LEN_YEAR						1
#define DS3231_LEN_FULL_DATE				(DS3231_LEN_DOW + DS3231_LEN_DATE + DS3231_LEN_MONTH_CENTURY + DS3231_LEN_YEAR)

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

#endif /* INC_DS3231_RTC_DRIVER_H_ */
