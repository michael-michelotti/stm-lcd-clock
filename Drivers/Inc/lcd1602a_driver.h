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

// LCD PIN NUMBER MACROS
#define VSS				0		// high voltage for entire module
#define VDD				1		// ground voltage for entire module
#define VO				2		// between VSS and VDD - controls contrast level (with potentiometer)
#define RS				3		// read select - low means module command, high means read or write
#define RW				4		// read or write - determines r/w whenever read select is low
#define E				5		// starts read or write process (will always be write here)
#define DB0				6		// data line 1
#define DB1				7		// data line 2
#define DB2				8		// data line 3
#define DB3				9		// data line 4
#define DB4				10		// data line 5
#define DB5				11		// data line 6
#define DB6				12		// data line 7
#define DB7				13		// data line 8
#define LCD_HI			14		// high voltage for lcd backlight
#define LCD_LO			15		// ground for lcd backlight

// PIN NUMBER ON GPIOD
#define RS_GPIO_PIN		1
#define E_GPIO_PIN		2
#define DB4_GPIO_PIN	3
#define DB5_GPIO_PIN	4
#define DB6_GPIO_PIN	5
#define DB7_GPIO_PIN	6

// Enumeration of LCD commands
typedef enum
{
	CLEAR_DISPLAY = 	0x1,
	RETURN_HOME = 		0x2,
	ENTRY_MODE_SET = 	0x4,
	DISPLAY_ON_OFF = 	0x8,

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

#define INIT_FUNCTION_SET	0x3


typedef enum
{
	LCD_CLR_DISP			= 0x01,
	LCD_RETURN_HOME			= 0x02,
	LCD_ENTRY_MODE			= 0x04,
	LCD_ALL_OFF				= 0x08,
	LCD_CUR_POS_ON			= 0x09,
	LCD_CUR_ON 				= 0x0A,
	LCD_CUR_CUR_POS_ON 		= 0x0B,
	LCD_DISP_ON				= 0x0C,
	LCD_DISP_CUR_POS_ON		= 0x0D,
	LCD_DISP_CUR_ON			= 0x0E,
	LCD_ALL_ON				= 0x0F
} LCD_1602A_Instruction_Cmd_t;



void LCD_Initialize();
void LCD_Power_Switch(uint8_t on_or_off);
void LCD_Backlight_Power_Switch(uint8_t on_or_off);
void LCD_Display_Char(uint8_t row, uint8_t col, char ch);
void LCD_Update_Time(DS3231_Time_t time, uint8_t row, uint8_t col);
void LCD_Update_Date(DS3231_Full_Date_t date, uint8_t row, uint8_t col);
void LCD_Update_Date_And_Time(DS3231_Datetime_t datetime, uint8_t row, uint8_t col);

#endif /* INC_LCD1602A_DRIVER_H_ */
