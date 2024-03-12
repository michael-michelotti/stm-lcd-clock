#ifndef INC_STM32F407XX_GPIO_DRIVER_H_
#define INC_STM32F407XX_GPIO_DRIVER_H_

#include "stm32f407xx.h"
#include <stdint.h>

#define GPIO_PIN_0              0
#define GPIO_PIN_1              1
#define GPIO_PIN_2              2
#define GPIO_PIN_3              3
#define GPIO_PIN_4              4
#define GPIO_PIN_5              5
#define GPIO_PIN_6              6
#define GPIO_PIN_7              7
#define GPIO_PIN_8              8
#define GPIO_PIN_9              9
#define GPIO_PIN_10             10   
#define GPIO_PIN_11             11
#define GPIO_PIN_12             12
#define GPIO_PIN_13             13
#define GPIO_PIN_14             14
#define GPIO_PIN_15             15

typedef struct
{
    uint8_t gpio_pin_num;
    uint8_t gpio_pin_mode;
    uint8_t gpio_pin_speed;
    uint8_t gpio_pin_pu_pd_ctrl;
    uint8_t gpio_pin_op_type;
    uint8_t gpio_pin_alt_fun_mode;
} GPIO_Pin_Config_t;

typedef struct
{
    GPIO_Register_Map_t *p_gpio_x;
    GPIO_Pin_Config_t gpio_pin_config;
} GPIO_Handle_t;

typedef enum
{
    GPIO_MODE_IN,
    GPIO_MODE_OUT,
    GPIO_MODE_ALT,
    GPIO_MODE_ANALOG,
    GPIO_MODE_IN_FE,    // interrupt mode, falling edge detection
    GPIO_MODE_IN_RE,    // interrupt mode, rising edge detection
    GPIO_MODE_IN_RFT    // interrupt mode, rising and falling edge detection
} GPIO_Pin_Mode_t;

typedef enum
{
    GPIO_OUT_PP,
    GPIO_OUT_OD
} GPIO_Output_Mode_t;

typedef enum
{
    GPIO_SPEED_LOW,
    GPIO_SPEED_MED,
    GPIO_SPEED_FAST,
    GPIO_SPEED_HIGH
} GPIO_Output_Speed_t;

typedef enum
{
    GPIO_PUPD_NONE,
    GPIO_PUPD_PU,
    GPIO_PUPD_PD
} GPIO_PUPD_Mode_t;



/* READ FUNCTIONALITY */
void GPIO_Init(GPIO_Handle_t *p_gpio_handle);
void GPIO_Cleanup(GPIO_Register_Map_t *p_gpio_x);
void GPIO_Peri_Clk_Ctrl(GPIO_Register_Map_t *p_gpio_x, uint8_t enable);
uint8_t GPIO_Read_From_Input_Pin(GPIO_Register_Map_t *p_gpio_x, uint8_t pin_num);
uint16_t GPIO_Read_From_Input_Port(GPIO_Register_Map_t *p_gpio_x);

/* WRITE FUNCTIONALITY */
void GPIO_Write_To_Output_Pin(GPIO_Register_Map_t *p_gpio_x, uint8_t pin_num, uint8_t value);
void GPIO_Write_To_Output_Port(GPIO_Register_Map_t *p_gpio_x, uint16_t value);
void GPIO_Toggle_Pin(GPIO_Register_Map_t *p_gpio_x, uint8_t pin_num);

/* INTERRUPT MANAGEMENT */
void GPIO_IRQ_Interrupt_Config(uint8_t irq_num, uint8_t enable);
void GPIO_IRQ_Priority_Config(uint8_t irq_num, uint32_t irq_prio);
void GPIO_IRQ_Handler(uint8_t pin_num);

#endif /* INC_STM32F407XX_GPIO_DRIVER_H_ */
