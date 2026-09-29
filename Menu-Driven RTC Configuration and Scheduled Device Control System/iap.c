#include "iap.h"
#include "lcd.h"
#include "keypad.h"
#include "timer.h"

static void ShowScheduleValues(void)
{
    LCD_CMD(LCD_CLEAR);
    LCD_CMD(LINE_1);
    LCD_STR(RTC_SHED_START);
    LCD_CMD(LINE_2);
    LCD_STR(RTC_SHED_END);
}

void Edit_On_Time(void)
{
    u32 h = GET_TIME_VAL(RTC_SHED_START, 3, 4);
    u32 m = GET_TIME_VAL(RTC_SHED_START, 6, 7);
    u32 s = GET_TIME_VAL(RTC_SHED_START, 9, 10);

    h = ReadNumberField("ON HOUR (0-23)", 23, h);
    if(edit_timeout) return;
    m = ReadNumberField("ON MIN (0-59)", 59, m);
    if(edit_timeout) return;
    s = ReadNumberField("ON SEC (0-59)", 59, s);
    if(edit_timeout) return;

    RTC_SHED_START[3] = '0' + h / 10;
    RTC_SHED_START[4] = '0' + h % 10;
    RTC_SHED_START[6] = '0' + m / 10;
    RTC_SHED_START[7] = '0' + m % 10;
    RTC_SHED_START[9] = '0' + s / 10;
    RTC_SHED_START[10] = '0' + s % 10;

    ShowScheduleValues();
    delay_ms(300);
}

void Edit_Off_Time(void)
{
    u32 h = GET_TIME_VAL(RTC_SHED_END, 4, 5);
    u32 m = GET_TIME_VAL(RTC_SHED_END, 7, 8);
    u32 s = GET_TIME_VAL(RTC_SHED_END, 10, 11);

    h = ReadNumberField("OFF HOUR (0-23)", 23, h);
    if(edit_timeout) return;
    m = ReadNumberField("OFF MIN (0-59)", 59, m);
    if(edit_timeout) return;
    s = ReadNumberField("OFF SEC (0-59)", 59, s);
    if(edit_timeout) return;

    RTC_SHED_END[4] = '0' + h / 10;
    RTC_SHED_END[5] = '0' + h % 10;
    RTC_SHED_END[7] = '0' + m / 10;
    RTC_SHED_END[8] = '0' + m % 10;
    RTC_SHED_END[10] = '0' + s / 10;
    RTC_SHED_END[11] = '0' + s % 10;

    ShowScheduleValues();
    delay_ms(300);
}

/* Compatibility function: old code may still call this. */
void Edit_Sehd(void)
{
    Edit_On_Time();
    Edit_Off_Time();
}

void Edit_Shed_Time(u32 pos, u8 key)
{
    (void)pos;
    (void)key;
}

void Upload_shed(void)
{
    u32 Command[5], Result[3];
    IAP call_iap = (IAP)IAP_ADDR;

    strcpy(Data_Buffer, RTC_SHED_START);
    strcpy(Data_Buffer + 16, RTC_SHED_END);

    Command[0] = 50;
    Command[1] = Sector;
    Command[2] = Sector;
    call_iap(Command, Result);

    Command[0] = 52;
    Command[1] = Sector;
    Command[2] = Sector;
    Command[3] = CCLK_KHZ;
    __disable_irq();
    call_iap(Command, Result);
    __enable_irq();

    Command[0] = 50;
    Command[1] = Sector;
    Command[2] = Sector;
    call_iap(Command, Result);

    Command[0] = 51;
    Command[1] = Sector_Addr;
    Command[2] = (u32)Data_Buffer;
    Command[3] = 512;
    Command[4] = CCLK_KHZ;
    __disable_irq();
    call_iap(Command, Result);
    __enable_irq();
}

void Update_Shed(void)
{
    u8 *Data = (u8 *)Sector_Addr;
    u8 *EndData = Data + 16;

    if(Data[3] >= '0' && Data[3] <= '9' &&
       Data[4] >= '0' && Data[4] <= '9' &&
       Data[6] >= '0' && Data[6] <= '9' &&
       Data[7] >= '0' && Data[7] <= '9' &&
       Data[9] >= '0' && Data[9] <= '9' &&
       Data[10] >= '0' && Data[10] <= '9' &&
       ((Data[3]-'0')*10 + (Data[4]-'0')) <= 23 &&
       ((Data[6]-'0')*10 + (Data[7]-'0')) <= 59 &&
       ((Data[9]-'0')*10 + (Data[10]-'0')) <= 59)
    {
        RTC_SHED_START[3] = Data[3]; RTC_SHED_START[4] = Data[4];
        RTC_SHED_START[6] = Data[6]; RTC_SHED_START[7] = Data[7];
        RTC_SHED_START[9] = Data[9]; RTC_SHED_START[10] = Data[10];
    }

    if(EndData[4] >= '0' && EndData[4] <= '9' &&
       EndData[5] >= '0' && EndData[5] <= '9' &&
       EndData[7] >= '0' && EndData[7] <= '9' &&
       EndData[8] >= '0' && EndData[8] <= '9' &&
       EndData[10] >= '0' && EndData[10] <= '9' &&
       EndData[11] >= '0' && EndData[11] <= '9' &&
       ((EndData[4]-'0')*10 + (EndData[5]-'0')) <= 23 &&
       ((EndData[7]-'0')*10 + (EndData[8]-'0')) <= 59 &&
       ((EndData[10]-'0')*10 + (EndData[11]-'0')) <= 59)
    {
        RTC_SHED_END[4] = EndData[4]; RTC_SHED_END[5] = EndData[5];
        RTC_SHED_END[7] = EndData[7]; RTC_SHED_END[8] = EndData[8];
        RTC_SHED_END[10] = EndData[10]; RTC_SHED_END[11] = EndData[11];
    }
}
