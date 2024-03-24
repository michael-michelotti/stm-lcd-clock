#include <stdlib.h>

#include "ds3231_rtc_driver.h"
#include "i2c.h"


/*************** BLOCKING GETTER FUNCTIONS *****************/
static void DS3231_Initialize(Clock_Device_t *ds3231_dev);
static seconds_t DS3231_Get_Seconds(void);
static minutes_t DS3231_Get_Minutes(void);
static hours_t DS3231_Get_Hours(void);
static full_time_t DS3231_Get_Full_Time(void);
static day_of_week_t DS3231_Get_Day_Of_Week(void);
static date_t DS3231_Get_Date(void);
static month_t DS3231_Get_Month(void);
static year_t DS3231_Get_Year(void);
static century_t DS3231_Get_Century(void);
static full_date_t DS3231_Get_Full_Date(void);
static full_datetime_t DS3231_Get_Full_Datetime(void);
static float DS3231_Get_Temp(void);

/*************** INTERRUPT GETTER FUNCTIONS *****************/
static void DS3231_Get_Seconds_IT(void);
static void DS3231_Get_Minutes_IT(void);
static void DS3231_Get_Hours_IT(void);
static void DS3231_Get_Day_Of_Week_IT(void);
static void DS3231_Get_Date_IT(void);
static void DS3231_Get_Month_IT(void);
static void DS3231_Get_Year_IT(void);
static void DS3231_Get_Century_IT(void);
static void DS3231_Get_Full_Date_IT(void);
static void DS3231_Get_Full_Time_IT(void);
static void DS3231_Get_Datetime_IT(void);

/*************** BLOCKING SETTER FUNCTIONS *****************/
static void DS3231_Set_Seconds(seconds_t seconds);
static void DS3231_Set_Minutes(minutes_t mins);
static void DS3231_Set_Hours(hours_t hours);
static void DS3231_Set_Day_Of_Week(day_of_week_t dow);
static void DS3231_Set_Date(date_t date);
static void DS3231_Set_Month(month_t month);
static void DS3231_Set_Century(century_t century);
static void DS3231_Set_Year(year_t year);
static void DS3231_Set_Full_Date(full_date_t full_date);
static void DS3231_Set_Full_Time(full_time_t full_time);
static void DS3231_Set_Full_Datetime(full_datetime_t full_datetime);

/*************** INTERRUPT SETTER FUNCTIONS *****************/
static void DS3231_Set_Seconds_IT(seconds_t seconds);
static void DS3231_Set_Minutes_IT(minutes_t minutes);
static void DS3231_Set_Hours_IT(hours_t hours);
static void DS3231_Set_Day_Of_Week_IT(day_of_week_t dow);
static void DS3231_Set_Date_IT(date_t date);
static void DS3231_Set_Month_IT(month_t month);
static void DS3231_Set_Year_IT(year_t year);
static void DS3231_Set_Century_IT(century_t century);
static void DS3231_Set_Full_Date_IT(full_date_t full_date);
static void DS3231_Set_Full_Time_IT(full_time_t full_time);
static void DS3231_Set_Full_Datetime_IT(full_datetime_t full_datetime);

/*************** CONVERSION FUNCTIONS FROM DS3231 REGISTER FORMAT *****************/
static seconds_t Convert_Seconds_From_DS3231(uint8_t sec_byte);
static minutes_t Convert_Minutes_From_DS3231(uint8_t min_byte);
static hours_t Convert_Hours_From_DS3231(uint8_t hour_byte);
static day_of_week_t Convert_Day_From_DS3231(uint8_t dow_byte);
static date_t Convert_Date_From_DS3231(uint8_t date_byte);
static month_t Convert_Month_From_DS3231(uint8_t month_byte);
static century_t Convert_Century_From_DS3231(uint8_t month_century_byte);
static year_t Convert_Year_From_DS3231(uint8_t year_byte);
static full_time_t Convert_Full_Time_From_DS3231(uint8_t *p_rx_buffer);
static full_date_t Convert_Full_Date_From_DS3231(uint8_t *p_rx_buffer);
static full_datetime_t Convert_Datetime_From_DS3231(uint8_t *p_rx_buffer);
static float Convert_Temp_From_DS3231(uint8_t *p_rx_buffer);

/*************** CONVERSION FUNCTIONS TO DS3231 REGISTER FORMAT *****************/
static uint8_t Convert_Seconds_To_DS3231(seconds_t seconds);
static uint8_t Convert_Minutes_To_DS3231(minutes_t minutes);
static uint8_t Convert_Hours_To_DS3231(hours_t hour_struct);
static uint8_t Convert_Day_To_DS3231(day_of_week_t day);
static uint8_t Convert_Date_To_DS3231(date_t date);
static uint8_t Convert_Month_Century_To_DS3231(month_t month, century_t century);
static uint8_t Convert_Year_To_DS3231(year_t year);
static void Convert_Full_Time_To_DS3231(full_time_t full_time, uint8_t *p_tx_buffer);
static void Convert_Full_Date_To_DS3231(full_date_t full_date, uint8_t *p_tx_buffer);
static void Convert_Datetime_To_DS3231(full_datetime_t datetime, uint8_t *p_tx_buffer);

/*************** GENERAL UTILITY FUNCTIONS *****************/
static void Read_From_DS3231(uint8_t *p_rx_buffer, uint8_t ds3231_addr, uint8_t len);
static void Read_From_DS3231_IT(DS3231_Unit_t ds3231_unit, uint8_t len);
static void Write_To_DS3231(uint8_t *p_tx_buffer, uint8_t ds3231_addr, uint8_t len);
static void Write_To_DS3231_IT(uint8_t *p_tx_buffer, DS3231_Unit_t ds3231_unit, uint8_t len);
static uint8_t Convert_Binary_To_BCD(uint8_t binary_byte);
static uint8_t Convert_BCD_To_Binary(uint8_t bcd_byte);


