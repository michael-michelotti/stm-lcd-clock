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


Clock_Driver_t 			*app_clock_driver;
Display_Driver_t		*app_display_driver;

Clock_Device_t 			ds3231_dev = {
		.seconds = 0,
		.minutes = 0,
		.ctrl_stage = CLOCK_CTRL_INIT,
};
Display_Device_t		lcd1602a_dev = {
		.ctrl_stage = DISPLAY_CTRL_INIT,
};

int main(void)
{
	app_clock_driver = get_clock_driver();
	app_display_driver = get_display_driver();
	app_clock_driver->Initialize(ds3231_dev);
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
	datetime = app_clock_driver->Get_Full_Datetime();
	app_display_driver->Display_Update_Datetime(datetime);
	for(;;)
	{
		app_clock_driver->Get_Full_Time_IT();
		delay();
	}
}

void Clock_Get_Seconds_Complete_Callback(Clock_Device_t *clock_dev)
{
	app_display_driver->Display_Update_Seconds(clock_dev->seconds);
}

void Clock_Get_Minutes_Complete_Callback(Clock_Device_t *clock_dev)
{
	app_display_driver->Display_Update_Minutes(clock_dev->minutes);
}

void Clock_Get_Hours_Complete_Callback(Clock_Device_t *clock_dev)
{
	app_display_driver->Display_Update_Hours(clock_dev->hours);
}

void Clock_Get_Full_Time_Complete_Callback(Clock_Device_t *clock_dev)
{
	full_time_t full_time = {
			.hours = clock_dev->hours,
			.minutes = clock_dev->minutes,
			.seconds = clock_dev->seconds
	};
	app_display_driver->Display_Update_Time(full_time);
}

void Clock_Set_Seconds_Complete_Callback(Clock_Device_t *clock_dev)
{
	printf("Seconds Set: %u\n", (unsigned int) clock_dev->seconds);
}

