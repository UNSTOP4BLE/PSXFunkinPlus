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
	fixed_t shake;
} Event;

extern Event event;

void FollowCharCamera();
void Events();
void NoteHitEvent(u8 type);
void NoteHitEnemyEvent(u8 type);
void NoteMissEvent(u8 type, u8 state);

#endif