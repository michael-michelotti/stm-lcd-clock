#ifndef CLOCK_H_
#define CLOCK_H_

#include "time.h"
#define DS3231

typedef enum
{
	CLOCK_CTRL_IDLE,
	CLOCK_CTRL_INIT,
	CLOCK_CTRL_BUSY_GETTING,
	CLOCK_CTRL_BUSY_SETTING
} Clock_Ctrl_Stage_t;

typedef enum
{
	CLOCK_UNIT_NONE,
	CLOCK_UNIT_SECONDS,
	CLOCK_UNIT_MINUTES,
	CLOCK_UNIT_HOURS,
	CLOCK_UNIT_DOW,
	CLOCK_UNIT_DATE,
	CLOCK_UNIT_MONTH,
	CLOCK_UNIT_YEAR,
	CLOCK_UNIT_FULL_DATE,
	CLOCK_UNIT_FULL_TIME,
	CLOCK_UNIT_FULL_DATETIME
} Clock_Time_Unit_t;


typedef struct
{
	seconds_t			curr_seconds;
	minutes_t			curr_minutes;
	hours_t				curr_hours;
} Clock_Device_t;


typedef struct
{
	Clock_Ctrl_Stage_t	ctrl_stage;
	Clock_Time_Unit_t	curr_unit;
	Clock_Device_t		clock_dev;
} Clock_Handle_t;


typedef struct
{
	void			(*Initialize)(void);

	seconds_t 		(*Get_Seconds)(void);
	void			(*Get_Seconds_IT)(void);
	minutes_t 		(*Get_Minutes)(void);
	void			(*Get_Minutes_IT)(void);
	hours_t 		(*Get_Hours)(void);
	day_of_week_t	(*Get_Day_Of_Week)(void);
	date_t 			(*Get_Date)(void);
	month_t 		(*Get_Month)(void);
	year_t 			(*Get_Year)(void);
	full_date_t		(*Get_Full_Date)(void);
	full_time_t		(*Get_Full_Time)(void);
	full_datetime_t (*Get_Full_Datetime)(void);

	void 			(*Set_Seconds)(seconds_t secs);
	void			(*Set_Seconds_IT)(seconds_t secs);
	void 			(*Set_Minutes)(minutes_t mins);
	void 			(*Set_Hours)(hours_t hours);
	void 			(*Set_Day_Of_Week)(day_of_week_t dow);
	void 			(*Set_Date)(date_t date);
	void 			(*Set_Month)(month_t month);
	void 			(*Set_Year)(year_t year);
	void 			(*Set_Full_Date)(full_date_t full_date);
	void 			(*Set_Full_Time)(full_time_t full_time);
	void 			(*Set_Full_Datetime)(full_datetime_t full_datetime);
} Clock_Driver_t;

Clock_Driver_t *get_clock_driver();

void Clock_Get_Seconds_Complete_Callback(seconds_t secs);
void Clock_Get_Minutes_Complete_Callback(minutes_t mins);

#ifdef DS3231
#	include "ds3231_rtc_driver.h"
#else
#	error "RTC clock driver not defined."
#endif

#endif /* CLOCK_H_ */
