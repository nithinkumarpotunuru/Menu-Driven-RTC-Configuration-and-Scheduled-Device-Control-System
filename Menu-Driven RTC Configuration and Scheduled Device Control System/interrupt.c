#include "interrupt.h"
#include "lcd.h"
#include "keypad.h"
#include "timer.h"
#include "rtc.h"
#include "iap.h"

void INT0_CONF(void)
{
    PINSEL1 |= 1 << 0;
    VICIntSelect = 0 << ENT0_CHAN;
    VICIntEnable = 1 << ENT0_CHAN;
    VICVectAddr0 = (u32)INT_BUTTEN;
    VICVectCntl0 = 1 << 5 | ENT0_CHAN;
    EXTMODE |= 1 << 0;
}

void INT_BUTTEN(void) __irq
{
    flage = 1;
    VICVectAddr = 0;
    EXTINT |= 1 << ENT0_CHAN;
}

/* 5-second inactivity timeout used by every edit/menu level. */
static char WaitKey5s(void)
{
    T1MR0 = 5000;
    T1PR = 15000 - 1;
    T1TC = 0;
    T1TCR = 1;

    while(T1TC != T1MR0)
    {
        if(((IOPIN1 >> COL_PINS) & 15) != 15)
        {
            T1TCR = 1 << 1;
            return key_scan();
        }
    }

    T1TCR = 1 << 1;
    return 0;
}

static void TimeMenu(void)
{
    char key;

    while(1)
    {
        LCD_CMD(LCD_CLEAR);
        LCD_CMD(LINE_1);
        LCD_STR("1.HOUR   2.MIN");
        LCD_CMD(LINE_2);
        LCD_STR("3.SEC    4.BACK");

        key = WaitKey5s();
        if(key == 0 || key == '4') return;

        if(key == '1') Edit_Time_Hour();
        else if(key == '2') Edit_Time_Min();
        else if(key == '3') Edit_Time_Sec();

        if(edit_timeout) return;
        display_refresh = 1;
    }
}

static void DateMenu(void)
{
    char key;

    while(1)
    {
        LCD_CMD(LCD_CLEAR);
        LCD_CMD(LINE_1);
        LCD_STR("1.DAY    2.MONTH");
        LCD_CMD(LINE_2);
        LCD_STR("3.YEAR   4.BACK");

        key = WaitKey5s();
        if(key == 0 || key == '4') return;

        if(key == '1') Edit_Date_Day();
        else if(key == '2') Edit_Date_Month();
        else if(key == '3') Edit_Date_Year();

        if(edit_timeout) return;
        display_refresh = 1;
    }
}

static void ScheduleMenu(void)
{
    char key;

    while(1)
    {
        LCD_CMD(LCD_CLEAR);
        LCD_CMD(LINE_1);
        LCD_STR("1.ON    2.OFF");
        LCD_CMD(LINE_2);
        LCD_STR("3.BACK          ");

        key = WaitKey5s();
        if(key == 0 || key == '3') return;

        if(key == '1') Edit_On_Time();
        else if(key == '2') Edit_Off_Time();

        if(edit_timeout) return;
        display_refresh = 1;
    }
}

void Flage_call(void)
{
    char key;

    while(1)
    {
        LCD_CMD(LCD_CLEAR);
        LCD_CMD(LINE_1);
        LCD_STR("1.RTC   2.DATE");
        LCD_CMD(LINE_2);
        LCD_STR("3.SCHED  4.EXIT");

        key = WaitKey5s();
        if(key == 0 || key == '4')
        {
            flage = 0;
            display_refresh = 1;
            LCD_CMD(LCD_CLEAR);
            return;
        }

        edit_timeout = 0;

        switch(key)
        {
            case '1':
                TimeMenu();
                break;
            case '2':
                DateMenu();
                break;
            case '3':
                ScheduleMenu();
                break;
        }

        /* Any 5-second timeout inside a submenu OR a numeric field exits the
           entire edit mode and returns to the normal RTC display. */
        if(edit_timeout)
        {
            flage = 0;
            display_refresh = 1;
            LCD_CMD(LCD_CLEAR);
            return;
        }
    }
}

void DisMoveUp(void) { }
void DisMoveDw(void) { }
