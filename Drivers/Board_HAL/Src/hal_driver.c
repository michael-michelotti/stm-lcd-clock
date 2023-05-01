#include "hal_driver.h"
#include "stm32f407xx_gpio_driver.h"
#include "stm32f407xx_i2c_driver.h"

I2C_Handle_t global_i2c_handle = { '\0' };
GPIO_Handle_t global_gpio_handle = { '\0' };

void STM32F407XX_System_Clock_Config(void)
{

}

void STM32F407XX_GPIO_Init(void)
{
	// PB10 = SCL, PB11 = SDA
	GPIO_Pin_Config_t pb10 = { 10, GPIO_MODE_ALT, GPIO_SPEED_HIGH, GPIO_PUPD_NONE, GPIO_OUT_OD, 4 };
	GPIO_Handle_t scl_handle = { GPIOB, pb10 };
	GPIO_Init(&scl_handle);

	GPIO_Pin_Config_t pb11 = { 11, GPIO_MODE_ALT, GPIO_SPEED_HIGH, GPIO_PUPD_NONE, GPIO_OUT_OD, 4 };
	GPIO_Handle_t sda_handle = { GPIOB, pb11 };
	GPIO_Init(&sda_handle);

	GPIO_Pin_Config_t toggle = { 0, GPIO_MODE_OUT, GPIO_SPEED_HIGH, GPIO_PUPD_NONE, GPIO_OUT_PP, 0 };
	global_gpio_handle.gpio_pin_config = toggle;
	global_gpio_handle.p_gpio_x = GPIOE;
	GPIO_Init(&global_gpio_handle);
}

void STM32F407XX_I2C_Init(void)
{
	I2C_Config_t init_config = { I2C_SPEED_SM, 0x62, 1, I2C_FM_DUTY_2 };
	global_i2c_handle.p_i2c_x = I2C2;
	global_i2c_handle.i2c_config = init_config;
	global_i2c_handle.p_tx_buffer = tx_buffer;
	global_i2c_handle.p_rx_buffer = rx_buffer;
	global_i2c_handle.tx_len = 0;
	global_i2c_handle.rx_len = 0;
	global_i2c_handle.tx_rx_state = 0;
	global_i2c_handle.slave_addr = DS3231_SLAVE_ADDR;
	global_i2c_handle.rx_size = 0;
	global_i2c_handle.sr = 0;
}

HAL_Driver stm32f407xx_hal_driver =
{
	.HAL_System_Clock_Config = STM32F407XX_System_Clock_Config,
	.HAL_GPIO_Init = STM32F407XX_GPIO_Init,
	.HAL_I2C_Init = STM32F407XX_I2C_Init
};
