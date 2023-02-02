/*
 * ds3231_rtc_driver.c
 *
 *  Created on: Feb 1, 2023
 *      Author: Michael
 */

#include "ds3231_rtc_driver.h"

static DS3231_Full_Date_t Convert_Full_Date(uint8_t *p_rx_buffer);
static DS3231_Full_Time_t Convert_Full_Time(uint8_t *p_rx_buffer);
static uint8_t Convert_Seconds(uint8_t sec_byte);
static uint8_t Convert_Minutes(uint8_t min_byte);
static DS3231_Hours_t Convert_Hours(uint8_t hour_byte);
static DS3231_DOW_t Convert_DOW(uint8_t dow_byte);
static uint8_t Convert_Month(uint8_t month_byte);
static uint8_t Convert_Date(uint8_t date_byte);
static uint8_t Convert_Year(uint8_t year_byte);
static uint8_t Convert_BCD_To_Binary(uint8_t bcd_byte);

uint8_t DS3231_Get_Seconds(I2C_Handle_t *p_i2c_handle)
{
	uint8_t p_tx_buffer[1] = { DS3231_SECONDS };
	uint8_t p_rx_buffer[1];

	// write seconds register into register pointer
	I2C_Master_Send(p_i2c_handle, p_tx_buffer, 1, DS3231_SLAVE_ADDR, I2C_ENABLE_SR);
	// read one byte
	I2C_Master_Receive(p_i2c_handle, p_rx_buffer, 1, DS3231_SLAVE_ADDR, I2C_DISABLE_SR);

	return Convert_Seconds(*p_rx_buffer);
}

uint8_t DS3231_Get_Minutes(I2C_Handle_t *p_i2c_handle)
{
	uint8_t p_tx_buffer[1] = { DS3231_MINUTES };
	uint8_t p_rx_buffer[1];

	// write seconds register into register pointer
	I2C_Master_Send(p_i2c_handle, p_tx_buffer, 1, DS3231_SLAVE_ADDR, I2C_ENABLE_SR);
	// read one byte
	I2C_Master_Receive(p_i2c_handle, p_rx_buffer, 1, DS3231_SLAVE_ADDR, I2C_DISABLE_SR);

	return Convert_Minutes(*p_rx_buffer);
}

DS3231_Hours_t DS3231_Get_Hours(I2C_Handle_t *p_i2c_handle)
{
	uint8_t p_tx_buffer[1] = { DS3231_HOURS };
	uint8_t p_rx_buffer[1];

	// write seconds register into register pointer
	I2C_Master_Send(p_i2c_handle, p_tx_buffer, 1, DS3231_SLAVE_ADDR, I2C_ENABLE_SR);
	// read one byte
	I2C_Master_Receive(p_i2c_handle, p_rx_buffer, 1, DS3231_SLAVE_ADDR, I2C_DISABLE_SR);

	return Convert_Hours(*p_rx_buffer);
}

DS3231_DOW_t DS3231_Get_Day_Of_Week(I2C_Handle_t *p_i2c_handle)
{
	uint8_t p_tx_buffer[1] = { DS3231_DAY };
	uint8_t p_rx_buffer[1];

	// write seconds register into register pointer
	I2C_Master_Send(p_i2c_handle, p_tx_buffer, 1, DS3231_SLAVE_ADDR, I2C_ENABLE_SR);
	// read one byte
	I2C_Master_Receive(p_i2c_handle, p_rx_buffer, 1, DS3231_SLAVE_ADDR, I2C_DISABLE_SR);

	return Convert_DOW(*p_rx_buffer);
}

uint8_t DS3231_Get_Date(I2C_Handle_t *p_i2c_handle)
{
	uint8_t p_tx_buffer[1] = { DS3231_DATE };
	uint8_t p_rx_buffer[1];

	// write seconds register into register pointer
	I2C_Master_Send(p_i2c_handle, p_tx_buffer, 1, DS3231_SLAVE_ADDR, I2C_ENABLE_SR);
	// read one byte
	I2C_Master_Receive(p_i2c_handle, p_rx_buffer, 1, DS3231_SLAVE_ADDR, I2C_DISABLE_SR);

	return Convert_Date(*p_rx_buffer);
}

