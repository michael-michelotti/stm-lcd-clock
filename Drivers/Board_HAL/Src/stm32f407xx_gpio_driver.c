/*
 * stm32f407xx_gpio_driver.c
 *
 *  Created on: Jan 28, 2023
 *      Author: Michael
 */

#include "stm32f407xx_gpio_driver.h"

/*************** PRIVATE UTILITY FUNCTION DECLARATIONS*****************/
static void GPIO_Configure_Mode(GPIO_Handle_t *p_gpio_handle);
static void GPIO_EXTI_Rising_Falling_Config(uint8_t gpio_pin_num, uint8_t enable, uint8_t rise_or_fall);
static uint8_t GPIO_EXTI_Base_Addr_To_Code(GPIO_Register_Map_t *p_gpio_base_addr);
static void GPIO_EXTI_Config(GPIO_Handle_t *p_gpio_handle);
static void GPIO_Configure_Alternate_Function(GPIO_Handle_t *p_gpio_handle);

/* READ FUNCTIONALITY */
void GPIO_Init(GPIO_Handle_t *p_gpio_handle)
{
	// enable the clock for your given register
	GPIO_Peri_Clk_Ctrl(p_gpio_handle->p_gpio_x, ENABLE);

	// 1. Configure pin mode
	if (p_gpio_handle->gpio_pin_config.gpio_pin_mode <= GPIO_MODE_ANALOG)
	{
		GPIO_Configure_Mode(p_gpio_handle);
	}
	else	// pin must be in interrupt mode
	{
		if (p_gpio_handle->gpio_pin_config.gpio_pin_mode == GPIO_MODE_IN_FE)
		{
			// enable falling, disable rising trigger
			GPIO_EXTI_Rising_Falling_Config(p_gpio_handle->gpio_pin_config.gpio_pin_num, ENABLE, FALLING);
			GPIO_EXTI_Rising_Falling_Config(p_gpio_handle->gpio_pin_config.gpio_pin_num, DISABLE, RISING);
		}
		else if (p_gpio_handle->gpio_pin_config.gpio_pin_mode == GPIO_MODE_IN_RE)
		{
			// enable rising, disable falling trigger
			GPIO_EXTI_Rising_Falling_Config(p_gpio_handle->gpio_pin_config.gpio_pin_num, DISABLE, FALLING);
			GPIO_EXTI_Rising_Falling_Config(p_gpio_handle->gpio_pin_config.gpio_pin_num, ENABLE, RISING);
		}
		else if (p_gpio_handle->gpio_pin_config.gpio_pin_mode == GPIO_MODE_IN_RFT)
		{
			GPIO_EXTI_Rising_Falling_Config(p_gpio_handle->gpio_pin_config.gpio_pin_num, ENABLE, FALLING);
			GPIO_EXTI_Rising_Falling_Config(p_gpio_handle->gpio_pin_config.gpio_pin_num, ENABLE, RISING);
		}

		GPIO_EXTI_Config(p_gpio_handle);
		// unmask interrupts for desired input line
		SET_BIT(&EXTI->IMR, (1 << p_gpio_handle->gpio_pin_config.gpio_pin_num));
	}

	uint32_t gpio_register_mask = 0b11 << (2 * p_gpio_handle->gpio_pin_config.gpio_pin_num);

	// 2. Configure speed
	SET_FIELD(&p_gpio_handle->p_gpio_x->OSPEEDR, gpio_register_mask, p_gpio_handle->gpio_pin_config.gpio_pin_speed);

	// 3. Configure PU PD settings
	SET_FIELD(&p_gpio_handle->p_gpio_x->PUPDR, gpio_register_mask, p_gpio_handle->gpio_pin_config.gpio_pin_pu_pd_ctrl);

	gpio_register_mask = 0b1 << (p_gpio_handle->gpio_pin_config.gpio_pin_num);
	// 4. Configure output type
	SET_FIELD(&p_gpio_handle->p_gpio_x->OTYPER, gpio_register_mask, p_gpio_handle->gpio_pin_config.gpio_pin_op_type);

	// 5. Configure alternate functionality
	if (p_gpio_handle->gpio_pin_config.gpio_pin_mode == GPIO_MODE_ALT)
		GPIO_Configure_Alternate_Function(p_gpio_handle);
}

void GPIO_Cleanup(GPIO_Register_Map_t *p_gpio_x)
{
	switch ((uint32_t)p_gpio_x)
	{
	case GPIOA_BASE_ADDR:
		GPIOA_RESET();
		break;
	case GPIOB_BASE_ADDR:
		GPIOB_RESET();
	case GPIOC_BASE_ADDR:
		GPIOC_RESET();
	case GPIOD_BASE_ADDR:
		GPIOD_RESET();
	case GPIOE_BASE_ADDR:
		GPIOE_RESET();
	case GPIOF_BASE_ADDR:
		GPIOF_RESET();
	case GPIOG_BASE_ADDR:
		GPIOG_RESET();
	case GPIOH_BASE_ADDR:
		GPIOH_RESET();
	case GPIOI_BASE_ADDR:
		GPIOI_RESET();
	}
}

