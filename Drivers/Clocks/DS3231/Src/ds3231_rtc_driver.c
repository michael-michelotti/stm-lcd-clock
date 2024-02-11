#include <stdlib.h>

#include "globals.h"
#include "ds3231_rtc_driver.h"

/*************** CONVERSION FUNCTIONS FROM DS3231 REGISTER FORMAT *****************/
static seconds_t Convert_Seconds_From_DS3231(uint8_t sec_byte);
static minutes_t Convert_Minutes_From_DS3231(uint8_t min_byte);
static hours_t Convert_Hours_From_DS3231(uint8_t hour_byte);
static day_of_week_t Convert_Day_From_DS3231(uint8_t dow_byte);
static date_t Convert_Date_From_DS3231(uint8_t date_byte);
static month_t Convert_Month_From_DS3231(uint8_t month_byte);
static year_t Convert_Year_From_DS3231(uint8_t year_byte);
static full_time_t Convert_Full_Time_From_DS3231(uint8_t *p_rx_buffer);
static full_date_t Convert_Full_Date_From_DS3231(uint8_t *p_rx_buffer);
static full_datetime_t Convert_Datetime_From_DS3231(uint8_t *p_rx_buffer);
static float Convert_Temp_From_DS3231(uint8_t *p_rx_buffer);

/*************** CONVERSION FUNCTIONS TO DS3231 REGISTER FORMAT *****************/
static uint8_t Convert_Seconds_To_DS3231(seconds_t seconds);
static uint8_t Convert_Minutes_To_DS3231(minutes_t minutes);
static uint8_t Convert_Hours_To_DS3231(hours_t hour_struct);
static uint8_t Convert_Day_To_DS3231(day_of_week_t day);
static uint8_t Convert_Date_To_DS3231(date_t date);
static uint8_t Convert_Month_To_DS3231(month_t month);
static uint8_t Convert_Year_To_DS3231(year_t year);
static uint8_t *Convert_Time_To_DS3231(full_time_t full_time);
static uint8_t *Convert_Full_Date_To_DS3231(full_date_t full_date);
static uint8_t *Convert_Dateime_To_DS3231(full_datetime_t datetime);

/*************** GENERAL UTILITY FUNCTIONS *****************/
//static void Read_From_DS3231_IT(uint8_t *p_rx_buffer, uint8_t ds3231_addr, uint8_t len);
static void Read_From_DS3231(I2C_Handle_t *p_i2c_handle, uint8_t *p_rx_buffer, uint8_t ds3231_addr, uint8_t len);
static void Write_To_DS3231(I2C_Handle_t *p_i2c_handle, uint8_t *p_tx_buffer, uint8_t len);
//static void Write_To_DS3231_IT(uint8_t *p_tx_buffer, uint8_t len);
static uint8_t Convert_Hours_12_24(uint8_t current_byte, hour_format_t new_mode);
static uint8_t Convert_Binary_To_BCD(uint8_t binary_byte);
static uint8_t Convert_BCD_To_Binary(uint8_t bcd_byte);

typedef struct
{
	DS3231_State_t 		state;
	DS3231_Unit_t		unit;
	seconds_t			curr_secs;
	minutes_t			curr_mins;
} DS3231_Handle_t;

static DS3231_Handle_t ds3231_handle = { DS3231_STATE_IDLE, 0, 0 };

