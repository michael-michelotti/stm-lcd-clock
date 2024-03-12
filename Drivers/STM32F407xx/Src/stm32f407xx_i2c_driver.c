#include <stdio.h>

#include "stm32f407xx_i2c_driver.h"
#include "stm32f407xx_rcc_driver.h"
#include "stm32f407xx_gpio_driver.h"

/*************** PRIVATE IMPLEMENTATION FUNCTION DECLARATIONS START *****************/
static void I2C_Init(void);
static void I2C_Master_Send(uint8_t *p_tx_buffer, uint32_t len, uint8_t slave_addr, uint8_t repeat_start);
static void I2C_Master_Send_IT(uint8_t *p_tx_buffer, uint32_t len, uint8_t slave_addr, uint8_t repeat_start);
static void I2C_Master_Receive(uint8_t *p_rx_buffer, uint32_t len, uint8_t slave_addr , uint8_t repeat_start);
static void I2C_Master_Receive_IT(uint32_t len, uint8_t slave_addr , uint8_t repeat_start);
static void I2C_DeInit(void);

static void I2C_Handle_SB(void);
static void I2C_Handle_ADDR(void);
static void I2C_Handle_TXE(void);
static void I2C_Handle_RXNE(void);

static void I2C1_GPIO_Pin_Init(void);
static void I2C_Clk_Ctrl(I2C_Register_Map_t *p_i2c_x, uint8_t enable);
static void I2C_Generate_Start_Condition(I2C_Handle_t *p_i2c_handle);
static void I2C_Generate_Stop_Condition(I2C_Handle_t *p_i2c_handle);
static void I2C_Enable_Interrupts(void);
static void I2C_Set_Interrupt_Priority(uint8_t priority);
static uint8_t I2C_Check_Status_Flag(I2C_Handle_t *p_i2c_handle, uint8_t flag_num, uint8_t sr_1_or_2);
static void I2C_Write_Address_Byte(I2C_Handle_t *p_i2c_handle, uint8_t slave_addr, uint8_t read_or_write);
static void I2C_Ack_Control(I2C_Register_Map_t *p_i2c_x, uint8_t enable);
static void I2C_Configure_Clock_Registers(I2C_Handle_t *p_i2c_handle);
static void I2C_Set_CR2_Freq(I2C_Register_Map_t *p_i2c_x, uint32_t sys_clk_freq);
static void I2C_Set_CCR(I2C_Handle_t *p_i2c_handle, uint32_t sys_clk_freq);
static void I2C_Configure_TRISE(I2C_Handle_t *p_i2c_handle, uint32_t sys_clk_freq);
static void I2C_Set_Own_Address(I2C_Handle_t *p_i2c_handle);
/*************** PRIVATE IMPLEMENTATION FUNCTION DECLARATIONS END *****************/

/*************** LOCAL I2C DRIVER VARIABLES START *****************/
static I2C_Handle_t p_i2c_handle;

static uint8_t p_tx_ring_buffer[TX_RING_BUFFER_SIZE];
static uint8_t p_rx_ring_buffer[RX_RING_BUFFER_SIZE];

/* Implements the I2C interface defined in Inc/i2c.h for a STM32F407 I2C peripheral */
static I2C_Interface_t i2c_driver = {
		.Initialize 		= I2C_Init,
		.Write_Bytes 		= I2C_Master_Send,
		.Write_Bytes_IT		= I2C_Master_Send_IT,
		.Read_Bytes 		= I2C_Master_Receive,
		.Read_Bytes_IT		= I2C_Master_Receive_IT,
		.Deinitialize 		= I2C_DeInit,
};

I2C_Interface_t *get_i2c_interface(void)
{
	return &i2c_driver;
}
/*************** LOCAL I2C DRIVER VARIABLES END *****************/