static DS3231_Handle_t ds3231_handle;

/* Implements the clock driver interface defined in Inc/clock.h for the DS3231 I2C RTC chip */
static Clock_Driver_t ds3231_clock_driver = {
        .Initialize                = DS3231_Initialize,

        .Get_Seconds             = DS3231_Get_Seconds,
        .Get_Minutes             = DS3231_Get_Minutes,
        .Get_Hours               = DS3231_Get_Hours,
        .Get_Day_Of_Week         = DS3231_Get_Day_Of_Week,
        .Get_Date                = DS3231_Get_Date,
        .Get_Month               = DS3231_Get_Month,
        .Get_Year                = DS3231_Get_Year,
        .Get_Century             = DS3231_Get_Century,
        .Get_Full_Date           = DS3231_Get_Full_Date,
        .Get_Full_Time           = DS3231_Get_Full_Time,
        .Get_Full_Datetime       = DS3231_Get_Full_Datetime,

        .Get_Seconds_IT          = DS3231_Get_Seconds_IT,
        .Get_Minutes_IT          = DS3231_Get_Minutes_IT,
        .Get_Hours_IT            = DS3231_Get_Hours_IT,
        .Get_Day_Of_Week_IT      = DS3231_Get_Day_Of_Week_IT,
        .Get_Date_IT             = DS3231_Get_Date_IT,
        .Get_Month_IT            = DS3231_Get_Month_IT,
        .Get_Year_IT             = DS3231_Get_Year_IT,
        .Get_Century_IT          = DS3231_Get_Century_IT,
        .Get_Full_Date_IT        = DS3231_Get_Full_Date_IT,
        .Get_Full_Time_IT        = DS3231_Get_Full_Time_IT,
        .Get_Datetime_IT         = DS3231_Get_Datetime_IT,

        .Set_Seconds             = DS3231_Set_Seconds,
        .Set_Minutes             = DS3231_Set_Minutes,
        .Set_Hours               = DS3231_Set_Hours,
        .Set_Day_Of_Week         = DS3231_Set_Day_Of_Week,
        .Set_Date                = DS3231_Set_Date,
        .Set_Month               = DS3231_Set_Month,
        .Set_Century             = DS3231_Set_Century,
        .Set_Year                = DS3231_Set_Year,
        .Set_Full_Date           = DS3231_Set_Full_Date,
        .Set_Full_Time           = DS3231_Set_Full_Time,
        .Set_Full_Datetime       = DS3231_Set_Full_Datetime,

        .Set_Seconds_IT          = DS3231_Set_Seconds_IT,
        .Set_Minutes_IT          = DS3231_Set_Minutes_IT,
        .Set_Hours_IT            = DS3231_Set_Hours_IT,
        .Set_Day_Of_Week_IT      = DS3231_Set_Day_Of_Week_IT,
        .Set_Date_IT             = DS3231_Set_Date_IT,
        .Set_Month_IT            = DS3231_Set_Month_IT,
        .Set_Year_IT             = DS3231_Set_Year_IT,
        .Set_Century_IT          = DS3231_Set_Century_IT,
        .Set_Full_Date_IT        = DS3231_Set_Full_Date_IT,
        .Set_Full_Time_IT        = DS3231_Set_Full_Time_IT,
        .Set_Full_Datetime_IT    = DS3231_Set_Full_Datetime_IT
};

Clock_Driver_t *get_clock_driver(void)
{
    return &ds3231_clock_driver;
}

static void DS3231_Initialize(Clock_Device_t *ds3231_dev)
{
    ds3231_handle.clock_dev = ds3231_dev;
    ds3231_handle.state = DS3231_STATE_IDLE;
    ds3231_handle.curr_unit = DS3231_UNIT_NONE;
    ds3231_handle.i2c_interface = get_i2c_interface();
    ds3231_handle.i2c_interface->Initialize();
}

