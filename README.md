# STM32F407xx LCD Clock
A [STM32F407G-DISC1](https://www.st.com/en/evaluation-tools/stm32f4discovery.html) development board interfaces with a [DS3231 Real Time Clock (RTC) module](https://www.analog.com/media/en/technical-documentation/data-sheets/DS3231.pdf) and a [16x2 LCD screen](https://www.sunfounder.com/products/lcd1602-module) controlled by an HD44780U controller to display the time.

<div align="center">
  <img src="https://github.com/michael-michelotti/stm-lcd-clock/blob/main/Img/stm-lcd-clock-demo.gif" alt="Demo usage of STM32 LCD clock" width="700"/>
</div>

## Table of Contents
1. [About the Project](#about-the-project)
2. [Getting Started](#getting-started)
3. [Setup](#setup)
4. [Challenges and Solutions](#challenges-and-solutions)
5. [Future Improvements](#future-improvements)

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

## Getting Started
I don't necessarily expect that anybody will be building this project in their local environment, so I won't get too into detail about the setup. I will simply list out the hardware and software I used.

### Hardware I Used
* [STM32F407G-DISC1 Microcontroller](https://www.st.com/en/evaluation-tools/stm32f4discovery.html).
<div align="left">
  <img src="https://github.com/michael-michelotti/stm-lcd-clock/blob/main/Img/stm32f4-disc1-board.jpg" alt="Demo usage of STM32 LCD clock" width="300"/>
</div>

* [DS3231 RTC Module](https://www.analog.com/media/en/technical-documentation/data-sheets/DS3231.pdf).
<div align="left">
  <img src="https://github.com/michael-michelotti/stm-lcd-clock/blob/main/Img/ds3231-rtc-module.jpg" alt="Demo usage of STM32 LCD clock" width="300"/>
</div>

* [LCD 1602 Module](https://www.sunfounder.com/products/lcd1602-module).
<div align="left">
  <img src="https://github.com/michael-michelotti/stm-lcd-clock/blob/main/Img/lcd-1602a-display.webp" alt="Demo usage of STM32 LCD clock" width="300"/>
</div>

* USB Micro-AB to USB A cable.
  * Connects the discovery board to your host PC via ST-Link.
* Breadboard
* Jumper Cables (M-M, M-F).
* Two (2) 1k resistors.
  * I2C SDA and SCL must be pulled high.
* 10k potentiometer.
  * V0 pin of the LCD screen attaches to middle leg of pot, controls screen contrast. 

### Software I Used
* [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html)

## Setup
#### Power, Code Flashing
The STM32F4 discovery board is hooked up to my host PC via USB. Over that interface, I'm able to flash the board from STM32CubeIDE using ST-Link. 

#### Power Distribution, Ground
The 5V and 3V power rails are powered by the 3V and 5V rails on the STM32F4 discovery board. All grounds are connected to the STM32F4 board grounds.

#### DS3231 Real Time Clock (RTC) module
The Vcc pin is hooked up to the 3V power rail. The SCL and SDA lines are hooked up to PB6 and PB7 of the STM32F4 board, respectively. Both lines are pulled high to the 3V power rail through a 1k resistor.

#### LCD1602A
The LCD Vdd and backlight anode are connected to the 5V power rail. The control pins are hooked up to the STM32F4 board as follows:
* RS: PA1
* R/W: ground
* E: PA2
* D4-D7: PA3-PA6

## Challenges and Solutions
### Driver Abstractions
It took me a while to wrap my head around how I wanted to abstract the application code from the driver specifics of the clock/timekeeper and the display. I eventually settled on defining a clock and display interface in `Inc/clock.h` and `Inc/display.h`, and implementing that interface in the driver code. This should allow the clock and display being used to be configurable.

I'm not sure whether the solution I ended up on is "best practice," or something that a more experienced Embedded Systems engineer would do. Perhaps there is a simpler or more optimized approach that is more typical.

### Handling Devices at Application Level versus Driver Level
I wanted to create "handle" objects to have all the information centralized that I need to communicate with the DS3231 or LCD1602. However, I didn't want this handle exposed at the application level, as it contains implementation details that are specific to those devices, which would hinder the modularity I was aiming for. 

My solution was to define "device" objects, `Clock_Device_t` and `Display_Device_t`. These objects contain the subset of information about these devices that I wanted expoed to the application code. In other words, information that applies to every clock and every display. Then, I attached those device objects to handle objects, which contained additional information that was specific to the DS3231 or LCD1602. The handle objects are dealt with at the driver level. The device objects are handled at the application level.

Once again, I'm not sure whether this approach is common, or if there is a more typical approach that more experienced engineers apply.

### Managing Device Object States
As described above, the state for the clock and display objects that is required at the application level is encapsulated in device objects. Those device objects contain a "state" which I called `ctrl_stage`. This field defines whether the clock is currently initializing, busy, errored out, etc.

Right now, the device state is not manipulated at all at the driver level, and it is left to the application code entirely to manipulate the device states while implementing API calls, callbacks, etc. I'm not sure whether this is typical, or if it would be more ideal/typical to have the driver updating the state on the application-level object (the device) automatically whenever the API calls are made.

## Future Improvements
### GPIO Interface Abstraction
Right now, my bare metal STM32F407xx I2C driver code implements an I2C interface which I defined in `Inc/i2c.h`. However, my bare metal GPIO driver implements no such interface. As such, the LCD1602A driver code is coupled quite tightly to the specifics of the STM32F407 GPIO implementation. I could implement a layer of abstraction (GPIO interface) which would make my LCD1602A driver code more portable.

### Interrupt-based Display APIs
Right now, I only implemented interrupt-based APIs and callbacks for the clock, and not the display. This is less than ideal considering that updating the display takes much longer than setting or getting from the clock, so blocking calls to the display are much more costly than to the clock. In the future, I could implement interrupt-based APIs and callbacks for updating the display.

### Alarms
The DS3231 chip offers an alarm functionality. I could implement clock and display APIs for setting and handling alarms.

### Error States
Right now, there is not much validation logic in general. I am not passing any success or failure notifications between functions, and I'm not implementing any error handling functionality. There is also no error state implemented for the display (flashing "error" on screen, etc.) 