/*************** INTERFACE IMPLEMENTATION FUNCTIONS START *****************/
// Driver functions in order defined above
static void I2C_Init(void)
{
	I2C_Device_t i2c_dev = {
			.clock_speed = I2C_SPEED_SM,
			.own_address = I2C_OWN_ADDR,
			.p_tx_buffer = p_tx_ring_buffer,
			.tx_ring_buffer_write_ptr = 0,
			.tx_ring_buffer_read_ptr = 0,
			.p_rx_buffer = p_rx_ring_buffer,
			.rx_ring_buffer_write_ptr = 0,
			.rx_ring_buffer_read_ptr = 0,
			.tx_len = 0,
			.rx_len = 0,
			.rx_size = 0,
			.control_stage = I2C_CTRL_IDLE,
			.slave_addr = 0,
			.repeat_start = I2C_DISABLE_SR,
			.ack_ctrl = I2C_ACK_EN,
	};

	p_i2c_handle.p_i2c_x = I2C_REG;
	p_i2c_handle.i2c_dev = i2c_dev;
	I2C1_GPIO_Pin_Init();
	I2C_Clk_Ctrl(p_i2c_handle.p_i2c_x, ENABLE);
	I2C_Configure_Clock_Registers(&p_i2c_handle);
	I2C_Set_Own_Address(&p_i2c_handle);
	SET_BIT(I2C_REG->CR1, I2C_CR1_PE_MASK);
	I2C_Enable_Interrupts();
	I2C_Set_Interrupt_Priority(16);
	I2C_Ack_Control(p_i2c_handle.p_i2c_x, p_i2c_handle.i2c_dev.ack_ctrl);
}

static void I2C_Master_Send(uint8_t *p_tx_buffer, uint32_t len, uint8_t slave_addr, uint8_t repeat_start)
{
	/* Sequence diagram for master transmission is on page 849 of the board reference manual */
	/* 1) Generate start condition */
	I2C_Generate_Start_Condition(&p_i2c_handle);

	/* 2) EV5: Start Bit (SB) in SR1. Check SB flag in SR1 to clear EV5 */
	while (!I2C_Check_Status_Flag(&p_i2c_handle, I2C_SR1_SB_POS, I2C_SR1_CHECK));

	/* 3) EV6: ADDR bit set high (meaning address was matched, ACK received from slave) */
	I2C_Write_Address_Byte(&p_i2c_handle, slave_addr, I2C_WRITE);
	while (!I2C_Check_Status_Flag(&p_i2c_handle, I2C_SR1_ADDR_POS, I2C_SR1_CHECK));

	if (!I2C_Check_Status_Flag(&p_i2c_handle, I2C_SR2_TRA_POS, I2C_SR2_CHECK));

	while (len > 0)
	{
		/* 4) EV8_1: TxE = 1, transmit buffer is empty. Write data to DR. */
		while(!I2C_Check_Status_Flag(&p_i2c_handle, I2C_SR1_TXE_POS, I2C_SR1_CHECK));
		p_i2c_handle.p_i2c_x->DR = *p_tx_buffer;
		p_tx_buffer++;
		len--;
	}

	/* 5) After every byte has been sent, wait for TXE=1 and BTF=1. Generate the stop condition. */
	while (!I2C_Check_Status_Flag(&p_i2c_handle, I2C_SR1_TXE_POS, I2C_SR1_CHECK));
	while (!I2C_Check_Status_Flag(&p_i2c_handle, I2C_SR1_BTF_POS, I2C_SR1_CHECK));
	if (repeat_start == I2C_DISABLE_SR)
	{
		I2C_Generate_Stop_Condition(&p_i2c_handle);
	}
}

static void I2C_Master_Send_IT(uint8_t *p_tx_buffer, uint32_t len, uint8_t slave_addr, uint8_t repeat_start)
{
	if (p_i2c_handle.i2c_dev.control_stage == I2C_CTRL_IDLE)
	{
		/* Enable I2C interrupts */
		p_i2c_handle.p_i2c_x->CR2 |= ( 1 << I2C_CR2_ITBUFEN_POS );
		p_i2c_handle.p_i2c_x->CR2 |= ( 1 << I2C_CR2_ITEVTEN_POS );

		/* Fill the I2C TX ring buffer with the data to be sent. */
		I2C_TX_Ring_Buffer_Write(&p_i2c_handle.i2c_dev, p_tx_buffer, len);
		p_i2c_handle.i2c_dev.tx_len = len;
		p_i2c_handle.i2c_dev.slave_addr = slave_addr;
		p_i2c_handle.i2c_dev.repeat_start = repeat_start;
		p_i2c_handle.i2c_dev.control_stage = I2C_CTRL_BUSY_TX;
		I2C_Generate_Start_Condition(&p_i2c_handle);
	}
}