Clock_Driver_t get_clock()
{
	Clock_Driver_t ds3231_clock_driver = {
			.Initialize				= DS3231_Initialize,

			.Get_Seconds 			= DS3231_Get_Seconds,
			.Get_Seconds_IT			= DS3231_Get_Seconds_IT,
			.Get_Minutes 			= DS3231_Get_Minutes,
			.Get_Minutes_IT			= DS3231_Get_Minutes_IT,
			.Get_Hours 				= DS3231_Get_Hours,
			.Get_Day_Of_Week 		= DS3231_Get_Day_Of_Week,
			.Get_Date				= DS3231_Get_Date,
			.Get_Month				= DS3231_Get_Month,
			.Get_Year				= DS3231_Get_Year,
			.Get_Full_Date			= DS3231_Get_Full_Date,
			.Get_Full_Time			= DS3231_Get_Full_Time,
			.Get_Full_Datetime 		= DS3231_Get_Full_Datetime,

			.Set_Seconds			= DS3231_Set_Seconds,
			.Set_Seconds_IT			= DS3231_Set_Seconds_IT,
			.Set_Minutes			= DS3231_Set_Minutes,
			.Set_Hours				= DS3231_Set_Hours,
			.Set_Day_Of_Week		= DS3231_Set_Day,
			.Set_Date				= DS3231_Set_Date,
			.Set_Month				= DS3231_Set_Month,
			.Set_Year				= DS3231_Set_Year,
			.Set_Full_Date			= DS3231_Set_Full_Date,
			.Set_Full_Time			= DS3231_Set_Full_Time,
			.Set_Full_Datetime		= DS3231_Set_Full_Datetime
	};

	return ds3231_clock_driver;
}


void DS3231_Initialize(void)
{
	I2C_Interface_t i2c_interface = get_i2c_interface();
	i2c_interface.Initialize();
}

/*************** CLOCK MODULE GETTER FUNCTIONS *****************/
/*
static void Read_From_DS3231_IT(I2C_Interface_t *i2c_interface, uint8_t len)
{
	// Before reading data from DS3231, you must write an 8-bit register pointer
	//ds3231_state = DS3231_STATE_WRITE_POINTER_FOR_READ;
	//i2c_tx_buffer[i2c_tx_buffer_pos] = ds3231_addr;
	//i2c_interface->I2C_Write_IT(i2c_tx_buffer, 1, DS3231_SLAVE_ADDR, I2C_DISABLE_SR);
	hal_i2c_interface.Read_Bytes(*p_rx_buffer, len, slave_addr);
}
*/

/*
void DS3231_I2C_Receive_Complete_Callback(void)
{
}
*/

/*
void DS3231_I2C_Transfer_Complete_Callback(I2C_Handle_t *p_i2c_handle)
{
	switch (ds3231_state)
	{
	case S3231_STATE_WRITING_POINTER_FOR_WRITE:
		// finished writing pointer, ready to write data now!
		ds3231_state = DS3231_STATE_WRITING_DATA;
		app_hal_driver->HAL_I2C_Write_IT(i2c_tx_buffer, p_i2c_handle->tx_len, p_i2c_handle->slave_addr, p_i2c_handle->sr);
		break;
	case DS3231_STATE_WRITE_POINTER_FOR_READ:
		// finished writing pointer, ready to read data now!
		ds3231_state = DS3231_STATE_READING_DATA;
		app_hal_driver->HAL_I2C_Read_IT(i2c_rx_buffer, len, DS3231_SLAVE_ADDR, I2C_ENABLE_SR);
		break;
	case DS3231_STATE_WRITING_DATA:
		// completed transaction, setting state to idle!
		ds3231_state = DS3231_STATE_IDLE;
		break;
	default:
		// state is not valid, returning an error status
		return -1;
	}
}
*/

/*
void I2C_Receive_Complete_Callback(I2C_Handle_t *p_i2c_handle)
{
	switch (ds3231_state)
	{
	case DS3231_STATE_READING_DATA:
		// completed transaction, setting state to idle!
		ds3231_state = DS3231_STATE_IDLE;
		break;
	default:
		// state is not valid, returning an error status
		return -1;
	}
}
*/


static void Read_From_DS3231(I2C_Handle_t *p_i2c_handle, uint8_t *p_rx_buffer, uint8_t ds3231_addr, uint8_t len)
{
	/*
	uint8_t p_tx_buffer[1] = { ds3231_addr };
	I2C_Master_Send(p_i2c_handle, p_tx_buffer, 1, DS3231_SLAVE_ADDR, I2C_ENABLE_SR);
	I2C_Master_Receive(p_i2c_handle, p_rx_buffer, len, DS3231_SLAVE_ADDR, I2C_DISABLE_SR);
	*/
}


