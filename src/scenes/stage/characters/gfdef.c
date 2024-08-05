#include "gfdef.h"
#include "../stage.h"

Speaker speaker; //sorry about global vars

//hardcoding cus im stupdi
void GirlFriend_Generic_Tick(Character *character)
{
    if (stage.flag & STAGE_FLAG_JUST_STEP)
    {
        //Stage specific animations
        if (stage.note_scroll >= 0)
        {
            switch (stage.stage_id)
            {
                case StageId_1_4: //Tutorial cheer
                    if (stage.song_step > 64 && stage.song_step < 192 && (stage.song_step & 0x3F) == 60)
                        character->set_anim(character, CharAnim_UpAlt);
                    break;
                default:
                    break;
            }
        }
           
        //Perform dance
        if (stage.note_scroll >= character->sing_end && (stage.song_step % stage.gf_speed) == 0)
        {
            //Switch animation
            if (character->animatable.anim == CharAnim_LeftAlt || character->animatable.anim == CharAnim_Right)
                character->set_anim(character, CharAnim_RightAlt);
            else
                character->set_anim(character, CharAnim_LeftAlt);
            
            //Bump speakers
            Speaker_Bump(&speaker);
        }
    }

	//Get parallax
	fixed_t parallax;
	parallax = FIXED_UNIT;
    
    //Animate and draw
    Animatable_Animate(&character->animatable, (void*)character, Char_SetFrame);
    Character_DrawParallax(character, &character->tex, &character->frames[character->frame], parallax);
    
    //Tick speakers
    Speaker_Tick(&speaker, character->x, character->y, parallax);
}

void GirlFriend_Generic_SetAnim(Character *character, u8 anim)
{
    if (anim == CharAnim_Left || anim == CharAnim_Down || anim == CharAnim_Up || anim == CharAnim_Right || anim == CharAnim_UpAlt)
        character->sing_end = stage.note_scroll + FIXED_DEC(22,1); //Nearly 2 steps
    Animatable_SetAnim(&character->animatable, anim);
}