void I2C_Write_Complete_Callback(I2C_Device_t *p_i2c_dev)
{
    uint8_t byte_len;

    switch (ds3231_handle.state)
    {
        case DS3231_STATE_POINTER_WRITE_FOR_READ:
            ds3231_handle.state = DS3231_STATE_DATA_READ;

            if (ds3231_handle.curr_unit == DS3231_UNIT_SECONDS)
            {
                byte_len = DS3231_LEN_SECONDS;
            }
            else if (ds3231_handle.curr_unit == DS3231_UNIT_MINUTES)
            {
                byte_len = DS3231_LEN_MINUTES;
            }
            else if (ds3231_handle.curr_unit == DS3231_UNIT_HOURS)
            {
                byte_len = DS3231_LEN_HOURS;
            }
            else if (ds3231_handle.curr_unit == DS3231_UNIT_FULL_TIME)
            {
                byte_len = DS3231_LEN_FULL_TIME;
            }
            else if (ds3231_handle.curr_unit == DS3231_UNIT_DOW)
            {
                byte_len = DS3231_LEN_DOW;
            }
            else if (ds3231_handle.curr_unit == DS3231_UNIT_DATE)
            {
                byte_len = DS3231_LEN_DATE;
            }
            else if (ds3231_handle.curr_unit == DS3231_UNIT_MONTHS)
            {
                byte_len = DS3231_LEN_MONTH_CENTURY;
            }
            else if (ds3231_handle.curr_unit == DS3231_UNIT_YEAR)
            {
                byte_len = DS3231_LEN_YEAR;
            }
            else if (ds3231_handle.curr_unit == DS3231_UNIT_CENTURY)
            {
                byte_len = DS3231_LEN_MONTH_CENTURY;
            }
            else if (ds3231_handle.curr_unit == DS3231_UNIT_FULL_DATE)
            {
                byte_len = DS3231_LEN_FULL_DATE;
            }
            else if (ds3231_handle.curr_unit == DS3231_UNIT_DATETIME)
            {
                byte_len = DS3231_LEN_DATETIME;
            }
            ds3231_handle.i2c_interface->Read_Bytes_IT(byte_len,
                                                       DS3231_SLAVE_ADDR,
                                                       I2C_DISABLE_SR);
            break;
        case DS3231_STATE_DATA_WRITE:
            ds3231_handle.state = DS3231_STATE_IDLE;
            switch (ds3231_handle.curr_unit)
            {
            case DS3231_UNIT_SECONDS:
                Clock_Set_Seconds_Complete_Callback(ds3231_handle.clock_dev);
                break;
            case DS3231_UNIT_MINUTES:
                Clock_Set_Minutes_Complete_Callback(ds3231_handle.clock_dev);
                break;
            case DS3231_UNIT_HOURS:
                Clock_Set_Hours_Complete_Callback(ds3231_handle.clock_dev);
                break;
            case DS3231_UNIT_DOW:
                Clock_Set_Day_Of_Week_Complete_Callback(ds3231_handle.clock_dev);
                break;
            case DS3231_UNIT_DATE:
                Clock_Set_Date_Complete_Callback(ds3231_handle.clock_dev);
                break;
            case DS3231_UNIT_MONTHS:
                Clock_Set_Months_Complete_Callback(ds3231_handle.clock_dev);
                break;
            case DS3231_UNIT_CENTURY:
                Clock_Set_Century_Complete_Callback(ds3231_handle.clock_dev);
                break;
            case DS3231_UNIT_YEAR:
                Clock_Set_Years_Complete_Callback(ds3231_handle.clock_dev);
                break;
            case DS3231_UNIT_FULL_TIME:
                Clock_Set_Full_Time_Complete_Callback(ds3231_handle.clock_dev);
                break;
            case DS3231_UNIT_FULL_DATE:
                Clock_Set_Full_Date_Complete_Callback(ds3231_handle.clock_dev);
                break;
            case DS3231_UNIT_DATETIME:
                Clock_Set_Datetime_Complete_Callback(ds3231_handle.clock_dev);
                break;
            }
    }
}

void I2C_Read_Complete_Callback(I2C_Device_t *p_i2c_dev)
{
    uint8_t *out_buffer;
    full_date_t full_date;
    full_time_t full_time;

    switch (ds3231_handle.curr_unit)
    {
        case DS3231_UNIT_SECONDS:
            seconds_t new_secs = Convert_Seconds_From_DS3231(*I2C_RX_Ring_Buffer_Read(p_i2c_dev, DS3231_LEN_SECONDS));
            ds3231_handle.clock_dev->time.seconds = new_secs;
            Clock_Get_Seconds_Complete_Callback(ds3231_handle.clock_dev);
            break;
        case DS3231_UNIT_MINUTES:
            minutes_t new_mins = Convert_Minutes_From_DS3231(*I2C_RX_Ring_Buffer_Read(p_i2c_dev, DS3231_LEN_MINUTES));
            ds3231_handle.clock_dev->time.minutes = new_mins;
            Clock_Get_Minutes_Complete_Callback(ds3231_handle.clock_dev);
            break;
        case DS3231_UNIT_HOURS:
            hours_t new_hours = Convert_Hours_From_DS3231(*I2C_RX_Ring_Buffer_Read(p_i2c_dev, DS3231_LEN_HOURS));
            ds3231_handle.clock_dev->time.hours = new_hours;
            Clock_Get_Hours_Complete_Callback(ds3231_handle.clock_dev);
            break;
        case DS3231_UNIT_DOW:
            day_of_week_t new_dow = Convert_Day_From_DS3231(*I2C_RX_Ring_Buffer_Read(p_i2c_dev, DS3231_LEN_DOW));
            ds3231_handle.clock_dev->date.day_of_week = new_dow;
            Clock_Get_Day_Of_Week_Complete_Callback(ds3231_handle.clock_dev);
            break;
        case DS3231_UNIT_DATE:
            date_t new_date = Convert_Date_From_DS3231(*I2C_RX_Ring_Buffer_Read(p_i2c_dev, DS3231_LEN_DATE));
            ds3231_handle.clock_dev->date.date = new_date;
            Clock_Get_Date_Complete_Callback(ds3231_handle.clock_dev);
            break;
        case DS3231_UNIT_MONTHS:
            month_t new_month = Convert_Month_From_DS3231(*I2C_RX_Ring_Buffer_Read(p_i2c_dev, DS3231_LEN_MONTH_CENTURY));
            ds3231_handle.clock_dev->date.month = new_month;
            Clock_Get_Month_Complete_Callback(ds3231_handle.clock_dev);
            break;
        case DS3231_UNIT_YEAR:
            year_t new_year = Convert_Month_From_DS3231(*I2C_RX_Ring_Buffer_Read(p_i2c_dev, DS3231_LEN_YEAR));
            ds3231_handle.clock_dev->date.year = new_year;
            Clock_Get_Year_Complete_Callback(ds3231_handle.clock_dev);
            break;
        case DS3231_UNIT_CENTURY:
            century_t new_century = Convert_Month_From_DS3231(*I2C_RX_Ring_Buffer_Read(p_i2c_dev, DS3231_LEN_MONTH_CENTURY));
            ds3231_handle.state = DS3231_STATE_IDLE;
            ds3231_handle.clock_dev->date.century = new_century;
            Clock_Get_Century_Complete_Callback(ds3231_handle.clock_dev);
            break;
        case DS3231_UNIT_FULL_DATE:
            out_buffer = I2C_RX_Ring_Buffer_Read(p_i2c_dev, DS3231_LEN_FULL_DATE);
            full_date = Convert_Full_Date_From_DS3231(out_buffer);
            ds3231_handle.clock_dev->date = full_date;
            Clock_Get_Full_Date_Complete_Callback(ds3231_handle.clock_dev);
            break;
        case DS3231_UNIT_FULL_TIME:
            out_buffer = I2C_RX_Ring_Buffer_Read(p_i2c_dev, DS3231_LEN_FULL_TIME);
            full_time = Convert_Full_Time_From_DS3231(out_buffer);
            ds3231_handle.clock_dev->time = full_time;
            Clock_Get_Full_Time_Complete_Callback(ds3231_handle.clock_dev);
            break;
        case DS3231_UNIT_DATETIME:
            out_buffer = I2C_RX_Ring_Buffer_Read(p_i2c_dev, DS3231_LEN_DATETIME);
            full_time = Convert_Full_Time_From_DS3231(out_buffer);
            full_date = Convert_Full_Date_From_DS3231(out_buffer + DS3231_LEN_FULL_TIME);
            ds3231_handle.clock_dev->time = full_time;
            ds3231_handle.clock_dev->date = full_date;
            Clock_Get_Datetime_Complete_Callback(ds3231_handle.clock_dev);
            break;
    }
    ds3231_handle.state = DS3231_STATE_IDLE;
}