/*
static void Write_To_DS3231_IT(uint8_t len)
{
	// do not need SR for a write command - will never be switching from write to read
	i2c_interface->I2C_Write_IT(i2c_tx_buffer, len, DS3231_SLAVE_ADDR, I2C_DISABLE_SR);
}
*/


static void Write_To_DS3231(I2C_Handle_t *p_i2c_handle, uint8_t *p_tx_buffer, uint8_t len)
{
	//I2C_Master_Send(p_i2c_handle, p_tx_buffer, len, DS3231_SLAVE_ADDR, I2C_DISABLE_SR);
}


seconds_t DS3231_Get_Seconds(void)
{
	uint8_t p_rx_buffer[1];
	uint8_t p_tx_buffer[1] = { DS3231_SECONDS };
	I2C_Interface_t i2c_interface = get_i2c_interface();
	// First, I need to write the memory pointer to the RTC chip, so it's pointing at the seconds
	i2c_interface.Write_Bytes(p_tx_buffer, 1, DS3231_SLAVE_ADDR, I2C_ENABLE_SR);

	// Then, I need to read the byte at that memory, which will be my seconds value
	i2c_interface.Read_Bytes(p_rx_buffer, 1, DS3231_SLAVE_ADDR, I2C_DISABLE_SR);
	return Convert_Seconds_From_DS3231(*p_rx_buffer);
}

void DS3231_Get_Seconds_IT(void)
{
	if (ds3231_handle.state != DS3231_STATE_IDLE)
	{
		return;
	}

	uint8_t p_tx_buffer[1] = { DS3231_SECONDS };
	I2C_Interface_t i2c_interface = get_i2c_interface();
	// First, I need to write the memory pointer to the RTC chip, so it's pointing at the seconds
	ds3231_handle.state = DS3231_STATE_POINTER_WRITE_FOR_READ;
	ds3231_handle.unit = DS3231_UNIT_SECONDS;
	i2c_interface.Write_Bytes_IT(p_tx_buffer, 1, DS3231_SLAVE_ADDR, I2C_ENABLE_SR);
	return;
}

void DS3231_Get_Minutes_IT(void)
{
	if (ds3231_handle.state != DS3231_STATE_IDLE)
	{
		return;
	}

	uint8_t p_tx_buffer[1] = { DS3231_MINUTES };
	I2C_Interface_t i2c_interface = get_i2c_interface();
	// First, I need to write the memory pointer to the RTC chip, so it's pointing at the seconds
	ds3231_handle.state = DS3231_STATE_POINTER_WRITE_FOR_READ;
	ds3231_handle.unit = DS3231_UNIT_MINUTES;
	i2c_interface.Write_Bytes_IT(p_tx_buffer, 1, DS3231_SLAVE_ADDR, I2C_ENABLE_SR);
	return;
}

void DS3231_Set_Seconds_IT(seconds_t seconds)
{
	if (ds3231_handle.state != DS3231_STATE_IDLE)
	{
		return;
	}

	uint8_t p_tx_buffer[2] = { DS3231_SECONDS, Convert_Seconds_To_DS3231(seconds) };
	I2C_Interface_t i2c_interface = get_i2c_interface();

	// First, I need to write the memory pointer to the RTC chip, so it's pointing at the seconds
	ds3231_handle.state = DS3231_STATE_DATA_WRITE;
	ds3231_handle.unit = DS3231_UNIT_SECONDS;
	i2c_interface.Write_Bytes_IT(p_tx_buffer, 2, DS3231_SLAVE_ADDR, I2C_ENABLE_SR);
}

