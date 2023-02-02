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

uint8_t DS3231_Get_Minutes(I2C_Handle_t *p_i2c_handle)
{
	uint8_t p_tx_buffer[1] = { DS3231_MINUTES };
	uint8_t p_rx_buffer[1];

	// write seconds register into register pointer
	I2C_Master_Send(p_i2c_handle, p_tx_buffer, 1, DS3231_SLAVE_ADDR, I2C_ENABLE_SR);
	// read one byte
	I2C_Master_Receive(p_i2c_handle, p_rx_buffer, 1, DS3231_SLAVE_ADDR, I2C_DISABLE_SR);

	return Convert_BCD_To_Binary(*p_rx_buffer);
}

DS3231_Hours_t DS3231_Get_Hours(I2C_Handle_t *p_i2c_handle)
{
	uint8_t p_tx_buffer[1] = { DS3231_HOURS };
	uint8_t p_rx_buffer[1];
	uint8_t hour_buffer;
	uint8_t hour_tens;
	DS3231_Hours_t hour_struct;

	// write seconds register into register pointer
	I2C_Master_Send(p_i2c_handle, p_tx_buffer, 1, DS3231_SLAVE_ADDR, I2C_ENABLE_SR);
	// read one byte
	I2C_Master_Receive(p_i2c_handle, p_rx_buffer, 1, DS3231_SLAVE_ADDR, I2C_DISABLE_SR);

	hour_buffer = *p_rx_buffer;
	hour_struct.hour_12_24 = (hour_buffer >> DS3231_12_24_BIT) & 1;

	if (hour_struct.hour_12_24 == DS3231_12_HOUR)
		hour_struct.am_pm = (hour_buffer >> DS3231_AM_PM_BIT) & 1;

	// convert hour BCD to binary
	hour_tens = (hour_buffer >> 4) & 1;
	hour_struct.hour = (hour_buffer & 0xF) + (hour_tens * 10);

	return hour_struct;
}

DS3231_DOW_t DS3231_Get_Day_Of_Week(I2C_Handle_t *p_i2c_handle)
{
	uint8_t p_tx_buffer[1] = { DS3231_DAY };
	uint8_t p_rx_buffer[1];

	// write seconds register into register pointer
	I2C_Master_Send(p_i2c_handle, p_tx_buffer, 1, DS3231_SLAVE_ADDR, I2C_ENABLE_SR);
	// read one byte
	I2C_Master_Receive(p_i2c_handle, p_rx_buffer, 1, DS3231_SLAVE_ADDR, I2C_DISABLE_SR);

	return *p_rx_buffer & 0x7;
}

uint8_t DS3231_Get_Date(I2C_Handle_t *p_i2c_handle)
{
	uint8_t p_tx_buffer[1] = { DS3231_DATE };
	uint8_t p_rx_buffer[1];

	// write seconds register into register pointer
	I2C_Master_Send(p_i2c_handle, p_tx_buffer, 1, DS3231_SLAVE_ADDR, I2C_ENABLE_SR);
	// read one byte
	I2C_Master_Receive(p_i2c_handle, p_rx_buffer, 1, DS3231_SLAVE_ADDR, I2C_DISABLE_SR);

	// TODO: Possibly update BCD function to include how many bits for 10s place
	uint8_t date_buffer = *p_rx_buffer;
	// zeroes place seconds (bottom 4 bits)
	uint8_t zeroes_place = date_buffer & 0xF;
	// tens place seconds (bits 4-5)
	uint8_t tens_place = (date_buffer >> 4)& 0x3;
	return zeroes_place + (tens_place * 10);
}

uint8_t DS3231_Get_Month(I2C_Handle_t *p_i2c_handle)
{
	uint8_t p_tx_buffer[1] = { DS3231_MONTH_CENTURY };
	uint8_t p_rx_buffer[1];

	// write seconds register into register pointer
	I2C_Master_Send(p_i2c_handle, p_tx_buffer, 1, DS3231_SLAVE_ADDR, I2C_ENABLE_SR);
	// read one byte
	I2C_Master_Receive(p_i2c_handle, p_rx_buffer, 1, DS3231_SLAVE_ADDR, I2C_DISABLE_SR);

	// TODO: Possibly update BCD function to include how many bits for 10s place
	uint8_t month_buffer = *p_rx_buffer;
	// zeroes place seconds (bottom 4 bits)
	uint8_t zeroes_place = month_buffer & 0xF;
	// tens place seconds (bit 4)
	uint8_t tens_place = (month_buffer >> 4)& 0x1;
	return zeroes_place + (tens_place * 10);
}

uint8_t DS3231_Get_Century(I2C_Handle_t *p_i2c_handle)
{
	uint8_t p_tx_buffer[1] = { DS3231_MONTH_CENTURY };
	uint8_t p_rx_buffer[1];

	// write seconds register into register pointer
	I2C_Master_Send(p_i2c_handle, p_tx_buffer, 1, DS3231_SLAVE_ADDR, I2C_ENABLE_SR);
	// read one byte
	I2C_Master_Receive(p_i2c_handle, p_rx_buffer, 1, DS3231_SLAVE_ADDR, I2C_DISABLE_SR);

	return (*p_rx_buffer) >> 7;
}

DS3231_Month_Century_t DS3231_Get_Month_Century(I2C_Handle_t *p_i2c_handle)
{
	DS3231_Month_Century_t m_c_struct;
	m_c_struct.month = DS3231_Get_Month(p_i2c_handle);
	m_c_struct.century = DS3231_Get_Century(p_i2c_handle);
	return m_c_struct;
}

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