static void I2C_Master_Receive(uint8_t *p_rx_buffer, uint32_t len, uint8_t slave_addr , uint8_t repeat_start)
{
	I2C_Generate_Start_Condition(&p_i2c_handle);

	/* 1) Wait for SB to indicate start condition created. */
	while (!I2C_Check_Status_Flag(&p_i2c_handle, I2C_SR1_SB_POS, I2C_SR1_CHECK));

	/* 2) Write slave address to DR. */
	I2C_Write_Address_Byte(&p_i2c_handle, slave_addr, I2C_READ);

	/* 3) Wait for ADDR bit to go high (meaning address was matched, ACK received from slave). */
	/* TODO: Add timeout in case of NACK for address. */
	while (!I2C_Check_Status_Flag(&p_i2c_handle, I2C_SR1_ADDR_POS, I2C_SR1_CHECK));
	if (!I2C_Check_Status_Flag(&p_i2c_handle, I2C_SR2_TRA_POS, I2C_SR2_CHECK)) {}

	if (len == 1)
	{
		/* If only sending 1 byte, NACK must be sent on first byte. */
		I2C_Ack_Control(p_i2c_handle.p_i2c_x, DISABLE);
		while (!I2C_Check_Status_Flag(&p_i2c_handle, I2C_SR1_RXNE_POS, I2C_SR1_CHECK));
		I2C_Generate_Stop_Condition(&p_i2c_handle);
		*p_rx_buffer = p_i2c_handle.p_i2c_x->DR;
	}
	else
	{
		/* 4) Wait for RxNE equal 1, meaning DR is full. */
		while (len > 0)
		{
			while (!I2C_Check_Status_Flag(&p_i2c_handle, I2C_SR1_RXNE_POS, I2C_SR1_CHECK));
			if (len == 2)
			{
				/* Last byte must be NACKed. When len = 1, ACK must be disabled. */
				I2C_Ack_Control(p_i2c_handle.p_i2c_x, DISABLE);
				I2C_Generate_Stop_Condition(&p_i2c_handle);
			}
			*p_rx_buffer = p_i2c_handle.p_i2c_x->DR;
			p_rx_buffer++;
			len--;
		}
	}

	if (p_i2c_handle.i2c_dev.ack_ctrl == I2C_ACK_EN)
	{
		I2C_Ack_Control(p_i2c_handle.p_i2c_x, ENABLE);
	}

	if ( repeat_start == I2C_DISABLE_SR )
	{
		I2C_Generate_Stop_Condition(&p_i2c_handle);
	}
}

static void I2C_Master_Receive_IT(uint32_t len, uint8_t slave_addr , uint8_t repeat_start)
{
	if (p_i2c_handle.i2c_dev.control_stage == I2C_CTRL_IDLE)
	{
		/* Set the interrupt enable bits for events and buffer events. */
		p_i2c_handle.p_i2c_x->CR2 |= ( 1 << I2C_CR2_ITBUFEN_POS );
		p_i2c_handle.p_i2c_x->CR2 |= ( 1 << I2C_CR2_ITEVTEN_POS );

		p_i2c_handle.i2c_dev.rx_len = len;
		p_i2c_handle.i2c_dev.rx_size = len;
		p_i2c_handle.i2c_dev.slave_addr = slave_addr;
		p_i2c_handle.i2c_dev.repeat_start = repeat_start;
		p_i2c_handle.i2c_dev.control_stage = I2C_CTRL_BUSY_RX;
		I2C_Generate_Start_Condition(&p_i2c_handle);
	}
}

static void I2C_DeInit(void)
{
	/* TODO: Implement I2C deinitialization */
}
/*************** INTERFACE IMPLEMENTATION FUNCTIONS END *****************/

/*************** INTERRUPT HANDLERS START *****************/
/* Overrides STM32F407 I2C interrupt, which is defined in the interrupt vector table.
 * Decodes the event type, routes to appropriate handler */
