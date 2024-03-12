#include "clock.h"
#include "time.h"

full_datetime_t clock_device_get_datetime(Clock_Device_t *clock_dev)
{
	full_datetime_t datetime = {
			.date = clock_dev->date,
			.time = clock_dev->time
	};

	return datetime;
}

__weak void Clock_Get_Seconds_Complete_Callback(Clock_Device_t *clock_dev)
{
	/* implemented in application code */
}

__weak void Clock_Get_Minutes_Complete_Callback(Clock_Device_t *clock_dev)
{
	/* implemented in application code */
}

__weak void Clock_Get_Hours_Complete_Callback(Clock_Device_t *clock_dev)
{
	/* implemented in application code */
}

__weak void Clock_Get_Day_Of_Week_Complete_Callback(Clock_Device_t *clock_dev)
{
	/* implemented in application code */
}

__weak void Clock_Get_Date_Complete_Callback(Clock_Device_t *clock_dev)
{
	/* implemented in application code */
}

__weak void Clock_Get_Month_Complete_Callback(Clock_Device_t *clock_dev)
{
	/* implemented in application code */
}

__weak void Clock_Get_Year_Complete_Callback(Clock_Device_t *clock_dev)
{
	/* implemented in application code */
}

__weak void Clock_Get_Century_Complete_Callback(Clock_Device_t *clock_dev)
{
	/* implemented in application code */
}

__weak void Clock_Get_Full_Date_Complete_Callback(Clock_Device_t *clock_dev)
{
	/* implemented in application code */
}

__weak void Clock_Get_Full_Time_Complete_Callback(Clock_Device_t *clock_dev)
{
	/* implemented in application code */
}

__weak void Clock_Get_Datetime_Complete_Callback(Clock_Device_t *clock_dev)
{
	/* implemented in application code */
}

__weak void Clock_Set_Seconds_Complete_Callback(Clock_Device_t *clock_dev)
{
	/* implemented in application code */
}

__weak void Clock_Set_Minutes_Complete_Callback(Clock_Device_t *clock_dev)
{
	/* implemented in application code */
}

__weak void Clock_Set_Hours_Complete_Callback(Clock_Device_t *clock_dev)
{
	/* implemented in application code */
}

__weak void Clock_Set_Day_Of_Week_Complete_Callback(Clock_Device_t *clock_dev)
{
	/* implemented in application code */
}

__weak void Clock_Set_Date_Complete_Callback(Clock_Device_t *clock_dev)
{
	/* implemented in application code */
}

__weak void Clock_Set_Months_Complete_Callback(Clock_Device_t *clock_dev)
{
	/* implemented in application code */
}

__weak void Clock_Set_Century_Complete_Callback(Clock_Device_t *clock_dev)
{
	/* implemented in application code */
}

__weak void Clock_Set_Years_Complete_Callback(Clock_Device_t *clock_dev)
{
	/* implemented in application code */
}

__weak void Clock_Set_Full_Time_Complete_Callback(Clock_Device_t *clock_dev)
{
	/* implemented in application code */
}

__weak void Clock_Set_Full_Date_Complete_Callback(Clock_Device_t *clock_dev)
{
	/* implemented in application code */
}

__weak void Clock_Set_Datetime_Complete_Callback(Clock_Device_t *clock_dev)
{
	/* implemented in application code */
}
