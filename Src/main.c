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
	Clock_Driver_t 			*app_clock_driver = get_clock_driver();
	// Display_Driver_t		app_display_driver = get_display();

	app_clock_driver->Initialize();
	app_clock_driver->Set_Seconds_IT(22);

	seconds_t secs;
	for(;;)
	{
		delay();
		app_clock_driver->Get_Seconds_IT();
		delay();
		app_clock_driver->Get_Minutes_IT();
		delay();
		secs = app_clock_driver->Get_Seconds();
		printf("Blocking secs: %u\n", (unsigned int) secs);
		delay();
		//app_display_driver.Display_Update_Time(curr_time);
		//curr_time = app_clock_driver.Get_Full_Time();
	}
}

void Clock_Get_Seconds_Complete_Callback(seconds_t secs)
{
	// this is probably where i should update my display right?
	printf("Current Seconds: %u\n", (unsigned int) secs);
}

void Clock_Get_Minutes_Complete_Callback(minutes_t mins)
{
	printf("Current Minutes: %u\n", (unsigned int) mins);
}
