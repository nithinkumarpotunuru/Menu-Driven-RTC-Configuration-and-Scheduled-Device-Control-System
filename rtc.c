#include "rtc.h"
#include "lcd.h"
#include "keypad.h"
#include "timer.h"

void SetRTCTimeInfo(u32 hour, u32 minute, u32 second)
{
    HOUR = hour;
    MIN  = minute;
    SEC  = second;
}

void SetRTCDateInfo(u32 date, u32 month, u32 year)
{
    DOM   = date;
    MONTH = month;
    YEAR  = year;
}

void SetRTCDayInfo(u32 day)
{
    DOW = day;
}

void rtc_init(void)
{
	CCR=(1<<1);
	CCR=(1<<0)|(1<<4);
}

static u32 ScheduleSeconds(u32 hour, u32 minute, u32 second)
{
    return (hour * 3600) + (minute * 60) + second;
}

/*
 * Return 1 while the device must be ON.
 * This also handles schedules that cross midnight, for example:
 * ON 20:35:00 -> OFF 05:35:00.
 */
static u8 IsScheduleOn(void)
{
    u32 on_time;
    u32 off_time;
    u32 now;

    on_time  = ScheduleSeconds(GET_TIME_VAL(RTC_SHED_START, 3, 4),
                               GET_TIME_VAL(RTC_SHED_START, 6, 7),
                               GET_TIME_VAL(RTC_SHED_START, 9, 10));
    off_time = ScheduleSeconds(GET_TIME_VAL(RTC_SHED_END, 4, 5),
                               GET_TIME_VAL(RTC_SHED_END, 7, 8),
                               GET_TIME_VAL(RTC_SHED_END, 10, 11));
    now = ScheduleSeconds(HOUR, MIN, SEC);

    if(on_time == off_time)
        return 0;

    if(on_time < off_time)
    {
        /* Same-day interval: ON at ON time, OFF at OFF time. */
        return (now >= on_time && now < off_time) ? 1 : 0;
    }
    else
    {
        /* Midnight-crossing interval: ON after ON time OR before OFF time. */
        return (now >= on_time || now < off_time) ? 1 : 0;
    }
}

static void UpdateDevice(void)
{
    /* The Proteus LED is wired as an active-low load: P1.30 sinks
       current when the LED/device is ON. */
    if(IsScheduleOn())
        IOCLR1 = 1 << DEV0_PIN;
    else
        IOSET1 = 1 << DEV0_PIN;
}

/* Write the normal screen without clearing the LCD every second.
 * Repositioning and rewriting the 16 characters is stable in Proteus and
 * does not create the visible blinking caused by LCD_CLEAR. */
static void ShowNormalScreen(void)
{
    u8 device_on;

    TIME();
    DAY();
    DATE();
    device_on = IsScheduleOn();

    /* 16 columns: HH:MM:SS__DAY */
    LCD_CMD(LINE_1);
    LCD_STR(Time);

    /* 16 columns: DD:MM:YYYY S: ON/OFF */
    LCD_CMD(LINE_2);
    LCD_STR(Date);
    LCD_CMD(LINE_2 + 11);
    if(device_on)
        LCD_STR("S:ON ");
    else
        LCD_STR("S:OFF");
}

static void ShowScheduleScreen(void)
{
    LCD_CMD(LINE_1);
    LCD_STR("                ");
    LCD_CMD(LINE_1);
    LCD_STR(RTC_SHED_START);

    LCD_CMD(LINE_2);
    LCD_STR("                ");
    LCD_CMD(LINE_2);
    LCD_STR(RTC_SHED_END);
}

void Display(void)
{
    static u32 last_sec = 0xFFFFFFFF;
    static u8 last_mode = 0xFF;
    static u8 first_display = 1;
    u8 mode;

    /* 0-4 seconds: normal display (5 seconds).
       5-6 seconds: ON/OFF schedule (2 seconds). */
    mode = ((SEC % 7) >= 5) ? 1 : 0;

    /* Control the LED/relay from the complete time interval, not only when
       the current second happens to equal ON/OFF. This means it also works
       correctly immediately after reset and across midnight. */
    UpdateDevice();

    if(first_display || display_refresh || last_mode != mode ||
       (mode == 0 && last_sec != SEC))
    {
        if(first_display || last_mode != mode)
            LCD_CMD(LCD_CLEAR);

        if(mode == 0)
            ShowNormalScreen();
        else
            ShowScheduleScreen();

        LCD_CMD(DISP_ON);
        last_sec = SEC;
        last_mode = mode;
        first_display = 0;
        display_refresh = 0;
    }
}

void TIME(void)
{
    Time[0] = HOUR / 10 + '0';
    Time[1] = HOUR % 10 + '0';
    Time[3] = MIN / 10 + '0';
    Time[4] = MIN % 10 + '0';
    Time[6] = SEC / 10 + '0';
    Time[7] = SEC % 10 + '0';
}

void DAY(void)
{
    /* DOW: 0=SUN, 1=MON, ... 6=SAT. */
    switch(DOW)
    {
        case 0: Time[11]='S'; Time[12]='U'; Time[13]='N'; break;
        case 1: Time[11]='M'; Time[12]='O'; Time[13]='N'; break;
        case 2: Time[11]='T'; Time[12]='U'; Time[13]='E'; break;
        case 3: Time[11]='W'; Time[12]='E'; Time[13]='D'; break;
        case 4: Time[11]='T'; Time[12]='H'; Time[13]='U'; break;
        case 5: Time[11]='F'; Time[12]='R'; Time[13]='I'; break;
        case 6: Time[11]='S'; Time[12]='A'; Time[13]='T'; break;
        default: Time[11]=' '; Time[12]=' '; Time[13]=' '; break;
    }
}

