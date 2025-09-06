#ifndef __BLDC_H
#define __BLDC_H

#include "main.h"
#include "../motor.h"

void BLDC_Control(u32 halls, bool direction, bool isStop);

#endif /* __BLDC_H */