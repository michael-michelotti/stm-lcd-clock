/*
 * ds3231_rtc_driver.c
 *
 *  Created on: Feb 1, 2023
 *      Author: Michael
 */

#include "ds3231_rtc_driver.h"

static uint8_t Convert_Binary_To_BCD_Seconds_Minutes(uint8_t binary_byte);
static uint8_t Convert_Binary_To_BCD_Date(uint8_t binary_byte);
static uint8_t Convert_Hours_12_24(uint8_t current_byte, DS3231_12_24_Hour_t hour_mode);
static float Convert_Temp(uint8_t *p_rx_buffer);
static DS3231_Full_Date_t Convert_Full_Date(uint8_t *p_rx_buffer);
static DS3231_Full_Time_t Convert_Full_Time(uint8_t *p_rx_buffer);
static uint8_t Convert_Seconds(uint8_t sec_byte);
static uint8_t Convert_Minutes(uint8_t min_byte);
static DS3231_Hours_t Convert_Hours(uint8_t hour_byte);
static DS3231_DOW_t Convert_DOW(uint8_t dow_byte);
static uint8_t Convert_Month(uint8_t month_byte);
static uint8_t Convert_Date(uint8_t date_byte);
static uint8_t Convert_Year(uint8_t year_byte);
static uint8_t Convert_Binary_To_BCD_Month(uint8_t binary_byte);
static uint8_t Convert_Binary_To_BCD(uint8_t current_byte, uint8_t hour);
static uint8_t Convert_BCD_To_Binary(uint8_t bcd_byte);

/*************** CLOCK MODULE GETTER FUNCTIONS *****************/
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

float DS3231_Get_Temp(I2C_Handle_t *p_i2c_handle)
{
	uint8_t p_tx_buffer[1] = { DS3231_MSB_TEMP };
	uint8_t p_rx_buffer[2];

	// write register pointer
	I2C_Master_Send(p_i2c_handle, p_tx_buffer, 1, DS3231_SLAVE_ADDR, I2C_ENABLE_SR);
	// read seven bytes - seconds, minutes, hours, day, date, month, year
	I2C_Master_Receive(p_i2c_handle, p_rx_buffer, 2, DS3231_SLAVE_ADDR, I2C_DISABLE_SR);

	return Convert_Temp(p_rx_buffer);
}

/*************** CLOCK MODULE SETTER FUNCTIONS *****************/
void DS3231_Set_12_24_Hour(I2C_Handle_t *p_i2c_handle, DS3231_12_24_Hour_t hour_mode)
{
	DS3231_12_24_Hour_t current_mode;
	uint8_t current_hour_byte;

	// write register pointer to hours byte
	uint8_t p_tx_buffer[2] = { DS3231_HOURS, 0 };
	uint8_t p_rx_buffer[1];
	I2C_Master_Send(p_i2c_handle, p_tx_buffer, 1, DS3231_SLAVE_ADDR, I2C_ENABLE_SR);
	// if 12/24 bit (bit 6) is already set to desired mode, do nothing
	I2C_Master_Receive(p_i2c_handle, p_rx_buffer, 1, DS3231_SLAVE_ADDR, I2C_DISABLE_SR);

	current_hour_byte = *p_rx_buffer;
	current_mode = ((current_hour_byte >> DS3231_12_24_BIT) & 1);

	if (current_mode == hour_mode)
	{
		return;
	}
	else
	{
		// if 12/24 bit is not set to desired mode, set it and calculate new hour value
		if (current_mode == DS3231_12_HOUR)
		{
			// calculate new hour value
			current_hour_byte = Convert_Hours_12_24(current_hour_byte, DS3231_24_HOUR);
		}
		else
		{
			// calculate new hour value
			current_hour_byte = Convert_Hours_12_24(current_hour_byte, DS3231_12_HOUR);
		}
	}
	// rewrite hour register pointer to clock module
	p_tx_buffer[1] = current_hour_byte;
	I2C_Master_Send(p_i2c_handle, p_tx_buffer, 2, DS3231_SLAVE_ADDR, I2C_ENABLE_SR);
}


