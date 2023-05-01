/*
 * lcd1602a_driver.h
 *
 *  Created on: Feb 4, 2023
 *      Author: Michael
 */

#ifndef INC_LCD1602A_DRIVER_H_
#define INC_LCD1602A_DRIVER_H_

#include "stdint.h"
#include "ds3231_rtc_driver.h"

#define LCD_GPIO_PORT		GPIOA
// GPIO PIN NUMBERS
#define RS_GPIO_PIN			1
#define E_GPIO_PIN			2
#define DB4_GPIO_PIN		3
#define DB5_GPIO_PIN		4
#define DB6_GPIO_PIN		5
#define DB7_GPIO_PIN		6

// ENTRY MODE SET CONFIGURATION BITS
#define LCD_INCREMENT		1
#define LCD_DECREMENT		0
#define LCD_SHIFT			1
#define LCD_NO_SHIFT		0

// ENTRY DISPLAY AND CURSOR CONFIG BITS
#define LCD_DISP_OFF		0
#define LCD_DISP_ON			1
#define LCD_CURSOR_OFF		0
#define LCD_CURSOR_ON		1
#define LCD_BLINK_OFF		0
#define LCD_BLINK_ON		1

#define LCD_4_BIT			0
#define LCD_8_BIT			1
#define LCD_1_LINE			0
#define LCD_2_LINES			1
#define LCD_5_8_DOTS		0
#define LCD_5_10_DOTS		1

// Default row and column position for date and time
#define DEFAULT_DATE_COL	1
#define DEFAULT_DATE_ROW	2
#define DEFAULT_TIME_COL	1
#define DEFAULT_TIME_ROW	1

#define ENALBE_PULSE_US		500		// how long to hold enable high when sending nybble
#define LCD_TAS_US			1		// how long to wait after toggling RS or RW pins for address setup
#define LCD_HOLD_TIME_US	500		// how long to hold data lines valid after enable high to low transition

#define ASCII_DIGIT_OFFSET	48


// Enumeration of LCD COMMAND CATEGORIES
typedef enum
{
	CLEAR_DISPLAY = 0x1,
	RETURN_HOME = 0x2,
	ENTRY_MODE_SET,
	DISPLAY_ON_OFF_CTRL,
	CURSOR_DISPLAY_SHIFT,
	FUNCTION_SET,
	SET_CGRAM_ADDR,
	SET_DDRAM_ADDR,
	READ_BF_DDRAM_ADDR,
	WRITE_TO_RAM,
	READ_FROM_RAM
} LCD_1602A_Commands_t;

// INSTRUCTION CODE BIT POSITIONS
#define IC_DB0			0
#define IC_DB1			1
#define IC_DB2			2
#define IC_DB3			3
#define IC_DB4			4
#define IC_DB5			5
#define IC_DB6			6
#define IC_DB7			7
#define IC_RW			8
#define IC_RS			9

void LCD_Initialize();
void LCD_Set_Cursor(uint8_t row, uint8_t column);
void LCD_Display_Char(char ch);
void LCD_Display_Str(char *str);
void LCD_Power_Switch(uint8_t on_or_off);
// void LCD_Backlight_Power_Switch(uint8_t on_or_off);
void LCD_Update_Time_Format(char *fmt);
void LCD_Update_Date_Format(char *fmt);
void LCD_Update_Time(DS3231_Time_t time);
void LCD_Update_Date(DS3231_Full_Date_t date);
void LCD_Update_Date_And_Time(DS3231_Datetime_t datetime);

#endif /* INC_LCD1602A_DRIVER_H_ */
