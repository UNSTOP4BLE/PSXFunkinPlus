#ifndef PSXF_GUARD_CHARACTER_H
#define PSXF_GUARD_CHARACTER_H

#include "../../psx/io.h"
#include "../../psx/gfx.h"

#include "../../psx/fixed.h"
#include "animation.h"

//Character specs
typedef u8 CharSpec;
#define CHAR_SPEC_MISSANIM   (1 << 0) //Has miss animations
#define CHAR_SPEC_GIRLFRIEND (2 << 0) //Has gf animations

//Character enums
typedef enum
{
	CharAnim_Idle,
	CharAnim_Left,  CharAnim_LeftAlt,
	CharAnim_Down,  CharAnim_DownAlt,
	CharAnim_Up,    CharAnim_UpAlt,
	CharAnim_Right, CharAnim_RightAlt,
	
	CharAnim_Max //Max standard/shared animation
} CharAnim;

//Character structures
typedef struct
{
	u8 tex;
	u16 src[4];
	s16 off[2];
} CharFrame;

typedef struct Character
{
	//Character functions
	void (*tick)(struct Character*);
	void (*set_anim)(struct Character*, u8);
	
	//Position
	fixed_t x, y;
	
	//Character information
	CharSpec spec;
    u16 health_i[2][4];
	u32 health_bar;
	fixed_t focus_x, focus_y, focus_zoom;
	
	fixed_t scale;
	
	//Animation state
    const CharFrame *frames;
	Animatable animatable;
	fixed_t sing_end;
	u16 pad_held;
    u8 *file;

    //Render data and state
    IO_Data arc_main;
    IO_Data *arc_ptr;

    Gfx_Tex tex;
    u8 frame, tex_id;
} Character;

typedef struct __attribute__((packed)) CharacterFileHeader
{
    s32 size_struct;
    s32 size_frames;
    s32 size_animation;
    s32 sizes_scripts[32]; 
    s32 size_textures;

    //Character information
    u16 spec;
    u16 health_i[2][4]; //hud1.tim
    u32 health_bar; //hud1.tim
    char archive_path[128];
    s32 focus_x, focus_y, focus_zoom;
    s32 scale;
} CharacterFileHeader;

//Character File functions
void Char_SetFrame(void *user, u8 frame);
Character *Character_FromFile(Character *this, const char *path, fixed_t x, fixed_t y);

//Character functions
void Character_Free(Character *this);
void Character_Init(Character *this, fixed_t x, fixed_t y);
void Character_DrawParallax(Character *this, Gfx_Tex *tex, const CharFrame *cframe, fixed_t parallax);
void Character_Draw(Character *this, Gfx_Tex *tex, const CharFrame *cframe);

void Character_CheckStartSing(Character *this);
void Character_CheckEndSing(Character *this);
void Character_PerformIdle(Character *this);

#endif