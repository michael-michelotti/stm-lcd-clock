/*
 * stm32f407xx_gpio_driver.c
 *
 *  Created on: Jan 28, 2023
 *      Author: Michael
 */


#include "stm32f407xx_gpio_driver.h"

static void GPIO_Configure_Mode(GPIO_Handle_t *p_gpio_handle)
{
	// clear mode bits for proper GPIO pin
	p_gpio_handle->p_gpio_x->MODER &= ~( 0b11 << (2 * p_gpio_handle->gpio_pin_config.gpio_pin_num) );
	// set mode bits properly
	p_gpio_handle->p_gpio_x->MODER |= ( p_gpio_handle->gpio_pin_config.gpio_pin_mode << (2 * p_gpio_handle->gpio_pin_config.gpio_pin_num) );
}

static void GPIO_EXTI_Rising_Falling_Config(uint8_t gpio_pin_num, uint8_t enable, uint8_t rise_or_fall)
{
	if (rise_or_fall == RISING)
	{
		if (enable == ENABLE)
			EXTI->RTSR |= (1 << gpio_pin_num);
		else
			EXTI->RTSR &= ~(1 << gpio_pin_num);
	}
	else if (rise_or_fall == FALLING)
	{
		if (enable == ENABLE)
			EXTI->FTSR |= (1 << gpio_pin_num);
		else
			EXTI->FTSR &= ~(1 << gpio_pin_num);
	}
}

static uint8_t GPIO_EXTI_Base_Addr_To_Code(GPIO_Register_Map_t *p_gpio_base_addr)
{
	if (p_gpio_base_addr == GPIOA)
		return 0b0000;
	else if (p_gpio_base_addr == GPIOB)
		return 0b0001;
	else if (p_gpio_base_addr == GPIOC)
		return 0b0010;
	else if (p_gpio_base_addr == GPIOD)
		return 0b0011;
	else if (p_gpio_base_addr == GPIOE)
		return 0b0100;
	else if (p_gpio_base_addr == GPIOF)
		return 0b0101;
	else if (p_gpio_base_addr == GPIOG)
		return 0b0110;
	else if (p_gpio_base_addr == GPIOH)
		return 0b0111;
	else if (p_gpio_base_addr == GPIOI)
		return 0b1000;
	else
		return -1;
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

	// depending on whether the pin number is greater than or less than 8, i have to assign the alternate
	// function using either the alternate function high or alternate function low registers
	high_or_low = p_gpio_handle->gpio_pin_config.gpio_pin_num / 8;
	shift_amount = p_gpio_handle->gpio_pin_config.gpio_pin_num % 8;
	if (high_or_low == 0)
	{
		p_gpio_handle->p_gpio_x->AFRL &= ~(0b1111 << (shift_amount * 4));
		p_gpio_handle->p_gpio_x->AFRL |= p_gpio_handle->gpio_pin_config.gpio_pin_alt_fun_mode << (shift_amount * 4);
	}
	else if (high_or_low == 1)
	{
		p_gpio_handle->p_gpio_x->AFRH &= ~(0b1111 << (shift_amount * 4));
		p_gpio_handle->p_gpio_x->AFRH |= p_gpio_handle->gpio_pin_config.gpio_pin_alt_fun_mode << (shift_amount * 4);
	}
}

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
		EXTI->IMR |= (1 << p_gpio_handle->gpio_pin_config.gpio_pin_num);
	}

	// 2. Configure speed
	p_gpio_handle->p_gpio_x->OSPEEDR |= (p_gpio_handle->gpio_pin_config.gpio_pin_speed << (2 * p_gpio_handle->gpio_pin_config.gpio_pin_num));

	// 3. Configure PU PD settings
	p_gpio_handle->p_gpio_x->PUPDR |= (p_gpio_handle->gpio_pin_config.gpio_pin_pu_pd_ctrl << (2 * p_gpio_handle->gpio_pin_config.gpio_pin_num));

	// 4. Configure output type
	p_gpio_handle->p_gpio_x->OTYPER |= (p_gpio_handle->gpio_pin_config.gpio_pin_op_type << p_gpio_handle->gpio_pin_config.gpio_pin_num);

	// 5. Configure alternate functionality
	if (p_gpio_handle->gpio_pin_config.gpio_pin_mode == GPIO_MODE_ALT)
		GPIO_Configure_Alternate_Function(p_gpio_handle);
}

void GPIO_Cleanup(GPIO_Register_Map_t *p_gpio_x)
{
	if (p_gpio_x == GPIOA)
		GPIOA_RESET();
	else if (p_gpio_x == GPIOB)
		GPIOB_RESET();
	else if (p_gpio_x == GPIOC)
		GPIOC_RESET();
	else if (p_gpio_x == GPIOD)
		GPIOD_RESET();
	else if (p_gpio_x == GPIOE)
		GPIOE_RESET();
	else if (p_gpio_x == GPIOF)
		GPIOF_RESET();
	else if (p_gpio_x == GPIOG)
		GPIOG_RESET();
	else if (p_gpio_x == GPIOH)
		GPIOH_RESET();
	else if (p_gpio_x == GPIOI)
		GPIOI_RESET();
}

void GPIO_Peri_Clk_Ctrl(GPIO_Register_Map_t *p_gpio_x, uint8_t enable)
{
	if (enable == ENABLE)
	{
		if (p_gpio_x == GPIOA)
			GPIOA_PCLK_EN();
		else if (p_gpio_x == GPIOB)
			GPIOB_PCLK_EN();
		else if (p_gpio_x == GPIOC)
			GPIOC_PCLK_EN();
		else if (p_gpio_x == GPIOD)
			GPIOD_PCLK_EN();
		else if (p_gpio_x == GPIOE)
			GPIOE_PCLK_EN();
		else if (p_gpio_x == GPIOF)
			GPIOF_PCLK_EN();
		else if (p_gpio_x == GPIOG)
			GPIOG_PCLK_EN();
		else if (p_gpio_x == GPIOH)
			GPIOH_PCLK_EN();
		else if (p_gpio_x == GPIOI)
			GPIOI_PCLK_EN();
	}
	else
	{
		if (p_gpio_x == GPIOA)
			GPIOA_PCLK_DI();
		else if (p_gpio_x == GPIOB)
			GPIOB_PCLK_DI();
		else if (p_gpio_x == GPIOC)
			GPIOC_PCLK_DI();
		else if (p_gpio_x == GPIOD)
			GPIOD_PCLK_DI();
		else if (p_gpio_x == GPIOE)
			GPIOE_PCLK_DI();
		else if (p_gpio_x == GPIOF)
			GPIOF_PCLK_DI();
		else if (p_gpio_x == GPIOG)
			GPIOG_PCLK_DI();
		else if (p_gpio_x == GPIOH)
			GPIOH_PCLK_DI();
		else if (p_gpio_x == GPIOI)
			GPIOI_PCLK_DI();
	}
}

uint8_t GPIO_Read_From_Input_Pin(volatile GPIO_Register_Map_t *p_gpio_x, uint8_t pin_num)
{
	return (uint8_t) ((p_gpio_x->IDR >> pin_num) & 0x1);
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
		p_gpio_x->ODR &= ~(0x1 << pin_num); // clear the bit
		p_gpio_x->ODR |= 0x1 << pin_num; // set the bit
	}
	else
	{
		p_gpio_x->ODR &= ~(0x1 << pin_num); // just clear the bit
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
	if (EXTI->PR & (1 << pin_num))
	{
		// clear the bit
		EXTI->PR |= (1 << pin_num);
	}
}
