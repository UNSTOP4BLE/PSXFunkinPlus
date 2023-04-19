#ifndef PSXF_GUARD_DEBUG_H
#define PSXF_GUARD_DEBUG_H

typedef struct
{
	int debugx;
	int debugy;
	int debugw;
	int debugh;
	int selection;
	int switchcooldown;
	int mode;

	char debugtext[200];
} Debug;

extern Debug debug;

void Debug_StageDebug();
void Debug_StageMoveDebug(RECT_FIXED *dst, int dacase, fixed_t fx, fixed_t fy);
void Debug_GfxMoveDebug(RECT *dst, int dacase);
void Debug_ScrollCamera(void);

#endif
