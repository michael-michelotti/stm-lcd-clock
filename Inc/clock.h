#ifndef CLOCK_H_
#define CLOCK_H_

#include "time.h"
#define DS3231

typedef struct
{
	void			(*Initialize)(void);

	seconds_t 		(*Get_Seconds)(void);
	minutes_t 		(*Get_Minutes)(void);
	hours_t 		(*Get_Hours)(void);
	day_of_week_t	(*Get_Day_Of_Week)(void);
	date_t 			(*Get_Date)(void);
	month_t 		(*Get_Month)(void);
	year_t 			(*Get_Year)(void);
	full_date_t		(*Get_Full_Date)(void);
	full_time_t		(*Get_Full_Time)(void);
	full_datetime_t (*Get_Full_Datetime)(void);

	void 			(*Set_Seconds)(seconds_t secs);
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

Clock_Driver_t get_clock();

#ifdef DS3231
#	include "ds3231_rtc_driver.h"
#else
#	error "RTC clock driver not defined."
#endif

#endif /* CLOCK_H_ */
