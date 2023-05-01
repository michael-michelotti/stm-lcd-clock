#include "clock.h"

Clock_Driver ds3231_clock_driver =
{
	.Clock_Get_Seconds 			= DS3231_Get_Seconds,
	.Clock_Get_Minutes 			= DS3231_Get_Minutes,
	.Clock_Get_Hours 			= DS3231_Get_Hours,
	.Clock_Get_Day_Of_Week 		= DS3231_Get_Day_Of_Week,
	.Clock_Get_Date				= DS3231_Get_Date,
	.Clock_Get_Month			= DS3231_Get_Month,
	.Clock_Get_Year				= DS3231_Get_Year,
	.Clock_Get_Full_Date		= DS3231_Get_Full_Date,
	.Clock_Get_Full_Time		= DS3231_Get_Full_Time,
	.Clock_Get_Full_Datetime 	= DS3231_Get_Full_Datetime,

	.Clock_Set_Seconds			= DS3231_Set_Seconds,
	.Clock_Set_Minutes			= DS3231_Set_Minutes,
	.Clock_Set_Hours			= DS3231_Set_Hours,
	.Clock_Set_Day_Of_Week		= DS3231_Set_Day,
	.Clock_Set_Date				= DS3231_Set_Date,
	.Clock_Set_Month			= DS3231_Set_Month,
	.Clock_Set_Year				= DS3231_Set_Year,
	.Clock_Set_Full_Date		= DS3231_Set_Full_Date,
	.Clock_Set_Full_Time		= DS3231_Set_Full_Time,
	.Clock_Set_Full_Datetime	= DS3231_Set_Full_Datetime
};