void DS3231_Set_Seconds(I2C_Handle_t *p_i2c_handle, uint8_t seconds)
{
	uint8_t p_tx_buffer[2] = { DS3231_SECONDS, Convert_Binary_To_BCD_Seconds_Minutes(seconds) };
	I2C_Master_Send(p_i2c_handle, p_tx_buffer, 2, DS3231_SLAVE_ADDR, I2C_DISABLE_SR);
}

void DS3231_Set_Minutes(I2C_Handle_t *p_i2c_handle, uint8_t minutes)
{
	uint8_t p_tx_buffer[2] = { DS3231_MINUTES, Convert_Binary_To_BCD_Seconds_Minutes(minutes) };
	I2C_Master_Send(p_i2c_handle, p_tx_buffer, 2, DS3231_SLAVE_ADDR, I2C_DISABLE_SR);
}

void DS3231_Set_Hours(I2C_Handle_t *p_i2c_handle, DS3231_Hours_t hours)
{
	uint8_t hour_byte = 0;

	hour_byte = Convert_Binary_To_BCD(hour_byte, hours.hour);
	if (hours.hour_12_24 == DS3231_12_HOUR)
	{
		// set 12/24 hour bit
		hour_byte |= (1 << DS3231_12_24_BIT);
		if (hours.am_pm == DS3231_AM)
		{
			// reset am/pm bit
			hour_byte &= ~(1 << DS3231_AM_PM_BIT);
		}
		else
		{
			// set am/pm bit
			hour_byte |= (1 << DS3231_AM_PM_BIT);
		}
	}
	else
	{
		// reset 12/24 hour bit
		hour_byte &= ~(1 << DS3231_12_24_BIT);
	}

	uint8_t p_tx_buffer[2] = { DS3231_HOURS, hour_byte };
	I2C_Master_Send(p_i2c_handle, p_tx_buffer, 2, DS3231_SLAVE_ADDR, I2C_DISABLE_SR);
}

void DS3231_Set_Day(I2C_Handle_t *p_i2c_handle, DS3231_DOW_t day_of_week)
{
	uint8_t p_tx_buffer[2] = { DS3231_DAY, day_of_week };
	I2C_Master_Send(p_i2c_handle, p_tx_buffer, 2, DS3231_SLAVE_ADDR, I2C_DISABLE_SR);
}

void DS3231_Set_Date(I2C_Handle_t *p_i2c_handle, uint8_t date)
{
	uint8_t p_tx_buffer[2] = { DS3231_DATE, Convert_Binary_To_BCD_Date(date) };
	I2C_Master_Send(p_i2c_handle, p_tx_buffer, 2, DS3231_SLAVE_ADDR, I2C_DISABLE_SR);
}

void DS3231_Set_Month(I2C_Handle_t *p_i2c_handle, uint8_t month)
{
	// need to get the current month byte in order to preserve the century
	uint8_t p_tx_buffer[2] = { DS3231_MONTH_CENTURY, 0 };
	uint8_t p_rx_buffer[1];
	uint8_t current_byte;
	uint8_t current_century_bit;

	I2C_Master_Send(p_i2c_handle, p_tx_buffer, 1, DS3231_SLAVE_ADDR, I2C_ENABLE_SR);
	I2C_Master_Receive(p_i2c_handle, p_rx_buffer, 1, DS3231_SLAVE_ADDR, I2C_DISABLE_SR);

	current_byte = *p_rx_buffer;
	current_century_bit = (current_byte >> DS3231_CENTURY_BIT) & 1;

	// clear out current month bytes (bottom 5)
	current_byte &= ~(0x1F);
	current_byte = Convert_Binary_To_BCD_Month(month);
	// reset century bit
	current_byte |= (current_century_bit << DS3231_CENTURY_BIT);

	p_tx_buffer[1] = current_byte;
	I2C_Master_Send(p_i2c_handle, p_tx_buffer, 2, DS3231_SLAVE_ADDR, I2C_DISABLE_SR);
}

void DS3231_Set_Year();

void DS3231_Set_Full_Date();
void DS3231_Set_Full_Time();

