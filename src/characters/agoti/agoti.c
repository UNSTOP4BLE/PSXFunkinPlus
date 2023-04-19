#include "agoti.h"

#include "../../psx/mem.h"
#include "../../psx/archive.h"
#include "../../scenes/stage/stage.h"
#include "../../main.h"

//Agoti character structure
enum
{
	Agoti_ArcMain_idle0,
	Agoti_ArcMain_idle1,
	Agoti_ArcMain_idle2,
	Agoti_ArcMain_left0,
	Agoti_ArcMain_left1,
	Agoti_ArcMain_left2,
	Agoti_ArcMain_left3,
	Agoti_ArcMain_down0,
	Agoti_ArcMain_down1,
	Agoti_ArcMain_down2,
	Agoti_ArcMain_down3,
	Agoti_ArcMain_down4,
	Agoti_ArcMain_up0,
	Agoti_ArcMain_up1,
	Agoti_ArcMain_up2,
	Agoti_ArcMain_right0,
	Agoti_ArcMain_right1,
	Agoti_ArcMain_right2,
	Agoti_ArcMain_right3,
	Agoti_ArcMain_right4,
	Agoti_ArcMain_right5,
	Agoti_ArcMain_right6,
	Agoti_ArcMain_right7,

	Agoti_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Agoti_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Agoti;

//Agoti character definitions
static const CharFrame char_agoti_frame[] = {
	{Agoti_ArcMain_idle0,{0,0,116,212},{59,197}}, //0 Idle
	{Agoti_ArcMain_idle0,{117,0,115,212},{58,197}}, //1 Idle
	{Agoti_ArcMain_idle1,{0,0,113,216},{57,198}}, //2 Idle
	{Agoti_ArcMain_idle1,{114,0,113,216},{56,198}}, //3 Idle
	{Agoti_ArcMain_idle2,{0,0,113,216},{57,198}}, //4 Idle
	
	{Agoti_ArcMain_left0,{0,0,204,220},{120,207}}, //5 Left
	{Agoti_ArcMain_left1,{0,0,204,224},{118,209}}, //6 Left
	{Agoti_ArcMain_left2,{0,0,204,224},{117,209}}, //7 Left
	{Agoti_ArcMain_left3,{0,0,204,224},{117,208}}, //8 Left
	
	{Agoti_ArcMain_down0,{0,0,152,164},{64,151}}, //9 Down
	{Agoti_ArcMain_down1,{0,0,152,164},{62,153}}, //10 Down
	{Agoti_ArcMain_down2,{0,0,152,152},{62,138}}, //11 Down
	{Agoti_ArcMain_down3,{0,0,152,152},{62,141}}, //12 Down
	{Agoti_ArcMain_down4,{0,0,152,156},{62,142}}, //13 Down
	
	{Agoti_ArcMain_up0,{0,0,125,236},{74,223}}, //14 Up
	{Agoti_ArcMain_up0,{126,0,124,236},{71,221}}, //15 Up
	{Agoti_ArcMain_up1,{0,0,123,236},{70,223}}, //16 Up
	{Agoti_ArcMain_up1,{124,0,123,236},{70,222}}, //17 Up
	{Agoti_ArcMain_up2,{0,0,124,232},{70,222}}, //18 Up
	
	{Agoti_ArcMain_right0,{0,0,200,204},{79,185}}, //19 Right
	{Agoti_ArcMain_right1,{0,0,200,204},{80,186}}, //20 Right
	{Agoti_ArcMain_right2,{0,0,200,196},{81,179}}, //21 Right
	{Agoti_ArcMain_right3,{0,0,200,196},{80,179}}, //22 Right
	{Agoti_ArcMain_right4,{0,0,200,196},{81,180}}, //23 Right
	{Agoti_ArcMain_right5,{0,0,200,196},{80,180}}, //24 Right
	{Agoti_ArcMain_right6,{0,0,200,200},{81,181}}, //25 Right
	{Agoti_ArcMain_right7,{0,0,200,200},{80,181}}, //26 Right
	

};

static const Animation char_agoti_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, ASCR_BACK, 1}},		//CharAnim_Idle
	{1, (const u8[]){ 5, 6, 7, 8, ASCR_BACK, 1}},		//CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},		//CharAnim_LeftAlt
	{1, (const u8[]){ 9, 10, 11, 12, 13, ASCR_BACK, 1}},		//CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},		//CharAnim_DownAlt
	{1, (const u8[]){ 14, 15, 16, 17, 18, ASCR_BACK, 1}},		//CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},		//CharAnim_UpAlt
	{1, (const u8[]){ 19, 20, 21, 22, 23, 24, 25, 26, ASCR_BACK, 1}},		//CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},		//CharAnim_RightAlt

};

//Agoti character functions
void Char_Agoti_SetFrame(void *user, u8 frame)
{
	Char_Agoti *this = (Char_Agoti*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_agoti_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Agoti_Tick(Character *character)
{
	Char_Agoti *this = (Char_Agoti*)character;
	
	//Perform idle dance
	if ((character->pad_held & (stage.prefs.control_keys[0] | stage.prefs.control_keys[1] | stage.prefs.control_keys[2] | stage.prefs.control_keys[3])) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Agoti_SetFrame);
	Character_Draw(character, &this->tex, &char_agoti_frame[this->frame]);
}

void Char_Agoti_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Agoti_Free(Character *character)
{
	Char_Agoti *this = (Char_Agoti*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Agoti_New(fixed_t x, fixed_t y)
{
	//Allocate agoti object
	Char_Agoti *this = Mem_Alloc(sizeof(Char_Agoti));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Agoti_New] Failed to allocate agoti object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Agoti_Tick;
	this->character.set_anim = Char_Agoti_SetAnim;
	this->character.free = Char_Agoti_Free;
	
	Animatable_Init(&this->character.animatable, char_agoti_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i[0][0] = 0;
	this->character.health_i[0][1] = 28;
	this->character.health_i[0][2] = 39;
	this->character.health_i[0][3] = 30;
	
	this->character.health_i[1][0] = 40;
	this->character.health_i[1][1] = 28;
	this->character.health_i[1][2] = 39;
	this->character.health_i[1][3] = 30;
	
	//health bar color
	this->character.health_bar = 0xFF2F2F2F;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-115,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.size = FIXED_DEC(100,100);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\AGOTI.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim",
		"idle1.tim",
		"idle2.tim",
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
		"right0.tim",
		"right1.tim",
		"right2.tim",
		"right3.tim",
		"right4.tim",
		"right5.tim",
		"right6.tim",
		"right7.tim",

		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
