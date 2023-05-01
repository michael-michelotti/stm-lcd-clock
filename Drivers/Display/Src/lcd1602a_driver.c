/*
 * lcd1602a_driver.c
 *
 *  Created on: Feb 4, 2023
 *      Author: Michael
 */

#include <stdlib.h>

#include "lcd1602a_driver.h"
#include "stm32f407xx.h"
#include "globals.h"

static char int_to_ascii_char(uint8_t int_to_covert);
static void int_to_zero_padded_ascii(char *result, uint8_t int_to_convert);
static void write_char(char ch);
static void write_command(uint8_t cmd_word, uint32_t address_setup_us);
static void send_nybble(uint8_t nybble);
static void clear_display();
static void return_home();
static void entry_mode_set(uint8_t inc_dec, uint8_t shift);
static void display_on_off(uint8_t disp, uint8_t cursor, uint8_t blink);
static void cursor_display_shift(uint8_t shift_or_cursor, uint8_t right_left);
static void function_set(uint8_t bit_len, uint8_t num_lines, uint8_t font);
static void set_cgram_addr(uint8_t cgram_addr);
static void set_ddram_addr(uint8_t ddram_addr);
static void pulse_enable(uint32_t us_hold_time);
static void mdelay(uint32_t cnt);
static void udelay(uint32_t cnt);
static char *convert_time_to_str(DS3231_Time_t time);
static char *convert_date_to_str(DS3231_Full_Date_t date);

// DEFINE the global state variables which were initially DECLARED in globals.h
// Any of the global state variables can be accessed from separate modules by including globals.h
// Display strings for LCD (maximum size of 16 bytes), initiated with all null characters
char global_time_str[16] = { '\0' };
char global_date_str[16] = { '\0' };

void init_global_state()
{
	// Initialize global LCD display strings with blank date and time
	char *default_time = "00:00:00";
	char *default_date = "00/00/0000";
	strncpy(global_time_str, default_time, strlen(default_time));
	strncpy(global_date_str, default_date, strlen(default_date));
}

void LCD_Power_Switch(uint8_t on_or_off)
{
	// Power switch pin - PB7
	GPIO_Pin_Config_t pin_conf = { 7, GPIO_MODE_OUT, GPIO_SPEED_LOW, GPIO_PUPD_NONE, GPIO_OUT_PP, 0 };
	GPIO_Handle_t pin_handle = { GPIOB, pin_conf };
	GPIO_Init(&pin_handle);

	if (on_or_off == ON)
	{
		GPIO_Write_To_Output_Pin(GPIOB, 7, HIGH);
	}
	else
	{
		GPIO_Write_To_Output_Pin(GPIOB, 7, LOW);
	}
}

void LCD_Initialize()
{
	// initialize GPIO pins RS, DB7-DB4
	// RS = PD0
	// DB4-7 = PD1-4
	// E = PD5
	// Initialize PA1 for RS
	GPIO_Pin_Config_t pin_conf = { RS_GPIO_PIN, GPIO_MODE_OUT, GPIO_SPEED_HIGH, GPIO_PUPD_NONE, GPIO_OUT_PP, 0 };
	GPIO_Handle_t pin_handle = { GPIOA, pin_conf };
	GPIO_Init(&pin_handle);

	// Initialize PA2 for E (Enable)
	pin_handle.gpio_pin_config.gpio_pin_num = E_GPIO_PIN;
	GPIO_Init(&pin_handle);
	// Initialize PA3 for DB4
	pin_handle.gpio_pin_config.gpio_pin_num = DB4_GPIO_PIN;
	GPIO_Init(&pin_handle);
	// Initialize PA4 for DB5
	pin_handle.gpio_pin_config.gpio_pin_num = DB5_GPIO_PIN;
	GPIO_Init(&pin_handle);
	// Initialize PA5 for DB6
	pin_handle.gpio_pin_config.gpio_pin_num = DB6_GPIO_PIN;
	GPIO_Init(&pin_handle);
	// Initialize PA6 for DB7
	pin_handle.gpio_pin_config.gpio_pin_num = DB7_GPIO_PIN;
	GPIO_Init(&pin_handle);

	// set all pins to ground (clear entire GPIOD ODR port)
	GPIO_Write_To_Output_Port(LCD_GPIO_PORT, 0);
	LCD_Power_Switch(ON);
	mdelay(40);

	// as part of initialization, 0x3 must be sent twice, then 0x2 to initiate 4-bit mode
	send_nybble(0x3);
	mdelay(5);
	send_nybble(0x3);
	udelay(150);
	send_nybble(0x3);
	send_nybble(0x2);

	// now we need to start sending 8-bit words in 2 nybbles separately (4-bit mode)
	// 0x28 = 0010 1000, sets line number to 2 and style to 5x8 dot characters
	function_set(LCD_4_BIT, LCD_2_LINES, LCD_5_8_DOTS);
	display_on_off(LCD_DISP_ON, LCD_CURSOR_ON, LCD_BLINK_ON);
	clear_display();
	entry_mode_set(LCD_INCREMENT, LCD_NO_SHIFT);
}

