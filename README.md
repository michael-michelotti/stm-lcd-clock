# STM32F407xx LCD Clock
A [STM32F407G-DISC1](https://www.st.com/en/evaluation-tools/stm32f4discovery.html) development board interfaces with a [DS3231 RTC chip](https://www.analog.com/media/en/technical-documentation/data-sheets/DS3231.pdf) and a [16x2 LCD screen](https://www.sunfounder.com/products/lcd1602-module) controlled by an HD44780U controller to display the time.

## Table of Contents
1. [About the Project](#about-the-project)
2. [How To Use](#how-to-use)

## About the Project
I'm new to Embedded Systems, so I wanted to build a simple application where I build up various bare metal drivers, then apply some layers of abstraction to utilize those drivers from the application code. I decided to build a clock on an LCD screen. The timekeeping device is a DS3231, which is an RTC chip which talks over I2C. The display device is a 16x2 LCD which is controlled via GPIOs.

* The STM32F407 bare metal drivers are defined in Drivers/STM32F407xx.
  * These contain functionality to control the GPIO, I2C, and RCC peripherals are the register level.
*  Rather than having the DS3231 driver call the STM32F407 I2C driver functions directly, I wanted to separate the I2C protocol logic from the STM32F4 specifics.
  * To that end, I defined an `I2C_Interface_t` in `Inc/i2c.h`, and implemented that interface in the STM32F4 I2C driver (`Drivers/STM32F407xx/Src/stm32F407xx_i2c_driver.h`).
  * This should allow for my DS3231 driver code to be portable to different I2C peripherals, as long as those peripherals implement this interface.
* Rather than calling DS3231 driver functions directly from application code, I designed a `Clock_Device_t` and `Clock_Driver_t` in `Inc/clock.h`, which I implemented in the DS3231 driver (`Drivers/Clocks/DS3231/Src/ds3231_rtc_driver.c`).
  * I took a similar approach for the LCD1602 display (defined in `Inc/display.h`).
  * With this approach, I can swap in any timekeeping or display device, as long as they implement the driver/device interface I defined.
* The `Clock_Driver_t` interface imlpements one set of blocking getter and setter functions, and one set that is interrupt-based. The interrupt-based APIs eventually call callback functions, which are defined in `Src/clock.c`.
  * To use the interrupt-based APIs, the user should implement these callbacks in application code.
  * There is a `Clock_Ctrl_Stage_t` field in the `Clock_Device_t` object which the user can use to track the current state of the clock during callback handling.

## How To Use
### Required Hardware
* [STM32F407G-DISC1 Microcontroller](https://www.st.com/en/evaluation-tools/stm32f4discovery.html)
* [DS3231 RTC Module](https://www.analog.com/media/en/technical-documentation/data-sheets/DS3231.pdf)
* [LCD 1602 Module](https://www.sunfounder.com/products/lcd1602-module)
* USB Micro-AB to USB A cable

### Required Software
* [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html)

### Steps