uint8_t DS3231_Get_Month(I2C_Handle_t *p_i2c_handle)
{
	uint8_t p_tx_buffer[1] = { DS3231_MONTH_CENTURY };
	uint8_t p_rx_buffer[1];

	// write seconds register into register pointer
	I2C_Master_Send(p_i2c_handle, p_tx_buffer, 1, DS3231_SLAVE_ADDR, I2C_ENABLE_SR);
	// read one byte
	I2C_Master_Receive(p_i2c_handle, p_rx_buffer, 1, DS3231_SLAVE_ADDR, I2C_DISABLE_SR);

	return Convert_Month(*p_rx_buffer);
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

uint8_t DS3231_Get_Year(I2C_Handle_t *p_i2c_handle)
{
	uint8_t p_tx_buffer[1] = { DS3231_YEAR };
	uint8_t p_rx_buffer[1];

	// write seconds register into register pointer
	I2C_Master_Send(p_i2c_handle, p_tx_buffer, 1, DS3231_SLAVE_ADDR, I2C_ENABLE_SR);
	// read one byte
	I2C_Master_Receive(p_i2c_handle, p_rx_buffer, 1, DS3231_SLAVE_ADDR, I2C_DISABLE_SR);
	return Convert_Year(*p_rx_buffer);
}

DS3231_Full_Date_t DS3231_Get_Full_Date(I2C_Handle_t *p_i2c_handle)
{
	uint8_t p_tx_buffer[1] = { DS3231_DAY };
	uint8_t p_rx_buffer[4];
	DS3231_Full_Date_t full_date;

	// write register pointer
	I2C_Master_Send(p_i2c_handle, p_tx_buffer, 1, DS3231_SLAVE_ADDR, I2C_ENABLE_SR);
	// read four bytes - day, date, month, year
	I2C_Master_Receive(p_i2c_handle, p_rx_buffer, 4, DS3231_SLAVE_ADDR, I2C_DISABLE_SR);

	// index 0 = dow, 1 = day of month, 2 = month, 3 = year
	full_date.dow = Convert_DOW(p_rx_buffer[0]);
	full_date.date = Convert_Date(p_rx_buffer[1]);
	full_date.month = Convert_Month(p_rx_buffer[2]);
	full_date.year = Convert_Year(p_rx_buffer[3]);

	return full_date;
}

DS3231_Full_Time_t DS3231_Get_Full_Time(I2C_Handle_t *p_i2c_handle)
{
	uint8_t p_tx_buffer[1] = { DS3231_SECONDS };
	uint8_t p_rx_buffer[3];
	DS3231_Full_Time_t full_time;

	// write register pointer
	I2C_Master_Send(p_i2c_handle, p_tx_buffer, 1, DS3231_SLAVE_ADDR, I2C_ENABLE_SR);
	// read four bytes - day, date, month, year
	I2C_Master_Receive(p_i2c_handle, p_rx_buffer, 3, DS3231_SLAVE_ADDR, I2C_DISABLE_SR);

	// index 0 = dow, 1 = day of month, 2 = month, 3 = year
	full_time.seconds = Convert_Seconds(p_rx_buffer[0]);
	full_time.minutes = Convert_Minutes(p_rx_buffer[1]);
	full_time.hours = Convert_Hours(p_rx_buffer[2]);

	return full_time;
}

DS3231_Datetime_t DS3231_Get_Full_Datetime(I2C_Handle_t *p_i2c_handle)
{
	uint8_t p_tx_buffer[1] = { DS3231_SECONDS };
	uint8_t p_rx_buffer[7];
	DS3231_Datetime_t datetime;

	// write register pointer
	I2C_Master_Send(p_i2c_handle, p_tx_buffer, 1, DS3231_SLAVE_ADDR, I2C_ENABLE_SR);
	// read seven bytes - seconds, minutes, hours, day, date, month, year
	I2C_Master_Receive(p_i2c_handle, p_rx_buffer, 7, DS3231_SLAVE_ADDR, I2C_DISABLE_SR);

	datetime.time = Convert_Full_Time(p_rx_buffer);
	datetime.date = Convert_Full_Date(p_rx_buffer+3);

	return datetime;
}

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

static DS3231_Full_Time_t Convert_Full_Time(uint8_t *p_rx_buffer)
{
	DS3231_Full_Time_t time;
	time.seconds = Convert_Seconds(*p_rx_buffer);
	p_rx_buffer++;
	time.minutes = Convert_Minutes(*p_rx_buffer);
	p_rx_buffer++;
	time.hours = Convert_Hours(*p_rx_buffer);
	return time;
}

static DS3231_Full_Date_t Convert_Full_Date(uint8_t *p_rx_buffer)
{
	DS3231_Full_Date_t date;
	date.dow = Convert_DOW(*p_rx_buffer);
	p_rx_buffer++;
	date.date = Convert_Date(*p_rx_buffer);
	p_rx_buffer++;
	date.month = Convert_Month(*p_rx_buffer);
	p_rx_buffer++;
	date.year = Convert_Year(*p_rx_buffer);
	return date;
}

static uint8_t Convert_Seconds(uint8_t sec_byte)
{
	return Convert_BCD_To_Binary(sec_byte);
}

static uint8_t Convert_Minutes(uint8_t min_byte)
{
	return Convert_BCD_To_Binary(min_byte);
}

static DS3231_Hours_t Convert_Hours(uint8_t hour_byte)
{
	uint8_t hour_tens;
	DS3231_Hours_t hour_struct;

	hour_struct.hour_12_24 = (hour_byte >> DS3231_12_24_BIT) & 1;

	if (hour_struct.hour_12_24 == DS3231_12_HOUR)
		hour_struct.am_pm = (hour_byte >> DS3231_AM_PM_BIT) & 1;

	// convert hour BCD to binary
	hour_tens = (hour_byte >> 4) & 1;
	hour_struct.hour = (hour_byte & 0xF) + (hour_tens * 10);

	return hour_struct;
}

static DS3231_DOW_t Convert_DOW(uint8_t dow_byte)
{
	return dow_byte & 0x7;
}

static uint8_t Convert_Month(uint8_t month_byte)
{
	// zeroes place seconds (bottom 4 bits)
	uint8_t zeroes_place = month_byte & 0xF;
	// tens place seconds (bit 4)
	uint8_t tens_place = (month_byte >> 4)& 0x1;
	return zeroes_place + (tens_place * 10);
}

static uint8_t Convert_Date(uint8_t date_byte)
{
	// zeroes place seconds (bottom 4 bits)
	uint8_t zeroes_place = date_byte & 0xF;
	// tens place seconds (bits 4-5)
	uint8_t tens_place = (date_byte >> 4)& 0x3;
	return zeroes_place + (tens_place * 10);
}

static uint8_t Convert_Year(uint8_t year_byte)
{
	// zeroes place seconds (bottom 4 bits)
	uint8_t zeroes_place = year_byte & 0xF;
	// tens place seconds (bit 4)
	uint8_t tens_place = (year_byte >> 4)& 0xF;
	return zeroes_place + (tens_place * 10);
}

// TODO: Possibly update BCD function to include how many bits for 10s place
static uint8_t Convert_BCD_To_Binary(uint8_t bcd_byte)
{
	// zeroes place seconds (bottom 4 bits)
	uint8_t zeroes_place = bcd_byte & 0xF;
	// tens place seconds (bits 4-6)
	uint8_t tens_place = (bcd_byte >> 4)& 0x7;

	return zeroes_place + (tens_place * 10);
}
