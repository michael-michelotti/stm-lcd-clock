#include <string.h>

#include "lcd1602a_display_driver.h"


static void LCD1602A_Initialize(Display_Device_t *lcd1602a_dev);
static void LCD1602A_On(void);
static void LCD1602A_Off(void);
static void LCD1602A_Clear(void);
static void LCD1602A_Update_Seconds(seconds_t seconds);
static void LCD1602A_Update_Buffer_Seconds(seconds_t seconds);
static void LCD1602A_Update_Minutes(minutes_t minutes);
static void LCD1602A_Update_Buffer_Minutes(minutes_t minutes);
static void LCD1602A_Update_Hours(hours_t hours);
static void LCD1602A_Update_Buffer_Hours(hours_t hours);
static void LCD1602A_Update_Time(full_time_t full_time);
static void LCD1602A_Update_Buffer_Time(full_time_t full_time);
static void LCD1602A_Update_Date(date_t date);
static void LCD1602A_Update_Buffer_Date(date_t date);
static void LCD1602A_Update_Day_Of_Week(day_of_week_t dow);
static void LCD1602A_Update_Buffer_Day_Of_Week(day_of_week_t dow);
static void LCD1602A_Update_Month(month_t month);
static void LCD1602A_Update_Buffer_Month(month_t month);
static void LCD1602A_Update_Year(year_t year, century_t century);
static void LCD1602A_Update_Buffer_Year(year_t year, century_t century);
static void LCD1602A_Update_Full_Date(full_date_t full_date);
static void LCD1602A_Update_Buffer_Full_Date(full_date_t full_date);
static void LCD1602A_Update_Datetime(full_datetime_t datetime);
static void LCD1602A_Set_Cursor(uint8_t row, uint8_t column);
static void LCD1602A_Display_Char(char ch);
static void LCD1602A_Display_Str(char *str, size_t num_chars);

static char int_to_ascii_char(uint8_t int_to_covert);
static void int_to_zero_padded_ascii(char *result, uint8_t int_to_convert);
static void write_char(char ch);
static void write_command(uint8_t cmd_word, uint32_t address_setup_us);
static void send_nybble(uint8_t nybble);
static void clear_display();
static void return_home();
static void entry_mode_set(uint8_t inc_dec, uint8_t shift);
static void display_on_off(uint8_t disp, uint8_t cursor, uint8_t blink);
static void cursor_display_shift(uint8_t shift_or_cursor, uint8_t right_left);
static void function_set(uint8_t bit_len, uint8_t num_lines, uint8_t font);
static void set_cgram_addr(uint8_t cgram_addr);
static void set_ddram_addr(uint8_t ddram_addr);
static void pulse_enable(uint32_t us_hold_time);
static void mdelay(uint32_t cnt);
static void udelay(uint32_t cnt);

static LCD1602A_Handle_t lcd1602a_handle;
static const char RESET_TIME_STR[] = "HH:MM:SS AM";
static const char RESET_DATE_STR[] = "DOW MM/DD/YYYY";

/* Implements the display driver interface defined in Inc/display.h for a HD44780U-controlled 16x2 LCD*/
static Display_Driver_t lcd1602_display_driver = {
        .Display_Initialize             = LCD1602A_Initialize,
        .Display_On                     = LCD1602A_On,
        .Display_Off                    = LCD1602A_Off,
        .Display_Clear                  = LCD1602A_Clear,
        .Display_Update_Seconds         = LCD1602A_Update_Seconds,
        .Display_Update_Minutes         = LCD1602A_Update_Minutes,
        .Display_Update_Hours           = LCD1602A_Update_Hours,
        .Display_Update_Time            = LCD1602A_Update_Time,
        .Display_Update_Date            = LCD1602A_Update_Date,
        .Display_Update_Day_Of_Week     = LCD1602A_Update_Day_Of_Week,
        .Display_Update_Month           = LCD1602A_Update_Month,
        .Display_Update_Year            = LCD1602A_Update_Year,
        .Display_Update_Full_Date       = LCD1602A_Update_Full_Date,
        .Display_Update_Datetime        = LCD1602A_Update_Datetime,
};

Display_Driver_t *get_display_driver()
{
    return &lcd1602_display_driver;
}

