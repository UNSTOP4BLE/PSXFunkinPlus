#ifndef PSXF_GUARD_GAMEOVER_H
#define PSXF_GUARD_GAMEOVER_H

static const char *gameover_paths[] = {
	"\\DEATHS\\BF.ARC;1",   //Boyfriend
	NULL,
};

typedef struct
{
	u8 retry_bump, retry_visibility, retry_fade;
	RECT retry_src;
	RECT_FIXED retry_dst;
	boolean retry;
} Retry;

void Stage_Retry_Tick(void);

#endif