void LCD_Set_Cursor(uint8_t row, uint8_t column)
{
	uint8_t ddram_addr = 0;
	if (row == 2)
	{
		// most significant bit needs to be set for 2nd column
		ddram_addr |= (1 << 6);
	}
	// row number is 4 bits, 0 through F for 1 to 16
	ddram_addr |= ((column-1) & 0xF);

	set_ddram_addr(ddram_addr);
}

void LCD_Display_Str(char *str)
{
	char curr;
	while ((curr = *str) != '\0')
	{
		LCD_Display_Char(curr);
		str++;
	}
}

void LCD_Display_Char(char ch)
{
	write_char(ch);
}

void LCD_Update_Time(DS3231_Time_t time)
{
	// set cursor to time position
	LCD_Set_Cursor(DEFAULT_TIME_ROW, DEFAULT_TIME_COL);

	char *my_time_str = convert_time_to_str(time);
	LCD_Display_Str(my_time_str);
}

void LCD_Update_Date(DS3231_Full_Date_t date)
{
	// set cursor to time position
	LCD_Set_Cursor(DEFAULT_DATE_ROW, DEFAULT_DATE_COL);

	char *my_date_str = convert_date_to_str(date);
	LCD_Display_Str(my_date_str);
}

void LCD_Update_Date_And_Time(DS3231_Datetime_t datetime)
{
	LCD_Update_Time(datetime.time);
	LCD_Update_Date(datetime.date);
}

// PRIVATE UTILITY FUNCTIONS
static char int_to_ascii_char(uint8_t int_to_covert)
{
	return int_to_covert + ASCII_DIGIT_OFFSET;
}

static void int_to_zero_padded_ascii(char *result, uint8_t int_to_convert)
{
	if (int_to_convert < 10)
	{
		result[0] = '0';
		result[1] = int_to_ascii_char(int_to_convert);
	}
	else
	{
		result[0] = int_to_ascii_char(int_to_convert / 10);
		result[1] = int_to_ascii_char(int_to_convert % 10);
	}
}

static char *convert_date_to_str(DS3231_Full_Date_t date)
{
	char result_buffer[2] = { '\0' };

	global_date_str[8] = '\0';

	int_to_zero_padded_ascii(result_buffer, date.month);
	global_date_str[0] = result_buffer[0];
	global_date_str[1] = result_buffer[1];
	global_date_str[2] = '/';

	int_to_zero_padded_ascii(result_buffer, date.date);
	global_date_str[3] = result_buffer[0];
	global_date_str[4] = result_buffer[1];
	global_date_str[5] = '/';

	int_to_zero_padded_ascii(result_buffer, date.year);
	global_date_str[6] = result_buffer[0];
	global_date_str[7] = result_buffer[1];

	return global_date_str;
}

static char *convert_time_to_str(DS3231_Time_t time)
{
	// format "HH:MM:SS AM"
	// manipulates the global time string, declared in globa
	// convert hour number to HH
	char result_buffer[2] = { '\0' };

	int_to_zero_padded_ascii(result_buffer, time.hours.hour);
	global_time_str[0] = result_buffer[0];
	global_time_str[1] = result_buffer[1];
	global_time_str[2] = ':';

	int_to_zero_padded_ascii(result_buffer, time.minutes);
	global_time_str[3] = result_buffer[0];
	global_time_str[4] = result_buffer[1];
	global_time_str[5] = ':';

	int_to_zero_padded_ascii(result_buffer, time.seconds);
	global_time_str[6] = result_buffer[0];
	global_time_str[7] = result_buffer[1];

	if (time.hours.hour_12_24 == DS3231_12_HOUR)
	{
		global_time_str[8] = ' ';
		if (time.hours.am_pm == DS3231_AM)
		{
			global_time_str[9] = 'A';
		}
		else if (time.hours.am_pm == DS3231_PM)
		{
			global_time_str[9] = 'P';
		}
		global_time_str[10] = 'M';
		global_time_str[11] = '\0';
	}
	else
	{
		global_time_str[8] = '\0';
	}

	return global_time_str;
}

