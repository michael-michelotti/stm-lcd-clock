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
		.ctrl_stage = CLOCK_CTRL_INIT,
};
Display_Device_t		lcd1602a_dev = {
		.ctrl_stage = DISPLAY_CTRL_INIT,
};

int main(void)
{
	app_clock_driver = get_clock_driver();
	app_display_driver = get_display_driver();

	app_clock_driver->Initialize(&ds3231_dev);
	ds3231_dev.ctrl_stage = CLOCK_CTRL_IDLE;

	app_display_driver->Display_Initialize(&lcd1602a_dev);

	hours_t hours = {
			.am_pm = AM_PM_PM,
			.hour = 11,
			.hour_format = HOUR_FORMAT_12_HOUR
	};
	full_date_t date = {
			.date = 31,
			.day_of_week = DAY_OF_WEEK_TUE,
			.month = MONTH_DEC,
			.year = 24,
			.century = CENTURY_21ST
	};
	full_time_t time = {
			.hours = hours,
			.minutes = 59,
			.seconds = 40
	};
	ds3231_dev.date = date;
	ds3231_dev.time = time;


	ds3231_dev.ctrl_stage = CLOCK_CTRL_BUSY_SETTING;
	app_clock_driver->Set_Full_Datetime_IT(clock_device_get_datetime(&ds3231_dev));
	while (ds3231_dev.ctrl_stage == CLOCK_CTRL_BUSY_SETTING) {}

	app_display_driver->Display_Update_Datetime(clock_device_get_datetime(&ds3231_dev));

	for(;;)
	{
		ds3231_dev.ctrl_stage = CLOCK_CTRL_BUSY_GETTING;
		app_clock_driver->Get_Datetime_IT();
		while (ds3231_dev.ctrl_stage == CLOCK_CTRL_BUSY_GETTING)
		{
			/* user code could go here! */
		}
	}
}

void Clock_Get_Seconds_Complete_Callback(Clock_Device_t *clock_dev)
{
	app_display_driver->Display_Update_Seconds(clock_dev->time.seconds);
}

void Clock_Get_Minutes_Complete_Callback(Clock_Device_t *clock_dev)
{
	app_display_driver->Display_Update_Minutes(clock_dev->time.minutes);
}

void Clock_Get_Hours_Complete_Callback(Clock_Device_t *clock_dev)
{
	app_display_driver->Display_Update_Hours(clock_dev->time.hours);
}

void Clock_Get_Day_Of_Week_Complete_Callback(Clock_Device_t *clock_dev)
{
	app_display_driver->Display_Update_Day_Of_Week(clock_dev->date.day_of_week);
}

void Clock_Get_Date_Complete_Callback(Clock_Device_t *clock_dev)
{
	app_display_driver->Display_Update_Date(clock_dev->date.date);
}

void Clock_Get_Month_Complete_Callback(Clock_Device_t *clock_dev)
{
	app_display_driver->Display_Update_Month(clock_dev->date.month);
}

void Clock_Get_Full_Time_Complete_Callback(Clock_Device_t *clock_dev)
{
	full_time_t full_time = {
			.hours = clock_dev->time.hours,
			.minutes = clock_dev->time.minutes,
			.seconds = clock_dev->time.seconds
	};
	app_display_driver->Display_Update_Time(full_time);
}

void Clock_Get_Datetime_Complete_Callback(Clock_Device_t *clock_dev)
{
	full_time_t full_time = {
			.hours = clock_dev->time.hours,
			.minutes = clock_dev->time.minutes,
			.seconds = clock_dev->time.seconds
	};
	full_date_t full_date = {
			.day_of_week = clock_dev->date.day_of_week,
			.date = clock_dev->date.date,
			.month = clock_dev->date.month,
			.century = clock_dev->date.century,
			.year = clock_dev->date.year
	};
	full_datetime_t datetime = {
			.date = full_date,
			.time = full_time
	};
	app_display_driver->Display_Update_Datetime(datetime);
	clock_dev->ctrl_stage = CLOCK_CTRL_IDLE;
}

void Clock_Set_Seconds_Complete_Callback(Clock_Device_t *clock_dev)
{
	printf("Seconds Set: %u\n", (unsigned int) clock_dev->time.seconds);
}

void Clock_Set_Datetime_Complete_Callback(Clock_Device_t *clock_dev)
{
	clock_dev->ctrl_stage = CLOCK_CTRL_IDLE;
}
