#include "morde.h"

#include "../../psx/mem.h"
#include "../../psx/archive.h"
#include "../../scenes/stage/stage.h"
#include "../../main.h"

//Morde character structure
enum
{
	Morde_ArcMain_Idle0,
	Morde_ArcMain_Idle1,
	Morde_ArcMain_Left0,
	Morde_ArcMain_Left1,
	Morde_ArcMain_Down0,
	Morde_ArcMain_Down1,
	Morde_ArcMain_Up0,
	Morde_ArcMain_Up1,
	Morde_ArcMain_Right0,
	Morde_ArcMain_Right1,
	
	Morde_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Morde_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Morde;

//Morde character definitions
static const CharFrame char_morde_frame[] = {
	{Morde_ArcMain_Idle0,{0,0,85,166},{39,139}},
	{Morde_ArcMain_Idle0,{87,0,85,166},{39,139}},
	{Morde_ArcMain_Idle0,{175,0,81,166},{38,138}},
	{Morde_ArcMain_Idle1,{0,0,81,168},{37,139}},
	{Morde_ArcMain_Idle1,{87,0,81,168},{37,139}},
	
	{Morde_ArcMain_Left0,{0,0,86,166},{52,140}},
	{Morde_ArcMain_Left0,{87,0,79,166},{45,140}},
	{Morde_ArcMain_Left1,{0,0,76,166},{41,140}},
	{Morde_ArcMain_Left1,{77,0,77,166},{41,140}},
	
	{Morde_ArcMain_Down0,{0,0,74,154},{36,129}},
	{Morde_ArcMain_Down0,{75,0,76,154},{36,129}},
	{Morde_ArcMain_Down1,{0,0,72,154},{32,130}},
	{Morde_ArcMain_Down1,{76,0,72,154},{32,130}},
	
	{Morde_ArcMain_Up0,{0,0,89,176},{48,150}},
	{Morde_ArcMain_Up0,{97,0,91,176},{46,151}},
	{Morde_ArcMain_Up1,{0,0,89,170},{44,145}},
	{Morde_ArcMain_Up1,{90,0,88,170},{44,145}},
	
	{Morde_ArcMain_Right0,{0,0,105,166},{37,138}},
	{Morde_ArcMain_Right0,{106,0,100,166},{36,138}},
	{Morde_ArcMain_Right1,{0,0,101,166},{37,138}},
	{Morde_ArcMain_Right1,{102,0,101,166},{37,138}},
};

static const Animation char_morde_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  4, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 5,  6,  7,  8,  ASCR_BACK, 1}},    //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{2, (const u8[]){ 9, 10, 11, 12, ASCR_BACK, 1}},     //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{2, (const u8[]){13, 14, 15, 16, ASCR_BACK, 1}},     //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{2, (const u8[]){17, 18, 19, 20, ASCR_BACK, 1}},     //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
};

//Morde character functions
void Char_Morde_SetFrame(void *user, u8 frame)
{
	Char_Morde *this = (Char_Morde*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_morde_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Morde_Tick(Character *character)
{
	Char_Morde *this = (Char_Morde*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Morde_SetFrame);
	Character_Draw(character, &this->tex, &char_morde_frame[this->frame]);
}

void Char_Morde_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Morde_Free(Character *character)
{
	Char_Morde *this = (Char_Morde*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Morde_New(fixed_t x, fixed_t y)
{
	//Allocate morde object
	Char_Morde *this = Mem_Alloc(sizeof(Char_Morde));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Morde_New] Failed to allocate morde object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Morde_Tick;
	this->character.set_anim = Char_Morde_SetAnim;
	this->character.free = Char_Morde_Free;
	
	Animatable_Init(&this->character.animatable, char_morde_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-80,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.size = FIXED_DEC(10,10);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\MORDE.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim",
		"idle1.tim",
		"left0.tim",
		"left1.tim",
		"down0.tim",
		"down1.tim",
		"up0.tim",
		"up1.tim",
		"right0.tim",
		"right1.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
