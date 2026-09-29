#include "config.h"
#include "lcd.h"
#include "timer.h"
#include "rtc.h"
#include "iap.h"
#include "interrupt.h"

// Definition of Global Variables
char Data_Buffer[512] __attribute__((aligned(4)));

u8 Time[] = {"00:00:00        "};
u8 Date[] = {"01/01/2026      "};

u8 KPM[4][4] = {
    "789%",
    "456*",
    "123-",
    "c0=+"
};

u8 MENU[4][15] = {
    "1.RTC",
    "2.DATE",
    "3.SCHED",
    "4.EXIT"
};

u8 RTC_SHED_START[] = {"ON:09:00:00"};
u8 RTC_SHED_END[]   = {"OFF:17:00:00"};

u8 CGRAM_SPC[] = {
    0x04, 0x0E, 0X1F, 0X1F, 0X04, 0X04, 0X04, 0X00, 
    0x04, 0x04, 0x04, 0X1F, 0X1F, 0X0E, 0X04, 0X00, 
    0x20, 0x20, 0x01, 0x03, 0x16, 0x1c, 0x08, 0x00, 
    0x20, 0x11, 0x0a, 0x04, 0x0a, 0x11, 0x20, 0x00, 
    0X20, 0X04, 0X0e, 0X1F, 0X0E, 0X04, 0X20, 0X00 
};

u32 flage = 0;
u32 display_refresh = 1;

void IO_DIR(void)
{
    IODIR0 |= 0xff << LCD_PINS | 1 << LCD_RS | 1 << LCD_ENB;
    IODIR1 |= 0xf << ROW_PINS | 1 << DEV0_PIN;
}

void INIT(void)
{
    IO_DIR();
    delay_init();
    INT0_CONF();
    LCD_INIT();
    rtc_init();
}

int main(void)
{
    INIT();

    /* Deterministic startup values for Proteus demonstration.
       12:00:00 is deliberately outside the default overnight schedule
       (20:35:00 -> 05:35:00), so startup is OFF as requested.
       The user can change all of these from the edit menu. */
    //SetRTCTimeInfo(12, 0, 0);
    //SetRTCDateInfo(16, 9, 2026);
    DOW = 0;                 /* SUN */

    DATE();
    Update_Shed();

    while(1)
    {
        Display();
        if(flage)
        {
            Flage_call();
        }
    }
}
