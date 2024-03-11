#include "clock.h"
#include "stm32f407xx.h"


__weak void Clock_Get_Seconds_Complete_Callback(Clock_Device_t *clock_dev)
{
	// implemented in application code
}

__weak void Clock_Get_Minutes_Complete_Callback(Clock_Device_t *clock_dev)
{
	// implemented in application code
}


__weak void Clock_Set_Seconds_Complete_Callback(Clock_Device_t *clock_dev)
{
	// implemented in application code
}

__weak void Clock_Get_Hours_Complete_Callback(Clock_Device_t *clock_dev)
{
	// implemented in application code
}
