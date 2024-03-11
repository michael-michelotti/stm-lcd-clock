#ifndef INC_LCD1602A_DRIVER_H_
#define INC_LCD1602A_DRIVER_H_

#include <stdint.h>

#include "ds3231_rtc_driver.h"
#include "stm32f407xx_gpio_driver.h"
#include "stm32f407xx.h"
#include "display.h"

#define LCD_GPIO_PORT		GPIOA
// GPIO PIN NUMBERS
#define RS_GPIO_PIN			GPIO_PIN_1
#define RS_GPIO_PORT		GPIOA
#define E_GPIO_PIN			GPIO_PIN_2
#define E_GPIO_PORT			GPIOA
#define DB4_GPIO_PIN		GPIO_PIN_3
#define DB4_GPIO_PORT		GPIOA
#define DB5_GPIO_PIN		GPIO_PIN_4
#define DB5_GPIO_PORT		GPIOA
#define DB6_GPIO_PIN		GPIO_PIN_5
#define DB6_GPIO_PORT		GPIOA
#define DB7_GPIO_PIN		GPIO_PIN_6
#define DB7_GPIO_PORT		GPIOA
#define RW_GPIO_PIN			GPIO_PIN_7
#define RW_GPIO_PORT		GPIOA

// ENTRY MODE SET CONFIGURATION BITS
#define LCD_INCREMENT		1
#define LCD_DECREMENT		0
#define LCD_SHIFT			1
#define LCD_NO_SHIFT		0

#define LCD1602A_HRS_OFFSET		0
#define LCD1602A_MINS_OFFSET	3
#define LCD1602A_SECS_OFFSET	6
#define LCD1602A_HR_FMT_OFFSET	9

#define LCD1602A_TIME_ROW		0
#define LCD1602A_TIME_COL		0
#define LCD1602A_HRS_ROW		LCD1602A_TIME_ROW
#define LCD1602A_HRS_COL		LCD1602A_TIME_COL
#define LCD1602A_MINS_ROW		LCD1602A_TIME_ROW
#define LCD1602A_MINS_COL		(LCD1602A_TIME_COL + LCD1602A_MINS_OFFSET)
#define LCD1602A_SECS_ROW		LCD1602A_TIME_ROW
#define LCD1602A_SECS_COL		(LCD1602A_TIME_COL + LCD1602A_SECS_OFFSET)
#define LCD1602A_HR_FMT_ROW		LCD1602A_TIME_ROW
#define LCD1602A_HR_FMT_COL		(LCD1602A_TIME_COL + LCD1602A_HR_FMT_OFFSET)


#define LCD1602A_DOW_OFFSET		0
#define LCD1602A_MONTH_OFFSET	4
#define LCD1602A_DATE_OFFSET	7
#define LCD1602A_YEAR_OFFSET	10

#define LCD1602A_FULL_DATE_ROW	1
#define LCD1602A_FULL_DATE_COL	0
#define LCD1602A_DOW_ROW		LCD1602A_FULL_DATE_ROW
#define LCD1602A_DOW_COL		LCD1602A_FULL_DATE_COL
#define LCD1602A_MONTH_ROW		LCD1602A_FULL_DATE_ROW
#define LCD1602A_MONTH_COL		(LCD1602A_FULL_DATE_COL + LCD1602A_MONTH_OFFSET)
#define LCD1602A_DATE_ROW		LCD1602A_FULL_DATE_ROW
#define LCD1602A_DATE_COL		(LCD1602A_FULL_DATE_COL + LCD1602A_DATE_OFFSET)
#define LCD1602A_YEAR_ROW		LCD1602A_FULL_DATE_ROW
#define LCD1602A_YEAR_COL		(LCD1602A_FULL_DATE_COL + LCD1602A_YEAR_OFFSET)


// ENTRY DISPLAY AND CURSOR CONFIG BITS
#define LCD_DISP_OFF		0
#define LCD_DISP_ON			1
#define LCD_CURSOR_OFF		0
#define LCD_CURSOR_ON		1
#define LCD_BLINK_OFF		0
#define LCD_BLINK_ON		1

#define LCD_4_BIT			0
#define LCD_8_BIT			1
#define LCD_1_LINE			0
#define LCD_2_LINES			1
#define LCD_5_8_DOTS		0
#define LCD_5_10_DOTS		1

// Default row and column position for date and time
#define DEFAULT_DATE_COL	1
#define DEFAULT_DATE_ROW	2
#define DEFAULT_TIME_COL	1
#define DEFAULT_TIME_ROW	1

#define ENALBE_PULSE_US		500		// how long to hold enable high when sending nybble
#define LCD_TAS_US			1		// how long to wait after toggling RS or RW pins for address setup
#define LCD_HOLD_TIME_US	500		// how long to hold data lines valid after enable high to low transition

#define ASCII_DIGIT_OFFSET	48


// Enumeration of LCD COMMAND CATEGORIES
typedef enum
{
	CLEAR_DISPLAY = 0x1,
	RETURN_HOME = 0x2,
	ENTRY_MODE_SET,
	DISPLAY_ON_OFF_CTRL,
	CURSOR_DISPLAY_SHIFT,
	FUNCTION_SET,
	SET_CGRAM_ADDR,
	SET_DDRAM_ADDR,
	READ_BF_DDRAM_ADDR,
	WRITE_TO_RAM,
	READ_FROM_RAM
} LCD_1602A_Commands_t;

typedef struct
{
	Display_Device_t 		display_dev;
	LCD_1602A_Commands_t	cmd_stage;
	char 					time_str_buffer[12];
	char					date_str_buffer[15];
	uint8_t					cursor_row_pos;
	uint8_t 				cursor_col_pos;
	GPIO_Handle_t			db4_gpio_handle;
	GPIO_Handle_t			db5_gpio_handle;
	GPIO_Handle_t			db6_gpio_handle;
	GPIO_Handle_t			db7_gpio_handle;
	GPIO_Handle_t			e_gpio_handle;
	GPIO_Handle_t			rw_gpio_handle;
	GPIO_Handle_t			rs_gpio_handle;
} LCD1602A_Handle_t;

// INSTRUCTION CODE BIT POSITIONS
#define IC_DB0			0
#define IC_DB1			1
#define IC_DB2			2
#define IC_DB3			3
#define IC_DB4			4
#define IC_DB5			5
#define IC_DB6			6
#define IC_DB7			7
#define IC_RW			8
#define IC_RS			9


#endif /* INC_LCD1602A_DRIVER_H_ */
