#include "aldryx.h"

#include "../../psx/mem.h"
#include "../../psx/archive.h"
#include "../../scenes/stage/stage.h"
#include "../../main.h"

//Aldryx character structure
enum
{
	Aldryx_ArcMain_idle0,
	Aldryx_ArcMain_idle1,
	Aldryx_ArcMain_left0,
	Aldryx_ArcMain_left1,
	Aldryx_ArcMain_left2,
	Aldryx_ArcMain_left3,
	Aldryx_ArcMain_left4,
	Aldryx_ArcMain_down0,
	Aldryx_ArcMain_down1,
	Aldryx_ArcMain_down2,
	Aldryx_ArcMain_down3,
	Aldryx_ArcMain_down4,
	Aldryx_ArcMain_up0,
	Aldryx_ArcMain_up1,
	Aldryx_ArcMain_right0,
	Aldryx_ArcMain_right1,
	Aldryx_ArcMain_right2,
	Aldryx_ArcMain_right3,
	Aldryx_ArcMain_right4,
	Aldryx_ArcMain_right5,

	Aldryx_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Aldryx_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Aldryx;

//Aldryx character definitions
static const CharFrame char_aldryx_frame[] = {
	{Aldryx_ArcMain_idle0,{0,0,83,188},{35,171}}, //0 Idle
	{Aldryx_ArcMain_idle0,{84,0,83,186},{35,169}}, //1 Idle
	{Aldryx_ArcMain_idle0,{168,0,83,187},{35,170}}, //2 Idle
	{Aldryx_ArcMain_idle1,{0,0,83,187},{35,171}}, //3 Idle
	{Aldryx_ArcMain_idle1,{84,0,83,188},{35,171}}, //4 Idle
	{Aldryx_ArcMain_idle1,{169,0,82,188},{35,171}}, //5 Idle
	
	{Aldryx_ArcMain_left0,{0,0,113,192},{70,171}}, //6 Left
	{Aldryx_ArcMain_left0,{115,0,113,192},{71,171}}, //7 Left
	{Aldryx_ArcMain_left1,{0,0,129,192},{71,171}}, //8 Left
	{Aldryx_ArcMain_left2,{0,0,133,191},{69,171}}, //9 Left
	{Aldryx_ArcMain_left3,{0,0,132,192},{68,171}}, //10 Left
	{Aldryx_ArcMain_left4,{0,0,136,192},{69,171}}, //11 Left
	
	{Aldryx_ArcMain_down0,{0,0,160,152},{50,138}}, //12 Down
	{Aldryx_ArcMain_down1,{0,0,160,152},{50,137}}, //13 Down
	{Aldryx_ArcMain_down2,{0,0,160,152},{50,138}}, //14 Down
	{Aldryx_ArcMain_down3,{0,0,160,152},{51,138}}, //15 Down
	{Aldryx_ArcMain_down4,{0,0,160,152},{51,138}}, //16 Down
	
	{Aldryx_ArcMain_up0,{0,0,109,252},{40,231}}, //17 Up
	{Aldryx_ArcMain_up0,{110,0,110,249},{41,230}}, //18 Up
	{Aldryx_ArcMain_up1,{0,0,111,248},{41,225}}, //19 Up
	{Aldryx_ArcMain_up1,{112,0,110,248},{40,225}}, //20 Up
	
	{Aldryx_ArcMain_right0,{0,0,172,192},{59,175}}, //21 Right
	{Aldryx_ArcMain_right1,{0,0,172,192},{58,176}}, //22 Right
	{Aldryx_ArcMain_right2,{0,0,172,196},{60,177}}, //23 Right
	{Aldryx_ArcMain_right3,{0,0,172,196},{59,177}}, //24 Right
	{Aldryx_ArcMain_right4,{0,0,172,196},{59,177}}, //25 Right
	{Aldryx_ArcMain_right5,{0,0,172,196},{59,177}}, //26 Right
	

};

static const Animation char_aldryx_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, 5, ASCR_BACK, 1}},		//CharAnim_Idle
	{2, (const u8[]){ 6, 7, 8, 9, 10, 11, ASCR_BACK, 1}},		//CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},		//CharAnim_LeftAlt
	{2, (const u8[]){ 12, 13, 14, 15, 16, ASCR_BACK, 1}},		//CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},		//CharAnim_DownAlt
	{2, (const u8[]){ 17, 18, 19, 20, ASCR_BACK, 1}},		//CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},		//CharAnim_UpAlt
	{2, (const u8[]){ 21, 22, 23, 24, 25, 26, ASCR_BACK, 1}},		//CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},		//CharAnim_RightAlt

};

//Aldryx character functions
void Char_Aldryx_SetFrame(void *user, u8 frame)
{
	Char_Aldryx *this = (Char_Aldryx*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_aldryx_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Aldryx_Tick(Character *character)
{
	Char_Aldryx *this = (Char_Aldryx*)character;
	
	//Perform idle dance
	if ((character->pad_held & (stage.prefs.control_keys[0] | stage.prefs.control_keys[1] | stage.prefs.control_keys[2] | stage.prefs.control_keys[3])) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Aldryx_SetFrame);
	Character_Draw(character, &this->tex, &char_aldryx_frame[this->frame]);
}

void Char_Aldryx_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Aldryx_Free(Character *character)
{
	Char_Aldryx *this = (Char_Aldryx*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Aldryx_New(fixed_t x, fixed_t y)
{
	//Allocate aldryx object
	Char_Aldryx *this = Mem_Alloc(sizeof(Char_Aldryx));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Aldryx_New] Failed to allocate aldryx object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Aldryx_Tick;
	this->character.set_anim = Char_Aldryx_SetAnim;
	this->character.free = Char_Aldryx_Free;
	
	Animatable_Init(&this->character.animatable, char_aldryx_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i[0][0] = 140;
	this->character.health_i[0][1] = 0;
	this->character.health_i[0][2] = 33;
	this->character.health_i[0][3] = 36;
	
	this->character.health_i[1][0] = 174;
	this->character.health_i[1][1] = 0;
	this->character.health_i[1][2] = 32;
	this->character.health_i[1][3] = 36;
	
	//health bar color
	this->character.health_bar = 0xFFBA1E24;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-115,1);
	this->character.focus_zoom = FIXED_DEC(90,100);
	
	this->character.size = FIXED_DEC(120,100);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\ALDRYX.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim",
		"idle1.tim",
		"left0.tim",
		"left1.tim",
		"left2.tim",
		"left3.tim",
		"left4.tim",
		"down0.tim",
		"down1.tim",
		"down2.tim",
		"down3.tim",
		"down4.tim",
		"up0.tim",
		"up1.tim",
		"right0.tim",
		"right1.tim",
		"right2.tim",
		"right3.tim",
		"right4.tim",
		"right5.tim",

		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
