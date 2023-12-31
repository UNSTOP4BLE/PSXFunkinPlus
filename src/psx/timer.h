#ifndef PSXF_GUARD_TIMER_H
#define PSXF_GUARD_TIMER_H

#include "psx.h"
#include "fixed.h"

//Timer state
extern u32 frame_count, animf_count;
extern fixed_t timer_sec, timer_dt;
extern u8 frame_rate;

typedef struct
{
	int timer;
	char timer_display[5];
} Timer;

extern Timer timer;

//Timer interface 
void Timer_Init(void);
void Timer_Tick(void);
void Timer_Reset(void);
void StageTimer_Tick();
void StageTimer_Draw();

#endif