#ifndef PSXF_GUARD_GAMEOVER_H
#define PSXF_GUARD_GAMEOVER_H

#include "../../psx/io.h"
#include "../../psx/gfx.h"

#include "../../psx/fixed.h"

static const char *gameover_paths[] = {
	"\\DEATHS\\BLUEBALL.ARC;1",   //Boyfriend
	NULL,
};

typedef struct 
{
	u8 type;
	fixed_t time;
	u32 value;
} EventTrack;

typedef struct
{
	u16 timer, offset;
	u8 frame;
	boolean retry;
	
	Gfx_Tex tex;
	IO_Data arc;
} Retry;

void Stage_Retry_Tick(void);
void Stage_Retry_Reset(void);

void Stage_Retry_Load(u8 offset);
void Stage_Retry_Unload(void);

#endif