void I2C_Master_Send_Complete_Callback(I2C_Handle_t *p_i2c_handle)
{
	I2C_Interface_t i2c_interface = get_i2c_interface();
	uint8_t p_rx_buffer[1];

	switch (ds3231_handle.state)
	{
		case DS3231_STATE_POINTER_WRITE_FOR_READ:
			if (ds3231_handle.unit == DS3231_UNIT_SECONDS)
			{
				ds3231_handle.state = DS3231_STATE_DATA_READ;
				i2c_interface.Read_Bytes_IT(p_rx_buffer, 1, DS3231_SLAVE_ADDR, I2C_DISABLE_SR);
			}
			else if (ds3231_handle.unit == DS3231_UNIT_MINUTES)
			{
				ds3231_handle.state = DS3231_STATE_DATA_READ;
				i2c_interface.Read_Bytes_IT(p_rx_buffer, 1, DS3231_SLAVE_ADDR, I2C_DISABLE_SR);
			}
			break;
		case DS3231_STATE_DATA_WRITE:
			ds3231_handle.state = DS3231_STATE_IDLE;
			printf("Successfully set seconds\n");
			break;
	}
}

void I2C_Master_Receive_Complete_Callback(I2C_Handle_t p_i2c_handle)
{
	switch (ds3231_handle.unit)
	{
		case DS3231_UNIT_SECONDS:
			seconds_t new_secs = Convert_Seconds_From_DS3231(*(p_i2c_handle.p_rx_buffer - 1));
			ds3231_handle.state = DS3231_STATE_IDLE;
			Clock_Get_Seconds_Complete_Callback(new_secs);
			break;
		case DS3231_UNIT_MINUTES:
			minutes_t new_mins = Convert_Minutes_From_DS3231(*(p_i2c_handle.p_rx_buffer - 1));
			ds3231_handle.state = DS3231_STATE_IDLE;
			Clock_Get_Minutes_Complete_Callback(new_mins);
			break;
	}
}

minutes_t DS3231_Get_Minutes(void)
{
	uint8_t p_rx_buffer[1];
	//Read_From_DS3231(p_i2c_handle, p_rx_buffer, DS3231_MINUTES, 1);
	return Convert_Minutes_From_DS3231(*p_rx_buffer);
}

hours_t DS3231_Get_Hours(void)
{
	uint8_t p_rx_buffer[1];
	//Read_From_DS3231(p_i2c_handle, p_rx_buffer, DS3231_HOURS, 1);
	return Convert_Hours_From_DS3231(*p_rx_buffer);
}

day_of_week_t DS3231_Get_Day_Of_Week(void)
{
	uint8_t p_rx_buffer[1];
	//Read_From_DS3231(p_i2c_handle, p_rx_buffer, DS3231_DAY, 1);
	return Convert_Day_From_DS3231(*p_rx_buffer);
}

date_t DS3231_Get_Date(void)
{
	uint8_t p_rx_buffer[1];
	//Read_From_DS3231(p_i2c_handle, p_rx_buffer, DS3231_DATE, 1);
	return Convert_Date_From_DS3231(*p_rx_buffer);
}

month_t DS3231_Get_Month(void)
{
	uint8_t p_rx_buffer[1];
	//Read_From_DS3231(p_i2c_handle, p_rx_buffer, DS3231_MONTH_CENTURY, 1);
	return Convert_Month_From_DS3231(*p_rx_buffer);
}

/*
 * Not going to implement century APIs for the time being
uint8_t DS3231_Get_Century(void)
{
	uint8_t p_rx_buffer[1];
	Read_From_DS3231(p_i2c_handle, p_rx_buffer, DS3231_MONTH_CENTURY, 1);
	return (*p_rx_buffer) >> 7;
}

void DS3231_Get_Month_Century(void)
{
	DS3231_Month_Century_t m_c_struct;
	m_c_struct.month = DS3231_Get_Month(p_i2c_handle);
	m_c_struct.century = DS3231_Get_Century(p_i2c_handle);
	return m_c_struct;
}
*/