void DATE(void)
{
    u32 year = YEAR;
    Date[0] = DOM / 10 + '0';
    Date[1] = DOM % 10 + '0';
    Date[3] = MONTH / 10 + '0';
    Date[4] = MONTH % 10 + '0';
    Date[6] = (year / 1000) % 10 + '0';
    Date[7] = (year / 100) % 10 + '0';
    Date[8] = (year / 10) % 10 + '0';
    Date[9] = year % 10 + '0';
}

static u32 DaysInMonth(u32 month, u32 year)
{
    switch(month)
    {
        case 2:
            if((year % 400 == 0) || ((year % 4 == 0) && (year % 100 != 0)))
                return 29;
            return 28;
        case 4:
        case 6:
        case 9:
        case 11:
            return 30;
        default:
            return 31;
    }
}

static void TimeUpdatedMessage(void)
{
    TIME();
    LCD_CMD(LCD_CLEAR);
    LCD_CMD(LINE_1);
    LCD_STR("  TIME UPDATED  ");
    LCD_CMD(LINE_2);
    LCD_STR("   *SUCCESS*   ");
    delay_ms(500);
}

void Edit_Time_Hour(void)
{
    u32 hour = ReadNumberField("HOUR (0-23)", 23, HOUR);
    if(edit_timeout) return;
    HOUR = hour;
    TimeUpdatedMessage();
}

void Edit_Time_Min(void)
{
    u32 min = ReadNumberField("MIN (0-59)", 59, MIN);
    if(edit_timeout) return;
    MIN = min;
    TimeUpdatedMessage();
}

void Edit_Time_Sec(void)
{
    u32 sec = ReadNumberField("SEC (0-59)", 59, SEC);
    if(edit_timeout) return;
    SEC = sec;
    TimeUpdatedMessage();
}

void Edit_Time(void)
{
    u32 hour = HOUR;
    u32 min  = MIN;
    u32 sec  = SEC;

    hour = ReadNumberField("HOUR (0-23)", 23, hour);
    if(edit_timeout) return;

    min = ReadNumberField("MIN (0-59)", 59, min);
    if(edit_timeout) return;

    sec = ReadNumberField("SEC (0-59)", 59, sec);
    if(edit_timeout) return;

    SetRTCTimeInfo(hour, min, sec);

    LCD_CMD(LCD_CLEAR);
    LCD_CMD(LINE_1);
    LCD_STR("  TIME UPDATED  ");
    LCD_CMD(LINE_2);
    LCD_STR("   *SUCCESS*   ");
    delay_ms(700);
}

static void DateUpdatedMessage(void)
{
    DATE();
    LCD_CMD(LCD_CLEAR);
    LCD_CMD(LINE_1);
    LCD_STR("  DATE UPDATED  ");
    LCD_CMD(LINE_2);
    LCD_STR("   *SUCCESS*   ");
    delay_ms(500);
}

void Edit_Date_Day(void)
{
    u32 day = DOM;
    day = ReadNumberField("DAY (1-31)", DaysInMonth(MONTH, YEAR), day);
    if(edit_timeout) return;
    if(day < 1 || day > DaysInMonth(MONTH, YEAR))
        return;
    DOM = day;
    DateUpdatedMessage();
}

void Edit_Date_Month(void)
{
    u32 month = MONTH;
    month = ReadNumberField("MONTH (1-12)", 12, month);
    if(edit_timeout) return;
    if(month < 1 || month > 12)
        return;
    if(DOM > DaysInMonth(month, YEAR))
        DOM = DaysInMonth(month, YEAR);
    MONTH = month;
    DateUpdatedMessage();
}

void Edit_Date_Year(void)
{
    u32 year = YEAR;
    year = Read4NumberField("YEAR (1-9999)", 9999, year);
    if(edit_timeout) return;
    if(year > 9999)
        return;
    if(DOM > DaysInMonth(MONTH, year))
        DOM = DaysInMonth(MONTH, year);
    YEAR = year;
    DateUpdatedMessage();
}

/* Compatibility: edit all date fields in sequence if called by old code. */
void Edit_Date(void)
{
    Edit_Date_Day();
    Edit_Date_Month();
    Edit_Date_Year();
}

void Time_set(u32 pos, u8 key)
{
    (void)pos;
    (void)key;
}

void Date_set(u32 pos, u8 key)
{
    (void)pos;
    (void)key;
}

void Update_Time(void)
{
    SEC = (Time[6]-'0')*10 + (Time[7]-'0');
    MIN = (Time[3]-'0')*10 + (Time[4]-'0');
    HOUR = (Time[0]-'0')*10 + (Time[1]-'0');
}

void Update_Date(void)
{
    DOM = GET_TIME_VAL(Date, 0, 1);
    MONTH = GET_TIME_VAL(Date, 3, 4);
    YEAR = GET_TIME_VAL(Date, 6, 7) * 100 + GET_TIME_VAL(Date, 8, 9);
}
