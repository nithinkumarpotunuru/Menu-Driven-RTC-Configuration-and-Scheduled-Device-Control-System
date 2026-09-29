#ifndef KEYPAD_H
#define KEYPAD_H

#include "config.h"

char key_scan(void);
int col_scan(void);
int row_scan(void);

extern u32 edit_timeout;

u32 ReadNumberField(u8 *label, u32 max_value, u32 current_value);
u32 Read4NumberField(u8 *label, u32 max_value, u32 current_value);

#endif // KEYPAD_H
