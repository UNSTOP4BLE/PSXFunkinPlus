#include "event.h"
#include "../scenes/stage/stage.h"

#include "../audio.h"
#include "../psx/mem.h"
#include "../psx/timer.h"
#include "../psx/pad.h"
#include "../psx/random.h"
#include "../psx/movie.h"
#include "../psx/mutil.h"

Event event;

void Load_Events()
{
	event.zoom = FIXED_DEC(1,1);
	event.speed = FIXED_DEC(5,100);
	event.shake = 0;
	event.flash = 0;
	event.fade = 0;
	event.fadebool = false;
	event.movieview = false;
	event.fadebehind = false;
}

void Events()
{
	event.shake = lerp(event.shake,0,FIXED_DEC(2,10));
	//FntPrint("steps: %d", stage.song_step);
	//if(stage.prefs.followcamera)
	//	FollowCharCamera();
}

void Events_Front()
{
	
}

void Events_Back()
{
	
}


void FollowCharCamera()
{
	u8 sensitivity = 2;
	if (stage.cur_section->flag & SECTION_FLAG_OPPFOCUS)
	{
		if (stage.opponent->animatable.anim == CharAnim_Up)
			stage.camera.y -= FIXED_DEC(sensitivity,10);
		if (stage.opponent->animatable.anim == CharAnim_Down)
			stage.camera.y += FIXED_DEC(sensitivity,10);
		if (stage.opponent->animatable.anim == CharAnim_Left)
			stage.camera.x -= FIXED_DEC(sensitivity,10);
		if (stage.opponent->animatable.anim == CharAnim_Right)
			stage.camera.x += FIXED_DEC(sensitivity,10);
	}
	else
	{
		if (stage.player->animatable.anim == CharAnim_Up)
			stage.camera.y -= FIXED_DEC(sensitivity,10);
		if (stage.player->animatable.anim == CharAnim_Down)
			stage.camera.y += FIXED_DEC(sensitivity,10);
		if (stage.player->animatable.anim == CharAnim_Left)
			stage.camera.x -= FIXED_DEC(sensitivity,10);
		if (stage.player->animatable.anim == CharAnim_Right)
			stage.camera.x += FIXED_DEC(sensitivity,10);
	}
}

void NoteHitEvent(u8 type)
{
	
}

void NoteHitEnemyEvent(u8 type)
{
	
}

void NoteMissEvent(u8 type, u8 state)
{
	PlayerState *this = &stage.player_state[state];
	if (type & NOTE_FLAG_BULLET)
		this->health = -0x7000;
}