void I2C1_EV_IRQHandler(void)
{
	if ( GET_BIT(I2C_REG->SR1, I2C_SR1_SB_MASK) )
	{
		/* Handle EV5 - SB is set */
		I2C_Handle_SB();
		return;
	}
	else if ( GET_BIT(I2C_REG->SR1, I2C_SR1_ADDR_MASK) )
	{
		/* Handle EV6 - ADDR is set */
		I2C_Handle_ADDR();
		return;
	}
	else if ( GET_BIT(I2C_REG->SR1, I2C_SR1_TXE_MASK) )
	{
		/* Handle EV8_1, EV8_2 and EV8 - both shift register and DR empty */
		I2C_Handle_TXE();
		return;
	}
	else if ( GET_BIT(I2C_REG->SR1, I2C_SR1_RXNE_MASK) )
	{
		I2C_Handle_RXNE();
		return;
	}
}

static void I2C_Handle_SB(void)
{
	if (p_i2c_handle.i2c_dev.control_stage == I2C_CTRL_BUSY_TX)
	{
		I2C_Write_Address_Byte(&p_i2c_handle, p_i2c_handle.i2c_dev.slave_addr, I2C_WRITE);
	}
	else if (p_i2c_handle.i2c_dev.control_stage == I2C_CTRL_BUSY_RX)
	{
		I2C_Write_Address_Byte(&p_i2c_handle, p_i2c_handle.i2c_dev.slave_addr, I2C_READ);
	}
}

static void I2C_Handle_ADDR(void)
{
	if (p_i2c_handle.i2c_dev.control_stage == I2C_CTRL_BUSY_TX)
	{
		I2C_Check_Status_Flag(&p_i2c_handle, I2C_SR2_TRA_POS, I2C_SR2_CHECK);
	}
	else if (p_i2c_handle.i2c_dev.control_stage == I2C_CTRL_BUSY_RX)
	{
		if (p_i2c_handle.i2c_dev.rx_size == 1)
		{
			/* ACKing must be disabled on peripheral before last byte */
			I2C_Ack_Control(p_i2c_handle.p_i2c_x, DISABLE);
		}
		/* Clear ADDR flag by reading SR2 */
		I2C_Check_Status_Flag(&p_i2c_handle, I2C_SR2_TRA_POS, I2C_SR2_CHECK);
	}
}

static void I2C_Handle_TXE(void)
{
	if (p_i2c_handle.i2c_dev.tx_len <= 0)
	{
		/* If BTF isn't set, transmission isn't done, wait for BTF before stop */
		if (!I2C_Check_Status_Flag(&p_i2c_handle, I2C_SR1_BTF_POS, I2C_SR1_CHECK))
		{
			/* Disable buffer interrupts until BTF, since TXE will trigger repeatedly */
			CLEAR_BIT(p_i2c_handle.p_i2c_x->CR2, I2C_CR2_ITBUFEN_MASK);
			return;
		}
		/* Depending on handle SR field, either generate stop condition or not */
		if (p_i2c_handle.sr == I2C_DISABLE_SR)
		{
			I2C_Generate_Stop_Condition(&p_i2c_handle);
		}
		p_i2c_handle.p_i2c_x->CR2 &= ~( 1 << I2C_CR2_ITBUFEN_POS );
		p_i2c_handle.p_i2c_x->CR2 &= ~( 1 << I2C_CR2_ITEVTEN_POS );
		p_i2c_handle.i2c_dev.control_stage = I2C_CTRL_IDLE;
		I2C_Write_Complete_Callback(&(p_i2c_handle.i2c_dev));
		return;
	}

	/* DR is empty, shift register may or may not be empty. Either way, write next byte into DR */
	p_i2c_handle.p_i2c_x->DR = *I2C_TX_Ring_Buffer_Read(&p_i2c_handle.i2c_dev, 1);
	p_i2c_handle.i2c_dev.tx_len--;
}