static void LCD1602A_Initialize(Display_Device_t *lcd1602a_dev)
{
    // Register select pin
    GPIO_Pin_Config_t pin_conf = { RS_GPIO_PIN, GPIO_MODE_OUT, GPIO_SPEED_HIGH, GPIO_PUPD_NONE, GPIO_OUT_PP, 0 };
    lcd1602a_handle.rs_gpio_handle.p_gpio_x = RS_GPIO_PORT;
    lcd1602a_handle.rs_gpio_handle.gpio_pin_config = pin_conf;
    GPIO_Init(&lcd1602a_handle.rs_gpio_handle);

    // Read write pin
    pin_conf.gpio_pin_num = RW_GPIO_PIN;
    lcd1602a_handle.rw_gpio_handle.p_gpio_x = RW_GPIO_PORT;
    lcd1602a_handle.rw_gpio_handle.gpio_pin_config = pin_conf;
    GPIO_Init(&lcd1602a_handle.rw_gpio_handle);

    // Enable pin
    pin_conf.gpio_pin_num = E_GPIO_PIN;
    lcd1602a_handle.e_gpio_handle.p_gpio_x = E_GPIO_PORT;
    lcd1602a_handle.e_gpio_handle.gpio_pin_config = pin_conf;
    GPIO_Init(&lcd1602a_handle.e_gpio_handle);

    // Data lines - DB4
    pin_conf.gpio_pin_num = DB4_GPIO_PIN;
    lcd1602a_handle.db4_gpio_handle.p_gpio_x = DB4_GPIO_PORT;
    lcd1602a_handle.db4_gpio_handle.gpio_pin_config = pin_conf;
    GPIO_Init(&lcd1602a_handle.db4_gpio_handle);

    // DB5
    pin_conf.gpio_pin_num = DB5_GPIO_PIN;
    lcd1602a_handle.db5_gpio_handle.p_gpio_x = DB5_GPIO_PORT;
    lcd1602a_handle.db5_gpio_handle.gpio_pin_config = pin_conf;
    GPIO_Init(&lcd1602a_handle.db5_gpio_handle);

    // DB6
    pin_conf.gpio_pin_num = DB6_GPIO_PIN;
    lcd1602a_handle.db6_gpio_handle.p_gpio_x = DB6_GPIO_PORT;
    lcd1602a_handle.db6_gpio_handle.gpio_pin_config = pin_conf;
    GPIO_Init(&lcd1602a_handle.db6_gpio_handle);

    // DB7
    pin_conf.gpio_pin_num = DB7_GPIO_PIN;
    lcd1602a_handle.db7_gpio_handle.p_gpio_x = DB7_GPIO_PORT;
    lcd1602a_handle.db7_gpio_handle.gpio_pin_config = pin_conf;
    GPIO_Init(&lcd1602a_handle.db7_gpio_handle);

    lcd1602a_handle.cursor_col_pos = 0;
    lcd1602a_handle.cursor_row_pos = 0;
    lcd1602a_handle.display_dev = lcd1602a_dev;

    strncpy(lcd1602a_handle.time_str_buffer,
            RESET_TIME_STR,
            sizeof(lcd1602a_handle.time_str_buffer) - 1);
    strncpy(lcd1602a_handle.date_str_buffer,
            RESET_DATE_STR,
            sizeof(lcd1602a_handle.date_str_buffer) - 1);

    // set all pins to ground (clear entire GPIOD ODR port)
    GPIO_Write_To_Output_Port(LCD_GPIO_PORT, 0);
    mdelay(40);

    // as part of initialization, 0x3 must be sent twice, then 0x2 to initiate 4-bit mode
    send_nybble(0x3);
    mdelay(5);
    send_nybble(0x3);
    udelay(150);
    send_nybble(0x3);
    send_nybble(0x2);

    // now we need to start sending 8-bit words in 2 nybbles separately (4-bit mode)
    // 0x28 = 0010 1000, sets line number to 2 and style to 5x8 dot characters
    function_set(LCD_4_BIT, LCD_2_LINES, LCD_5_8_DOTS);
    display_on_off(LCD_DISP_ON, LCD_CURSOR_OFF, LCD_BLINK_OFF);
    clear_display();
    entry_mode_set(LCD_INCREMENT, LCD_NO_SHIFT);
    LCD1602A_Set_Cursor(LCD1602A_TIME_ROW, LCD1602A_TIME_COL);
    LCD1602A_Display_Str(lcd1602a_handle.time_str_buffer,
                         sizeof(lcd1602a_handle.time_str_buffer) - 1);
    LCD1602A_Set_Cursor(LCD1602A_FULL_DATE_ROW, LCD1602A_FULL_DATE_COL);
    LCD1602A_Display_Str(lcd1602a_handle.date_str_buffer,
                         sizeof(lcd1602a_handle.date_str_buffer) - 1);
}

