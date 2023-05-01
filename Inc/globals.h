/*
 * globals.h
 *
 *  Created on: Feb 12, 2023
 *      Author: Michael
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_

#include "stm32f407xx_i2c_driver.h"

// LCD display strings
extern char global_time_str[16];
extern char global_date_str[16];

extern I2C_Handle_t global_i2c_handle;
extern GPIO_Handle_t global_gpio_handle;

extern uint8_t rx_buffer[255];
extern uint8_t tx_buffer[255];
extern uint8_t tx_buffer_pos;
extern uint8_t rx_buffer_pos;

extern HAL_Driver app_hal_driver;

#endif /* GLOBALS_H_ */