/***************************************************************/
/***************************************************************/
/* Blocking Getter APIs                                        */
/***************************************************************/
/***************************************************************/
static seconds_t DS3231_Get_Seconds(void)
{
    uint8_t p_rx_buffer[DS3231_LEN_SECONDS];
    Read_From_DS3231(p_rx_buffer, DS3231_ADDR_SECONDS, DS3231_LEN_SECONDS);
    seconds_t new_secs = Convert_Seconds_From_DS3231(*p_rx_buffer);
    return new_secs;
}

static minutes_t DS3231_Get_Minutes(void)
{
    uint8_t p_rx_buffer[DS3231_LEN_MINUTES];
    Read_From_DS3231(p_rx_buffer, DS3231_ADDR_MINUTES, DS3231_LEN_MINUTES);
    seconds_t new_mins = Convert_Minutes_From_DS3231(*p_rx_buffer);
    return new_mins;
}

static hours_t DS3231_Get_Hours(void)
{
    uint8_t p_rx_buffer[DS3231_LEN_HOURS];
    Read_From_DS3231(p_rx_buffer, DS3231_ADDR_HOURS, DS3231_LEN_HOURS);
    hours_t new_hours = Convert_Hours_From_DS3231(*p_rx_buffer);
    return new_hours;
}

static full_time_t DS3231_Get_Full_Time(void)
{
    hours_t hours = DS3231_Get_Hours();
    minutes_t mins = DS3231_Get_Minutes();
    seconds_t secs = DS3231_Get_Seconds();
    full_time_t full_time = { .hours = hours, .minutes = mins, .seconds = secs };
    return full_time;
}

static day_of_week_t DS3231_Get_Day_Of_Week(void)
{
    uint8_t p_rx_buffer[DS3231_LEN_DOW];
    Read_From_DS3231(p_rx_buffer, DS3231_ADDR_DAY, DS3231_LEN_DOW);
    day_of_week_t dow = Convert_Day_From_DS3231(*p_rx_buffer);
    return dow;
}

static date_t DS3231_Get_Date(void)
{
    uint8_t p_rx_buffer[DS3231_LEN_DATE];
    Read_From_DS3231(p_rx_buffer, DS3231_ADDR_DATE, DS3231_LEN_DATE);
    date_t new_date = Convert_Date_From_DS3231(*p_rx_buffer);
    return new_date;
}

static month_t DS3231_Get_Month(void)
{
    uint8_t p_rx_buffer[DS3231_LEN_MONTH_CENTURY];
    Read_From_DS3231(p_rx_buffer, DS3231_ADDR_MONTH_CENTURY, DS3231_LEN_MONTH_CENTURY);
    month_t new_month = Convert_Month_From_DS3231(*p_rx_buffer);
    return new_month;
}

static century_t DS3231_Get_Century(void)
{
    uint8_t p_rx_buffer[DS3231_LEN_MONTH_CENTURY];
    Read_From_DS3231(p_rx_buffer, DS3231_ADDR_MONTH_CENTURY, DS3231_LEN_MONTH_CENTURY);
    century_t new_century = (*p_rx_buffer) >> 7;
    return new_century;
}

static year_t DS3231_Get_Year(void)
{
    uint8_t p_rx_buffer[DS3231_LEN_YEAR];
    Read_From_DS3231(p_rx_buffer, DS3231_ADDR_YEAR, DS3231_LEN_YEAR);
    year_t new_year = Convert_Year_From_DS3231(*p_rx_buffer);
    return new_year;
}

static full_date_t DS3231_Get_Full_Date(void)
{
    full_date_t full_date;
    full_date.date = DS3231_Get_Date();
    full_date.day_of_week = DS3231_Get_Day_Of_Week();
    full_date.month = DS3231_Get_Month();
    full_date.year = DS3231_Get_Year();
    full_date.century = DS3231_Get_Century();
    return full_date;
}

static full_datetime_t DS3231_Get_Full_Datetime(void)
{
    full_datetime_t datetime;
    datetime.time = DS3231_Get_Full_Time();
    datetime.date = DS3231_Get_Full_Date();
    return datetime;
}