year_t DS3231_Get_Year(void)
{
	uint8_t p_rx_buffer[1];
	//Read_From_DS3231(p_i2c_handle, p_rx_buffer, DS3231_YEAR, 1);
	return Convert_Year_From_DS3231(*p_rx_buffer);
}

full_date_t DS3231_Get_Full_Date(void)
{
	uint8_t p_rx_buffer[FULL_DATE_LEN];
	full_date_t full_date;
	//Read_From_DS3231(p_i2c_handle, p_rx_buffer, DS3231_DAY, FULL_DATE_LEN);
	full_date.day_of_week = Convert_Day_From_DS3231(p_rx_buffer[0]);
	full_date.date = Convert_Date_From_DS3231(p_rx_buffer[1]);
	full_date.month = Convert_Month_From_DS3231(p_rx_buffer[2]);
	full_date.year = Convert_Year_From_DS3231(p_rx_buffer[3]);

	return full_date;
}

full_time_t DS3231_Get_Full_Time(void)
{
	uint8_t p_rx_buffer[FULL_TIME_LEN];
	full_time_t full_time;
	//Read_From_DS3231(p_i2c_handle, p_rx_buffer, DS3231_SECONDS, FULL_TIME_LEN);
	full_time.seconds = Convert_Seconds_From_DS3231(p_rx_buffer[0]);
	full_time.minutes = Convert_Minutes_From_DS3231(p_rx_buffer[1]);
	full_time.hours = Convert_Hours_From_DS3231(p_rx_buffer[2]);

	return full_time;
}

full_datetime_t DS3231_Get_Full_Datetime(void)
{
	uint8_t p_rx_buffer[FULL_DATETIME_LEN];
	//Read_From_DS3231(p_i2c_handle, p_rx_buffer, DS3231_SECONDS, FULL_DATETIME_LEN);
	return Convert_Datetime_From_DS3231(p_rx_buffer);
}

float DS3231_Get_Temp(void)
{
	uint8_t p_rx_buffer[2];
	//Read_From_DS3231(p_i2c_handle, p_rx_buffer, DS3231_MSB_TEMP, FULL_DATETIME_LEN);
	return Convert_Temp_From_DS3231(p_rx_buffer);
}

/***************************************************************/
/***************************************************************/
/***************** DS3231 RTC SETTER FUNCTIONS *****************/
/***************************************************************/
/***************************************************************/
void DS3231_Set_Seconds(seconds_t seconds)
{
	/*
	i2c_tx_buffer[0] = DS3231_SECONDS;
	i2c_tx_buffer[1] = Convert_Seconds_To_DS3231(seconds);
	Write_To_DS3231_IT(2);
	*/
}

void DS3231_Set_Minutes(minutes_t mins)
{
	uint8_t p_tx_buffer[2] = { DS3231_MINUTES, Convert_Minutes_To_DS3231(mins) };
	//Write_To_DS3231(p_i2c_handle, p_tx_buffer, 2);
}

void DS3231_Set_Hours(hours_t hours)
{
	uint8_t p_tx_buffer[2] = { DS3231_HOURS, Convert_Hours_To_DS3231(hours) };
	//Write_To_DS3231(p_i2c_handle, p_tx_buffer, 2);
}

void DS3231_Set_Day(day_of_week_t dow)
{
	uint8_t p_tx_buffer[2] = { DS3231_DAY, Convert_Day_To_DS3231(dow) };
	//Write_To_DS3231(p_i2c_handle, p_tx_buffer, 2);
}

void DS3231_Set_Date(date_t date)
{
	uint8_t p_tx_buffer[2] = { DS3231_DATE, Convert_Date_To_DS3231(date) };
	//Write_To_DS3231(p_i2c_handle, p_tx_buffer, 2);
}

void DS3231_Set_Month(month_t month)
{
	uint8_t p_tx_buffer[2] = { DS3231_MONTH_CENTURY, Convert_Month_To_DS3231(month) };
	//Write_To_DS3231(p_i2c_handle, p_tx_buffer, 2);
}

