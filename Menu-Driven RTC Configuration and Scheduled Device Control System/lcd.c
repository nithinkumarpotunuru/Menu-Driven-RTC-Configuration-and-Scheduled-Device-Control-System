#include "lcd.h"
#include "timer.h"

void LCD_INIT(void)
{
    delay_ms(15);
    LCD_CMD(FUN_SET);
    delay_ms(5);
    LCD_CMD(FUN_SET);
    delay_ms(1);
    LCD_CMD(FUN_SET);
    LCD_CMD(LCD_2L);
    LCD_CMD(LCD_CLEAR);
    LCD_CMD(CUR_SHIFT);
    LCD_CMD(DISP_ON);
}

void LCD_CMD(u8 cmd)
{
    IOCLR0 = 0xff << LCD_PINS;
    IOSET0 = cmd << LCD_PINS;
    IOCLR0 = 1 << LCD_RS;
    IOSET0 = 1 << LCD_ENB;
    delay_ms(1);
    IOCLR0 = 1 << LCD_ENB;
}

void LCD_DETA(u8 data)
{
    IOCLR0 = 0xff << LCD_PINS;
    IOSET0 = data << LCD_PINS;
    IOSET0 = 1 << LCD_RS;
    IOSET0 = 1 << LCD_ENB;
    delay_ms(1);
    IOCLR0 = 1 << LCD_ENB;
}

void CGRAM_INIT(void)
{
    u32 ch = 0;
    u8 *p = CGRAM_SPC;
    LCD_CMD(CGRAM);
    while(ch++ < 5)
    {
        while(*p)
        {
            LCD_DETA(*p++);
        }
        LCD_DETA(DISP_ON);
        p++;
    }
}

void LCD_STR(u8 *deta_str)
{
    while(*deta_str)
    {
        LCD_DETA(*deta_str);
        deta_str++;
    }
}

void LCD_INT(u32 num)
{
    int pos = 0;
    u8 itoa[20];
    while(num)
    {
        itoa[pos++] = num % 10 + '0';
        num /= 10;
    }
    itoa[pos] = '\0';
    
    while(itoa[pos] != itoa[0])
    {
        pos--;
        LCD_DETA(itoa[pos]);
    }
}

void LCD_FLOAT(f32 fnum)
{
}
