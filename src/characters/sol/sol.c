#include "sol.h"

#include "../../psx/mem.h"
#include "../../psx/archive.h"
#include "../../scenes/stage/stage.h"
#include "../../main.h"

//Sol character structure
enum
{
	Sol_ArcMain_idle0,
	Sol_ArcMain_idle1,
	Sol_ArcMain_idle2,
	Sol_ArcMain_idle3,
	Sol_ArcMain_idle4,
	Sol_ArcMain_idle5,
	Sol_ArcMain_idle6,
	Sol_ArcMain_idle7,
	Sol_ArcMain_idle8,
	Sol_ArcMain_idle9,
	Sol_ArcMain_idle10,
	Sol_ArcMain_left0,
	Sol_ArcMain_left1,
	Sol_ArcMain_left2,
	Sol_ArcMain_left3,
	Sol_ArcMain_down0,
	Sol_ArcMain_down1,
	Sol_ArcMain_down2,
	Sol_ArcMain_down3,
	Sol_ArcMain_down4,
	Sol_ArcMain_up0,
	Sol_ArcMain_up1,
	Sol_ArcMain_up2,
	Sol_ArcMain_up3,
	Sol_ArcMain_right0,
	Sol_ArcMain_right1,
	Sol_ArcMain_right2,
	Sol_ArcMain_right3,
	Sol_ArcMain_right4,

	Sol_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Sol_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Sol;

//Sol character definitions
static const CharFrame char_sol_frame[] = {
	{Sol_ArcMain_idle0,{0,0,132,236},{68,215}}, //0 Idle
	{Sol_ArcMain_idle1,{0,0,132,236},{68,215}}, //1 Idle
	{Sol_ArcMain_idle2,{0,0,132,236},{68,215}}, //2 Idle
	{Sol_ArcMain_idle3,{0,0,126,236},{68,215}}, //3 Idle
	{Sol_ArcMain_idle3,{127,0,126,236},{68,215}}, //4 Idle
	{Sol_ArcMain_idle4,{0,0,128,236},{68,215}}, //5 Idle
	{Sol_ArcMain_idle5,{0,0,132,236},{68,215}}, //6 Idle
	{Sol_ArcMain_idle6,{0,0,132,236},{68,215}}, //7 Idle
	{Sol_ArcMain_idle7,{0,0,132,236},{68,215}}, //8 Idle
	{Sol_ArcMain_idle8,{0,0,132,236},{68,215}}, //9 Idle
	{Sol_ArcMain_idle9,{0,0,127,236},{68,215}}, //10 Idle
	{Sol_ArcMain_idle9,{128,0,126,236},{68,215}}, //11 Idle
	{Sol_ArcMain_idle10,{0,0,126,236},{68,215}}, //12 Idle
	
	{Sol_ArcMain_left0,{0,0,216,228},{103,210}}, //13 Left
	{Sol_ArcMain_left1,{0,0,212,232},{101,213}}, //14 Left
	{Sol_ArcMain_left2,{0,0,212,224},{100,206}}, //15 Left
	{Sol_ArcMain_left3,{0,0,212,228},{98,209}}, //16 Left
	
	{Sol_ArcMain_down0,{0,0,224,216},{123,200}}, //17 Down
	{Sol_ArcMain_down1,{0,0,224,232},{121,215}}, //18 Down
	{Sol_ArcMain_down2,{0,0,220,232},{122,217}}, //19 Down
	{Sol_ArcMain_down3,{0,0,220,240},{120,222}}, //20 Down
	{Sol_ArcMain_down4,{0,0,224,240},{120,223}}, //21 Down
	
	{Sol_ArcMain_up0,{0,0,136,244},{75,229}}, //22 Up
	{Sol_ArcMain_up1,{0,0,136,244},{74,229}}, //23 Up
	{Sol_ArcMain_up2,{0,0,132,248},{68,233}}, //24 Up
	{Sol_ArcMain_up3,{0,0,125,248},{65,233}}, //25 Up
	{Sol_ArcMain_up3,{126,0,125,248},{65,233}}, //26 Up
	
	{Sol_ArcMain_right0,{0,0,236,212},{57,199}}, //27 Right
	{Sol_ArcMain_right1,{0,0,236,212},{58,202}}, //28 Right
	{Sol_ArcMain_right2,{0,0,236,224},{57,211}}, //29 Right
	{Sol_ArcMain_right3,{0,0,236,228},{57,218}}, //30 Right
	{Sol_ArcMain_right4,{0,0,236,228},{58,218}}, //31 Right
	

};

static const Animation char_sol_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, ASCR_BACK, 1}},		//CharAnim_Idle
	{2, (const u8[]){ 13, 14, 15, 16, ASCR_BACK, 1}},		//CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},		//CharAnim_LeftAlt
	{2, (const u8[]){ 17, 18, 19, 20, 21, ASCR_BACK, 1}},		//CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},		//CharAnim_DownAlt
	{2, (const u8[]){ 22, 23, 24, 25, 26, ASCR_BACK, 1}},		//CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},		//CharAnim_UpAlt
	{2, (const u8[]){ 27, 28, 29, 30, 31, ASCR_BACK, 1}},		//CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},		//CharAnim_RightAlt

};

//Sol character functions
void Char_Sol_SetFrame(void *user, u8 frame)
{
	Char_Sol *this = (Char_Sol*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_sol_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Sol_Tick(Character *character)
{
	Char_Sol *this = (Char_Sol*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Sol_SetFrame);
	Character_Draw(character, &this->tex, &char_sol_frame[this->frame]);
}

void Char_Sol_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Sol_Free(Character *character)
{
	Char_Sol *this = (Char_Sol*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Sol_New(fixed_t x, fixed_t y)
{
	//Allocate sol object
	Char_Sol *this = Mem_Alloc(sizeof(Char_Sol));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Sol_New] Failed to allocate sol object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Sol_Tick;
	this->character.set_anim = Char_Sol_SetAnim;
	this->character.free = Char_Sol_Free;
	
	Animatable_Init(&this->character.animatable, char_sol_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i[0][0] = 122;
	this->character.health_i[0][1] = 37;
	this->character.health_i[0][2] = 41;
	this->character.health_i[0][3] = 32;
	
	this->character.health_i[1][0] = 164;
	this->character.health_i[1][1] = 37;
	this->character.health_i[1][2] = 31;
	this->character.health_i[1][3] = 33;
	
	//health bar color
	this->character.health_bar = 0xFF3A9AA8;
	
	this->character.focus_x = FIXED_DEC(60,1);
	this->character.focus_y = FIXED_DEC(-90,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.size = FIXED_DEC(100,100);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\SOL.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim",
		"idle1.tim",
		"idle2.tim",
		"idle3.tim",
		"idle4.tim",
		"idle5.tim",
		"idle6.tim",
		"idle7.tim",
		"idle8.tim",
		"idle9.tim",
		"idle10.tim",
		"left0.tim",
		"left1.tim",
		"left2.tim",
		"left3.tim",
		"down0.tim",
		"down1.tim",
		"down2.tim",
		"down3.tim",
		"down4.tim",
		"up0.tim",
		"up1.tim",
		"up2.tim",
		"up3.tim",
		"right0.tim",
		"right1.tim",
		"right2.tim",
		"right3.tim",
		"right4.tim",

		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
