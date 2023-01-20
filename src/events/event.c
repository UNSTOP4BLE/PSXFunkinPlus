#include "event.h"
#include "../scenes/stage/stage.h"

#include "../audio.h"
#include "../psx/mem.h"
#include "../psx/timer.h"
#include "../psx/pad.h"
#include "../psx/random.h"
#include "../psx/movie.h"
#include "../psx/mutil.h"


#include "../characters/benson/benson.h"

Event event;

static void Unemployed()
{
	if (stage.song_step == 250)
		benson.angry = true;
}

void Events()
{
	event.shake = lerp(event.shake,0,FIXED_DEC(2,10));
	//FntPrint("steps: %d", stage.song_step);
	if(stage.prefs.followcamera)
		FollowCharCamera();
	if (stage.stage_id == StageId_1_3)
		Unemployed();
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
	stage.bump += FIXED_DEC(10,1000);
	if (type & NOTE_FLAG_BULLET)
	{
		stage.camera.zoom += FIXED_DEC(200,1000);
		stage.bump += FIXED_DEC(100,1000);
	}
}

void NoteHitEnemyEvent(u8 type)
{
	if (type & NOTE_FLAG_BULLET)
	{
		stage.bump += FIXED_DEC(100,1000);
	}
	if (stage.stage_id == StageId_1_6)
		event.shake = 1000;
}

void NoteMissEvent(u8 type, u8 state)
{
	PlayerState *this = &stage.player_state[state];
	if (type & NOTE_FLAG_BULLET)
		this->health = -0x7000;
}

