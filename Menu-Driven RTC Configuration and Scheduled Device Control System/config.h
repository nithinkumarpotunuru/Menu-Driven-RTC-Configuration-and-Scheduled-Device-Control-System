#ifndef CONFIG_H
#define CONFIG_H

#include <lpc21xx.h>
#include <string.h>

// Clock Configurations
#define FCLK 12000000        // Crystal oscillator frequency (12 MHz)
#define CCLK (5*FCLK)        // Core clock frequency via PLL (60 MHz)
#define PCLK (CCLK/4)       // Peripheral clock frequency (15 MHz)

// RTC Clock Prescaler Definitions
#define PREINT_VAL ((PCLK/32768)-1)
#define PREFRAC_VAL (PCLK-((PREINT_VAL+1)*32768))

// Hardware Pin Maps
#define LCD_PINS 8       // P0.8 - P0.15 for LCD Data bus
#define LCD_RS 17        // P0.17 connected to RS
#define LCD_ENB 18       // P0.18 connected to EN

#define ROW_PINS 16      // P1.16 - P1.19 connected to Keypad Rows
#define COL_PINS 20      // P1.20 - P1.23 connected to Keypad Columns

#define ENT0_CHAN 14     // EINT0 VIC Channel
#define DEV0_PIN 30      // P1.30 connected to Relay/Device pin

#define GET_TIME_VAL(arr, i, j) (((arr)[i]-'0')*10 + ((arr)[j]-'0'))

// Flash In-Application Programming (IAP) Definitions
#define IAP_ADDR 0x7ffffff1
#define Sector 7
#define Sector_Addr 0x00007000
#define CCLK_KHZ 60000

// Type Aliases
typedef char u8;
typedef unsigned int u32;
typedef float f32;

typedef void (*IAP)(u32 [], u32 []);

// Shared Global Variables
extern char Data_Buffer[512];
extern u8 Time[];
extern u8 Date[];
extern u8 KPM[4][4];
extern u8 MENU[4][15];
extern u8 RTC_SHED_START[];
extern u8 RTC_SHED_END[];
extern u8 CGRAM_SPC[];
extern u32 flage;
extern u32 display_refresh;

void IO_DIR(void);

#endif // CONFIG_H
