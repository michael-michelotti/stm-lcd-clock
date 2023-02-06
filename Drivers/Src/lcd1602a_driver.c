/*
 * lcd1602a_driver.c
 *
 *  Created on: Feb 4, 2023
 *      Author: Michael
 */

#include "lcd1602a_driver.h"
#include "stm32f407xx.h"

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
// static uint8_t construct_command(LCD_1602A_Commands_t cmd);

static uint8_t construct_clear_display_cmd();
static uint8_t construct_return_home_cmd();
static uint8_t construct_entry_mode_set_cmd(uint8_t inc_dec, uint8_t shift);
static uint8_t construct_display_on_off_ctrl_cmd(uint8_t disp, uint8_t cursor, uint8_t blink);
static uint8_t construct_cursor_display_shift_cmd(uint8_t shift_or_cursor, uint8_t right_left);
static uint8_t construct_function_set_cmd(uint8_t bit_len, uint8_t num_lines, uint8_t font);
static uint8_t constrcut_set_cgram_addr_cmd(uint8_t cgram_addr);
static uint8_t constrcut_set_ddram_addr_cmd(uint8_t ddram_addr);
static void pulse_enable(uint32_t us_hold_time);
static void mdelay(uint32_t cnt);
static void udelay(uint32_t cnt);

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

	LCD_Power_Switch(ON);
	mdelay(40);

	// set all pins to ground (clear entire GPIOD ODR port)
	GPIO_Write_To_Output_Port(LCD_GPIO_PORT, 0);
	mdelay(1);

	// init function set nybble
	send_nybble(0x3);
	mdelay(5);

	// send second nybble for initializaiton
	send_nybble(0x3);
	udelay(150);
	send_nybble(0x3);

	// last part of init, send 0010 for 4-bit mode
	send_nybble(0x2);

	// now we need to start sending 8-bit words in 2 nybbles separately (4-bit mode)
	// 0x28 = 0010 1000, sets line number to 2 and style to 5x8 dot characters
	function_set(LCD_4_BIT, LCD_2_LINES, LCD_5_8_DOTS);
	// display_on_off(LCD_DISP_OFF, LCD_CURSOR_OFF, LCD_BLINK_OFF);
	display_on_off(LCD_DISP_ON, LCD_CURSOR_ON, LCD_BLINK_OFF);
	clear_display();
	entry_mode_set(LCD_INCREMENT, LCD_NO_SHIFT);

}

void LCD_Set_Cursor(uint8_t row, uint8_t column)
{
	uint8_t ddram_addr = 0;
	if (column == 2)
	{
		// most significant bit needs to be set for 2nd column
		ddram_addr |= (1 << 6);
	}
	// row number is 4 bits, 0 through F for 1 to 16
	ddram_addr |= ((row-1) & 0xF);

	set_ddram_addr(ddram_addr);
}

void LCD_Display_Str(char *str)
{
	char curr;
	while ((curr = *str) != '\0')
	{
		LCD_Display_Char(0, 0, curr);
		str++;
	}
}

void LCD_Display_Char(uint8_t row, uint8_t col, char ch)
{
	write_char(ch);
}


// PRIVATE UTILITY FUNCTIONS
static void write_char(char ch)
{
	uint8_t low_nybble = (ch & 0xF);
	uint8_t high_nybble = (ch >> 4);

	// set RS high (for data not command)
	GPIO_Write_To_Output_Pin(LCD_GPIO_PORT, RS_GPIO_PIN, HIGH);
	udelay(100);

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
	pulse_enable(500);
	// wait 1us for total cycle
	// (data must be held valid for 10ns, enable cannot pulse high again for 500ns)
	udelay(500);
}

static void clear_display()
{
	// uint8_t cmd_byte = construct_command(CLEAR_DISPLAY);
	uint8_t cmd_byte = construct_clear_display_cmd();
	write_command(cmd_byte, 1);
	// 2ms delay - clear display takes ~1.5ms
	mdelay(2);
}

static void return_home()
{
	// uint8_t cmd_byte = construct_command(RETURN_HOME);
	uint8_t cmd_byte = construct_return_home_cmd();
	write_command(cmd_byte, 1);
	// 2ms delay - return home takes ~1.5ms
	mdelay(2);
}

