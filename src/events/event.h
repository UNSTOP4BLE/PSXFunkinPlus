#ifndef PSXF_GUARD_EVENT_H
#define PSXF_GUARD_EVENT_H

#include "../psx/io.h"
#include "../psx/gfx.h"
#include "../psx/pad.h"

#include "../psx/fixed.h"
#include "../scenes/stage/character.h"
#include "../scenes/stage/player.h"
#include "../scenes/stage/object.h"
#include "../fonts/font.h"

typedef struct
{
	fixed_t zoom, speed;
	boolean hidehud;
	
	fixed_t shake;
	
	u16 flash, whiteflash, fade;
	boolean fadebool;
	boolean fadebehind;
	
	boolean movieview;
} Event;

extern Event event;

void FollowCharCamera();
void Load_Events();
void Events_Front();
void Events();
void Events_Back();
void NoteHitEvent(u8 type);
void NoteHitEnemyEvent(u8 type);
void NoteMissEvent(u8 type, u8 state);

#endif