static void I2C_Handle_RXNE(void)
{
	uint8_t temp;

	if (p_i2c_handle.i2c_dev.rx_size == 1)
	{
		/* NACK must be sent on first byte for a 1-byte reception. */
		temp = p_i2c_handle.p_i2c_x->DR;
		I2C_RX_Ring_Buffer_Write(&p_i2c_handle.i2c_dev, &temp, 1);
		p_i2c_handle.i2c_dev.rx_len--;
	}
	else
	{
		if (p_i2c_handle.i2c_dev.rx_len == 2)
		{
			/* Last byte must be NACK'd, so ACK must be disabled when len = 2. */
			I2C_Ack_Control(p_i2c_handle.p_i2c_x, DISABLE);
		}
		temp = p_i2c_handle.p_i2c_x->DR;
		I2C_RX_Ring_Buffer_Write(&p_i2c_handle.i2c_dev, &temp, 1);
		p_i2c_handle.i2c_dev.rx_len--;
	}

	if (p_i2c_handle.i2c_dev.rx_len == 0)
	{
		if (p_i2c_handle.i2c_dev.repeat_start == I2C_DISABLE_SR)
		{
			I2C_Generate_Stop_Condition(&p_i2c_handle);
		}
		p_i2c_handle.p_i2c_x->CR2 &= ~( 1 << I2C_CR2_ITBUFEN_POS );
		p_i2c_handle.p_i2c_x->CR2 &= ~( 1 << I2C_CR2_ITEVTEN_POS );
		I2C_Ack_Control(p_i2c_handle.p_i2c_x, ENABLE);
		p_i2c_handle.i2c_dev.control_stage = I2C_CTRL_IDLE;
		I2C_Read_Complete_Callback(&p_i2c_handle.i2c_dev);
	}
}

__weak void I2C_Write_Complete_Callback(I2C_Device_t *p_i2c_dev)
{
	/* implemented at the driver level */
}

__weak void I2C_Read_Complete_Callback(I2C_Device_t *p_i2c_dev)
{
	/* implemented at the driver level */
}

/* Utility functions */
static void I2C1_GPIO_Pin_Init(void)
{
	/* Configure GPIOs for I2C1 peripheral - PB6 = SCL, PB7 = SDA */
	GPIO_Pin_Config_t pb6_scl_config = {
			.gpio_pin_num 			= SCL_PIN_NUM,
			.gpio_pin_mode 			= GPIO_MODE_ALT,
			.gpio_pin_speed			= GPIO_SPEED_MED,
			.gpio_pin_pu_pd_ctrl	= GPIO_PUPD_NONE,
			.gpio_pin_op_type		= GPIO_OUT_OD,
			.gpio_pin_alt_fun_mode	= SDA_ALT_FUN
	};

	GPIO_Handle_t pb6_scl_handle = {
			.p_gpio_x 				= SCL_GPIO,
			.gpio_pin_config		= pb6_scl_config
	};

	GPIO_Init(&pb6_scl_handle);

	GPIO_Pin_Config_t pb7_sda_config = {
			.gpio_pin_num 			= SDA_PIN_NUM,
			.gpio_pin_mode 			= GPIO_MODE_ALT,
			.gpio_pin_speed			= GPIO_SPEED_MED,
			.gpio_pin_pu_pd_ctrl	= GPIO_PUPD_NONE,
			.gpio_pin_op_type		= GPIO_OUT_OD,
			.gpio_pin_alt_fun_mode	= SDA_ALT_FUN
	};

	GPIO_Handle_t pb7_sda_handle = {
			.p_gpio_x 				= SDA_GPIO,
			.gpio_pin_config		= pb7_sda_config
	};

	GPIO_Init(&pb7_sda_handle);
}

static void I2C_Clk_Ctrl(I2C_Register_Map_t *p_i2c_x, uint8_t enable)
{
	if (enable == ENABLE)
	{
		switch ((uint32_t) p_i2c_x)
		{
		case I2C1_BASE_ADDR:
			I2C1_PCLK_EN();
			break;
		case I2C2_BASE_ADDR:
			I2C2_PCLK_EN();
			break;
		case I2C3_BASE_ADDR:
			I2C3_PCLK_EN();
			break;
		}
	}
	if (enable == DISABLE)
	{
		switch ((uint32_t) p_i2c_x)
		{
		case I2C1_BASE_ADDR:
			I2C1_PCLK_DI();
			break;
		case I2C2_BASE_ADDR:
			I2C2_PCLK_DI();
			break;
		case I2C3_BASE_ADDR:
			I2C3_PCLK_DI();
			break;
		}
	}
}

