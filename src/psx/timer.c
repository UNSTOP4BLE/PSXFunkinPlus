#include "timer.h"
#include "../scenes/stage/stage.h"
#include "../fonts/font.h"

#define TIMER_BITS (3)

//Timer state
Timer timer;
u32 frame_count, animf_count;
u32 timer_count, timer_lcount, timer_countbase;
u32 timer_persec;

fixed_t timer_sec, timer_dt, timer_secbase;

u8 timer_brokeconf;

//Timer interface
extern void InterruptCallback(int index, void *cb);
extern void ChangeClearRCnt(int t, int m);

void Timer_Callback(void) {
	timer_count++;
}

void Timer_Init(boolean pal_console, boolean pal_video)
{
	//Initialize counters
	frame_count = animf_count = timer_count = timer_lcount = timer_countbase = 0;
	timer_sec = timer_dt = timer_secbase = 0;
	
	//Setup counter IRQ
	static const u32 hblanks_per_sec[4] = {
		15734 >> TIMER_BITS, //!console && !video => 262.5 * 59.940
		15591 >> TIMER_BITS, //!console &&  video => 262.5 * 59.393
		15769 >> TIMER_BITS, //console && !video => 312.5 * 50.460
		15625 >> TIMER_BITS, //console &&  video => 312.5 * 50.000
	};
	timer_persec = hblanks_per_sec[(pal_console << 1) | pal_video];
	
	EnterCriticalSection();
	
	SetRCnt(RCntCNT1, 1 << TIMER_BITS, RCntMdINTR);
	InterruptCallback(5, (void*)Timer_Callback); //IRQ5 is RCNT1
	StartRCnt(RCntCNT1);
	ChangeClearRCnt(1, 0);
	
	ExitCriticalSection();
	
	timer_brokeconf = 0;
}

//Increment the frame count and update the counter time
void Timer_Tick(void)
{
	u32 status = *((volatile const u32*)0x1f801814);

	//Increment frame count
	frame_count++;

	//Update counter time
	if (timer_count == timer_lcount)
	{
		if (timer_brokeconf != 0xFF)
			timer_brokeconf++;
		if (timer_brokeconf >= 10)
		{
			//Update timer count based on status value
			if ((status & 0x00100000) != 0)
				timer_count += timer_persec / 50;
			else
				timer_count += timer_persec / 60;
		}
	}
	else
	{
		if (timer_brokeconf != 0)
			timer_brokeconf--;
	}

	//Update timer_sec and animf_count
	if (timer_count < timer_lcount)
	{
		timer_secbase = timer_sec;
		timer_countbase = timer_lcount;
	}
	fixed_t next_sec = timer_secbase + FIXED_DIV(timer_count - timer_countbase, timer_persec);
	timer_dt = next_sec - timer_sec;
	timer_sec = next_sec;
	animf_count = (timer_sec * 24) >> FIXED_SHIFT;
	timer_lcount = timer_count;
}

//Reset the timer and timer_dt
void Timer_Reset(void)
{
	Timer_Tick();
	timer_dt = 0;
}

//Update the remaining time for the stage timer
void StageTimer_Tick()
{
	int remaining_time = (Audio_GetLength(stage.stage_def->music_track) - stage.song_time / 1000);
	if (remaining_time < (Audio_GetLength(stage.stage_def->music_track) + 1))
		timer.timer = remaining_time;
}

//Draw the stage timer and length
void StageTimer_Draw()
{
	s8 timerdata[2] = {
		timer.timer / ((stage.prefs.palmode) ? 50 : 60),
		timer.timer % ((stage.prefs.palmode) ? 50 : 60)
	};

	//Format the timer display
	if (timerdata[1] >= 10)
		sprintf(timer.timer_display, "%d:%d", timerdata[0], timerdata[1]);
	else
		sprintf(timer.timer_display, "%d:0%d", timerdata[0], (timerdata[1] > 0 ? timerdata[1] : 0));

	//Draw the timer
	fonts.font_cdr.draw(&fonts.font_cdr,
		timer.timer_display,
		FIXED_DEC(2, 1),
		FIXED_DEC((stage.prefs.downscroll && !(stage.prefs.mode >= StageMode_2P)) ? 100 : -109, 1),
		FontAlign_Center
	);

	//Draw the length
	const RECT texture_src = { 2, 230, 2, 2 };
	RECT_FIXED back_dst = {
		FIXED_DEC(-46, 1),
		FIXED_DEC(-108, 1),
		FIXED_DEC(92, 1),
		FIXED_DEC(8, 1)
	};
	RECT_FIXED front_dst = {
		FIXED_DEC(-44, 1),
		FIXED_DEC(-106, 1),
		((88 * stage.song_time) / (Audio_GetLength(stage.stage_def->music_track) * 1024)) << FIXED_SHIFT,
		FIXED_DEC(4, 1)
	};

	//Adjust the coordinates for downscroll mode
	if (stage.prefs.downscroll && !(stage.prefs.mode >= StageMode_2P))
	{
		back_dst.y = -back_dst.y - back_dst.h + FIXED_DEC(1, 1);
		front_dst.y = -front_dst.y - front_dst.h + FIXED_DEC(1, 1);
	}

	//Draw the front and back length indicators
	if (stage.song_step >= 0)
	{
		Stage_DrawTexCol(&stage.tex_hud0, &texture_src, &front_dst, stage.bump, stage.camera.hudangle, 255, 255, 255);
	}
	Stage_DrawTexCol(&stage.tex_hud0, &texture_src, &back_dst, stage.bump, stage.camera.hudangle, 0, 0, 0);
}