static void LCD1602A_On(void)
{
    display_on_off(LCD_DISP_ON, LCD_CURSOR_OFF, LCD_BLINK_OFF);
}

static void LCD1602A_Off(void)
{
    display_on_off(LCD_DISP_OFF, LCD_CURSOR_OFF, LCD_BLINK_OFF);
}

/* Resets the date and time to the default strings defined in this file */
static void LCD1602A_Clear(void)
{
    strncpy(lcd1602a_handle.time_str_buffer,
            RESET_TIME_STR,
            sizeof(RESET_TIME_STR) - 1);
    LCD1602A_Set_Cursor(LCD1602A_TIME_ROW, LCD1602A_TIME_COL);
    LCD1602A_Display_Str(lcd1602a_handle.time_str_buffer,
                         sizeof(lcd1602a_handle.time_str_buffer) - 1);

    strncpy(lcd1602a_handle.date_str_buffer,
           RESET_DATE_STR,
           sizeof(RESET_DATE_STR) - 1);
    LCD1602A_Set_Cursor(LCD1602A_FULL_DATE_ROW, LCD1602A_FULL_DATE_COL);
    LCD1602A_Display_Str(lcd1602a_handle.date_str_buffer,
                         sizeof(lcd1602a_handle.date_str_buffer) - 1);
}

static void LCD1602A_Update_Seconds(seconds_t seconds)
{
    LCD1602A_Update_Buffer_Seconds(seconds);
    LCD1602A_Set_Cursor(LCD1602A_SECS_ROW, LCD1602A_SECS_COL);
    LCD1602A_Display_Str(lcd1602a_handle.time_str_buffer + LCD1602A_SECS_OFFSET, 2);
}

static void LCD1602A_Update_Buffer_Seconds(seconds_t seconds)
{
    char seconds_str[3] = {0};
    int_to_zero_padded_ascii(seconds_str, (uint8_t)seconds);
    strncpy(lcd1602a_handle.time_str_buffer + LCD1602A_SECS_OFFSET,
            seconds_str,
            2);
}

static void LCD1602A_Update_Minutes(minutes_t minutes)
{
    LCD1602A_Update_Buffer_Minutes(minutes);
    LCD1602A_Set_Cursor(LCD1602A_MINS_ROW, LCD1602A_MINS_COL);
    LCD1602A_Display_Str(lcd1602a_handle.time_str_buffer + LCD1602A_MINS_OFFSET, 2);
}

static void LCD1602A_Update_Buffer_Minutes(minutes_t minutes)
{
    char minutes_str[3] = {0};
    int_to_zero_padded_ascii(minutes_str, (uint8_t)minutes);
    strncpy(lcd1602a_handle.time_str_buffer + LCD1602A_MINS_OFFSET,
            minutes_str,
            2);
}

static void LCD1602A_Update_Hours(hours_t hours)
{
    LCD1602A_Update_Buffer_Hours(hours);
    LCD1602A_Set_Cursor(LCD1602A_HRS_ROW, LCD1602A_HRS_COL);
    LCD1602A_Display_Str(lcd1602a_handle.time_str_buffer + LCD1602A_HRS_OFFSET, 2);
    LCD1602A_Set_Cursor(LCD1602A_HR_FMT_ROW, LCD1602A_HR_FMT_COL);
    LCD1602A_Display_Str(lcd1602a_handle.time_str_buffer + LCD1602A_HR_FMT_OFFSET, 2);
}