static void I2C_Generate_Start_Condition(I2C_Handle_t *p_i2c_handle)
{
	SET_BIT(p_i2c_handle->p_i2c_x->CR1, I2C_CR1_START_MASK);
}


static void I2C_Generate_Stop_Condition(I2C_Handle_t *p_i2c_handle)
{
	SET_BIT(p_i2c_handle->p_i2c_x->CR1, I2C_CR1_STOP_MASK);
}

static void I2C_Enable_Interrupts(void)
{
	/* Determine which ISER register (0-7) will be used. */
	uint8_t iser_num;
	uint8_t bit_pos;

	switch ((uint32_t) p_i2c_handle.p_i2c_x)
	{
	case I2C1_BASE_ADDR:
		iser_num = I2C1_EV_NVIC_POS / 32;
		bit_pos = I2C1_EV_NVIC_POS % 32;
		break;
	case I2C2_BASE_ADDR:
		iser_num = I2C2_EV_NVIC_POS / 32;
		bit_pos = I2C2_EV_NVIC_POS % 32;
		break;
	case I2C3_BASE_ADDR:
		iser_num = I2C3_EV_NVIC_POS / 32;
		bit_pos = I2C3_EV_NVIC_POS % 32;
		break;
	}

	switch (iser_num)
	{
	case 0:
		*NVIC_ISER_0 |= (1 << bit_pos);
		break;
	case 1:
		*NVIC_ISER_1 |= (1 << bit_pos);
		break;
	case 2:
		*NVIC_ISER_2 |= (1 << bit_pos);
		break;
	case 3:
		*NVIC_ISER_3 |= (1 << bit_pos);
		break;
	}
}

static void I2C_Set_Interrupt_Priority(uint8_t priority)
{
	uint8_t ipr_num;
	uint8_t byte_offset;
	uint8_t bit_offset;

	switch ((uint32_t) p_i2c_handle.p_i2c_x)
	{
	case I2C1_BASE_ADDR:
		ipr_num = I2C1_EV_NVIC_POS / 4;
		byte_offset = I2C1_EV_NVIC_POS % 4;
		break;
	case I2C2_BASE_ADDR:
		ipr_num = I2C2_EV_NVIC_POS / 4;
		byte_offset = I2C2_EV_NVIC_POS % 4;
		break;
	case I2C3_BASE_ADDR:
		ipr_num = I2C3_EV_NVIC_POS / 4;
		byte_offset = I2C3_EV_NVIC_POS % 4;
		break;
	}
	bit_offset = byte_offset * 8;

	uint32_t *p_ipr = NVIC_IPR_0 + ipr_num;
	*p_ipr &= ~(priority << bit_offset);
	*p_ipr |= (priority << bit_offset);
}



static uint8_t I2C_Check_Status_Flag(I2C_Handle_t *p_i2c_handle, uint8_t flag_num, uint8_t sr_1_or_2)
{
	/* 0 = SR1, 1 = SR2 */
	if (!sr_1_or_2)
	{
		return GET_BIT(p_i2c_handle->p_i2c_x->SR1, (1 << flag_num));
	}
	else
	{
		return GET_BIT(p_i2c_handle->p_i2c_x->SR2, (1 << flag_num));
	}
}

static void I2C_Write_Address_Byte(I2C_Handle_t *p_i2c_handle, uint8_t slave_addr, uint8_t read_or_write)
{
	slave_addr <<= 1;
	/* 0 for write, 1 for read. */
	if (!read_or_write)
	{
		slave_addr &= ~1;
	}
	else
	{
		slave_addr |= 1;
	}
	p_i2c_handle->p_i2c_x->DR = slave_addr;
}

static void I2C_Ack_Control(I2C_Register_Map_t *p_i2c_x, uint8_t enable)
{
	if (enable == ENABLE)
	{
		SET_BIT(p_i2c_x->CR1, I2C_CR1_ACK_MASK);
	}
	else
	{
		CLEAR_BIT(p_i2c_x->CR1, I2C_CR1_ACK_MASK);
	}
}