/*************** PRIVATE UTILITY FUNCTIONS *****************/
static uint8_t Convert_Hours_12_24(uint8_t current_byte, DS3231_12_24_Hour_t hour_mode)
{
	DS3231_Hours_t hour_struct;
	uint8_t hour;
	uint8_t bottom_nybble;
	uint8_t top_nybble;

	hour_struct = Convert_Hours(current_byte);
	// if 12 hour mode, current byte is in 24 hour format, needs to be converted to 12 hour format
	if (hour_mode == DS3231_12_HOUR)
	{
		// get hour struct from current byte
		hour_struct = Convert_Hours(current_byte);
		// set 12/24 hour bit to high (12 hour format)
		current_byte |= (1 << DS3231_12_24_BIT);
		// if hour value is greater than 12, set AM/PM to high (PM)
		if (hour_struct.hour > 12)
		{
			current_byte |= (1 << DS3231_AM_PM_BIT);
		}
		else
		{
			current_byte &= ~(1 << DS3231_AM_PM_BIT);
		}
		// if PM, subtract 12
		if (hour_struct.am_pm == DS3231_PM)
			hour_struct.hour -= 12;

		return Convert_Binary_To_BCD(current_byte, hour_struct.hour);
	}
	else // current byte is 12 hour mode, needs to be 24 hour
	{
		// get hour struct from current byte
		hour_struct = Convert_Hours(current_byte);
		// set 12/24 hour bit to low (24 hour format)
		current_byte &= ~(1 << DS3231_12_24_BIT);
		// convert hour to 24 hour from AM/PM
		if (hour_struct.am_pm == DS3231_PM)
			hour_struct.hour += 12;

		return Convert_Binary_To_BCD(current_byte, hour_struct.hour);
	}
}

static float Convert_Temp(uint8_t *p_rx_buffer)
{
	float temp = 0;
	float fractional = 0;
	uint8_t thing = 0;
	uint8_t temp_msb = *p_rx_buffer;
	p_rx_buffer++;
	uint8_t temp_lsb = *p_rx_buffer;

	// bottom 2 bits of temp - fractional portion
	thing = (temp_lsb >> 6);
	fractional = thing / 4;
	temp = temp_msb + fractional;

	return temp;
}

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
	{
		hour_struct.am_pm = (hour_byte >> DS3231_AM_PM_BIT) & 1;
		hour_tens = (hour_byte >> 4) & 0x1;
	}
	else
	{
		hour_struct.am_pm = DS3231_NO_AM_PM;
		hour_tens = (hour_byte >> 4) & 0x3;
	}

	// convert hour BCD to binary
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

static uint8_t Convert_Binary_To_BCD_Month(uint8_t binary_byte)
{
	uint8_t low_nybble;
	uint8_t high_nybble;

	low_nybble = binary_byte % 10;
	high_nybble = binary_byte / 10;

	return (high_nybble << 4) | (low_nybble & 0xF);
}

static uint8_t Convert_Binary_To_BCD_Date(uint8_t binary_byte)
{
	uint8_t low_nybble;
	uint8_t high_nybble;

	low_nybble = binary_byte % 10;
	high_nybble = binary_byte / 10;

	return (high_nybble << 4) | (low_nybble & 0xF);
}

static uint8_t Convert_Binary_To_BCD_Seconds_Minutes(uint8_t binary_byte)
{
	uint8_t low_nybble;
	uint8_t high_nybble;

	low_nybble = binary_byte % 10;
	high_nybble = binary_byte / 10;

	return (high_nybble << 4) | (low_nybble & 0xF);
}

// TODO: Currently only works for converting binary hour to BCD hour
static uint8_t Convert_Binary_To_BCD(uint8_t current_byte, uint8_t hour)
{
	uint8_t bottom_nybble;
	uint8_t top_nybble;

	// convert hour to BCD, then write to current byte
	bottom_nybble = hour % 10;
	top_nybble = hour / 10;

	// take bottom 4 bits of bottom nybble
	current_byte &= ~0xF;
	current_byte |= (bottom_nybble & 0xF);

	// set top nybble, clear bytes then shift 4 and then set bytes
	current_byte &= ~(0x3 << 4);
	current_byte |= (top_nybble << 4);

	return current_byte;
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
