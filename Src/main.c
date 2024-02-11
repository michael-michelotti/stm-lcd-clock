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


int main(void)
{
	Clock_Driver_t 			app_clock_driver = get_clock();
	// Display_Driver_t		app_display_driver = get_display();

	app_clock_driver.Initialize();
	//seconds_t secs = app_clock_driver.Get_Seconds_IT();
	app_clock_driver.Get_Seconds_IT();

	for(;;)
	{
		//app_display_driver.Display_Update_Time(curr_time);
		//curr_time = app_clock_driver.Get_Full_Time();
	}
}
