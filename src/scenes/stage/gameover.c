#include "stage.h"
#include "gameover.h"
#include "../../fonts/font.h"

#include "../../main.h"
#include "../../audio.h"

//Retry state
Retry retry;

void Stage_Retry_Tick(void)
{
	u8 imfunny;
	if (false) //play music
	{
		Audio_PlayXA_Track(XA_GameOver, 0x40, 1, true);
	}
}