static void entry_mode_set(uint8_t inc_dec, uint8_t shift)
{
	uint8_t cmd_byte = construct_entry_mode_set_cmd(inc_dec, shift);
	write_command(cmd_byte, 1);
}

static void display_on_off(uint8_t disp, uint8_t cursor, uint8_t blink)
{
	uint8_t cmd_byte = construct_display_on_off_ctrl_cmd(disp, cursor, blink);
	write_command(cmd_byte, 1);
}

static void cursor_display_shift(uint8_t shift_or_cursor, uint8_t right_left)
{
	uint8_t cmd_byte = construct_cursor_display_shift_cmd(shift_or_cursor, right_left);
	write_command(cmd_byte, 1);
}

static void function_set(uint8_t bit_len, uint8_t num_lines, uint8_t font)
{
	uint8_t cmd_byte = construct_function_set_cmd(bit_len, num_lines, font);
	write_command(cmd_byte, 1);
}

static void set_cgram_addr(uint8_t cgram_addr)
{
	uint8_t cmd_byte = constrcut_set_cgram_addr_cmd(cgram_addr);
	write_command(cmd_byte, 1);
}

static void set_ddram_addr(uint8_t ddram_addr)
{
	uint8_t cmd_byte = constrcut_set_ddram_addr_cmd(ddram_addr);
	write_command(cmd_byte, 1);
}
/*
static uint8_t construct_command(LCD_1602A_Commands_t cmd)
{
	switch (cmd)
	{
	case CLEAR_DISPLAY:
		return construct_clear_display_cmd();
		break;
	case RETURN_HOME:
		return construct_return_home_cmd();
		break;
	case ENTRY_MODE_SET:
		return construct_entry_mode_set_cmd();
		break;
	case DISPLAY_ON_OFF_CTRL:
		return construct_display_on_off_ctrl_cmd();
		break;
	case CURSOR_DISPLAY_SHIFT:
		return construct_cursor_display_shift_cmd();
		break;
	case FUNCTION_SET:
		return construct_function_set_cmd();
		break;
	case SET_CGRAM_ADDR:
		return constrcut_set_cgram_addr_cmd();
		break;
	case SET_DDRAM_ADDR:
		return constrcut_set_ddram_addr_cmd();
		break;
	}
}
*/

static uint8_t construct_clear_display_cmd()
{
	// clear display command is always 0x1
	return 0x1;
}

static uint8_t construct_return_home_cmd()
{
	// return home command is always 0x2
	return 0x2;
}

static uint8_t construct_entry_mode_set_cmd(uint8_t inc_dec, uint8_t shift)
{
	uint8_t cmd_byte = 0x04;
	cmd_byte |= (inc_dec << 1) + (shift << 0);
	return cmd_byte;
}

static uint8_t construct_display_on_off_ctrl_cmd(uint8_t disp, uint8_t cursor, uint8_t blink)
{
	uint8_t cmd_byte = 0x08;
	cmd_byte |= (disp << 2) + (cursor << 1) + (blink << 0);
	return cmd_byte;
}

static uint8_t construct_cursor_display_shift_cmd(uint8_t shift_or_cursor, uint8_t right_left)
{
	uint8_t cmd_byte = 0x10;
	cmd_byte |= (shift_or_cursor << 3) + (right_left << 2);
	return cmd_byte;
}

static uint8_t construct_function_set_cmd(uint8_t bit_len, uint8_t num_lines, uint8_t font)
{
	uint8_t cmd_byte = 0x20;
	cmd_byte |= (bit_len << 4) + (num_lines << 3) + (font << 2);
	return cmd_byte;
}

static uint8_t constrcut_set_cgram_addr_cmd(uint8_t cgram_addr)
{
	uint8_t cmd_byte = 0x40;
	cgram_addr &= 0x3F;
	cmd_byte |= cgram_addr;
	return cmd_byte;
}

static uint8_t constrcut_set_ddram_addr_cmd(uint8_t ddram_addr)
{
	uint8_t cmd_byte = 0x80;
	ddram_addr &= 0x7F;
	cmd_byte |= ddram_addr;
	return cmd_byte;
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