static float DS3231_Get_Temp(void)
{
    uint8_t p_rx_buffer[2];
    Read_From_DS3231(p_rx_buffer, DS3231_ADDR_MSB_TEMP, 2);
    return Convert_Temp_From_DS3231(p_rx_buffer);
}

/***************************************************************/
/***************************************************************/
/* Interrupt Based Getter APIs                                 */
/***************************************************************/
/***************************************************************/
static void DS3231_Get_Seconds_IT(void)
{
    Read_From_DS3231_IT(DS3231_UNIT_SECONDS, DS3231_LEN_SECONDS);
}

static void DS3231_Get_Minutes_IT(void)
{
    Read_From_DS3231_IT(DS3231_UNIT_MINUTES, DS3231_LEN_MINUTES);
}

static void DS3231_Get_Hours_IT(void)
{
    Read_From_DS3231_IT(DS3231_UNIT_HOURS, DS3231_LEN_HOURS);
}

static void DS3231_Get_Day_Of_Week_IT(void)
{
    Read_From_DS3231_IT(DS3231_UNIT_DOW, DS3231_LEN_DOW);
}

static void DS3231_Get_Date_IT(void)
{
    Read_From_DS3231_IT(DS3231_UNIT_DATE, DS3231_LEN_DATE);
}

static void DS3231_Get_Month_IT(void)
{
    Read_From_DS3231_IT(DS3231_UNIT_MONTHS, DS3231_LEN_MONTH_CENTURY);
}

static void DS3231_Get_Year_IT(void)
{
    Read_From_DS3231_IT(DS3231_UNIT_YEAR, DS3231_LEN_YEAR);
}

static void DS3231_Get_Century_IT(void)
{
    Read_From_DS3231_IT(DS3231_UNIT_CENTURY, DS3231_LEN_MONTH_CENTURY);
}

static void DS3231_Get_Full_Date_IT(void)
{
    Read_From_DS3231_IT(DS3231_UNIT_FULL_DATE, DS3231_LEN_FULL_DATE);
}

static void DS3231_Get_Full_Time_IT(void)
{
    Read_From_DS3231_IT(DS3231_UNIT_FULL_TIME, DS3231_LEN_FULL_TIME);
}

static void DS3231_Get_Datetime_IT(void)
{
    Read_From_DS3231_IT(DS3231_UNIT_DATETIME, DS3231_LEN_DATETIME);
}

/***************************************************************/
/***************************************************************/
/* Blocking Setter APIs                                        */
/***************************************************************/
/***************************************************************/
static void DS3231_Set_Seconds(seconds_t seconds)
{
    uint8_t tx_buffer[DS3231_LEN_SECONDS + 1] = { DS3231_ADDR_SECONDS, Convert_Seconds_To_DS3231(seconds) };
    Write_To_DS3231(tx_buffer, DS3231_ADDR_SECONDS, DS3231_LEN_SECONDS + 1);
}

static void DS3231_Set_Minutes(minutes_t mins)
{
    uint8_t p_tx_buffer[DS3231_LEN_MINUTES + 1] = { DS3231_ADDR_MINUTES, Convert_Minutes_To_DS3231(mins) };
    Write_To_DS3231(p_tx_buffer, DS3231_ADDR_MINUTES, DS3231_LEN_MINUTES + 1);
}

static void DS3231_Set_Hours(hours_t hours)
{
    uint8_t p_tx_buffer[DS3231_LEN_HOURS + 1] = { DS3231_ADDR_HOURS, Convert_Hours_To_DS3231(hours) };
    Write_To_DS3231(p_tx_buffer, DS3231_ADDR_HOURS, DS3231_LEN_HOURS + 1);
}

static void DS3231_Set_Day_Of_Week(day_of_week_t dow)
{
    uint8_t p_tx_buffer[DS3231_LEN_DOW + 1] = { DS3231_ADDR_DAY, Convert_Day_To_DS3231(dow) };
    Write_To_DS3231(p_tx_buffer, DS3231_ADDR_DAY, DS3231_LEN_DOW + 1);
}

static void DS3231_Set_Date(date_t date)
{
    uint8_t p_tx_buffer[DS3231_LEN_DATE + 1] = { DS3231_ADDR_DATE, Convert_Date_To_DS3231(date) };
    Write_To_DS3231(p_tx_buffer, DS3231_ADDR_DATE, DS3231_LEN_DATE + 1);
}

static void DS3231_Set_Month(month_t month)
{
    century_t current_century = DS3231_Get_Century();
    uint8_t p_tx_buffer[DS3231_LEN_MONTH_CENTURY + 1] = { DS3231_ADDR_MONTH_CENTURY, Convert_Month_Century_To_DS3231(month, current_century) };
    Write_To_DS3231(p_tx_buffer, DS3231_ADDR_MONTH_CENTURY, DS3231_LEN_MONTH_CENTURY + 1);
}

static void DS3231_Set_Century(century_t century)
{
    month_t current_month = DS3231_Get_Month();
    uint8_t p_tx_buffer[DS3231_LEN_MONTH_CENTURY + 1] = { DS3231_ADDR_MONTH_CENTURY, Convert_Month_Century_To_DS3231(current_month, century) };
    Write_To_DS3231(p_tx_buffer, DS3231_ADDR_MONTH_CENTURY, DS3231_LEN_MONTH_CENTURY + 1);
}

static void DS3231_Set_Year(year_t year)
{
    uint8_t p_tx_buffer[DS3231_LEN_YEAR + 1] = { DS3231_ADDR_YEAR, Convert_Year_To_DS3231(year) };
    Write_To_DS3231(p_tx_buffer, DS3231_ADDR_YEAR, DS3231_LEN_YEAR + 1);
}

