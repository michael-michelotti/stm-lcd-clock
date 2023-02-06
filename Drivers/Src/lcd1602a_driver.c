/*
 * lcd1602a_driver.c
 *
 *  Created on: Feb 4, 2023
 *      Author: Michael
 */

#include "lcd1602a_driver.h"
#include "stm32f407xx.h"

static void write_char(char ch);
static void write_command(uint8_t cmd_word);
static void send_nybble(uint8_t nybble);
static void pulse_enable(uint8_t us_hold_time);
static void mdelay(uint32_t cnt);
static void udelay(uint32_t cnt);

void LCD_Initialize()
{
	// initialize GPIO pins RS, DB7-DB4
	// RS = PD0
	// DB4-7 = PD1-4
	// E = PD5
	// Initialize PD0 for RS
	GPIO_Pin_Config_t pin_conf = { RS_GPIO_PIN, GPIO_MODE_OUT, GPIO_SPEED_HIGH, GPIO_PUPD_NONE, GPIO_OUT_PP, 0 };
	GPIO_Handle_t pin_handle = { GPIOA, pin_conf };
	GPIO_Init(&pin_handle);

	// Initialize PD1 for E (Enable)
	pin_handle.gpio_pin_config.gpio_pin_num = E_GPIO_PIN;
	GPIO_Init(&pin_handle);

	// Initialize PD2 for DB4
	pin_handle.gpio_pin_config.gpio_pin_num = DB4_GPIO_PIN;
	GPIO_Init(&pin_handle);

	// Initialize PD3 for DB5
	pin_handle.gpio_pin_config.gpio_pin_num = DB5_GPIO_PIN;
	GPIO_Init(&pin_handle);

	// Initialize PD4 for DB6
	pin_handle.gpio_pin_config.gpio_pin_num = DB6_GPIO_PIN;
	GPIO_Init(&pin_handle);

	// Initialize PD5 for DB7
	pin_handle.gpio_pin_config.gpio_pin_num = DB7_GPIO_PIN;
	GPIO_Init(&pin_handle);



	// apply power to LCD (use a 5V tolerate GPIO to digitally apply power?)
	// wait 15ms after power applied

	// set all pins to ground (clear entire GPIOD ODR port)
	GPIO_Write_To_Output_Port(GPIOA, 0);
	mdelay(1);

	// function set (DB4 (PD1) and DB5 (PD2) high, rest low), then toggle E pin (PD5) high
	GPIO_Write_To_Output_Pin(GPIOA, DB4_GPIO_PIN, HIGH);
	GPIO_Write_To_Output_Pin(GPIOA, DB5_GPIO_PIN, HIGH);

	pulse_enable(1);
	// hold for 5ms
	mdelay(5);

	// pulse E again for 1us with same data inputs. delay for 100us
	pulse_enable(1);
	mdelay(1);

	// pulse E again for 1us,
	pulse_enable(1);
	mdelay(1);

	// delay 1us (E cycle time), then function set again, but with 4-bit mode this time (DB4 low)
	GPIO_Write_To_Output_Pin(GPIOA, DB4_GPIO_PIN, LOW);
	pulse_enable(1);
	mdelay(1);

	// now we need to start sending 8-bit words in 2 nybbles separately (4-bit mode)
	// 0x28 = 0010 1000, sets line number to 2 and style to 5x8 dot characters
	write_command(0x28);

	// turn display off
	write_command(0x0E);

	// clear display
	write_command(LCD_CLR_DISP);

	// set entry parameters - I/D set to increment, shift off
	write_command(0x06);

	// turn display on, cursor on, cursor blink
	// write_command(0x0F);
}

void LCD_Display_Char(uint8_t row, uint8_t col, char ch)
{
	write_char(ch);
}


static void write_char(char ch)
{
	// set RS high (for data not command)
	GPIO_Write_To_Output_Pin(GPIOA, RS_GPIO_PIN, HIGH);
	udelay(100);

	uint8_t low_nybble = (ch & 0xF);
	uint8_t high_nybble = (ch >> 4);
	send_nybble(high_nybble);
	send_nybble(low_nybble);
}

// RS always low, RW always low
static void write_command(uint8_t cmd_word)
{
	// make sure RS pin is low to send command
	GPIO_Write_To_Output_Pin(GPIOA, RS_GPIO_PIN, LOW);
	udelay(100);

	uint8_t low_nybble = (cmd_word & 0xF);
	uint8_t high_nybble = (cmd_word >> 4);

	send_nybble(high_nybble);
	send_nybble(low_nybble);
}

static void send_nybble(uint8_t nybble)
{
	// set pins with nybble value
	GPIO_Write_To_Output_Pin(GPIOA, DB4_GPIO_PIN, ((nybble >> 0) & 1));
	GPIO_Write_To_Output_Pin(GPIOA, DB5_GPIO_PIN, ((nybble >> 1) & 1));
	GPIO_Write_To_Output_Pin(GPIOA, DB6_GPIO_PIN, ((nybble >> 2) & 1));
	GPIO_Write_To_Output_Pin(GPIOA, DB7_GPIO_PIN, ((nybble >> 3) & 1));

	// send nybble to LCD by pulsing enable, then delay 1us for cycle time
	pulse_enable(1);
	mdelay(1);
}

// PRIVATE UTILITY FUNCTIONS
static void pulse_enable(uint8_t us_hold_time)
{
	// pulse E again for 1us,
	GPIO_Write_To_Output_Pin(GPIOA, E_GPIO_PIN, HIGH);
	udelay(500);
	GPIO_Write_To_Output_Pin(GPIOA, E_GPIO_PIN, LOW);
	mdelay(1);
}

static void mdelay(uint32_t cnt)
{
	for(uint32_t i=0; i < (cnt * 1000); i++);
}

static void udelay(uint32_t cnt)
{
	for(uint32_t i=0; i < (cnt * 1); i++);
}