void DS3231_Set_Year(year_t year)
{
	uint8_t p_tx_buffer[2] = { DS3231_YEAR, Convert_Year_To_DS3231(year) };
	//Write_To_DS3231(p_i2c_handle, p_tx_buffer, 2);
}

void DS3231_Set_Full_Date(full_date_t full_date)
{
	uint8_t *date_bytes = Convert_Full_Date_To_DS3231(full_date);
	date_bytes[0] = DS3231_DAY;
	/*
	if (blocking == DS3231_BLOCKING_CALL)
		Write_To_DS3231(p_i2c_handle, date_bytes, FULL_DATE_LEN+1);
	else
		Write_To_DS3231_IT(date_bytes, FULL_DATE_LEN+1);
	*/
	free(date_bytes);
}

void DS3231_Set_Full_Time(full_time_t full_time)
{
	uint8_t *time_bytes = Convert_Time_To_DS3231(full_time);
	time_bytes[0] = DS3231_SECONDS;
	//Write_To_DS3231(p_i2c_handle, time_bytes, FULL_TIME_LEN+1);
	free(time_bytes);
}

void DS3231_Set_Full_Datetime(full_datetime_t full_datetime)
{
	uint8_t *datetime_bytes = Convert_Dateime_To_DS3231(full_datetime);
	datetime_bytes[0] = DS3231_SECONDS;
	//Write_To_DS3231(p_i2c_handle, datetime_bytes, FULL_DATETIME_LEN+1);
	free(datetime_bytes);
}

void DS3231_Convert_Hour_Format(I2C_Handle_t *p_i2c_handle, hour_format_t new_mode)
{
	hour_format_t current_mode;
	uint8_t current_hour_byte;
	uint8_t new_hour_byte;

	// receive current hours byte first
	uint8_t p_tx_buffer[2] = { DS3231_HOURS, 0 };
	uint8_t p_rx_buffer[1];
	Read_From_DS3231(p_i2c_handle, p_rx_buffer, DS3231_HOURS, 1);

	current_hour_byte = *p_rx_buffer;
	current_mode = ((current_hour_byte >> DS3231_12_24_BIT) & 1);

	if (new_mode == current_mode)
	{
		// hours are already in the desired format. do nothing.
		return;
	}
	else
	{
		// if 12/24 bit is not set to desired mode, set it and calculate new hour value
		if (current_mode == HOUR_FORMAT_12_HOUR)
		{
			// calculate new hour value
			new_hour_byte = Convert_Hours_12_24(current_hour_byte, HOUR_FORMAT_24_HOUR);
		}
		else
		{
			// calculate new hour value
			new_hour_byte = Convert_Hours_12_24(current_hour_byte, HOUR_FORMAT_12_HOUR);
		}
	}
	// rewrite hour register pointer to clock module
	p_tx_buffer[1] = new_hour_byte;
	//I2C_Master_Send(p_i2c_handle, p_tx_buffer, 2, DS3231_SLAVE_ADDR, I2C_ENABLE_SR);
}


/*************** PRIVATE UTILITY FUNCTIONS *****************/

/*************** CONVERSION FUNCTIONS FROM DS3231 REGISTER FORMAT *****************/
static seconds_t Convert_Seconds_From_DS3231(uint8_t sec_byte)
{
	return Convert_BCD_To_Binary(sec_byte);
}

static minutes_t Convert_Minutes_From_DS3231(uint8_t min_byte)
{
	return Convert_BCD_To_Binary(min_byte);
}

static hours_t Convert_Hours_From_DS3231(uint8_t hour_byte)
{
	uint8_t hour_tens;
	hours_t hour_struct;

	hour_struct.hour_format = (hour_byte >> DS3231_12_24_BIT) & 1;

	if (hour_struct.hour_format == HOUR_FORMAT_12_HOUR)
	{
		hour_struct.am_pm = (hour_byte >> DS3231_AM_PM_BIT) & 1;
		hour_tens = (hour_byte >> 4) & 0x1;
	}
	else
	{
		hour_struct.am_pm = AM_PM_NONE;
		hour_tens = (hour_byte >> 4) & 0x3;
	}

	// convert hour BCD to binary
	hour_struct.hour = (hour_byte & 0xF) + (hour_tens * 10);

	return hour_struct;
}

