#include "benson.h"

#include "../../psx/mem.h"
#include "../../psx/archive.h"
#include "../../scenes/stage/stage.h"
#include "../../main.h"

Benson benson;

//Benson character structure
enum
{
	Benson_ArcMain_idle,
	Benson_ArcMain_left,
	Benson_ArcMain_down,
	Benson_ArcMain_up,
	Benson_ArcMain_right,
	Benson_ArcMain_idleangry0,
	Benson_ArcMain_idleangry1,
	Benson_ArcMain_leftangry,
	Benson_ArcMain_downangry,
	Benson_ArcMain_upangry,
	Benson_ArcMain_rightangry,
	
	Benson_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Benson_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Benson;

//Benson character definitions
static const CharFrame char_benson_frame[] = {
	{Benson_ArcMain_idle,{0,0,62,174},{35,166}},
	{Benson_ArcMain_idle,{63,0,60,174},{33,166}},
	{Benson_ArcMain_idle,{124,0,60,174},{33,166}},
	{Benson_ArcMain_idle,{185,0,62,174},{35,166}},
	
	{Benson_ArcMain_left,{0,0,78,176},{51,168}},
	{Benson_ArcMain_left,{79,0,73,176},{46,168}},
	
	{Benson_ArcMain_down,{0,0,66,162},{25,153}},
	{Benson_ArcMain_down,{67,0,64,162},{25,153}},
	
	{Benson_ArcMain_up,{0,0,97,190},{34,179}},
	{Benson_ArcMain_up,{98,0,94,190},{33,180}},
	
	{Benson_ArcMain_right,{0,0,95,176},{25,168}},
	{Benson_ArcMain_right,{96,0,94,176},{28,168}},
	
	{Benson_ArcMain_idleangry0,{0,0,84,156},{26,148}},
	{Benson_ArcMain_idleangry0,{85,0,85,156},{26,148}},
	{Benson_ArcMain_idleangry0,{171,0,83,156},{25,148}},
	{Benson_ArcMain_idleangry1,{0,0,83,156},{24,147}},
	
	{Benson_ArcMain_leftangry,{0,0,128,146},{101,138}},
	{Benson_ArcMain_leftangry,{129,0,121,146},{94,138}},
	
	{Benson_ArcMain_downangry,{0,0,90,132},{28,120}},
	{Benson_ArcMain_downangry,{91,0,86,132},{27,119}},
	
	{Benson_ArcMain_upangry,{0,0,56,194},{27,184}},
	{Benson_ArcMain_upangry,{57,0,55,194},{25,184}},
	
	{Benson_ArcMain_rightangry,{0,0,155,142},{25,134}},
	{Benson_ArcMain_rightangry,{105,103,143,147},{25,139}},
};

static const Animation char_benson_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 16,  17,  ASCR_BACK, 1}},    //CharAnim_Left
	{2, (const u8[]){ 4,  5,  ASCR_BACK, 1}},       //CharAnim_LeftAlt
	{2, (const u8[]){ 18,  19,  ASCR_BACK, 1}},     //CharAnim_Down
	{2, (const u8[]){ 6,  7,  ASCR_BACK, 1}},       //CharAnim_DownAlt
	{2, (const u8[]){ 20,  21,  ASCR_BACK, 1}},     //CharAnim_Up
	{2, (const u8[]){ 8,  9,  ASCR_BACK, 1}},       //CharAnim_UpAlt
	{2, (const u8[]){22, 23,  ASCR_BACK, 1}},     //CharAnim_Right
	{2, (const u8[]){10, 11,  ASCR_BACK, 1}},       //CharAnim_RightAlt
};

static const Animation char_bensonsec_anim[CharAnim_Max] = {
	{2, (const u8[]){ 12,  13,  14,  15,  ASCR_BACK, 1}}, //CharAnim_IdleAlt
	{2, (const u8[]){ 16,  17,  ASCR_BACK, 1}},    //CharAnim_Left
	{2, (const u8[]){ 4,  5,  ASCR_BACK, 1}},       //CharAnim_LeftAlt
	{2, (const u8[]){ 18,  19,  ASCR_BACK, 1}},     //CharAnim_Down
	{2, (const u8[]){ 6,  7,  ASCR_BACK, 1}},       //CharAnim_DownAlt
	{2, (const u8[]){ 20,  21,  ASCR_BACK, 1}},     //CharAnim_Up
	{2, (const u8[]){ 8,  9,  ASCR_BACK, 1}},       //CharAnim_UpAlt
	{2, (const u8[]){22, 23,  ASCR_BACK, 1}},     //CharAnim_Right
	{2, (const u8[]){10, 11,  ASCR_BACK, 1}},       //CharAnim_RightAlt
};

//Benson character functions
void Char_Benson_SetFrame(void *user, u8 frame)
{
	Char_Benson *this = (Char_Benson*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_benson_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Benson_Tick(Character *character)
{
	Char_Benson *this = (Char_Benson*)character;
	
	if (benson.angry2 != benson.angry)
	{
		if (benson.angry)
			Animatable_Init(&this->character.animatable, char_bensonsec_anim);
		else
			Animatable_Init(&this->character.animatable, char_benson_anim);
		benson.angry2 = benson.angry;
	}
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Benson_SetFrame);
	Character_Draw(character, &this->tex, &char_benson_frame[this->frame]);
}

void Char_Benson_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Benson_Free(Character *character)
{
	Char_Benson *this = (Char_Benson*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Benson_New(fixed_t x, fixed_t y)
{
	//Allocate benson object
	Char_Benson *this = Mem_Alloc(sizeof(Char_Benson));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Benson_New] Failed to allocate benson object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Benson_Tick;
	this->character.set_anim = Char_Benson_SetAnim;
	this->character.free = Char_Benson_Free;
	
	Animatable_Init(&this->character.animatable, char_benson_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i[0][0] = 90;
	this->character.health_i[0][1] = 0;
	this->character.health_i[0][2] = 44;
	this->character.health_i[0][3] = 42;
	
	this->character.health_i[1][0] = 135;
	this->character.health_i[1][1] = 0;
	this->character.health_i[1][2] = 42;
	this->character.health_i[1][3] = 42;
	
	//health bar color
	this->character.health_bar = 0xFFA5004A;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-80,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.size = FIXED_DEC(10,10);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\BENSON.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle.tim",
		"left.tim",
		"down.tim",
		"up.tim",
		"right.tim",
		"idleangry0.tim",
		"idleangry1.tim",
		"leftangry.tim",
		"downangry.tim",
		"upangry.tim",
		"rightangry.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	benson.angry = false;
	benson.angry2 = false;
	
	return (Character*)this;
}