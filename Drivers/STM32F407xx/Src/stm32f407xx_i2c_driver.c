#include <stdio.h>

#include "stm32f407xx_i2c_driver.h"
#include "stm32f407xx_rcc_driver.h"
#include "stm32f407xx_gpio_driver.h"

/*************** PRIVATE IMPLEMENTATION FUNCTION DECLARATIONS START *****************/
static void I2C_Init();
static void I2C_Master_Send(uint8_t *p_tx_buffer, uint32_t len, uint8_t slave_addr, uint8_t repeat_start);
static void I2C_Master_Send_IT(uint8_t *p_tx_buffer, uint32_t len, uint8_t slave_addr, uint8_t repeat_start);
static void I2C_Master_Receive(uint8_t *p_rx_buffer, uint32_t len, uint8_t slave_addr , uint8_t repeat_start);
static void I2C_Master_Receive_IT(uint8_t *p_rx_buffer, uint32_t len, uint8_t slave_addr , uint8_t repeat_start);
static void I2C_Cleanup();

static void I2C_Handle_SB();
static void I2C_Handle_ADDR();
static void I2C_Handle_TXE();
static void I2C_Handle_RXNE(void);

static void I2C1_GPIO_Pin_Init();
static void I2C_Clk_Ctrl(I2C_Register_Map_t *p_i2c_x, uint8_t enable);
static void I2C_Generate_Start_Condition(I2C_Handle_t *p_i2c_handle);
static void I2C_Generate_Stop_Condition(I2C_Handle_t *p_i2c_handle);
static void I2C_Enable_Interrupts();
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
static uint8_t p_tx_ring_buffer[255];
static uint8_t p_rx_ring_buffer[255];

static const I2C_Interface_t i2c_driver = {
		.Initialize 		= I2C_Init,
		.Write_Bytes 		= I2C_Master_Send,
		.Write_Bytes_IT		= I2C_Master_Send_IT,
		.Read_Bytes 		= I2C_Master_Receive,
		.Read_Bytes_IT		= I2C_Master_Receive_IT,
		.Deinitialize 		= I2C_Cleanup,
};
/*************** LOCAL I2C DRIVER VARIABLES END *****************/

// Return pointer to the I2C Driver for external interface
I2C_Interface_t *get_i2c_interface()
{
	return &i2c_driver;
}

