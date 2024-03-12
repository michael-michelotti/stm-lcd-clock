#include "i2c.h"

uint8_t ring_buffer_output_buffer[256];

void I2C_Error_Handler()
{
    while(1){}
}

uint8_t *I2C_RX_Ring_Buffer_Read(I2C_Device_t *p_i2c_dev, size_t num_bytes)
{
    uint8_t *out_buffer_start = ring_buffer_output_buffer;
    for (int i = 0; i < num_bytes; i++)
    {
        *out_buffer_start = p_i2c_dev->p_rx_buffer[p_i2c_dev->rx_ring_buffer_read_ptr];
        p_i2c_dev->rx_ring_buffer_read_ptr++;
        p_i2c_dev->rx_ring_buffer_read_ptr %= RX_RING_BUFFER_SIZE;
        out_buffer_start++;
    }

    return ring_buffer_output_buffer;
}

void I2C_RX_Ring_Buffer_Write(I2C_Device_t *p_i2c_dev, uint8_t *p_src, size_t num_bytes)
{
    for (int i = 0; i < num_bytes; i++)
    {
        p_i2c_dev->p_rx_buffer[p_i2c_dev->rx_ring_buffer_write_ptr] = *p_src;
        p_i2c_dev->rx_ring_buffer_write_ptr++;
        p_i2c_dev->rx_ring_buffer_write_ptr %= TX_RING_BUFFER_SIZE;

        if (p_i2c_dev->rx_ring_buffer_write_ptr == p_i2c_dev->rx_ring_buffer_read_ptr)
        {
            /* Write pointer has lapped read pointer - ring buffer overflow */
            I2C_Error_Handler();
        }
        p_src++;
    }
}

uint8_t *I2C_TX_Ring_Buffer_Read(I2C_Device_t *p_i2c_dev, size_t num_bytes)
{
    uint8_t *out_buffer_start = ring_buffer_output_buffer;
    for (int i = 0; i < num_bytes; i++)
    {
        *out_buffer_start = p_i2c_dev->p_tx_buffer[p_i2c_dev->tx_ring_buffer_read_ptr];
        p_i2c_dev->tx_ring_buffer_read_ptr++;
        p_i2c_dev->tx_ring_buffer_read_ptr %= TX_RING_BUFFER_SIZE;
        out_buffer_start++;
    }

    return ring_buffer_output_buffer;
}

void I2C_TX_Ring_Buffer_Write(I2C_Device_t *p_i2c_dev, uint8_t *p_src, size_t num_bytes)
{
    for (int i = 0; i < num_bytes; i++)
    {
        p_i2c_dev->p_tx_buffer[p_i2c_dev->tx_ring_buffer_write_ptr] = *p_src;
        p_i2c_dev->tx_ring_buffer_write_ptr++;
        p_i2c_dev->tx_ring_buffer_write_ptr %= TX_RING_BUFFER_SIZE;

        if (p_i2c_dev->tx_ring_buffer_write_ptr == p_i2c_dev->tx_ring_buffer_read_ptr)
        {
            /* Write pointer has lapped read pointer - ring buffer overflow */
            I2C_Error_Handler();
        }
        p_src++;
    }
}
