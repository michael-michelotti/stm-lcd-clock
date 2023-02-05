/*
 * lcd1602a_driver.h
 *
 *  Created on: Feb 4, 2023
 *      Author: Michael
 */

#ifndef INC_LCD1602A_DRIVER_H_
#define INC_LCD1602A_DRIVER_H_


void LCD_Display_Char(uint8_t row, uint8_t col, char ch);
void LCD_Backlight_Power_Switch(uint8_t on_or_off);
void LCD_Initialize();
void LCD_Update_Time(DS3231_Time_t time, uint8_t row, uint8_t col);
void LCD_Update_Date(DS3231_Full_Date_t date, uint8_t row, uint8_t col);
void LCD_Update_Date_And_Time(DS3231_Datetime_t datetime, uint8_t row, uint8_t col);

#endif /* INC_LCD1602A_DRIVER_H_ */