static void write_char(char ch)
{
	uint8_t low_nybble = (ch & 0xF);
	uint8_t high_nybble = (ch >> 4);

	// set RS high (for data not command)
	GPIO_Write_To_Output_Pin(LCD_GPIO_PORT, RS_GPIO_PIN, HIGH);
	udelay(LCD_TAS_US);

	send_nybble(high_nybble);
	send_nybble(low_nybble);
}

// RS always low, RW always low
static void write_command(uint8_t cmd_word, uint32_t address_setup_us)
{
	uint8_t low_nybble = (cmd_word & 0xF);
	uint8_t high_nybble = (cmd_word >> 4);

	// set rs pin low for command; wait tAS (address setup time, 60ns min)
	GPIO_Write_To_Output_Pin(LCD_GPIO_PORT, RS_GPIO_PIN, LOW);
	udelay(address_setup_us);

	send_nybble(high_nybble);
	send_nybble(low_nybble);
}

static void send_nybble(uint8_t nybble)
{
	// set pins with nybble value, data will be valid before enable pulse starts
	GPIO_Write_To_Output_Pin(LCD_GPIO_PORT, DB4_GPIO_PIN, ((nybble >> 0) & 1));
	GPIO_Write_To_Output_Pin(LCD_GPIO_PORT, DB5_GPIO_PIN, ((nybble >> 1) & 1));
	GPIO_Write_To_Output_Pin(LCD_GPIO_PORT, DB6_GPIO_PIN, ((nybble >> 2) & 1));
	GPIO_Write_To_Output_Pin(LCD_GPIO_PORT, DB7_GPIO_PIN, ((nybble >> 3) & 1));

	// send nybble to LCD by pulsing enable, then delay 1us for enable pulse width (450ns min)
	pulse_enable(ENALBE_PULSE_US);
	// wait 1us for total cycle
	// (data must be held valid for 10ns, enable cannot pulse high again for 500ns)
	udelay(LCD_HOLD_TIME_US);
}

static void clear_display()
{
	// the command to clear display is always 0x1
	write_command(CLEAR_DISPLAY, LCD_TAS_US);
	// 2ms delay - clear display takes ~1.5ms for LCD to internally process
	mdelay(2);
}

static void return_home()
{
	// the command to return home is always 0x2
	write_command(RETURN_HOME, LCD_TAS_US);
	// 2ms delay - clear display takes ~1.5ms for LCD to internally process
	mdelay(2);
}

static void entry_mode_set(uint8_t inc_dec, uint8_t shift)
{
	uint8_t cmd_byte = 0x04;
	cmd_byte |= (inc_dec << 1) + (shift << 0);
	write_command(cmd_byte, LCD_TAS_US);
}

static void display_on_off(uint8_t disp, uint8_t cursor, uint8_t blink)
{
	uint8_t cmd_byte = 0x08;
	cmd_byte |= (disp << 2) + (cursor << 1) + (blink << 0);
	write_command(cmd_byte, LCD_TAS_US);
}

static void cursor_display_shift(uint8_t shift_or_cursor, uint8_t right_left)
{
	uint8_t cmd_byte = 0x10;
	cmd_byte |= (shift_or_cursor << 3) + (right_left << 2);
	write_command(cmd_byte, LCD_TAS_US);
}

static void function_set(uint8_t bit_len, uint8_t num_lines, uint8_t font)
{
	uint8_t cmd_byte = 0x20;
	cmd_byte |= (bit_len << 4) + (num_lines << 3) + (font << 2);
	write_command(cmd_byte, LCD_TAS_US);
}

static void set_cgram_addr(uint8_t cgram_addr)
{
	uint8_t cmd_byte = 0x40;
	cmd_byte |= (cgram_addr & 0x3F);
	write_command(cmd_byte, LCD_TAS_US);
}

static void set_ddram_addr(uint8_t ddram_addr)
{
	uint8_t cmd_byte = 0x80;
	cmd_byte |= (ddram_addr & 0x7F);
	write_command(cmd_byte, LCD_TAS_US);
}

static void pulse_enable(uint32_t us_hold_time)
{
	// pulse E again for 1us,
	GPIO_Write_To_Output_Pin(LCD_GPIO_PORT, E_GPIO_PIN, HIGH);
	udelay(us_hold_time);
	GPIO_Write_To_Output_Pin(LCD_GPIO_PORT, E_GPIO_PIN, LOW);
}

static void mdelay(uint32_t cnt)
{
	for(uint32_t i=0; i < (cnt * 1000); i++);
}

static void udelay(uint32_t cnt)
{
	for(uint32_t i=0; i < (cnt * 1); i++);
}