static void LCD1602A_Update_Buffer_Hours(hours_t hours)
{
    char hours_str[3] = {0};
    char hour_format[3] = "  ";

    int_to_zero_padded_ascii(hours_str, (uint8_t)hours.hour);
    strncpy(lcd1602a_handle.time_str_buffer + LCD1602A_HRS_OFFSET,
            hours_str,
            2);

    if (hours.hour_format == HOUR_FORMAT_12_HOUR)
    {
        if (hours.am_pm == AM_PM_AM)
        {
            strncpy(hour_format, "AM", sizeof(hour_format));
        }
        else
        {
            strncpy(hour_format, "PM", sizeof(hour_format));
        }
    }

    strncpy(lcd1602a_handle.time_str_buffer + LCD1602A_HR_FMT_OFFSET,
            hour_format,
            2);
}

static void LCD1602A_Update_Time(full_time_t full_time)
{
    LCD1602A_Update_Buffer_Time(full_time);
    LCD1602A_Update_Hours(full_time.hours);
    LCD1602A_Update_Minutes(full_time.minutes);
    LCD1602A_Update_Seconds(full_time.seconds);
}

static void LCD1602A_Update_Buffer_Time(full_time_t full_time)
{
    LCD1602A_Update_Buffer_Hours(full_time.hours);
    LCD1602A_Update_Buffer_Minutes(full_time.minutes);
    LCD1602A_Update_Buffer_Seconds(full_time.seconds);
}

static void LCD1602A_Update_Date(date_t date)
{
    LCD1602A_Update_Buffer_Date(date);
    LCD1602A_Set_Cursor(LCD1602A_DATE_ROW, LCD1602A_DATE_COL);
    LCD1602A_Display_Str(lcd1602a_handle.date_str_buffer + LCD1602A_DATE_OFFSET, 2);
}

static void LCD1602A_Update_Buffer_Date(date_t date)
{
    char date_str[3] = {0};
    int_to_zero_padded_ascii(date_str, (uint8_t)date);
    strncpy(lcd1602a_handle.date_str_buffer + LCD1602A_DATE_OFFSET,
            date_str,
            2);
}

static void LCD1602A_Update_Day_Of_Week(day_of_week_t dow)
{
    LCD1602A_Update_Buffer_Day_Of_Week(dow);
    LCD1602A_Set_Cursor(LCD1602A_DOW_ROW, LCD1602A_DOW_COL);
    LCD1602A_Display_Str(lcd1602a_handle.date_str_buffer + LCD1602A_DOW_OFFSET, 3);
}

static void LCD1602A_Update_Buffer_Day_Of_Week(day_of_week_t dow)
{
    char dow_str[4] = {0};
    switch (dow)
    {
    case DAY_OF_WEEK_SUN:
        strncpy(dow_str, "Sun", 3);
        break;
    case DAY_OF_WEEK_MON:
        strncpy(dow_str, "Mon", 3);
        break;
    case DAY_OF_WEEK_TUE:
        strncpy(dow_str, "Tue", 3);
        break;
    case DAY_OF_WEEK_WED:
        strncpy(dow_str, "Wed", 3);
        break;
    case DAY_OF_WEEK_THU:
        strncpy(dow_str, "Thu", 3);
        break;
    case DAY_OF_WEEK_FRI:
        strncpy(dow_str, "Fri", 3);
        break;
    case DAY_OF_WEEK_SAT:
        strncpy(dow_str, "Sat", 3);
        break;
    }
    strncpy(lcd1602a_handle.date_str_buffer + LCD1602A_DOW_OFFSET,
            dow_str,
            3);
}


static void LCD1602A_Update_Month(month_t month)
{
    LCD1602A_Update_Buffer_Month(month);
    LCD1602A_Set_Cursor(LCD1602A_MONTH_ROW, LCD1602A_MONTH_COL);
    LCD1602A_Display_Str(lcd1602a_handle.date_str_buffer + LCD1602A_MONTH_OFFSET, 2);
}

static void LCD1602A_Update_Buffer_Month(month_t month)
{
    char month_str[3] = {0};
    int_to_zero_padded_ascii(month_str, (uint8_t) month);
    strncpy(lcd1602a_handle.date_str_buffer + LCD1602A_MONTH_OFFSET,
            month_str,
            2);
}

static void LCD1602A_Update_Year(year_t year, century_t century)
{
    LCD1602A_Update_Buffer_Year(year, century);
    LCD1602A_Set_Cursor(LCD1602A_YEAR_ROW, LCD1602A_YEAR_COL);
    LCD1602A_Display_Str(lcd1602a_handle.date_str_buffer + LCD1602A_YEAR_OFFSET, 4);
}