static void I2C_Configure_Clock_Registers(I2C_Handle_t *p_i2c_handle)
{
	uint32_t sys_clk_freq = RCC_Get_Sys_Clk_Frequency();

	/* Set FREQ field in I2C_CR2. */
	I2C_Set_CR2_Freq(p_i2c_handle->p_i2c_x, sys_clk_freq);

	if (p_i2c_handle->i2c_dev.clock_speed <= I2C_SPEED_SM)
	{
		/* Cleared F/S bit means Standard Mode I2C. */
		CLEAR_BIT(p_i2c_handle->p_i2c_x->CCR, (1 << I2C_CCR_FS_POS));
		I2C_Set_CCR(p_i2c_handle, sys_clk_freq);
	}
	else
	{
		/* Set F/S bit means Fast Mode */
		SET_BIT(p_i2c_handle->p_i2c_x->CCR, (1 << I2C_CCR_FS_POS));
		I2C_Set_CCR(p_i2c_handle, sys_clk_freq);
	}

	I2C_Set_CCR(p_i2c_handle, sys_clk_freq);

	I2C_Configure_TRISE(p_i2c_handle, sys_clk_freq);
}

static void I2C_Set_CR2_Freq(I2C_Register_Map_t *p_i2c_x, uint32_t sys_clk_freq)
{
	/* Convert frequency from Hz to MHz */
	sys_clk_freq /= 1000000u;
	/* 5 bit frequency means maximum of 31 */
	if (sys_clk_freq > 31)
		return;
	/* Clear first 5 bits of CR2, then apply freq value */
	SET_FIELD(&p_i2c_x->CR2, I2C_CR2_FREQ_MASK, sys_clk_freq);
}

static void I2C_Set_CCR(I2C_Handle_t *p_i2c_handle, uint32_t sys_clk_freq)
{
	/* All I2C peripherals on APB1 - calculate APB1 frequency */
	uint32_t ccr;
	uint32_t ahb_prescaler = RCC_Get_AHB_Prescaler();
	uint32_t apb1_prescaler = RCC_Get_APB_Prescaler();
	uint32_t apb1_clk = sys_clk_freq / ahb_prescaler / apb1_prescaler;

	if (p_i2c_handle->i2c_dev.clock_speed <= I2C_SPEED_SM)
		ccr = apb1_clk / (I2C_SPEED_SM * 2);
	else
		ccr = apb1_clk / (I2C_SPEED_FM * 3);

	/* Clear bottom 12 bits, then set bottom 12 to calculated CCR */
	SET_FIELD(&p_i2c_handle->p_i2c_x->CCR, I2C_CCR_CCR_MASK, ccr);
}

static void I2C_Configure_TRISE(I2C_Handle_t *p_i2c_handle, uint32_t sys_clk_freq)
{
	uint8_t trise;

	if (p_i2c_handle->i2c_dev.clock_speed <= I2C_SPEED_SM)
	{
		/* Mode is standard - TRISE is (max rise time * apb1 clock) + 1 */
		/* Max rise time is 1000ns = 1e-6, simplifies to (apb1 clock / 1e6) + 1 */
		trise = (sys_clk_freq / 1000000u) + 1;
	}
	else
	{
		/* Mode is fast - max rise time is 300ns - simplifies to (apb1 clock * 3 / 1e7) + 1 */
		trise = ( (sys_clk_freq * 3) / 10000000u ) + 1;
	}

	/* Clear bottom 5 bits of I2C_TRISE register, then set to calculated value */
	SET_FIELD(&p_i2c_handle->p_i2c_x->TRISE, I2C_TRISE_TRISE_MASK, trise);
}

static void I2C_Set_Own_Address(I2C_Handle_t *p_i2c_handle)
{
	/* Clear top bit of own address (in case user sent 8 bit address) */
	p_i2c_handle->i2c_dev.own_address &= ~(1 << 8);
	/* Clear 7-bit address field, then set address */
	SET_FIELD(&p_i2c_handle->p_i2c_x->OAR1, I2C_OAR1_ADD_1_7_MASK, p_i2c_handle->i2c_dev.own_address);
	/* 14th bit must be kept high (unsure why) */
	SET_BIT(p_i2c_handle->p_i2c_x->OAR1, (1 << 14));
}
/*************** PRIVATE IMPLEMENTATION FUNCTIONS END *****************/
