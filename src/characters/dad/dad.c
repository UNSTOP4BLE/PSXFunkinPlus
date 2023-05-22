#include "dad.h"

#include "../../psx/mem.h"
#include "../../psx/archive.h"
#include "../../scenes/stage/stage.h"
#include "../../main.h"

//Dad character structure
enum
{
	Dad_ArcMain_idle0,
	Dad_ArcMain_idle1,
	Dad_ArcMain_left,
	Dad_ArcMain_down,
	Dad_ArcMain_up,
	Dad_ArcMain_right,

	Dad_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Dad_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Dad;

//Dad character definitions
static const u16 char_dad_icons[2][4] = {
	{140,0,33,39},
	{140,0,33,39}
};

static const CharFrame char_dad_frame[] = {
	{Dad_ArcMain_idle0,{107,0,108,190},{45,179}}, //0 Idle
	{Dad_ArcMain_idle1,{0,0,107,190},{44,179}}, //1 Idle
	{Dad_ArcMain_idle1,{108,0,105,192},{43,181}}, //2 Idle
	{Dad_ArcMain_idle0,{0,0,106,192},{44,181}}, //3 Idle
	
	{Dad_ArcMain_left,{0,0,93,195},{43,183}}, //4 Left
	{Dad_ArcMain_left,{94,0,96,195},{43,183}}, //5 Left
	
	{Dad_ArcMain_down,{0,0,118,183},{45,172}}, //6 Down
	{Dad_ArcMain_down,{119,0,117,184},{45,173}}, //7 Down
	
	{Dad_ArcMain_up,{0,0,102,205},{43,194}}, //8 Up
	{Dad_ArcMain_up,{103,0,102,203},{43,192}}, //9 Up
	
	{Dad_ArcMain_right,{0,0,117,199},{44,188}}, //10 Right
	{Dad_ArcMain_right,{118,0,114,199},{44,188}}, //11 Right
	

};

static const Animation char_dad_anim[CharAnim_Max] = {
	{12, (const u8[]){ 0, 1, 2, 3,  ASCR_BACK, 1}},		//CharAnim_Idle
	{12, (const u8[]){ 4, 5,  ASCR_BACK, 1}},		//CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},		//CharAnim_LeftAlt
	{12, (const u8[]){ 6, 7,  ASCR_BACK, 1}},		//CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},		//CharAnim_DownAlt
	{12, (const u8[]){ 8, 9,  ASCR_BACK, 1}},		//CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},		//CharAnim_UpAlt
	{12, (const u8[]){ 10, 11,  ASCR_BACK, 1}},		//CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},		//CharAnim_RightAlt

};

//Dad character functions
void Char_Dad_SetFrame(void *user, u8 frame)
{
	Char_Dad *this = (Char_Dad*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_dad_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Dad_Tick(Character *character)
{
	Char_Dad *this = (Char_Dad*)character;
	
	//Perform idle dance
	if ((character->pad_held & (stage.prefs.control_keys[0] | stage.prefs.control_keys[1] | stage.prefs.control_keys[2] | stage.prefs.control_keys[3])) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Dad_SetFrame);
	Character_Draw(character, &this->tex, &char_dad_frame[this->frame]);
}

void Char_Dad_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Dad_Free(Character *character)
{
	Char_Dad *this = (Char_Dad*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Dad_New(fixed_t x, fixed_t y)
{
	//Allocate dad object
	Char_Dad *this = Mem_Alloc(sizeof(Char_Dad));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Dad_New] Failed to allocate dad object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Dad_Tick;
	this->character.set_anim = Char_Dad_SetAnim;
	this->character.free = Char_Dad_Free;
	
	Animatable_Init(&this->character.animatable, char_dad_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	memcpy(this->character.health_i, char_dad_icons, sizeof(char_dad_icons));
	
	//health bar color
	this->character.health_bar = 0xFFAF66CE;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-115,1);
	this->character.focus_zoom = FIXED_DEC(100,100);
	
	this->character.size = FIXED_DEC(100,100);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\DAD.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim",
		"idle1.tim",
		"left.tim",
		"down.tim",
		"up.tim",
		"right.tim",

		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
