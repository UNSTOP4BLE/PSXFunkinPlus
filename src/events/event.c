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
	FollowCharCamera();
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
    fixed_t camera_speed = FIXED_DEC(sensitivity, 10);
    fixed_t char_dx = 0;
    fixed_t char_dy = 0;
    
    if (stage.cur_section->flag & SECTION_FLAG_OPPFOCUS)
    {
        switch (stage.opponent->animatable.anim)
        {
            case CharAnim_Up: char_dy = -camera_speed; break;
            case CharAnim_Down: char_dy = camera_speed; break;
            case CharAnim_Left: char_dx = -camera_speed; break;
            case CharAnim_Right: char_dx = camera_speed; break;
        }
    }
    else
    {
        switch (stage.player->animatable.anim)
        {
            case CharAnim_Up: char_dy = -camera_speed; break;
            case CharAnim_Down: char_dy = camera_speed; break;
            case CharAnim_Left: char_dx = -camera_speed; break;
            case CharAnim_Right: char_dx = camera_speed; break;
        }
    }
    
	if (!stage.paused)
	{
		stage.camera.x += char_dx;
		stage.camera.y += char_dy;
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

