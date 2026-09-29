#ifndef RTC_H
#define RTC_H

#include "config.h"

void rtc_init(void);
void SetRTCTimeInfo(u32 hour, u32 minute, u32 second);
void SetRTCDateInfo(u32 date, u32 month, u32 year);
void SetRTCDayInfo(u32 day);
void Display(void);
void TIME(void);
void DAY(void);
void DATE(void);
void Time_set(u32 pos, u8 k_value);
void Date_set(u32 pos, u8 k_value);
void Edit_Time(void);
void Edit_Time_Hour(void);
void Edit_Time_Min(void);
void Edit_Time_Sec(void);
void Edit_Date(void);
void Edit_Date_Day(void);
void Edit_Date_Month(void);
void Edit_Date_Year(void);
void Update_Time(void);
void Update_Date(void);

#endif // RTC_H
