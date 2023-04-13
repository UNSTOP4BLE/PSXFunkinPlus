#include "stage.h"
#include "../../psx/trans.h"
#include "../../audio.h"
void PausedState()
{
	static const char *stage_options[] = {
		"RESUME",
		"RESTART SONG",
		"EXIT TO MENU"
	};

	//Select option if cross or start is pressed
	if (pad_state.press & (PAD_CROSS | PAD_START))
	{
		switch (stage.pause_select)
		{
			case 0: //Resume
				Audio_ResumeXA();
				stage.paused = false;
					break;
			case 1: //Retry
				stage.trans = StageTrans_Reload;
				Trans_Start();
				break;
			case 2: //Quit
				stage.trans = StageTrans_Menu;
				Trans_Start();
				break;
		}
	}

	//Change option
	if (pad_state.press & PAD_UP)
	{
		if (stage.pause_select > 0)
			stage.pause_select--;
		else
			stage.pause_select = COUNT_OF(stage_options) - 1;
	}
	if (pad_state.press & PAD_DOWN)
	{
		if (stage.pause_select < COUNT_OF(stage_options) - 1)
			stage.pause_select++;
		else
			stage.pause_select = 0;
	}

	//draw options
	if (stage.pause_scroll == -1)
		stage.pause_scroll = COUNT_OF(stage_options) * FIXED_DEC(33,1);

	s32 next_scroll = stage.pause_select * FIXED_DEC(33,1);
	stage.pause_scroll += (next_scroll - stage.pause_scroll) >> 3;

	for (u8 i = 0; i < COUNT_OF(stage_options); i++)
	{
		//get position on screen
		s32 y = (i * 33) - 8 - (stage.pause_scroll >> FIXED_SHIFT);
		if (y <= -SCREEN_HEIGHT2 - 8)
			continue;
		if (y >= SCREEN_HEIGHT2 + 8)
			break;

		//draw text
		stage.font_bold.draw_col(&stage.font_bold,
		stage_options[i],
		SCREEN_WIDTH2,
		y + 120,
		FontAlign_Center,
		//if the option is the one you are selecting, draw in normal color, else, draw gray
		(i == stage.pause_select) ? 128 : 100,
		(i == stage.pause_select) ? 128 : 100,
		(i == stage.pause_select) ? 128 : 100
		);
	}
	//pog blend
	RECT scr = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
	Gfx_BlendRect(&scr, 0, 0, 0, 0);
}