static void DS3231_Set_Full_Date(full_date_t full_date)
{
    DS3231_Set_Day_Of_Week(full_date.day_of_week);
    DS3231_Set_Date(full_date.date);
    DS3231_Set_Month(full_date.month);
    DS3231_Set_Year(full_date.year);
}

static void DS3231_Set_Full_Time(full_time_t full_time)
{
    DS3231_Set_Seconds(full_time.seconds);
    DS3231_Set_Minutes(full_time.minutes);
    DS3231_Set_Hours(full_time.hours);
}

static void DS3231_Set_Full_Datetime(full_datetime_t full_datetime)
{
    DS3231_Set_Full_Date(full_datetime.date);
    DS3231_Set_Full_Time(full_datetime.time);
}


/***************************************************************/
/***************************************************************/
/* Interrupt Based Setter APIs                                 */
/***************************************************************/
/***************************************************************/
static void DS3231_Set_Seconds_IT(seconds_t seconds)
{
    uint8_t p_tx_buffer[DS3231_LEN_SECONDS + 1] = { DS3231_ADDR_SECONDS, Convert_Seconds_To_DS3231(seconds) };
    Write_To_DS3231_IT(p_tx_buffer, DS3231_UNIT_SECONDS, DS3231_LEN_SECONDS + 1);
}

static void DS3231_Set_Minutes_IT(minutes_t minutes)
{
    uint8_t p_tx_buffer[DS3231_LEN_MINUTES + 1] = { DS3231_ADDR_MINUTES, Convert_Minutes_To_DS3231(minutes) };
    Write_To_DS3231_IT(p_tx_buffer, DS3231_UNIT_MINUTES, DS3231_LEN_MINUTES + 1);
}

static void DS3231_Set_Hours_IT(hours_t hours)
{
    uint8_t p_tx_buffer[DS3231_LEN_HOURS + 1] = { DS3231_ADDR_HOURS, Convert_Hours_To_DS3231(hours) };
    Write_To_DS3231_IT(p_tx_buffer, DS3231_UNIT_HOURS, DS3231_LEN_HOURS + 1);
}

static void DS3231_Set_Day_Of_Week_IT(day_of_week_t dow)
{
    uint8_t p_tx_buffer[DS3231_LEN_DOW + 1] = { DS3231_ADDR_DAY, Convert_Day_To_DS3231(dow) };
    Write_To_DS3231_IT(p_tx_buffer, DS3231_UNIT_DOW, DS3231_LEN_DOW + 1);
}

static void DS3231_Set_Date_IT(date_t date)
{
    uint8_t p_tx_buffer[DS3231_LEN_DATE + 1] = { DS3231_ADDR_DATE, Convert_Day_To_DS3231(date) };
    Write_To_DS3231_IT(p_tx_buffer, DS3231_UNIT_DATE, DS3231_LEN_DATE + 1);
}

static void DS3231_Set_Month_IT(month_t month)
{
    /* TODO: find way to avoid this blocking call in the middle of an interrupt call */
    month_t current_century = DS3231_Get_Century();
    uint8_t p_tx_buffer[DS3231_LEN_MONTH_CENTURY + 1] = { DS3231_ADDR_MONTH_CENTURY, Convert_Month_Century_To_DS3231(month, current_century) };
    Write_To_DS3231_IT(p_tx_buffer, DS3231_UNIT_MONTHS, DS3231_LEN_MONTH_CENTURY + 1);
}

static void DS3231_Set_Year_IT(year_t year)
{
    uint8_t p_tx_buffer[DS3231_LEN_YEAR + 1] = { DS3231_ADDR_YEAR, Convert_Year_To_DS3231(year) };
    Write_To_DS3231_IT(p_tx_buffer, DS3231_UNIT_YEAR, DS3231_LEN_YEAR + 1);
}

static void DS3231_Set_Century_IT(century_t century)
{
    /* TODO: find way to avoid this blocking call in the middle of an interrupt call */
    month_t current_month = DS3231_Get_Month();
    uint8_t p_tx_buffer[DS3231_LEN_MONTH_CENTURY + 1] = { DS3231_ADDR_MONTH_CENTURY, Convert_Month_Century_To_DS3231(current_month, century) };
    Write_To_DS3231_IT(p_tx_buffer, DS3231_UNIT_CENTURY, DS3231_LEN_MONTH_CENTURY + 1);
}

static void DS3231_Set_Full_Date_IT(full_date_t full_date)
{
    uint8_t p_tx_buffer[DS3231_LEN_FULL_DATE + 1];
    p_tx_buffer[0] = DS3231_ADDR_DAY;
    Convert_Full_Date_To_DS3231(full_date, p_tx_buffer + 1);
    Write_To_DS3231_IT(p_tx_buffer, DS3231_UNIT_FULL_DATE, DS3231_LEN_FULL_DATE + 1);
}

static void DS3231_Set_Full_Time_IT(full_time_t full_time)
{
    uint8_t p_tx_buffer[DS3231_LEN_FULL_TIME + 1];
    p_tx_buffer[0] = DS3231_ADDR_SECONDS;
    Convert_Full_Time_To_DS3231(full_time, p_tx_buffer + 1);
    Write_To_DS3231_IT(p_tx_buffer, DS3231_UNIT_FULL_TIME, DS3231_LEN_FULL_TIME + 1);
}

