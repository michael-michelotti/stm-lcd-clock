#include "hal_driver.h"
#include "stm32f407xx_gpio_driver.h"
#include "stm32f407xx_i2c_driver.h"


#define I2C_BASE 		I2C2

I2C_Config_t stm32f407xx_i2c_config = {
		.scl_speed 		= I2C_SPEED_SM,
		.dev_addr 		= 0x62,
		.ack_ctrl		= I2C_ACK_EN,
		.fm_duty_cycle	= I2C_FM_DUTY_2
};

I2C_Handle_t stm32f407xx_i2c_handle = {
		.p_i2c_x 		= I2C2,
		.i2c_config 	= stm32f407xx_i2c_config,
		.p_tx_buffer	= NULL,
		.p_rx_buffer	= NULL,
		.tx_len			= 0,
		.rx_len			= 0,
		.tx_rx_state	= I2C_STATE_TX,
		.slave_addr		= 0,
		.sr				= I2C_DISABLE_SR
};

void STM32F407XX_System_Clock_Config(void)
{
}

// Adapts STM32F407XX-specific I2C write to generic HAL I2C write
void STM32F407XX_I2C_Write_IT(uint8_t *p_tx_buffer, uint32_t len, uint8_t slave_addr, uint8_t sr)
{
	I2C_Master_Send_IT(stm32f407xx_i2c_handle, p_tx_buffer, len, slave_addr, sr);
}

void STM32F407XX_I2C_Read_IT(uint8_t *p_rx_buffer, uint32_t len, uint8_t slave_addr, uint8_t sr)
{
	I2C_Master_Receive_IT(stm32f407xx_i2c_handle, p_tx_buffer, len, slave_addr, sr);
}

void STM32F407XX_GPIO_Init(void)
{
	// PB10 = I2C2 SCL, PB11 = I2C2 SDA
	GPIO_Pin_Config_t i2c_scl = {
			.gpio_pin_num = 10,
			.gpio_pin_mode = GPIO_MODE_ALT,
			.gpio_pin_speed = GPIO_SPEED_HIGH,
			.gpio_pin_pu_pd_ctrl = GPIO_PUPD_NONE, // must use external pull-up, internal too much bus capacitance
			.gpio_pin_op_type = GPIO_OUT_OD,
			.gpio_pin_alt_fun_mode = 4
	};

	GPIO_Handle_t scl_handle = {
			.GPIO_Register_Map_t = GPIOB,
			.GPIO_Pin_Config_t = i2c_scl
	};

	GPIO_Init(&scl_handle);

	GPIO_Pin_Config_t i2c_sda = {
			.gpio_pin_num = 11,
			.gpio_pin_mode = GPIO_MODE_ALT,
			.gpio_pin_speed = GPIO_SPEED_HIGH,
			.gpio_pin_pu_pd_ctrl = GPIO_PUPD_NONE, // must use external pull-up, internal too much bus capacitance
			.gpio_pin_op_type = GPIO_OUT_OD,
			.gpio_pin_alt_fun_mode = 4
	};
	GPIO_Handle_t sda_handle = { GPIOB, i2c_sda };
	GPIO_Init(&sda_handle);
}

void I2C2_EV_IRQHandler(void)
{
	HAL_I2C2_EV_IRQHandler(&stm32f407xx_i2c_handle);
}

void STM32F407XX_I2C_Init(void)
{
	I2C_Init(&stm32f407xx_i2c_handle, ENABLE);
}

HAL_Driver stm32f407xx_hal_driver = {
	.HAL_System_Clock_Config 	= STM32F407XX_System_Clock_Config,
	.HAL_GPIO_Init 				= STM32F407XX_GPIO_Init,
	.HAL_I2C_Init 				= STM32F407XX_I2C_Init,
	.HAL_I2C_Write_IT 			= STM32F407XX_I2C_Write_IT,
	.HAL_I2C_Read_IT 			= STM32F407XX_I2C_Read_IT,
};
