/*
 * ds3231_rtc_driver.c
 *
 *  Created on: Feb 1, 2023
 *      Author: Michael
 */

#include "ds3231_rtc_driver.h"

static uint8_t Convert_BCD_To_Binary(uint8_t bcd_byte);

uint8_t DS3231_Get_Seconds(I2C_Handle_t *p_i2c_handle)
{
	uint8_t p_tx_buffer[1] = { DS3231_SECONDS };
	uint8_t p_rx_buffer[1];

	// write seconds register into register pointer
	I2C_Master_Send(p_i2c_handle, p_tx_buffer, 1, DS3231_SLAVE_ADDR, I2C_ENABLE_SR);

	// read one byte
	I2C_Master_Receive(p_i2c_handle, p_rx_buffer, 1, DS3231_SLAVE_ADDR, I2C_DISABLE_SR);

	return Convert_BCD_To_Binary(*p_rx_buffer);
}

void DS3231_Get_Minutes();
void DS3231_Get_Hours();
void DS3231_Get_Day();
void DS3231_Get_Date();
void DS3231_Get_Month();
void DS3231_Get_Year();

void DS3231_Get_Full_Date();
void DS3231_Get_Full_Time();

void DS3231_Get_Temp();

void DS3231_Set_12_24_Hour();
void DS3231_Set_Seconds();
void DS3231_Set_Minutes();
void DS3231_Set_Hours();
void DS3231_Set_Day();
void DS3231_Set_Date();
void DS3231_Set_Month();
void DS3231_Set_Year();

void DS3231_Set_Full_Date();
void DS3231_Set_Full_Time();

static uint8_t Convert_BCD_To_Binary(uint8_t bcd_byte)
{
	// zeroes place seconds (bottom 4 bits)
	uint8_t zeroes_place = bcd_byte & 0xF;
	// tens place seconds (bits 4-6)
	uint8_t tens_place = (bcd_byte >> 4)& 0x7;

	return zeroes_place + (tens_place * 10);
}
