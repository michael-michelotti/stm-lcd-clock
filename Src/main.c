#define DS3231

#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include "clock.h"
#include "display.h"
#include "main.h"
#include "stm32f407xx.h"


#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif


int main(void)
{
	Clock_Driver_t 			*app_clock_driver = get_clock_driver();
	Clock_Device_t 			ds3231_dev = {
			.seconds = 0,
			.minutes = 0,
			.hours = 0,
			.ctrl_stage = CLOCK_CTRL_INIT,
	};

	app_clock_driver->Initialize(ds3231_dev);
	app_clock_driver->Set_Seconds_IT(22);

	Display_Driver_t		*app_display_driver = get_display_driver();
	Display_Device_t		lcd1602a_dev = {
			.ctrl_stage = DISPLAY_CTRL_INIT,
	};
	app_display_driver->Display_Initialize(lcd1602a_dev);

	full_date_t date = {
			.date = 10,
			.day_of_week = DAY_OF_WEEK_SUN,
			.month = MONTH_MAR,
			.year = 24,
			.century = CENTURY_21ST
	};
	hours_t hours = {
			.am_pm = AM_PM_PM,
			.hour = 5,
			.hour_format = HOUR_FORMAT_12_HOUR
	};
	full_time_t time = {
			.hours = hours,
			.minutes = 17,
			.seconds = 00
	};
	full_datetime_t datetime = {
			.date = date,
			.time = time
	};
	app_clock_driver->Set_Full_Datetime(datetime);
	for(;;)
	{
		datetime = app_clock_driver->Get_Full_Datetime();
		app_display_driver->Display_Update_Datetime(datetime);
		delay();
	}
}

void Clock_Get_Seconds_Complete_Callback(Clock_Device_t *clock_dev)
{
	// this is probably where i should update my display right?
	printf("Current Seconds: %u\n", (unsigned int) clock_dev->seconds);
}

void Clock_Get_Minutes_Complete_Callback(Clock_Device_t *clock_dev)
{
	printf("Current Minutes: %u\n", (unsigned int) clock_dev->minutes);
}

void Clock_Set_Seconds_Comlpete_Callback(Clock_Device_t *clock_dev)
{
	printf("Seconds Set: %u\n", (unsigned int) clock_dev->seconds);
}
