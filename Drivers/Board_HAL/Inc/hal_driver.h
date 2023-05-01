typedef struct
{
	void (*HAL_System_Clock_Config)(void);
	void (*HAL_GPIO_Init)(void);
	void (*HAL_I2C_Init)(void);
} HAL_Driver;
