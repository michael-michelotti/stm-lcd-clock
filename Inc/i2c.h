#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>
#include <stdlib.h>

typedef enum
{
	I2C_CTRL_IDLE,
	I2C_CTRL_BUSY_TX,
	I2C_CTRL_BUSY_RX
} I2C_Ctrl_Stage_t;

typedef enum
{
	I2C_DISABLE_SR,
	I2C_ENABLE_SR
} Repeated_Start_Enable_t;

/* Clock speed options */
#define I2C_SPEED_SM				100000		/* 100kHz clock for standard mode */
#define I2C_SPEED_FM				400000		/* 400kHz clock for fast mode */

/* Enable or disable acknowledge bit after data bit */
#define I2C_ACK_EN        			1
#define I2C_ACK_DI			    	0

typedef struct
{
	void							(*Initialize)();
	void 							(*Read_Bytes)(uint8_t *p_rx_buffer, uint32_t len, uint8_t slave_addr, uint8_t repeat_start);
	void							(*Read_Bytes_IT)(uint32_t len, uint8_t slave_addr, uint8_t repeat_start);
	void	 						(*Write_Bytes)(uint8_t *p_tx_buffer, uint32_t len, uint8_t slave_addr, uint8_t repeat_start);
	void	 						(*Write_Bytes_IT)(uint8_t *p_tx_buffer, uint32_t len, uint8_t slave_addr, uint8_t repeat_start);
	void	 						(*Deinitialize)();
} I2C_Interface_t;

typedef struct
{
	uint32_t						clock_speed;
	uint8_t							own_address;
	uint8_t							*p_tx_buffer;
	uint8_t							tx_ring_buffer_write_ptr;
	uint8_t 						tx_ring_buffer_read_ptr;
	uint8_t							*p_rx_buffer;
	uint8_t							rx_ring_buffer_write_ptr;
	uint8_t							rx_ring_buffer_read_ptr;
	uint32_t						tx_len;
	uint32_t						rx_len;
	uint32_t						rx_size;
	I2C_Ctrl_Stage_t				control_stage;
	uint8_t							slave_addr;
	Repeated_Start_Enable_t			repeat_start;
	uint8_t							ack_ctrl;
} I2C_Device_t;

#define TX_RING_BUFFER_SIZE 		256
#define RX_RING_BUFFER_SIZE 		256

/* The I2C device which implements the I2C interface must implement a TX and RX ring buffer */
uint8_t *I2C_RX_Ring_Buffer_Read(I2C_Device_t *p_i2c_dev, size_t num_bytes);
void I2C_RX_Ring_Buffer_Write(I2C_Device_t *p_i2c_dev, uint8_t *p_src, size_t num_bytes);
uint8_t *I2C_TX_Ring_Buffer_Read(I2C_Device_t *p_i2c_dev, size_t num_bytes);
void I2C_TX_Ring_Buffer_Write(I2C_Device_t *p_i2c_dev, uint8_t *p_src, size_t num_bytes);

/* These callbacks are called at the driver level, for whichever device driver is ipmlementing
 * this I2C interface. */
void I2C_Write_Complete_Callback(I2C_Device_t *p_i2c_dev);
void I2C_Read_Complete_Callback(I2C_Device_t *p_i2c_dev);

void I2C_Error_Handler();

I2C_Interface_t *get_i2c_interface();

#endif /* I2C_H_ */