static void LCD1602A_Update_Buffer_Year(year_t year, century_t century)
{
    char year_str[5] = {0};
    int_to_zero_padded_ascii(year_str + 2, (uint8_t)year);

    if (century == CENTURY_20TH)
    {
        strncpy(year_str, "19", 2);
    }
    else
    {
        strncpy(year_str, "20", 2);
    }

    strncpy(lcd1602a_handle.date_str_buffer + LCD1602A_YEAR_OFFSET,
            year_str,
            4);
}

static void LCD1602A_Update_Full_Date(full_date_t full_date)
{
    LCD1602A_Update_Buffer_Full_Date(full_date);
    LCD1602A_Set_Cursor(LCD1602A_FULL_DATE_ROW, LCD1602A_FULL_DATE_COL);
    LCD1602A_Display_Str(lcd1602a_handle.date_str_buffer,
                         sizeof(lcd1602a_handle.date_str_buffer) - 1);
}

static void LCD1602A_Update_Buffer_Full_Date(full_date_t full_date)
{
    LCD1602A_Update_Buffer_Date(full_date.date);
    LCD1602A_Update_Buffer_Day_Of_Week(full_date.day_of_week);
    LCD1602A_Update_Buffer_Month(full_date.month);
    LCD1602A_Update_Buffer_Year(full_date.year, full_date.century);
}

static void LCD1602A_Update_Datetime(full_datetime_t datetime)
{
    LCD1602A_Update_Time(datetime.time);
    LCD1602A_Update_Full_Date(datetime.date);
}

static void LCD1602A_Set_Cursor(uint8_t row, uint8_t column)
{
    uint8_t ddram_addr = 0;
    if (row == 1)
    {
        /* 6th bit in DDRAM defines first row vs. second row address */
        ddram_addr |= (1 << 6);
    }

    ddram_addr |= (column & 0xF);
    set_ddram_addr(ddram_addr);
}

static void LCD1602A_Display_Char(char ch)
{
    write_char(ch);
}

static void LCD1602A_Display_Str(char *str, size_t num_chars)
{
    char curr;
    for (int i = 0; i < num_chars; i++)
    {
        curr = *str;
        LCD1602A_Display_Char(curr);
        str++;
    }
}

/* Utility Functions*/
static char int_to_ascii_char(uint8_t int_to_covert)
{
    return int_to_covert + ASCII_DIGIT_OFFSET;
}

static void int_to_zero_padded_ascii(char *result, uint8_t int_to_convert)
{
    if (int_to_convert < 10)
    {
        result[0] = '0';
        result[1] = int_to_ascii_char(int_to_convert);
    }
    else
    {
        result[0] = int_to_ascii_char(int_to_convert / 10);
        result[1] = int_to_ascii_char(int_to_convert % 10);
    }
}

static void write_char(char ch)
{
    uint8_t low_nybble = (ch & 0xF);
    uint8_t high_nybble = (ch >> 4);

    /* Since I'm writing data, not a command, set RS high */
    GPIO_Write_To_Output_Pin(lcd1602a_handle.rs_gpio_handle.p_gpio_x,
                             lcd1602a_handle.rs_gpio_handle.gpio_pin_config.gpio_pin_num,
                             HIGH);
    udelay(LCD_TAS_US);

    send_nybble(high_nybble);
    send_nybble(low_nybble);
}

static void write_command(uint8_t cmd_word, uint32_t address_setup_us)
{
    uint8_t low_nybble = (cmd_word & 0xF);
    uint8_t high_nybble = (cmd_word >> 4);

    /* Since I'm sending a command, set RS low; wait for address setup time, 60ns minimum */
    GPIO_Write_To_Output_Pin(lcd1602a_handle.rs_gpio_handle.p_gpio_x,
                             lcd1602a_handle.rs_gpio_handle.gpio_pin_config.gpio_pin_num,
                             LOW);
    udelay(address_setup_us);

    send_nybble(high_nybble);
    send_nybble(low_nybble);
}