static day_of_week_t Convert_Day_From_DS3231(uint8_t dow_byte)
{
	return dow_byte & 0x7;
}

static date_t Convert_Date_From_DS3231(uint8_t date_byte)
{
	return Convert_BCD_To_Binary(date_byte);
}

static month_t Convert_Month_From_DS3231(uint8_t month_byte)
{
	return Convert_BCD_To_Binary(month_byte);
}

static year_t Convert_Year_From_DS3231(uint8_t year_byte)
{
	return Convert_BCD_To_Binary(year_byte);
}

static full_time_t Convert_Full_Time_From_DS3231(uint8_t *p_rx_buffer)
{
	full_time_t time;
	time.seconds = Convert_Seconds_From_DS3231(p_rx_buffer[0]);
	time.minutes = Convert_Minutes_From_DS3231(p_rx_buffer[1]);
	time.hours = Convert_Hours_From_DS3231(p_rx_buffer[2]);
	return time;
}

static full_date_t Convert_Full_Date_From_DS3231(uint8_t *p_rx_buffer)
{
	full_date_t date;
	date.day_of_week = Convert_Day_From_DS3231(p_rx_buffer[0]);
	date.date = Convert_Date_From_DS3231(p_rx_buffer[1]);
	date.month = Convert_Month_From_DS3231(p_rx_buffer[2]);
	date.year = Convert_Year_From_DS3231(p_rx_buffer[3]);
	return date;
}

static full_datetime_t Convert_Datetime_From_DS3231(uint8_t *p_rx_buffer)
{
	full_datetime_t datetime;
	datetime.time = Convert_Full_Time_From_DS3231(p_rx_buffer);
	datetime.date = Convert_Full_Date_From_DS3231(p_rx_buffer+FULL_TIME_LEN);
	return datetime;
}

static float Convert_Temp_From_DS3231(uint8_t *p_rx_buffer)
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

/*************** CONVERSION FUNCTIONS TO DS3231 REGISTER FORMAT *****************/
static uint8_t Convert_Seconds_To_DS3231(seconds_t seconds)
{
	return Convert_Binary_To_BCD(seconds);
}

static uint8_t Convert_Minutes_To_DS3231(minutes_t minutes)
{
	return Convert_Binary_To_BCD(minutes);
}

static uint8_t Convert_Hours_To_DS3231(hours_t hour_struct)
{
	// convert hours to BCD first
	uint8_t ds3231_hour_byte = Convert_Binary_To_BCD(hour_struct.hour);

	// after converting to BCD, set the utility bits (am/pm, 12/24)
	if (hour_struct.hour_format == HOUR_FORMAT_12_HOUR)
	{
		// set 12/24 hour bit
		ds3231_hour_byte |= (1 << DS3231_12_24_BIT);
		// set am/pm bit (0 if am, 1 if pm)
		ds3231_hour_byte |= (hour_struct.am_pm << DS3231_AM_PM_BIT);
	}

	return ds3231_hour_byte;
}

static uint8_t Convert_Day_To_DS3231(day_of_week_t day)
{
	return day & 0x7;
}

static uint8_t Convert_Date_To_DS3231(date_t date)
{
	return Convert_Binary_To_BCD(date);
}

static uint8_t Convert_Month_To_DS3231(month_t month)
{
	return Convert_Binary_To_BCD(month);
}

static uint8_t Convert_Year_To_DS3231(year_t year)
{
	return Convert_Binary_To_BCD(year);
}

