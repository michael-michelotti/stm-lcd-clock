typedef struct
{
	I2C_Register_Map_t	*p_i2c_x;
	I2C_Config_t		i2c_config;
	uint8_t 			*p_tx_buffer; 	// Pointer to data being transmitted
	uint8_t 			*p_rx_buffer;	// Pointer to data to be received
	uint32_t 			tx_len;			// Length of data to be transmitted
	uint32_t 			rx_len;			// Length of data to be received
	uint8_t 			tx_rx_state;	// Whether board is transmitting or receiving
	uint8_t 			slave_addr;		// Slave device address
    uint32_t        	rx_size;		//
    uint8_t         	sr;				// Repeated start value
    I2C_Task_t			task_queue[8];	// Queue of I2C tasks so I2C callback knows what next I2C task is
    uint8_t				current_task;
    uint8_t				num_tasks;
} I2C_Handle_t;

typedef struct
{
	void (*HAL_System_Clock_Config)(void);
	void (*HAL_GPIO_Init)(void);
	void (*HAL_I2C_Init)(void);
	void (*HAL_I2C_Write_IT)(void);
	void (*HAL_I2C_Read_IT)(void);
	I2C_Handle_t HAL_I2C_Handle;
} HAL_Driver;
