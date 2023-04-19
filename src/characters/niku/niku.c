#include "niku.h"

#include "../../psx/mem.h"
#include "../../psx/archive.h"
#include "../../scenes/stage/stage.h"
#include "../../main.h"

//Niku character structure
enum
{
	Niku_ArcMain_idle0,
	Niku_ArcMain_idle1,
	Niku_ArcMain_idle2,
	Niku_ArcMain_idle3,
	Niku_ArcMain_idle4,
	Niku_ArcMain_idle5,
	Niku_ArcMain_idle6,
	Niku_ArcMain_idle7,
	Niku_ArcMain_idle8,
	Niku_ArcMain_idle9,
	Niku_ArcMain_left0,
	Niku_ArcMain_left1,
	Niku_ArcMain_left2,
	Niku_ArcMain_left3,
	Niku_ArcMain_down0,
	Niku_ArcMain_down1,
	Niku_ArcMain_down2,
	Niku_ArcMain_down3,
	Niku_ArcMain_down4,
	Niku_ArcMain_down5,
	Niku_ArcMain_down6,
	Niku_ArcMain_up0,
	Niku_ArcMain_up1,
	Niku_ArcMain_up2,
	Niku_ArcMain_up3,
	Niku_ArcMain_up4,
	Niku_ArcMain_up5,
	Niku_ArcMain_up6,
	Niku_ArcMain_up7,
	Niku_ArcMain_right0,
	Niku_ArcMain_right1,
	Niku_ArcMain_right2,
	Niku_ArcMain_right3,
	Niku_ArcMain_right4,
	Niku_ArcMain_right5,

	Niku_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Niku_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Niku;

//Niku character definitions
static const CharFrame char_niku_frame[] = {
	{Niku_ArcMain_idle0,{0,0,140,236},{64,218}}, //0 Idle
	{Niku_ArcMain_idle1,{0,0,140,236},{64,219}}, //1 Idle
	{Niku_ArcMain_idle2,{0,0,140,236},{63,219}}, //2 Idle
	{Niku_ArcMain_idle3,{0,0,140,236},{63,219}}, //3 Idle
	{Niku_ArcMain_idle4,{0,0,144,236},{64,219}}, //4 Idle
	{Niku_ArcMain_idle5,{0,0,140,236},{63,219}}, //5 Idle
	{Niku_ArcMain_idle6,{0,0,140,240},{63,221}}, //6 Idle
	{Niku_ArcMain_idle7,{0,0,140,240},{62,221}}, //7 Idle
	{Niku_ArcMain_idle8,{0,0,140,240},{61,221}}, //8 Idle
	{Niku_ArcMain_idle9,{0,0,140,240},{61,221}}, //9 Idle
	
	{Niku_ArcMain_left0,{0,0,125,236},{66,218}}, //10 Left
	{Niku_ArcMain_left0,{126,0,128,236},{65,218}}, //11 Left
	{Niku_ArcMain_left1,{0,0,132,236},{65,218}}, //12 Left
	{Niku_ArcMain_left2,{0,0,132,236},{65,218}}, //13 Left
	{Niku_ArcMain_left3,{0,0,132,236},{65,218}}, //14 Left
	
	{Niku_ArcMain_down0,{0,0,136,232},{61,216}}, //15 Down
	{Niku_ArcMain_down1,{0,0,136,236},{62,217}}, //16 Down
	{Niku_ArcMain_down2,{0,0,140,236},{63,218}}, //17 Down
	{Niku_ArcMain_down3,{0,0,140,236},{63,218}}, //18 Down
	{Niku_ArcMain_down4,{0,0,144,236},{63,218}}, //19 Down
	{Niku_ArcMain_down5,{0,0,144,236},{63,218}}, //20 Down
	{Niku_ArcMain_down6,{0,0,144,236},{63,218}}, //21 Down
	
	{Niku_ArcMain_up0,{0,0,132,244},{61,226}}, //22 Up
	{Niku_ArcMain_up1,{0,0,136,240},{61,223}}, //23 Up
	{Niku_ArcMain_up2,{0,0,136,240},{60,221}}, //24 Up
	{Niku_ArcMain_up3,{0,0,136,240},{60,221}}, //25 Up
	{Niku_ArcMain_up4,{0,0,136,236},{60,220}}, //26 Up
	{Niku_ArcMain_up5,{0,0,136,236},{61,220}}, //27 Up
	{Niku_ArcMain_up6,{0,0,136,240},{61,221}}, //28 Up
	{Niku_ArcMain_up7,{0,0,136,236},{60,220}}, //29 Up
	
	{Niku_ArcMain_right0,{0,0,152,244},{61,221}}, //30 Right
	{Niku_ArcMain_right1,{0,0,152,240},{61,219}}, //31 Right
	{Niku_ArcMain_right2,{0,0,152,236},{61,220}}, //32 Right
	{Niku_ArcMain_right3,{0,0,152,236},{61,219}}, //33 Right
	{Niku_ArcMain_right4,{0,0,152,236},{61,219}}, //34 Right
	{Niku_ArcMain_right5,{0,0,148,236},{61,219}}, //35 Right
	

};

static const Animation char_niku_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, ASCR_BACK, 1}},		//CharAnim_Idle
	{2, (const u8[]){ 10, 11, 12, 13, 14, ASCR_BACK, 1}},		//CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},		//CharAnim_LeftAlt
	{2, (const u8[]){ 15, 16, 17, 18, 19, 20, 21, ASCR_BACK, 1}},		//CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},		//CharAnim_DownAlt
	{2, (const u8[]){ 22, 23, 24, 25, 26, 27, 28, 29, ASCR_BACK, 1}},		//CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},		//CharAnim_UpAlt
	{2, (const u8[]){ 30, 31, 32, 33, 34, 35, ASCR_BACK, 1}},		//CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},		//CharAnim_RightAlt

};

