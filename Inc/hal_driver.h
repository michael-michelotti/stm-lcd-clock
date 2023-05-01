I2C_Config_t stm32f407xx_i2c_config;
I2C_Handle_t stm32f407xx_i2c_handle;

typedef struct
{
    void (*I2C_Init)(void);
    void (*I2C_Write_IT)(void);
    void (*I2C_Read_IT)(void);
} I2C_Interface_t;

I2C_Interface_t HAL_Get_I2C_Interface(HAL_Driver *driver)
{
	I2C_Interface_t i2c_interface;
	i2c_interface.I2C_Init = driver->HAL_I2C_Init;
	i2c_interface.I2C_Write_IT = driver->HAL_I2C_Write_IT;
	i2c_interface.I2C_Read_IT = driver->HAL_I2C_Read_IT;
	return i2c_interface;
}

typedef struct
{
	void (*HAL_System_Clock_Config)(void);
	void (*HAL_GPIO_Init)(void);
	void (*HAL_I2C_Init)(void);
	void (*HAL_I2C_Write_IT)(void);
	void (*HAL_I2C_Read_IT)(void);
	I2C_Handle_t HAL_I2C_Handle;
} HAL_Driver;
