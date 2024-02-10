#ifndef TIME_H_
#define TIME_H_

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

#endif /* TIME_H_ */