static void DS3231_Set_Full_Datetime_IT(full_datetime_t full_datetime)
{
    uint8_t p_tx_buffer[DS3231_LEN_DATETIME + 1];
    p_tx_buffer[0] = DS3231_ADDR_SECONDS;
    Convert_Datetime_To_DS3231(full_datetime, p_tx_buffer + 1);
    Write_To_DS3231_IT(p_tx_buffer, DS3231_UNIT_DATETIME, DS3231_LEN_DATETIME + 1);
}

/*************** UTILITY FUNCTIONS *****************/
/* Functions for generalized case of reading and writing to DS3231. "*_IT" functions are interrupt-based. */
static void Read_From_DS3231(uint8_t *p_rx_buffer, uint8_t ds3231_addr, uint8_t len)
{
    /* Every read from DS3231 must begin with writing the register pointer, which the read will start from */
    uint8_t p_tx_buffer[1] = { ds3231_addr };
    ds3231_handle.i2c_interface->Write_Bytes(p_tx_buffer, DS3231_PTR_LEN, DS3231_SLAVE_ADDR, I2C_ENABLE_SR);
    ds3231_handle.i2c_interface->Read_Bytes(p_rx_buffer, len, DS3231_SLAVE_ADDR, I2C_DISABLE_SR);
}

static void Read_From_DS3231_IT(DS3231_Unit_t ds3231_unit, uint8_t len)
{
    if (ds3231_handle.state != DS3231_STATE_IDLE)
    {
        return;
    }

    uint8_t    *p_tx_buffer;
    uint8_t ds3231_addr;

    switch (ds3231_unit)
    {
    case DS3231_UNIT_SECONDS:
        ds3231_addr = DS3231_ADDR_SECONDS;
        break;
    case DS3231_UNIT_MINUTES:
        ds3231_addr = DS3231_ADDR_MINUTES;
        break;
    case DS3231_UNIT_HOURS:
        ds3231_addr = DS3231_ADDR_HOURS;
        break;
    case DS3231_UNIT_DOW:
        ds3231_addr = DS3231_ADDR_DAY;
        break;
    case DS3231_UNIT_DATE:
        ds3231_addr = DS3231_ADDR_DATE;
        break;
    case DS3231_UNIT_MONTHS:
        ds3231_addr = DS3231_ADDR_MONTH_CENTURY;
        break;
    case DS3231_UNIT_YEAR:
        ds3231_addr = DS3231_ADDR_YEAR;
        break;
    case DS3231_UNIT_CENTURY:
        ds3231_addr = DS3231_ADDR_MONTH_CENTURY;
        break;
    case DS3231_UNIT_FULL_DATE:
        ds3231_addr = DS3231_ADDR_DAY;
        break;
    case DS3231_UNIT_FULL_TIME:
        ds3231_addr = DS3231_ADDR_SECONDS;
        break;
    case DS3231_UNIT_DATETIME:
        ds3231_addr = DS3231_ADDR_SECONDS;
        break;
    }
    p_tx_buffer = &ds3231_addr;

    ds3231_handle.state = DS3231_STATE_POINTER_WRITE_FOR_READ;
    ds3231_handle.curr_unit = ds3231_unit;
    ds3231_handle.i2c_interface->Write_Bytes_IT(p_tx_buffer, DS3231_PTR_LEN, DS3231_SLAVE_ADDR, I2C_ENABLE_SR);
}

static void Write_To_DS3231(uint8_t *p_tx_buffer, uint8_t ds3231_addr, uint8_t len)
{
    ds3231_handle.i2c_interface->Write_Bytes(p_tx_buffer, len, DS3231_SLAVE_ADDR, I2C_DISABLE_SR);

}

static void Write_To_DS3231_IT(uint8_t *p_tx_buffer, DS3231_Unit_t ds3231_unit, uint8_t len)
{
    if (ds3231_handle.state != DS3231_STATE_IDLE)
    {
        return;
    }

    ds3231_handle.state = DS3231_STATE_DATA_WRITE;
    ds3231_handle.curr_unit = ds3231_unit;
    ds3231_handle.i2c_interface->Write_Bytes_IT(p_tx_buffer, len, DS3231_SLAVE_ADDR, I2C_DISABLE_SR);
}

/*********** CONVERSION FUNCTIONS FROM TIME TYPES TO DS3231 REGISTER FORMAT *************/
static seconds_t Convert_Seconds_From_DS3231(uint8_t sec_byte)
{
    return Convert_BCD_To_Binary(sec_byte);
}

static minutes_t Convert_Minutes_From_DS3231(uint8_t min_byte)
{
    return Convert_BCD_To_Binary(min_byte);
}

static hours_t Convert_Hours_From_DS3231(uint8_t hour_byte)
{
    uint8_t hour_tens;
    hours_t hour_struct;

    hour_struct.hour_format = (hour_byte >> DS3231_12_24_BIT) & 1;

    if (hour_struct.hour_format == HOUR_FORMAT_12_HOUR)
    {
        hour_struct.am_pm = (hour_byte >> DS3231_AM_PM_BIT) & 1;
        hour_tens = (hour_byte >> 4) & 0x1;
    }
    else
    {
        hour_struct.am_pm = AM_PM_NONE;
        hour_tens = (hour_byte >> 4) & 0x3;
    }

    /* Convert hour from BCD to binary */
    hour_struct.hour = (hour_byte & 0xF) + (hour_tens * 10);

    return hour_struct;
}

static day_of_week_t Convert_Day_From_DS3231(uint8_t dow_byte)
{
    return dow_byte & 0x7;
}

static date_t Convert_Date_From_DS3231(uint8_t date_byte)
{
    return Convert_BCD_To_Binary(date_byte);
}

static month_t Convert_Month_From_DS3231(uint8_t month_byte)
{
    return Convert_BCD_To_Binary(month_byte & 0x1F);
}

