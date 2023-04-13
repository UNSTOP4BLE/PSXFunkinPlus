#include "gfg.h"

#include "../../psx/mem.h"
#include "../../psx/archive.h"
#include "../../scenes/stage/stage.h"
#include "../../main.h"

//GFG character structure
enum
{
	GFG_ArcMain_gf0,
	GFG_ArcMain_gf1,
	GFG_ArcMain_gf2,

	GFG_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[GFG_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_GFG;

//GFG character definitions
static const CharFrame char_gfg_frame[] = {
	{GFG_ArcMain_gf0,{0,0,73,102},{33,72}}, //0 Left
	{GFG_ArcMain_gf0,{74,0,73,102},{33,71}}, //1 Left
	{GFG_ArcMain_gf0,{148,0,73,102},{32,72}}, //2 Left
	{GFG_ArcMain_gf0,{0,103,72,101},{33,71}}, //3 Left
	{GFG_ArcMain_gf0,{73,103,78,104},{35,74}}, //4 Left
	{GFG_ArcMain_gf0,{152,103,81,104},{38,74}}, //5 Left
	
	{GFG_ArcMain_gf1,{0,0,80,103},{37,73}}, //6 Right
	{GFG_ArcMain_gf1,{81,0,80,103},{37,73}}, //7 Right
	{GFG_ArcMain_gf1,{162,0,79,103},{36,73}}, //8 Right
	{GFG_ArcMain_gf1,{0,104,79,102},{36,73}}, //9 Right
	{GFG_ArcMain_gf1,{80,104,74,104},{30,74}}, //10 Right
	{GFG_ArcMain_gf1,{155,104,73,104},{29,74}}, //11 Right
	
	{GFG_ArcMain_gf2,{0,0,73,102},{30,71}}, //12 Cry
	{GFG_ArcMain_gf2,{74,0,73,102},{30,72}}, //13 Cry
	{GFG_ArcMain_gf2,{147,0,72,102},{30,72}}, //14 Cry
	

};

static const Animation char_gfg_anim[CharAnim_Max] = {
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                        //CharAnim_Idle
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},		//CharAnim_Left
	{1, (const u8[]){ 0,  0,  1,  1,  2,  2,  3,  4,  4,  5, ASCR_BACK, 1}}, //CharAnim_LeftAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},		//CharAnim_Down
	{1, (const u8[]){ 12, 13, 14, ASCR_REPEAT}},		//CharAnim_DownAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},		//CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},		//CharAnim_UpAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},		//CharAnim_Right
	{1, (const u8[]){ 6,  6,  7,  7,  8,  8,  9, 10, 10, 11, ASCR_BACK, 1}}, //CharAnim_RightAlt

};

//GFG character functions
void Char_GFG_SetFrame(void *user, u8 frame)
{
	Char_GFG *this = (Char_GFG*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_gfg_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_GFG_Tick(Character *character)
{
	Char_GFG *this = (Char_GFG*)character;
	
	//Perform dance
	if (stage.note_scroll >= character->sing_end && (stage.song_step % stage.gf_speed) == 0 && stage.flag & STAGE_FLAG_JUST_STEP)
	{
		//Switch animation
		if (character->animatable.anim == CharAnim_LeftAlt || character->animatable.anim == CharAnim_Right)
			character->set_anim(character, CharAnim_RightAlt);
		else
			character->set_anim(character, CharAnim_LeftAlt);
	}
	
	//Get parallax
	fixed_t parallax;
	parallax = FIXED_UNIT;
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_GFG_SetFrame);
	Character_DrawParallax(character, &this->tex, &char_gfg_frame[this->frame], parallax);
}

void Char_GFG_SetAnim(Character *character, u8 anim)
{
	//Set animation
	if (anim == CharAnim_Left || anim == CharAnim_Down || anim == CharAnim_Up || anim == CharAnim_Right || anim == CharAnim_UpAlt)
		character->sing_end = stage.note_scroll + FIXED_DEC(22,1); //Nearly 2 steps
	Animatable_SetAnim(&character->animatable, anim);
}

void Char_GFG_Free(Character *character)
{
	Char_GFG *this = (Char_GFG*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_GFG_New(fixed_t x, fixed_t y)
{
	//Allocate gfg object
	Char_GFG *this = Mem_Alloc(sizeof(Char_GFG));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_GFG_New] Failed to allocate gfg object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_GFG_Tick;
	this->character.set_anim = Char_GFG_SetAnim;
	this->character.free = Char_GFG_Free;
	
	Animatable_Init(&this->character.animatable, char_gfg_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i[0][0] = 0;
	this->character.health_i[0][1] = 0;
	this->character.health_i[0][2] = 46;
	this->character.health_i[0][3] = 30;
	
	this->character.health_i[1][0] = 47;
	this->character.health_i[1][1] = 0;
	this->character.health_i[1][2] = 43;
	this->character.health_i[1][3] = 35;
	
	//health bar color
	this->character.health_bar = 0xFF808080;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-115,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.size = FIXED_DEC(100,100);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\GFG.ARC;1");
	
	const char **pathp = (const char *[]){
		"gf0.tim",
		"gf1.tim",
		"gf2.tim",

		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