void GPIO_Peri_Clk_Ctrl(GPIO_Register_Map_t *p_gpio_x, uint8_t enable)
{
	if (enable == ENABLE)
	{
		switch ((uint32_t)p_gpio_x)
		{
		case GPIOA_BASE_ADDR:
			GPIOA_PCLK_EN();
			break;
		case GPIOB_BASE_ADDR:
			GPIOB_PCLK_EN();
		case GPIOC_BASE_ADDR:
			GPIOC_PCLK_EN();
		case GPIOD_BASE_ADDR:
			GPIOD_PCLK_EN();
		case GPIOE_BASE_ADDR:
			GPIOE_PCLK_EN();
		case GPIOF_BASE_ADDR:
			GPIOF_PCLK_EN();
		case GPIOG_BASE_ADDR:
			GPIOG_PCLK_EN();
		case GPIOH_BASE_ADDR:
			GPIOH_PCLK_EN();
		case GPIOI_BASE_ADDR:
			GPIOI_PCLK_EN();
		}
	}
	else
	{
		switch ((uint32_t)p_gpio_x)
		{
		case GPIOA_BASE_ADDR:
			GPIOA_PCLK_DI();
			break;
		case GPIOB_BASE_ADDR:
			GPIOB_PCLK_DI();
		case GPIOC_BASE_ADDR:
			GPIOC_PCLK_DI();
		case GPIOD_BASE_ADDR:
			GPIOD_PCLK_DI();
		case GPIOE_BASE_ADDR:
			GPIOE_PCLK_DI();
		case GPIOF_BASE_ADDR:
			GPIOF_PCLK_DI();
		case GPIOG_BASE_ADDR:
			GPIOG_PCLK_DI();
		case GPIOH_BASE_ADDR:
			GPIOH_PCLK_DI();
		case GPIOI_BASE_ADDR:
			GPIOI_PCLK_DI();
		}
	}
}

uint8_t GPIO_Read_From_Input_Pin(GPIO_Register_Map_t *p_gpio_x, uint8_t pin_num)
{
	return GET_BIT(&p_gpio_x->IDR, (pin_num << 0x1));
}

uint16_t GPIO_Read_From_Input_Port(GPIO_Register_Map_t *p_gpio_x)
{
	return (uint16_t) (p_gpio_x->IDR);
}

/* WRITE FUNCTIONALITY */
void GPIO_Write_To_Output_Pin(GPIO_Register_Map_t *p_gpio_x, uint8_t pin_num, uint8_t value)
{
	if (value == SET)
	{
		SET_FIELD(&p_gpio_x->ODR, (0x1 << pin_num), 0x1);
	}
	else
	{
		CLEAR_BIT(&p_gpio_x->ODR, (0x1 << pin_num)); // just clear the bit
	}
}

void GPIO_Write_To_Output_Port(GPIO_Register_Map_t *p_gpio_x, uint16_t value)
{
	p_gpio_x->ODR = value;
}

void GPIO_Toggle_Pin(GPIO_Register_Map_t *p_gpio_x, uint8_t pin_num)
{
	p_gpio_x->ODR ^= (1 << pin_num);
}

/* INTERRUPT MANAGEMENT */
void GPIO_IRQ_Interrupt_Config(uint8_t irq_num, uint8_t enable)
{
	uint8_t adjusted_irq_num = irq_num % 32;

	// INTERRUPT BEING ENABLED
	if (enable == ENABLE)
	{
		if (irq_num <= 31)
			*NVIC_ISER_0 |= (1 << adjusted_irq_num);
		else if (irq_num > 31 && irq_num < 64)
			*NVIC_ISER_1 |= (1 << adjusted_irq_num);
		else if (irq_num >= 64 && irq_num < 96)
			*NVIC_ISER_2 |= (1 << adjusted_irq_num);
	}
	// INTERRUPT BEING DISABLED
	else
	{
		if (irq_num <= 31)
			*NVIC_ICER_0 |= (1 << adjusted_irq_num);
		else if (irq_num > 31 && irq_num < 64)
			*NVIC_ICER_1 |= (1 << adjusted_irq_num);
		else if (irq_num >= 64 && irq_num < 96)
			*NVIC_ICER_2 |= (1 << adjusted_irq_num);
	}
}

