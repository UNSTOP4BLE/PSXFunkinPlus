#include "stage.h"
#include "gameover.h"
#include "../../fonts/font.h"

#include "../../psx/io.h"
#include "../../psx/gfx.h"

#include "../../psx/mem.h"
#include "../../psx/archive.h"
#include "../../psx/fixed.h"
#include "../../main.h"
#include "../../audio.h"

//Retry state
Retry retry;

typedef enum
{
	NONE,
	CHANGE_FRAME,
	LOAD_GFX,
	
	Retry_Type_Max
} Retry_Type;

const char *blueball_textures[] = {
	"dead0.tim",
	"dead1.tim",
	"dead2.tim",
	NULL,
};

const u8 blueball_frames[][3] = {
	{0, LOAD_GFX, 0},
    {4, CHANGE_FRAME, 1},
    {6, CHANGE_FRAME, 2},
    {8, CHANGE_FRAME, 3},
    {56, CHANGE_FRAME, 0},
	{56, LOAD_GFX, 1},
    {60, CHANGE_FRAME, 1},
    {62, CHANGE_FRAME, 2},
    {64, CHANGE_FRAME, 3},
	{66, LOAD_GFX, 2},
	{66, CHANGE_FRAME, 0},
    {128, 0, 128}
};

static void Draw_BlueBall(void) {
	while (retry.offset < sizeof(blueball_frames) / sizeof(blueball_frames[0]) &&
		retry.timer > blueball_frames[retry.offset][0]) {
		if (blueball_frames[retry.offset][1] == CHANGE_FRAME) {
			retry.frame = blueball_frames[retry.offset][2];
		}
		if (blueball_frames[retry.offset][1] == LOAD_GFX) {
			Gfx_LoadTex(&retry.tex, Archive_Find(retry.arc, blueball_textures[blueball_frames[retry.offset][2]]), 0);
		}

		retry.offset++;
	}

    RECT blueball_src = {
		(retry.frame % 2) * 128,
		((retry.frame / 2) % 2) * 128,
		128,
		128
	};

    RECT_FIXED blueball_dst = {
        stage.player->x - stage.camera.x,
        stage.player->y - stage.camera.y,
        FIXED_DEC(blueball_src.w, 1),
        FIXED_DEC(blueball_src.h, 1)
    };

    Stage_DrawTex(&retry.tex, &blueball_src, &blueball_dst, stage.camera.bzoom, stage.camera.angle);
}


void Stage_Retry_Tick(void)
{
	if (false) //play music
	{
		Audio_PlayXA_Track(XA_GameOver, 0x40, 1, true);
	}
	
	retry.timer++;
	Draw_BlueBall();
}

void Stage_Retry_Reset(void)
{
	retry.frame = 0;
	retry.timer = 0;
	retry.offset = 0;
}

void Stage_Retry_Load(u8 offset)
{
   	retry.arc = IO_Read(gameover_paths[offset]);
}

void Stage_Retry_Unload()
{
   	Mem_Free(retry.arc);
}