/*************** PRIVATE IMPLEMENTATION FUNCTIONS START *****************/
// Driver functions in order defined above
static void I2C_Init()
{
	I2C_Device_t i2c_dev = {
			.clock_speed = I2C_SPEED_SM,
			.own_address = I2C_OWN_ADDR,
			.p_tx_buffer = p_tx_ring_buffer,
			.p_rx_buffer = p_rx_ring_buffer,
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

	// configure frequency and CCR
	I2C_Configure_Clock_Registers(&p_i2c_handle);

	// CR1 AND CR2 CONFIGURATION
	I2C_Set_Own_Address(&p_i2c_handle);

	// Enable peripheral
	SET_BIT(I2C_REG->CR1, I2C_CR1_PE_MASK);

	// Enable interrupts
	I2C_Enable_Interrupts();
	I2C_Set_Interrupt_Priority(16);

	// Ack bit must be set after peripheral is enabled
	if (p_i2c_handle.i2c_dev.ack_ctrl == I2C_ACK_EN)
	{
		I2C_Ack_Control(p_i2c_handle.p_i2c_x, ENABLE);
	}
	else
	{
		I2C_Ack_Control(p_i2c_handle.p_i2c_x, DISABLE);
	}
}

static void I2C_Master_Send(uint8_t *p_tx_buffer, uint32_t len, uint8_t slave_addr, uint8_t repeat_start)
{
	// MASTER TRANSMISSION
	// sequence diagram for master transmission is on page 849 of the board reference manual
	// 1) generate start condition
	I2C_Generate_Start_Condition(&p_i2c_handle);

	// 2) EV5 (not an interrupt in this blocking API). Start Bit (SB) in SR1
	// check SB flag in SR1 to clear EV5
	while (!I2C_Check_Status_Flag(&p_i2c_handle, I2C_SR1_SB_POS, I2C_SR1_CHECK));

	// 3) After receiving ACK from slave, event EV6. ADDR bit set high (meaning address was matched)
	// check ADDR flag in SR1, then check TRA flag in SR2 to verify we are configured as transmitter
	I2C_Write_Address_Byte(&p_i2c_handle, slave_addr, I2C_WRITE);
	while (!I2C_Check_Status_Flag(&p_i2c_handle, I2C_SR1_ADDR_POS, I2C_SR1_CHECK));

	if (!I2C_Check_Status_Flag(&p_i2c_handle, I2C_SR2_TRA_POS, I2C_SR2_CHECK));

	while (len > 0)
	{
		// 4) After EV6 event cleared, EV8_1 event triggers immediately, meaning TxE = 1, transmit buffer is empty
		// wait for TxE to be set to 1, indicating EV8_1 event has triggered. write data to DR
		while(!I2C_Check_Status_Flag(&p_i2c_handle, I2C_SR1_TXE_POS, I2C_SR1_CHECK));
		p_i2c_handle.p_i2c_x->DR = *p_tx_buffer;
		p_tx_buffer++;
		len--;
	}

	// 6) After every byte has been sent, generate the stop condition
	// once length becomes 0, wait for txe=1 and btf=1, then generate stop condition
	while (!I2C_Check_Status_Flag(&p_i2c_handle, I2C_SR1_TXE_POS, I2C_SR1_CHECK));
	while (!I2C_Check_Status_Flag(&p_i2c_handle, I2C_SR1_BTF_POS, I2C_SR1_CHECK));

	// only generate the stop condition if we don't want to use repeated start
	if (repeat_start == I2C_DISABLE_SR)
	{
		I2C_Generate_Stop_Condition(&p_i2c_handle);
	}
}

static void I2C_Master_Send_IT(uint8_t *p_tx_buffer, uint32_t len, uint8_t slave_addr, uint8_t repeat_start)
{
	if (p_i2c_handle.i2c_dev.control_stage == I2C_CTRL_IDLE)
	{
		// set the interrupt enable bits for events and buffer events
		p_i2c_handle.p_i2c_x->CR2 |= ( 1 << I2C_CR2_ITBUFEN_POS );
		p_i2c_handle.p_i2c_x->CR2 |= ( 1 << I2C_CR2_ITEVTEN_POS );

		// fill the i2c ring buffer with the data to be sent
		uint8_t *curr_ring_buffer_pos = p_i2c_handle.i2c_dev.p_tx_buffer;
		for (int i = 0; i < len; i++)
		{
			*curr_ring_buffer_pos = *p_tx_buffer;
			curr_ring_buffer_pos++;
			p_tx_buffer++;
		}
		p_i2c_handle.i2c_dev.tx_len = len;
		p_i2c_handle.i2c_dev.slave_addr = slave_addr;
		p_i2c_handle.i2c_dev.repeat_start = repeat_start;
		p_i2c_handle.i2c_dev.control_stage = I2C_CTRL_BUSY_TX;
		I2C_Generate_Start_Condition(&p_i2c_handle);
	}
}

static void I2C_Master_Receive(uint8_t *p_rx_buffer, uint32_t len, uint8_t slave_addr , uint8_t repeat_start)
{
	// MASTER RECEIVER
	// 1) Generate start condition (same as master send..)
	I2C_Generate_Start_Condition(&p_i2c_handle);

	// 2) Wait for SB to indicate start condition created
	while (!I2C_Check_Status_Flag(&p_i2c_handle, I2C_SR1_SB_POS, I2C_SR1_CHECK));

	// 3) Write slave address to DR with read bit (last bit 1)
	I2C_Write_Address_Byte(&p_i2c_handle, slave_addr, I2C_READ);

	// wait for ADDR bit to set high. need to check SR2 as well (ADDR is in SR1)
	while (!I2C_Check_Status_Flag(&p_i2c_handle, I2C_SR1_ADDR_POS, I2C_SR1_CHECK));
	if (!I2C_Check_Status_Flag(&p_i2c_handle, I2C_SR2_TRA_POS, I2C_SR2_CHECK)) {}

	if (len == 1)
	{
		// NACK must be sent on first byte
		I2C_Ack_Control(p_i2c_handle.p_i2c_x, DISABLE);
		while (!I2C_Check_Status_Flag(&p_i2c_handle, I2C_SR1_RXNE_POS, I2C_SR1_CHECK));
		I2C_Generate_Stop_Condition(&p_i2c_handle);
		*p_rx_buffer = p_i2c_handle.p_i2c_x->DR;
	}
	else
	{
		// 4) Wait for RxNE equal 1, meaning DR is full
		while (len > 0)
		{
			while (!I2C_Check_Status_Flag(&p_i2c_handle, I2C_SR1_RXNE_POS, I2C_SR1_CHECK));
			if (len == 2)
			{
				// last byte when len = 1 must be NACK'd, so ACK must be disabled
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

static void I2C_Master_Receive_IT(uint8_t *p_rx_buffer, uint32_t len, uint8_t slave_addr , uint8_t repeat_start)
{
	if (p_i2c_handle.i2c_dev.control_stage == I2C_CTRL_IDLE)
	{
		// set the interrupt enable bits for events and buffer events
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

static void I2C_Cleanup()
{
	// TBD
}

// Interrupt handlers
void I2C1_EV_IRQHandler(void)
{
	if ( GET_BIT(I2C_REG->SR1, I2C_SR1_SB_MASK) )
	{
		// Handle EV5 - SB is set
		I2C_Handle_SB();
		return;
	}
	else if ( GET_BIT(I2C_REG->SR1, I2C_SR1_ADDR_MASK) )
	{
		// Handle EV6 - ADDR is set
		I2C_Handle_ADDR();
		return;
	}
	else if ( GET_BIT(I2C_REG->SR1, I2C_SR1_TXE_MASK) )
	{
		// Handle EV8_1, EV8_2 and EV8 - both shift register and DR empty
		I2C_Handle_TXE();
		return;
	}
	else if ( GET_BIT(I2C_REG->SR1, I2C_SR1_RXNE_MASK) )
	{
		I2C_Handle_RXNE();
		return;
	}
}

static void I2C_Handle_SB()
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

static void I2C_Handle_ADDR()
{
	if (p_i2c_handle.i2c_dev.control_stage == I2C_CTRL_BUSY_TX)
	{
		I2C_Check_Status_Flag(&p_i2c_handle, I2C_SR2_TRA_POS, I2C_SR2_CHECK);
	}
	else if (p_i2c_handle.i2c_dev.control_stage == I2C_CTRL_BUSY_RX)
	{
		if (p_i2c_handle.i2c_dev.rx_size == 1)
		{
			// ACKing must be disabled on peripheral before last byte
			I2C_Ack_Control(p_i2c_handle.p_i2c_x, DISABLE);
			//I2C_Generate_Stop_Condition(&p_i2c_handle);
		}
		// clear ADDR flag by reading SR2
		I2C_Check_Status_Flag(&p_i2c_handle, I2C_SR2_TRA_POS, I2C_SR2_CHECK);
	}
}

static void I2C_Handle_TXE()
{
	if (p_i2c_handle.i2c_dev.tx_len <= 0)
	{
		// if BTF isn't set, transmission isn't done, wait for BTF before stop
		if (!I2C_Check_Status_Flag(&p_i2c_handle, I2C_SR1_BTF_POS, I2C_SR1_CHECK))
		{
			// disable buffer interrupts until BTF, since TXE will trigger repeatedly
			CLEAR_BIT(p_i2c_handle.p_i2c_x->CR2, I2C_CR2_ITBUFEN_MASK);
			return;
		}
		// depending on handle SR field, either generate stop condition or not
		//if (p_i2c_handle.sr == I2C_DISABLE_SR)
		if (1)
		{
			I2C_Generate_Stop_Condition(&p_i2c_handle);
		}
		p_i2c_handle.p_i2c_x->CR2 &= ~( 1 << I2C_CR2_ITBUFEN_POS );
		p_i2c_handle.p_i2c_x->CR2 &= ~( 1 << I2C_CR2_ITEVTEN_POS );
		p_i2c_handle.i2c_dev.control_stage = I2C_CTRL_IDLE;
		I2C_Write_Complete_Callback(&(p_i2c_handle.i2c_dev));
		return;
	}

	// DR is empty, shift register may or may not be empty. Either way, write next byte into DR
	p_i2c_handle.p_i2c_x->DR = *p_i2c_handle.i2c_dev.p_tx_buffer;
	p_i2c_handle.i2c_dev.p_tx_buffer++;
	p_i2c_handle.i2c_dev.tx_len--;
}

static void I2C_Handle_RXNE(void)
{
	if (p_i2c_handle.i2c_dev.rx_size == 1)
	{
		// NACK must be sent on first byte for one byte receive
		//I2C_Ack_Control(p_i2c_handle.p_i2c_x, DISABLE);
		//I2C_Generate_Stop_Condition(&p_i2c_handle);
		*p_i2c_handle.i2c_dev.p_rx_buffer = p_i2c_handle.p_i2c_x->DR;
		p_i2c_handle.i2c_dev.p_rx_buffer++;
		p_i2c_handle.i2c_dev.rx_len--;
	}
	else
	{
		if (p_i2c_handle.i2c_dev.rx_len == 2)
		{
			// last byte when len = 1 must be NACK'd, so ACK must be disabled when len = 2
			I2C_Ack_Control(p_i2c_handle.p_i2c_x, DISABLE);
		}
		*p_i2c_handle.i2c_dev.p_rx_buffer = p_i2c_handle.p_i2c_x->DR;
		p_i2c_handle.i2c_dev.p_rx_buffer++;
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
		p_i2c_handle.i2c_dev.control_stage = I2C_CTRL_IDLE;
		I2C_Read_Complete_Callback(&p_i2c_handle.i2c_dev);
	}
}

__weak void I2C_Write_Complete_Callback(I2C_Device_t *p_i2c_dev)
{
	// implemented at the driver level
}

__weak void I2C_Read_Complete_Callback(I2C_Device_t *p_i2c_dev)
{
	// implemented at the driver level
}

// UTILITY FUNCTIONS
static void I2C1_GPIO_Pin_Init()
{
	// configure GPIOs for I2C2 peripheral - PB6 = SCL, PB7 = SDA
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

static void I2C_Enable_Interrupts()
{
	// determine which ISER register (0-7) will be used
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
	// 0 = SR1, 1 = SR2
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
	// 0 for write, 1 for read
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

	// set FREQ field in I2C_CR2
	I2C_Set_CR2_Freq(p_i2c_handle->p_i2c_x, sys_clk_freq);

	if (p_i2c_handle->i2c_dev.clock_speed <= I2C_SPEED_SM)
	{
		// cleared F/S bit means Standard Mode I2C
		CLEAR_BIT(p_i2c_handle->p_i2c_x->CCR, (1 << I2C_CCR_FS_POS));
		I2C_Set_CCR(p_i2c_handle, sys_clk_freq);
	}
	else
	{
		// Set F/S bit means Fast Mode
		SET_BIT(p_i2c_handle->p_i2c_x->CCR, (1 << I2C_CCR_FS_POS));
		I2C_Set_CCR(p_i2c_handle, sys_clk_freq);
	}

	I2C_Set_CCR(p_i2c_handle, sys_clk_freq);

	I2C_Configure_TRISE(p_i2c_handle, sys_clk_freq);
}

static void I2C_Set_CR2_Freq(I2C_Register_Map_t *p_i2c_x, uint32_t sys_clk_freq)
{
	// convert frequency from Hz to MHz
	sys_clk_freq /= 1000000u;
	// 5 bit frequency means maximum of 31
	if (sys_clk_freq > 31)
		return;
	// clear first 5 bits of CR2, then apply freq value
	SET_FIELD(&p_i2c_x->CR2, I2C_CR2_FREQ_MASK, sys_clk_freq);
}

static void I2C_Set_CCR(I2C_Handle_t *p_i2c_handle, uint32_t sys_clk_freq)
{
	// all I2C peripherals on APB1 - calculate APB1 frequency
	uint32_t ccr;
	uint32_t ahb_prescaler = RCC_Get_AHB_Prescaler();
	uint32_t apb1_prescaler = RCC_Get_APB_Prescaler();
	uint32_t apb1_clk = sys_clk_freq / ahb_prescaler / apb1_prescaler;

	if (p_i2c_handle->i2c_dev.clock_speed <= I2C_SPEED_SM)
		ccr = apb1_clk / (I2C_SPEED_SM * 2);
	else
		ccr = apb1_clk / (I2C_SPEED_FM * 3);

	// clear bottom 12 bits, then set bottom 12 to calculated CCR
	SET_FIELD(&p_i2c_handle->p_i2c_x->CCR, I2C_CCR_CCR_MASK, ccr);
}

static void I2C_Configure_TRISE(I2C_Handle_t *p_i2c_handle, uint32_t sys_clk_freq)
{
	uint8_t trise;

	if (p_i2c_handle->i2c_dev.clock_speed <= I2C_SPEED_SM)
	{
		// mode is standard - TRISE is (max rise time * apb1 clock) + 1
		// max rise time is 1000ns = 1e-6, simplifies to (apb1 clock / 1e6) + 1
		trise = (sys_clk_freq / 1000000u) + 1;
	}
	else
	{
		// mode is fast - max rise time is 300ns - simplifies to (apb1 clock * 3 / 1e7) + 1
		trise = ( (sys_clk_freq * 3) / 10000000u ) + 1;
	}

	// clear bottom 5 bits of I2C_TRISE register, then set to calculated value
	SET_FIELD(&p_i2c_handle->p_i2c_x->TRISE, I2C_TRISE_TRISE_MASK, trise);
}

static void I2C_Set_Own_Address(I2C_Handle_t *p_i2c_handle)
{
	// clear top bit of own address (in case user sent 8 bit address)
	p_i2c_handle->i2c_dev.own_address &= ~(1 << 8);
	// clear 7-bit address field, then set address
	SET_FIELD(&p_i2c_handle->p_i2c_x->OAR1, I2C_OAR1_ADD_1_7_MASK, p_i2c_handle->i2c_dev.own_address);
	// 14th bit must be kept high (unsure why)
	SET_BIT(p_i2c_handle->p_i2c_x->OAR1, (1 << 14));
}
/*************** PRIVATE IMPLEMENTATION FUNCTIONS END *****************/
