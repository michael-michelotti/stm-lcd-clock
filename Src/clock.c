#include "clock.h"
#include "stm32f407xx.h"


__weak void Clock_Get_Seconds_Complete_Callback(seconds_t secs)
{
	// implemented in application code
}

__weak void Clock_Get_Minutes_Complete_Callback(minutes_t mins)
{
	// implemented in application code
}
