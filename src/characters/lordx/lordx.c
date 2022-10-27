#include "lordx.h"

#include "../../psx/mem.h"
#include "../../psx/archive.h"
#include "../../stage/stage.h"
#include "../../main.h"

//LordX character structure
enum
{
	LordX_ArcMain_Idle0,
	LordX_ArcMain_Idle1,
	LordX_ArcMain_Idle2,
	LordX_ArcMain_Idle3,
	LordX_ArcMain_Idle4,
	LordX_ArcMain_Idle5,
	
	LordX_ArcMain_Left0,
	LordX_ArcMain_Left1,
	LordX_ArcMain_Left2,
	LordX_ArcMain_Left3,
	LordX_ArcMain_Left4,
	
	LordX_ArcMain_Down0,
	LordX_ArcMain_Down1,
	LordX_ArcMain_Down2,
	LordX_ArcMain_Down3,
	LordX_ArcMain_Down4,
	
	LordX_ArcMain_Up0,
	LordX_ArcMain_Up1,
	LordX_ArcMain_Up2,
	LordX_ArcMain_Up3,
	LordX_ArcMain_Up4,
	
	LordX_ArcMain_Right0,
	LordX_ArcMain_Right1,
	LordX_ArcMain_Right2,
	LordX_ArcMain_Right3,
	LordX_ArcMain_Right4,
	
	LordX_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[LordX_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_LordX;

//LordX character definitions
static const CharFrame char_lordx_frame[] = {
	{LordX_ArcMain_Idle0,{0,0,120,174},{56,158}},
	{LordX_ArcMain_Idle0,{121,0,121,174},{57,157}},
	{LordX_ArcMain_Idle1,{0,10,122,175},{58,160}},
	{LordX_ArcMain_Idle1,{123,0,127,182},{63,169}},
	{LordX_ArcMain_Idle2,{0,0,131,193},{67,180}},
	{LordX_ArcMain_Idle3,{0,0,133,193},{69,180}},
	{LordX_ArcMain_Idle4,{0,0,133,193},{70,180}},
	{LordX_ArcMain_Idle5,{0,0,132,188},{69,175}},
	
	{LordX_ArcMain_Left0,{0,0,246,194},{170+20,184}},
	{LordX_ArcMain_Left1,{0,0,230,190},{155+20,180}},
	{LordX_ArcMain_Left2,{0,0,224,188},{149+20,178}},
	{LordX_ArcMain_Left3,{0,0,221,188},{146+20,178}},
	{LordX_ArcMain_Left4,{0,0,222,188},{146+20,178}},
	
	{LordX_ArcMain_Down0,{0,0,138,166},{68,157}},
	{LordX_ArcMain_Down1,{0,0,136,180},{67,171}},
	{LordX_ArcMain_Down2,{0,0,134,186},{67,177}},
	{LordX_ArcMain_Down3,{0,0,134,188},{68,178}},
	{LordX_ArcMain_Down4,{0,0,134,186},{67,176}},
	
	{LordX_ArcMain_Up0,{0,0,130,256},{79,235}},
	{LordX_ArcMain_Up1,{0,0,126,234},{74,215}},
	{LordX_ArcMain_Up2,{0,0,130,228},{76,209}},
	{LordX_ArcMain_Up3,{0,0,132,228},{76,209}},
	{LordX_ArcMain_Up4,{0,0,132,228},{76,209}},
	
	{LordX_ArcMain_Right0,{0,0,184,234},{68,216}},
	{LordX_ArcMain_Right1,{0,0,196,216},{96,197}},
	{LordX_ArcMain_Right2,{0,0,196,216},{101,198}},
	{LordX_ArcMain_Right3,{0,0,196,216},{103,198}},
	{LordX_ArcMain_Right4,{0,0,196,216},{102,198}},
};

static const Animation char_lordx_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  4,  5,  6,  7, ASCR_BACK, 0}}, //CharAnim_Idle
	{1, (const u8[]){ 8,  9, 10, 11, 12, ASCR_BACK, 0}},          //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{1, (const u8[]){13, 14, 15, 16, 17, ASCR_BACK, 0}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{1, (const u8[]){18, 19, 20, 21, 22, ASCR_BACK, 0}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{1, (const u8[]){23, 24, 25, 26, 27, ASCR_BACK, 0}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//LordX character functions
void Char_LordX_SetFrame(void *user, u8 frame)
{
	Char_LordX *this = (Char_LordX*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_lordx_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_LordX_Tick(Character *character)
{
	Char_LordX *this = (Char_LordX*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_LordX_SetFrame);
	Character_Draw(character, &this->tex, &char_lordx_frame[this->frame]);
}

void Char_LordX_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_LordX_Free(Character *character)
{
	Char_LordX *this = (Char_LordX*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_LordX_New(fixed_t x, fixed_t y)
{
	//Allocate lordx object
	Char_LordX *this = Mem_Alloc(sizeof(Char_LordX));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_LordX_New] Failed to allocate lordx object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_LordX_Tick;
	this->character.set_anim = Char_LordX_SetAnim;
	this->character.free = Char_LordX_Free;
	
	Animatable_Init(&this->character.animatable, char_lordx_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;
	
	this->character.focus_x = FIXED_DEC(20,1);
	this->character.focus_y = FIXED_DEC(-90,1);
	this->character.focus_zoom = FIXED_DEC(4,10);
	
	this->character.size = FIXED_DEC(3,2);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\LORDX.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //LordX_ArcMain_Idle0
		"idle1.tim", //LordX_ArcMain_Idle1
		"idle2.tim", //LordX_ArcMain_Idle2
		"idle3.tim", //LordX_ArcMain_Idle3
		"idle4.tim", //LordX_ArcMain_Idle4
		"idle5.tim", //LordX_ArcMain_Idle5
		
		"left0.tim", //LordX_ArcMain_Left0
		"left1.tim", //LordX_ArcMain_Left1
		"left2.tim", //LordX_ArcMain_Left2
		"left3.tim", //LordX_ArcMain_Left3
		"left4.tim", //LordX_ArcMain_Left4
		
		"down0.tim", //LordX_ArcMain_Down0
		"down1.tim", //LordX_ArcMain_Down1
		"down2.tim", //LordX_ArcMain_Down2
		"down3.tim", //LordX_ArcMain_Down3
		"down4.tim", //LordX_ArcMain_Down4
		
		"up0.tim", //LordX_ArcMain_Up0
		"up1.tim", //LordX_ArcMain_Up1
		"up2.tim", //LordX_ArcMain_Up2
		"up3.tim", //LordX_ArcMain_Up3
		"up4.tim", //LordX_ArcMain_Up4
		
		"right0.tim", //LordX_ArcMain_Right0
		"right1.tim", //LordX_ArcMain_Right1
		"right2.tim", //LordX_ArcMain_Right2
		"right3.tim", //LordX_ArcMain_Right3
		"right4.tim", //LordX_ArcMain_Right4
		
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
