#include "loadscr.h"

#include "gfx.h"
#include "timer.h"
#include "io.h"
#include "../audio.h"
#include "trans.h"

//Loading screen functions
void LoadScr_Start(void)
{
	//Stop music and make sure frame has been drawn
	Gfx_Flip();
	Timer_Reset();
}

void LoadScr_End(void)
{
	//Handle transition out
	Timer_Reset();
	Trans_Clear();
	Trans_Start();
	Gfx_DisableClear();
	while (!Trans_Tick())
	{
		Timer_Tick();
		Gfx_Flip();
	}
	Gfx_EnableClear();
}
