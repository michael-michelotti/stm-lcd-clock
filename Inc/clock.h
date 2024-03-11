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


typedef struct
{
	seconds_t			seconds;
	minutes_t			minutes;
	hours_t				hours;
	date_t				date;
	day_of_week_t		dow;
	month_t				month;
	year_t				year;
	century_t			century;
	Clock_Ctrl_Stage_t	ctrl_stage;
} Clock_Device_t;


typedef struct
{
	void			(*Initialize)(Clock_Device_t);

	seconds_t 		(*Get_Seconds)(void);
	minutes_t 		(*Get_Minutes)(void);
	hours_t 		(*Get_Hours)(void);
	day_of_week_t	(*Get_Day_Of_Week)(void);
	date_t 			(*Get_Date)(void);
	month_t 		(*Get_Month)(void);
	year_t 			(*Get_Year)(void);
	century_t		(*Get_Century)(void);
	full_date_t		(*Get_Full_Date)(void);
	full_time_t		(*Get_Full_Time)(void);
	full_datetime_t (*Get_Full_Datetime)(void);

	void			(*Get_Hours_IT)(void);
	void			(*Get_Minutes_IT)(void);
	void			(*Get_Seconds_IT)(void);
	void 			(*Get_Day_Of_Week_IT)(void);
	void			(*Get_Full_Time_IT)(void);

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

Clock_Driver_t *get_clock_driver(void);

void Clock_Get_Seconds_Complete_Callback(Clock_Device_t *clock_dev);
void Clock_Get_Minutes_Complete_Callback(Clock_Device_t *clock_dev);
void Clock_Set_Seconds_Complete_Callback(Clock_Device_t *clock_dev);
void Clock_Get_Hours_Complete_Callback(Clock_Device_t *clock_dev);
void Clock_Get_Full_Time_Complete_Callback(Clock_Device_t *clock_dev);
void Clock_Get_Day_Of_Week_Complete_Callback(Clock_Device_t *clock_dev);

#ifdef DS3231
#	include "ds3231_rtc_driver.h"
#else
#	error "Clock driver not defined."
#endif

#endif /* CLOCK_H_ */