//Niku character functions
void Char_Niku_SetFrame(void *user, u8 frame)
{
	Char_Niku *this = (Char_Niku*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_niku_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Niku_Tick(Character *character)
{
	Char_Niku *this = (Char_Niku*)character;
	
	//Perform idle dance
	if ((character->pad_held & (stage.prefs.control_keys[0] | stage.prefs.control_keys[1] | stage.prefs.control_keys[2] | stage.prefs.control_keys[3])) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Niku_SetFrame);
	Character_Draw(character, &this->tex, &char_niku_frame[this->frame]);
}

void Char_Niku_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Niku_Free(Character *character)
{
	Char_Niku *this = (Char_Niku*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Niku_New(fixed_t x, fixed_t y)
{
	//Allocate niku object
	Char_Niku *this = Mem_Alloc(sizeof(Char_Niku));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Niku_New] Failed to allocate niku object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Niku_Tick;
	this->character.set_anim = Char_Niku_SetAnim;
	this->character.free = Char_Niku_Free;
	
	Animatable_Init(&this->character.animatable, char_niku_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i[0][0] = 0;
	this->character.health_i[0][1] = 59;
	this->character.health_i[0][2] = 32;
	this->character.health_i[0][3] = 43;
	
	this->character.health_i[1][0] = 33;
	this->character.health_i[1][1] = 59;
	this->character.health_i[1][2] = 33;
	this->character.health_i[1][3] = 43;
	
	//health bar color
	this->character.health_bar = 0xFFFFFFFF;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-130,1);
	this->character.focus_zoom = FIXED_DEC(100,100);
	
	this->character.size = FIXED_DEC(100,100);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\NIKU.ARC;1");
	
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
		"left0.tim",
		"left1.tim",
		"left2.tim",
		"left3.tim",
		"down0.tim",
		"down1.tim",
		"down2.tim",
		"down3.tim",
		"down4.tim",
		"down5.tim",
		"down6.tim",
		"up0.tim",
		"up1.tim",
		"up2.tim",
		"up3.tim",
		"up4.tim",
		"up5.tim",
		"up6.tim",
		"up7.tim",
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
