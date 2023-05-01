#include "display.h"

Display_Driver hd44780u_16x2_display_driver = {
	.Display_Initialize 		= LCD_Initialize,
	.Display_Increment_Second 	= LCD_Increment_Second,
	.Display_Update_Datetime 	= LCD_Update_Date_And_Time,
	.Display_Show_Text 			= LCD_Display_Str,
	.Display_Clear				= LCD_Clear,
	.Display_Off				= LCD_Power_Switch,
};