static uint8_t *Convert_Time_To_DS3231(full_time_t full_time)
{
	uint8_t *p_ds3231_time_registers = malloc((FULL_TIME_LEN+1) * sizeof(uint8_t));
	p_ds3231_time_registers[1] = Convert_Seconds_To_DS3231(full_time.seconds);
	p_ds3231_time_registers[2] = Convert_Minutes_To_DS3231(full_time.minutes);
	p_ds3231_time_registers[3] = Convert_Hours_To_DS3231(full_time.hours);
	return p_ds3231_time_registers;
}

static uint8_t *Convert_Full_Date_To_DS3231(full_date_t full_date)
{
	uint8_t *p_ds3231_time_registers = malloc((FULL_DATE_LEN+1) * sizeof(uint8_t));
	p_ds3231_time_registers[1] = Convert_Day_To_DS3231(full_date.day_of_week);
	p_ds3231_time_registers[2] = Convert_Date_To_DS3231(full_date.date);
	p_ds3231_time_registers[3] = Convert_Month_To_DS3231(full_date.month);
	p_ds3231_time_registers[4] = Convert_Year_To_DS3231(full_date.year);
	return p_ds3231_time_registers;
}

static uint8_t *Convert_Dateime_To_DS3231(full_datetime_t datetime)
{
	uint8_t *p_ds3231_time_registers = malloc((FULL_DATETIME_LEN+1) * sizeof(uint8_t));
	p_ds3231_time_registers[1] = Convert_Seconds_To_DS3231(datetime.time.seconds);
	p_ds3231_time_registers[2] = Convert_Minutes_To_DS3231(datetime.time.minutes);
	p_ds3231_time_registers[3] = Convert_Hours_To_DS3231(datetime.time.hours);
	p_ds3231_time_registers[4] = Convert_Day_To_DS3231(datetime.date.day_of_week);
	p_ds3231_time_registers[5] = Convert_Date_To_DS3231(datetime.date.date);
	p_ds3231_time_registers[6] = Convert_Month_To_DS3231(datetime.date.month);
	p_ds3231_time_registers[7] = Convert_Year_To_DS3231(datetime.date.year);
	return p_ds3231_time_registers;
}

/*************** GENERAL UTILITY FUNCTIONS *****************/
static uint8_t Convert_Hours_12_24(uint8_t current_byte, hour_format_t new_mode)
{
	hours_t hour_struct;
	uint8_t new_byte;

	hour_struct = Convert_Hours_From_DS3231(current_byte);
	// if 12 hour mode requested, current byte is in 24 hour format, needs to be converted to 12 hour format
	if (new_mode == HOUR_FORMAT_12_HOUR)
	{
		if (hour_struct.hour >= 12)
		{
			hour_struct.hour -= 12;
			new_byte = Convert_Binary_To_BCD(hour_struct.hour);
			current_byte |= (1 << DS3231_AM_PM_BIT);
		}
		// set 12/24 hour bit to high (12 hour format)
		current_byte |= (1 << DS3231_12_24_BIT);
		return new_byte;
	}
	else if (new_mode == HOUR_FORMAT_24_HOUR)
	{
		// get hour struct from current byte
		hour_struct = Convert_Hours_From_DS3231(current_byte);
		if (hour_struct.am_pm == AM_PM_PM)
			hour_struct.hour += 12;
		return Convert_Binary_To_BCD(hour_struct.hour);
	}
	return -1;
}

// TODO: Currently only works for converting to 2 digit BCD (99 is max value)
static uint8_t Convert_Binary_To_BCD(uint8_t binary_byte)
{
	uint8_t low_nybble = binary_byte % 10;
	uint8_t high_nybble = binary_byte / 10;

	return ((high_nybble << 4) | low_nybble);
}

// TODO: Currently only works for converting to 2 digit BCD (99 is max value)
static uint8_t Convert_BCD_To_Binary(uint8_t bcd_byte)
{
	// zeroes place (bottom 4 bits)
	uint8_t zeroes_place = bcd_byte & 0xF;
	// tens place (top 4 bits)
	uint8_t tens_place = (bcd_byte >> 4)& 0x7;

	return zeroes_place + (tens_place * 10);
}