static void send_nybble(uint8_t nybble)
{
    /* Configure DB4-7 GPIO pins with the nybble value */
    GPIO_Write_To_Output_Pin(lcd1602a_handle.db4_gpio_handle.p_gpio_x,
                             lcd1602a_handle.db4_gpio_handle.gpio_pin_config.gpio_pin_num,
                             ((nybble >> 0) & 1));
    GPIO_Write_To_Output_Pin(lcd1602a_handle.db5_gpio_handle.p_gpio_x,
                             lcd1602a_handle.db5_gpio_handle.gpio_pin_config.gpio_pin_num,
                             ((nybble >> 1) & 1));
    GPIO_Write_To_Output_Pin(lcd1602a_handle.db6_gpio_handle.p_gpio_x,
                             lcd1602a_handle.db6_gpio_handle.gpio_pin_config.gpio_pin_num,
                             ((nybble >> 2) & 1));
    GPIO_Write_To_Output_Pin(lcd1602a_handle.db7_gpio_handle.p_gpio_x,
                             lcd1602a_handle.db7_gpio_handle.gpio_pin_config.gpio_pin_num,
                             ((nybble >> 3) & 1));

    /* To send nybble to the LCD: pulse enable, then delay 1us for (enable pulse width = 450ns min) */
    pulse_enable(ENALBE_PULSE_US);
    /* wait for LCD to read data. Data must be held valid for 10ns, enable cannot pulse high again for 500ns */
    udelay(LCD_HOLD_TIME_US);
}

static void clear_display()
{
    write_command(CLEAR_DISPLAY, LCD_TAS_US);
    /* 2ms delay - clear display takes ~1.5ms for LCD to internally process */
    mdelay(2);
}

static void return_home()
{
    write_command(RETURN_HOME, LCD_TAS_US);
    /* 2ms delay - clear display takes ~1.5ms for LCD to internally process */
    mdelay(2);
}

static void entry_mode_set(uint8_t inc_dec, uint8_t shift)
{
    uint8_t cmd_byte = ENTRY_MODE_SET;
    cmd_byte |= (inc_dec << 1) + (shift << 0);
    write_command(cmd_byte, LCD_TAS_US);
}

static void display_on_off(uint8_t disp, uint8_t cursor, uint8_t blink)
{
    uint8_t cmd_byte = DISPLAY_ON_OFF_CTRL;
    cmd_byte |= (disp << 2) + (cursor << 1) + (blink << 0);
    write_command(cmd_byte, LCD_TAS_US);
}

static void cursor_display_shift(uint8_t shift_or_cursor, uint8_t right_left)
{
    uint8_t cmd_byte = CURSOR_DISPLAY_SHIFT;
    cmd_byte |= (shift_or_cursor << 3) + (right_left << 2);
    write_command(cmd_byte, LCD_TAS_US);
}

static void function_set(uint8_t bit_len, uint8_t num_lines, uint8_t font)
{
    uint8_t cmd_byte = FUNCTION_SET;
    cmd_byte |= (bit_len << 4) + (num_lines << 3) + (font << 2);
    write_command(cmd_byte, LCD_TAS_US);
}

static void set_cgram_addr(uint8_t cgram_addr)
{
    uint8_t cmd_byte = SET_CGRAM_ADDR;
    cmd_byte |= (cgram_addr & 0x3F);
    write_command(cmd_byte, LCD_TAS_US);
}

static void set_ddram_addr(uint8_t ddram_addr)
{
    uint8_t cmd_byte = SET_DDRAM_ADDR;
    cmd_byte |= (ddram_addr & 0x7F);
    write_command(cmd_byte, LCD_TAS_US);
}

static void pulse_enable(uint32_t us_hold_time)
{
    // pulse E again for 1us,
    GPIO_Write_To_Output_Pin(lcd1602a_handle.e_gpio_handle.p_gpio_x,
                             lcd1602a_handle.e_gpio_handle.gpio_pin_config.gpio_pin_num,
                             HIGH);
    udelay(us_hold_time);
    GPIO_Write_To_Output_Pin(lcd1602a_handle.e_gpio_handle.p_gpio_x,
                             lcd1602a_handle.e_gpio_handle.gpio_pin_config.gpio_pin_num,
                             LOW);
}

static void mdelay(uint32_t cnt)
{
    for (uint32_t i=0; i < (cnt * 1000); i++);
}

static void udelay(uint32_t cnt)
{
    for (uint32_t i=0; i < (cnt * 1); i++);
}
