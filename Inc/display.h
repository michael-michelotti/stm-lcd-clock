#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "time.h"

#define LCD1602A

typedef enum
{
	DISPLAY_CTRL_INIT,
	DISPLAY_CTRL_IDLE,
	DISPLAY_CTRL_UPDATING
} Display_Ctrl_Stage_t;


typedef struct
{
	Display_Ctrl_Stage_t ctrl_stage;
} Display_Device_t;


typedef struct
{
	void			(*Display_Initialize)(Display_Device_t);
	void			(*Display_On)(void);
	void			(*Display_Off)(void);
	void			(*Display_Clear)(void);
	void			(*Display_Update_Seconds)(seconds_t seconds);
	void			(*Display_Update_Minutes)(minutes_t minutes);
	void			(*Display_Update_Hours)(hours_t hours);
	void			(*Display_Update_Time)(full_time_t full_time);
	void			(*Display_Update_Date)(date_t date);
	void			(*Display_Update_Day_Of_Week)(day_of_week_t dow);
	void			(*Display_Update_Month)(month_t month);
	void			(*Display_Update_Year)(year_t year, century_t century);
	void			(*Display_Update_Full_Date)(full_date_t full_date);
	void			(*Display_Update_Datetime)(full_datetime_t datetime);
} Display_Driver_t;

Display_Driver_t *get_display_driver();

void Display_Update_Seconds_Callback();

#ifdef LCD1602A
#	include "lcd1602a_display_driver.h"
#else
#	error "Display driver not defined."
#endif

#endif /* DISPLAY_H_ */
