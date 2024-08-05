#include "chardef.h"
#include "../stage.h"
#include "../../../psx/mutil.h"

void Char_Generic_Tick(Character *character)
{
    switch (character->spec) {
        default:
            if ((character->pad_held & (stage.prefs.control_keys[0] | stage.prefs.control_keys[1] | stage.prefs.control_keys[2] | stage.prefs.control_keys[3])) == 0)
                Character_PerformIdle(character);
            break;
    }   
    
    //Animate and draw
    Animatable_Animate(&character->animatable, (void*)character, Char_SetFrame);
    Character_Draw(character, &character->tex, &character->frames[character->frame]);
}

void Char_Generic_SetAnim(Character *character, u8 anim)
{
    switch (character->spec) {
        default:
            Animatable_SetAnim(&character->animatable, anim);
            Character_CheckStartSing(character);
            break;
    }
}
