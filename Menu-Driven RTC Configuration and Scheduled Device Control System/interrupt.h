#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "config.h"

void INT_BUTTEN(void) __irq;
void INT0_CONF(void);
void Flage_call(void);
void DisMoveUp(void);
void DisMoveDw(void);

#endif // INTERRUPT_H
