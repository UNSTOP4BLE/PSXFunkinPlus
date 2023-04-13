#include "gfb.h"

#include "../../psx/mem.h"
#include "../../psx/archive.h"
#include "../../scenes/stage/stage.h"
#include "../../main.h"

//GFB character structure
enum
{
	GFB_ArcMain_idle,
	GFB_ArcMain_cry,

	GFB_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[GFB_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_GFB;

//GFB character definitions
static const CharFrame char_gfb_frame[] = {
	{GFB_ArcMain_idle,{0,0,74,124},{35,117}}, //0 Idle
	{GFB_ArcMain_idle,{75,0,73,124},{34,118}}, //1 Idle
	{GFB_ArcMain_idle,{149,0,79,127},{36,121}}, //2 Idle
	
	{GFB_ArcMain_cry,{0,0,74,123},{35,116}}, //3 Cry
	{GFB_ArcMain_cry,{75,0,73,123},{36,116}}, //4 Cry
	{GFB_ArcMain_cry,{149,0,74,123},{36,116}}, //5 Cry
	{GFB_ArcMain_cry,{0,124,73,123},{35,116}}, //6 Cry
	{GFB_ArcMain_cry,{74,124,73,123},{35,116}}, //7 Cry
	
	{GFB_ArcMain_idle,{0,128,82,124},{38,118}}, //8 Right
	{GFB_ArcMain_idle,{83,128,80,125},{37,119}}, //9 Right
	{GFB_ArcMain_idle,{164,128,74,126},{30,120}}, //10 Right
	

};

static const Animation char_gfb_anim[CharAnim_Max] = {
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},		//CharAnim_Idle
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},		//CharAnim_Left
	{1, (const u8[]){ 0,  0,  0,  1,  1,  1,  2,  2, 2, 2, ASCR_BACK, 1}},		//CharAnim_LeftAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},		//CharAnim_Down
	{2, (const u8[]){ 3, 4, 5, 6, 7, ASCR_REPEAT}},		//CharAnim_DownAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},		//CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},		//CharAnim_UpAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},		//CharAnim_Right
	{1, (const u8[]){ 8,  8,  8,  9,  9,  9, 10, 10, 10, 10, ASCR_BACK, 1}},		//CharAnim_RightAlt

};

//GFB character functions
void Char_GFB_SetFrame(void *user, u8 frame)
{
	Char_GFB *this = (Char_GFB*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_gfb_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_GFB_Tick(Character *character)
{
	Char_GFB *this = (Char_GFB*)character;
	
	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{
		//Perform dance
		if (stage.note_scroll >= character->sing_end && (stage.song_step % stage.gf_speed) == 0)
		{
			//Switch animation
			if (character->animatable.anim == CharAnim_LeftAlt || character->animatable.anim == CharAnim_Right)
				character->set_anim(character, CharAnim_RightAlt);
			else
				character->set_anim(character, CharAnim_LeftAlt);
		}
	}
	
	//Get parallax
	fixed_t parallax;
	parallax = FIXED_UNIT;
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_GFB_SetFrame);
	Character_DrawParallax(character, &this->tex, &char_gfb_frame[this->frame], parallax);
}

void Char_GFB_SetAnim(Character *character, u8 anim)
{
	//Set animation
	if (anim == CharAnim_Left || anim == CharAnim_Down || anim == CharAnim_Up || anim == CharAnim_Right || anim == CharAnim_UpAlt)
		character->sing_end = stage.note_scroll + FIXED_DEC(22,1); //Nearly 2 steps
	Animatable_SetAnim(&character->animatable, anim);
}

void Char_GFB_Free(Character *character)
{
	Char_GFB *this = (Char_GFB*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_GFB_New(fixed_t x, fixed_t y)
{
	//Allocate gfb object
	Char_GFB *this = Mem_Alloc(sizeof(Char_GFB));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_GFB_New] Failed to allocate gfb object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_GFB_Tick;
	this->character.set_anim = Char_GFB_SetAnim;
	this->character.free = Char_GFB_Free;
	
	Animatable_Init(&this->character.animatable, char_gfb_anim);
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
	this->arc_main = IO_Read("\\CHAR\\GFB.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle.tim",
		"cry.tim",

		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
