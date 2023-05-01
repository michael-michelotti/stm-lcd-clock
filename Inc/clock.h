#ifndef CLOCK_H_
#define CLOCK_H_

#include <stdint.h>

typedef uint8_t		seconds_t;
typedef uint8_t		minutes_t;

typedef enum
{
	HOUR_FORMAT_24_HOUR,
	HOUR_FORMAT_12_HOUR
} hour_format_t;

typedef enum
{
	AM_PM_AM,
	AM_PM_PM,
	AM_PM_NONE
} am_pm_t;

typedef struct
{
	hour_format_t	hour_format;
	am_pm_t			am_pm;
	uint8_t			hour;
} hours_t;

typedef enum
{
	DAY_OF_WEEK_SUN = 1,
	DAY_OF_WEEK_MON,
	DAY_OF_WEEK_TUE,
	DAY_OF_WEEK_WED,
	DAY_OF_WEEK_THU,
	DAY_OF_WEEK_FRI,
	DAY_OF_WEEK_SAT
} day_of_week_t;

typedef uint8_t		date_t;

typedef enum
{
	MONTH_JAN = 1,
	MONTH_FEB,
	MONTH_MAR,
	MONTH_APR,
	MONTH_MAY,
	MONTH_JUN,
	MONTH_JUL,
	MONTH_AUG,
	MONTH_SEP,
	MONTH_OCT,
	MONTH_NOV,
	MONTH_DEC
} month_t;

typedef uint16_t	year_t;

typedef struct
{
	day_of_week_t	day_of_week;
	date_t			date;
	month_t			month;
	year_t			year;
} full_date_t;

typedef struct
{
	seconds_t		seconds;
	minutes_t		minutes;
	hours_t			hours;
} full_time_t;

typedef struct
{
	full_date_t		date;
	full_time_t		time;
} full_datetime_t;

typedef struct
{
	void			(*Clock_Initialize)(void);

	seconds_t 		(*Clock_Get_Seconds)(void);
	minutes_t 		(*Clock_Get_Minutes)(void);
	hours_t 		(*Clock_Get_Hours)(void);
	day_of_week_t	(*Clock_Get_Day_Of_Week)(void);
	date_t 			(*Clock_Get_Date)(void);
	month_t 		(*Clock_Get_Month)(void);
	year_t 			(*Clock_Get_Year)(void);
	full_date_t		(*Clock_Get_Full_Date)(void);
	full_time_t		(*Clock_Get_Full_Time)(void);
	full_datetime_t (*Clock_Get_Full_Datetime)(void);

	void 			(*Clock_Set_Seconds)(seconds_t secs);
	void 			(*Clock_Set_Minutes)(minutes_t mins);
	void 			(*Clock_Set_Hours)(hours_t hours);
	void 			(*Clock_Set_Day_Of_Week)(day_of_week_t dow);
	void 			(*Clock_Set_Date)(date_t date);
	void 			(*Clock_Set_Month)(month_t month);
	void 			(*Clock_Set_Year)(year_t year);
	void 			(*Clock_Set_Full_Date)(full_date_t full_date);
	void 			(*Clock_Set_Full_Time)(full_time_t full_time);
	void 			(*Clock_Set_Full_Datetime)(full_datetime_t full_datetime);
} Clock_Driver;

#endif /* CLOCK_H_ */
