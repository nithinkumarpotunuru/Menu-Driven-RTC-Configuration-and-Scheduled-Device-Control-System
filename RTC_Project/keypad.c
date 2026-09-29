#include "keypad.h"
#include "timer.h"
#include "lcd.h"

u32 edit_timeout = 0;

char key_scan(void)
{
    int col, row;
    while(((IOPIN1 >> COL_PINS) & 15) != 15)
    {
        col = col_scan();
        row = row_scan();
        delay_ms(100);

        while(((IOPIN1 >> COL_PINS) & 15) != 15);
        return KPM[row][col];
    }
    return 0;
}

int col_scan(void)
{
    int inc = 0;
    while(((IOPIN1 >> (COL_PINS + inc)) & 1) == 1)
    {
        inc++;
    }
    return inc;
}

int row_scan(void)
{
    int inc = -1;
    while(((IOPIN1 >> COL_PINS) & 15) != 15)
    {
        inc++;
        IOPIN1 = 1 << (ROW_PINS + inc);
    }
    IOPIN1 &= ~(0xf << ROW_PINS);
    return inc;
}

/*
 * Wait for a key for at most 5 seconds.  This is deliberately non-blocking
 * with respect to key_scan(): key_scan() is called only after a key is seen.
 */
static char EditWaitKey5s(void)
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

/*
 * Numeric entry helper.
 *
 * A field has a 5-second inactivity timeout.  If no key is pressed for
 * 5 seconds, edit_timeout is set and the caller must leave the complete
 * edit mode and return to the normal RTC display.
 *
 * C = backspace, = = accept.
 */
static u32 ReadNumberInternal(u8 *label, u32 max_value, u32 current_value, u8 digits)
{
    u32 value;
    u8 count;
    char key;

    edit_timeout = 0;

    while(1)
    {
        value = 0;
        count = 0;

        LCD_CMD(LCD_CLEAR);
        LCD_CMD(LINE_1);
        LCD_STR(label);
        LCD_CMD(LINE_2);
        LCD_STR("NOW:");

        if(digits == 4)
        {
            LCD_DETA((u8)(((current_value / 1000) % 10) + '0'));
            LCD_DETA((u8)(((current_value / 100) % 10) + '0'));
            LCD_DETA((u8)(((current_value / 10) % 10) + '0'));
            LCD_DETA((u8)((current_value % 10) + '0'));
        }
        else
        {
            LCD_DETA((u8)(((current_value / 10) % 10) + '0'));
            LCD_DETA((u8)((current_value % 10) + '0'));
        }
      
        LCD_STR(" S:");
        LCD_CMD(DISP_CUR_BLINK_ON);

        while(1)
        {
            key = EditWaitKey5s();

            if(key == 0)
            {
                LCD_CMD(DISP_ON);
                edit_timeout = 1;
                return current_value;
            }

            if(key >= '0' && key <= '9')
            {
                if(count < digits)
                {
                    value = (value * 10) + (key - '0');
                    count++;
                    LCD_DETA((u8)key);
                }
            }
            else if(key == '=')
            {
                LCD_CMD(DISP_ON);

                if(count == 0 || value > max_value)
                {
                    LCD_CMD(LCD_CLEAR);
                    LCD_CMD(LINE_1);
                    LCD_STR(" INVALID VALUE ");
                    LCD_CMD(LINE_2);
                    LCD_STR("   TRY AGAIN!   ");
                    delay_ms(1000);
                    break;
                }
                return value;
            }
            else if(key == 'c')
            {
                /* C = BACKSPACE: remove only the last entered digit. */
                if(count > 0)
                {
                    value /= 10;
                    count--;
                    LCD_CMD(0x10);
                    LCD_DETA(' ');
                    LCD_CMD(0x10);
                }
            }
        }
    }
}

u32 ReadNumberField(u8 *label, u32 max_value, u32 current_value)
{
    return ReadNumberInternal(label, max_value, current_value, 2);
}

u32 Read4NumberField(u8 *label, u32 max_value, u32 current_value)
{
    return ReadNumberInternal(label, max_value, current_value, 4);
}