static century_t Convert_Century_From_DS3231(uint8_t month_century_byte)
{
    return (month_century_byte >> DS3231_CENTURY_BIT);
}

static year_t Convert_Year_From_DS3231(uint8_t year_byte)
{
    return Convert_BCD_To_Binary(year_byte);
}

static full_time_t Convert_Full_Time_From_DS3231(uint8_t *p_rx_buffer)
{
    full_time_t time;
    time.seconds = Convert_Seconds_From_DS3231(p_rx_buffer[0]);
    time.minutes = Convert_Minutes_From_DS3231(p_rx_buffer[1]);
    time.hours = Convert_Hours_From_DS3231(p_rx_buffer[2]);
    return time;
}

static full_date_t Convert_Full_Date_From_DS3231(uint8_t *p_rx_buffer)
{
    full_date_t date;
    date.day_of_week = Convert_Day_From_DS3231(p_rx_buffer[0]);
    date.date = Convert_Date_From_DS3231(p_rx_buffer[1]);
    date.month = Convert_Month_From_DS3231(p_rx_buffer[2]);
    date.century = Convert_Century_From_DS3231(p_rx_buffer[2]);
    date.year = Convert_Year_From_DS3231(p_rx_buffer[3]);
    return date;
}

static full_datetime_t Convert_Datetime_From_DS3231(uint8_t *p_rx_buffer)
{
    full_datetime_t datetime;
    datetime.time = Convert_Full_Time_From_DS3231(p_rx_buffer);
    datetime.date = Convert_Full_Date_From_DS3231(p_rx_buffer + DS3231_LEN_FULL_TIME);
    return datetime;
}

static float Convert_Temp_From_DS3231(uint8_t *p_rx_buffer)
{
    float temp = 0;
    float fractional = 0;
    uint8_t thing = 0;
    uint8_t temp_msb = *p_rx_buffer;
    p_rx_buffer++;
    uint8_t temp_lsb = *p_rx_buffer;

    /* Bottom 2 bits of temperature is the decimal portion */
    thing = (temp_lsb >> 6);
    fractional = thing / 4;
    temp = temp_msb + fractional;

    return temp;
}

/************ CONVERSION FUNCTIONS FROM TIME TYPES TO DS3231 REGISTER FORMAT **************/
static uint8_t Convert_Seconds_To_DS3231(seconds_t seconds)
{
    return Convert_Binary_To_BCD(seconds);
}

static uint8_t Convert_Minutes_To_DS3231(minutes_t minutes)
{
    return Convert_Binary_To_BCD(minutes);
}

static uint8_t Convert_Hours_To_DS3231(hours_t hour_struct)
{
    /* Convert hours to BCD first */
    uint8_t ds3231_hour_byte = Convert_Binary_To_BCD(hour_struct.hour);

    /* Then, set the utility bits (AM/PM, 12/24 hour) */
    if (hour_struct.hour_format == HOUR_FORMAT_12_HOUR)
    {
        ds3231_hour_byte |= (1 << DS3231_12_24_BIT);
        ds3231_hour_byte |= (hour_struct.am_pm << DS3231_AM_PM_BIT);
    }

    return ds3231_hour_byte;
}

static uint8_t Convert_Day_To_DS3231(day_of_week_t day)
{
    return day & 0x7;
}

static uint8_t Convert_Date_To_DS3231(date_t date)
{
    return Convert_Binary_To_BCD(date);
}

static uint8_t Convert_Month_Century_To_DS3231(month_t month, century_t century)
{
    uint8_t month_byte = Convert_Binary_To_BCD(month);
    return (month_byte |= (century << DS3231_CENTURY_BIT));
}

static uint8_t Convert_Year_To_DS3231(year_t year)
{
    return Convert_Binary_To_BCD(year);
}

static void Convert_Full_Time_To_DS3231(full_time_t full_time, uint8_t *p_tx_buffer)
{
    p_tx_buffer[0] = Convert_Seconds_To_DS3231(full_time.seconds);
    p_tx_buffer[1] = Convert_Minutes_To_DS3231(full_time.minutes);
    p_tx_buffer[2] = Convert_Hours_To_DS3231(full_time.hours);
}

static void Convert_Full_Date_To_DS3231(full_date_t full_date, uint8_t *p_tx_buffer)
{
    p_tx_buffer[0] = Convert_Day_To_DS3231(full_date.day_of_week);
    p_tx_buffer[1] = Convert_Date_To_DS3231(full_date.date);
    p_tx_buffer[2] = Convert_Month_Century_To_DS3231(full_date.month, full_date.century);
    p_tx_buffer[3] = Convert_Year_To_DS3231(full_date.year);
}

static void Convert_Datetime_To_DS3231(full_datetime_t datetime, uint8_t *p_tx_buffer)
{
    Convert_Full_Time_To_DS3231(datetime.time, p_tx_buffer);
    Convert_Full_Date_To_DS3231(datetime.date, p_tx_buffer + DS3231_LEN_FULL_TIME);
}

/*************** GENERAL UTILITY FUNCTIONS *****************/
static uint8_t Convert_Binary_To_BCD(uint8_t binary_byte)
{
    uint8_t low_nybble = binary_byte % 10;
    uint8_t high_nybble = binary_byte / 10;

    return ((high_nybble << 4) | low_nybble);
}

static uint8_t Convert_BCD_To_Binary(uint8_t bcd_byte)
{
    // zeroes place (bottom 4 bits)
    uint8_t zeroes_place = bcd_byte & 0xF;
    // tens place (top 4 bits)
    uint8_t tens_place = (bcd_byte >> 4)& 0x7;

    return zeroes_place + (tens_place * 10);
}