void GPIO_IRQ_Priority_Config(uint8_t irq_num, uint32_t irq_prio)
{
	uint8_t ipr_num = irq_num / 4;
	uint8_t ipr_offset = irq_num % 4;

	// clear the current priority
	*(NVIC_IPR_0 + ipr_num) &= ~(0b1111 << ((ipr_offset * 8) + (8-NO_PRIORITY_BITS_IMPLEMENTED)));
	// set the new priority
	*(NVIC_IPR_0 + ipr_num) |= (irq_prio << ((ipr_offset * 8) + (8-NO_PRIORITY_BITS_IMPLEMENTED)));

}

void GPIO_IRQ_Handler(uint8_t pin_num)
{
	// clear the exti pending register corresponding to pin number
	if (GET_BIT(&EXTI->PR, (1 << pin_num)))
	{
		// clear the bit
		CLEAR_BIT(&EXTI->PR, (1 << pin_num));
	}
}

/*************** PRIVATE UTILITY FUNCTIONS *****************/
static void GPIO_Configure_Mode(GPIO_Handle_t *p_gpio_handle)
{
	// clear mode bits for proper GPIO pin
	uint32_t moder_mask = 0b11 << (2 * p_gpio_handle->gpio_pin_config.gpio_pin_num);
	SET_FIELD(&p_gpio_handle->p_gpio_x->MODER, moder_mask, p_gpio_handle->gpio_pin_config.gpio_pin_mode);
}

static void GPIO_EXTI_Rising_Falling_Config(uint8_t gpio_pin_num, uint8_t enable, uint8_t rise_or_fall)
{
	if (rise_or_fall == RISING)
	{
		if (enable == ENABLE)
			SET_BIT(&EXTI->RTSR, (1 << gpio_pin_num));
		else
			CLEAR_BIT(&EXTI->RTSR, (1 << gpio_pin_num));
	}
	else if (rise_or_fall == FALLING)
	{
		if (enable == ENABLE)
			SET_BIT(&EXTI->FTSR, (1 << gpio_pin_num));
		else
			CLEAR_BIT(&EXTI->FTSR, (1 << gpio_pin_num));
	}
}

static uint8_t GPIO_EXTI_Base_Addr_To_Code(GPIO_Register_Map_t *p_gpio_base_addr)
{
	switch ((uint32_t)p_gpio_base_addr)
	{
	case GPIOA_BASE_ADDR:
		return 0b0000;
	case GPIOB_BASE_ADDR:
		return 0b0001;
	case GPIOC_BASE_ADDR:
		return 0b0010;
	case GPIOD_BASE_ADDR:
		return 0b0011;
	case GPIOE_BASE_ADDR:
		return 0b0100;
	case GPIOF_BASE_ADDR:
		return 0b0101;
	case GPIOG_BASE_ADDR:
		return 0b0110;
	case GPIOH_BASE_ADDR:
		return 0b0111;
	case GPIOI_BASE_ADDR:
		return 0b1000;
	default:
		return -1;
	}
}

static void GPIO_EXTI_Config(GPIO_Handle_t *p_gpio_handle)
{
	// give control of exti line to proper gpio a..i
	uint8_t exti_reg_num = p_gpio_handle->gpio_pin_config.gpio_pin_num / 4;
	uint8_t exti_bit_offset = (p_gpio_handle->gpio_pin_config.gpio_pin_num % 4) * 4;
	uint8_t port_code = GPIO_EXTI_Base_Addr_To_Code(p_gpio_handle->p_gpio_x);

	SYSCFG_PCLK_EN();
	SYSCFG->EXTICR[exti_reg_num] |= (port_code << exti_bit_offset);
}

static void GPIO_Configure_Alternate_Function(GPIO_Handle_t *p_gpio_handle)
{
	uint8_t high_or_low = 0;
	uint8_t shift_amount = 0;
	uint32_t afr_field_mask;

	// depending on whether the pin number is greater than or less than 8, i have to assign the alternate
	// function using either the alternate function high or alternate function low registers
	high_or_low = p_gpio_handle->gpio_pin_config.gpio_pin_num / 8;
	shift_amount = p_gpio_handle->gpio_pin_config.gpio_pin_num % 8;
	afr_field_mask = 0b1111 << (shift_amount * 4);
	if (high_or_low == 0)
	{
		SET_FIELD(&p_gpio_handle->p_gpio_x->AFRL, afr_field_mask, p_gpio_handle->gpio_pin_config.gpio_pin_alt_fun_mode);
	}
	else if (high_or_low == 1)
	{
		SET_FIELD(&p_gpio_handle->p_gpio_x->AFRH, afr_field_mask, p_gpio_handle->gpio_pin_config.gpio_pin_alt_fun_mode);
	}
}
