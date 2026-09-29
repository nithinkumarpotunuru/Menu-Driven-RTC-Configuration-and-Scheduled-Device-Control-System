#ifndef LCD_H
#define LCD_H

#include "config.h"

// LCD Command Definitions
#define FUN_SET 0x30
#define LCD_2L 0x38
#define LCD_CLEAR 0x1
#define DISP_ON 0xC
#define DISP_CUR_ON 0xE
#define DISP_CUR_BLINK_ON 0xF
#define LINE_1 0x80
#define LINE_2 0xC0
#define CUR_SHIFT 0x6
#define CGRAM 0x40

void LCD_INIT(void);
void LCD_CMD(u8 cmd);
void LCD_DETA(u8 data);
void LCD_STR(u8 *deta_str);
void LCD_INT(u32 num);
void CGRAM_INIT(void);

#endif // LCD_H


