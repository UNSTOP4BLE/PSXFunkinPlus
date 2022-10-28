#include "hypno.h"

#include "../../psx/mem.h"
#include "../../psx/archive.h"
#include "../../stage/stage.h"
#include "../../main.h"

//Hypno character structure
enum
{
	Hypno_ArcMain_Idle0,
	Hypno_ArcMain_Idle1,
	Hypno_ArcMain_Idle2,
	Hypno_ArcMain_Idle3,
	Hypno_ArcMain_Idle4,
	Hypno_ArcMain_Idle5,
	
	Hypno_ArcMain_Left0,
	Hypno_ArcMain_Left1,
	Hypno_ArcMain_Left2,
	Hypno_ArcMain_Left3,
	Hypno_ArcMain_Left4,
	
	Hypno_ArcMain_Down0,
	Hypno_ArcMain_Down1,
	Hypno_ArcMain_Down2,
	Hypno_ArcMain_Down3,
	Hypno_ArcMain_Down4,
	
	Hypno_ArcMain_Up0,
	Hypno_ArcMain_Up1,
	Hypno_ArcMain_Up2,
	Hypno_ArcMain_Up3,
	Hypno_ArcMain_Up4,
	Hypno_ArcMain_Up5,
	
	Hypno_ArcMain_Right0,
	Hypno_ArcMain_Right1,
	Hypno_ArcMain_Right2,
	Hypno_ArcMain_Right3,
	Hypno_ArcMain_Right4,
	
	Hypno_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Hypno_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Hypno;

//Hypno character definitions
static const CharFrame char_hypno_frame[] = {
	{Hypno_ArcMain_Idle0,{0,0,216,164},{110,160}},
	{Hypno_ArcMain_Idle1,{0,0,218,166},{112,161}},
	{Hypno_ArcMain_Idle2,{0,0,218,168},{113,164}},
	{Hypno_ArcMain_Idle3,{0,0,216,170},{112,166}},
	{Hypno_ArcMain_Idle4,{0,0,214,172},{111,167}},
	{Hypno_ArcMain_Idle5,{0,0,214,172},{111,167}},
	
	{Hypno_ArcMain_Left0,{0,0,238,176},{147,171}},
	{Hypno_ArcMain_Left1,{0,0,230,160},{126,155}},
	{Hypno_ArcMain_Left2,{0,0,222,158},{118,154}},
	{Hypno_ArcMain_Left3,{0,0,218,160},{114,156}},
	{Hypno_ArcMain_Left4,{0,0,216,162},{111,157}},
	
	{Hypno_ArcMain_Down0,{0,0,170,134},{72,131}},
	{Hypno_ArcMain_Down1,{0,0,174,138},{78,134}},
	{Hypno_ArcMain_Down2,{0,0,170,142},{77,137}},
	{Hypno_ArcMain_Down3,{0,0,172,142},{78,138}},
	{Hypno_ArcMain_Down4,{0,0,172,144},{79,140}},
	
	{Hypno_ArcMain_Up0,{0,0,160,226},{91,220}},
	{Hypno_ArcMain_Up1,{0,0,166,212},{93,207}},
	{Hypno_ArcMain_Up2,{0,0,166,209},{91,205}},
	{Hypno_ArcMain_Up3,{0,0,168,208},{90,203}},
	{Hypno_ArcMain_Up4,{0,0,168,208},{90,202}},
	{Hypno_ArcMain_Up5,{0,0,168,208},{90,202}},
	
	{Hypno_ArcMain_Right0,{0,0,202,188},{56,185}},
	{Hypno_ArcMain_Right1,{0,0,204,188},{57,185}},
	{Hypno_ArcMain_Right2,{0,0,194,182},{56,179}},
	{Hypno_ArcMain_Right3,{0,0,188,174},{56,171}},
	{Hypno_ArcMain_Right4,{0,0,188,174},{56,171}},
};

static const Animation char_hypno_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  4,  5, ASCR_BACK, 0}}, //CharAnim_Idle
	{2, (const u8[]){ 6,  7,  8,  9, 10, ASCR_BACK, 0}},          //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){11, 12, 13, 14, 15, ASCR_BACK, 0}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){16, 17, 18, 19, 20, 21, ASCR_BACK, 0}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){22, 23, 24, 25, 26, ASCR_BACK, 0}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Hypno character functions
void Char_Hypno_SetFrame(void *user, u8 frame)
{
	Char_Hypno *this = (Char_Hypno*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_hypno_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Hypno_Tick(Character *character)
{
	Char_Hypno *this = (Char_Hypno*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Hypno_SetFrame);
	Character_Draw(character, &this->tex, &char_hypno_frame[this->frame]);
}

void Char_Hypno_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Hypno_Free(Character *character)
{
	Char_Hypno *this = (Char_Hypno*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Hypno_New(fixed_t x, fixed_t y)
{
	//Allocate hypno object
	Char_Hypno *this = Mem_Alloc(sizeof(Char_Hypno));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Hypno_New] Failed to allocate hypno object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Hypno_Tick;
	this->character.set_anim = Char_Hypno_SetAnim;
	this->character.free = Char_Hypno_Free;
	
	Animatable_Init(&this->character.animatable, char_hypno_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;
	
	this->character.focus_x = FIXED_DEC(20,1);
	this->character.focus_y = FIXED_DEC(-90,1);
	this->character.focus_zoom = FIXED_DEC(9,10);
	
	this->character.size = FIXED_DEC(6,5);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\HYPNO.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //Hypno_ArcMain_Idle0
		"idle1.tim", //Hypno_ArcMain_Idle1
		"idle2.tim", //Hypno_ArcMain_Idle2
		"idle3.tim", //Hypno_ArcMain_Idle3
		"idle4.tim", //Hypno_ArcMain_Idle4
		"idle5.tim", //Hypno_ArcMain_Idle5
		
		"left0.tim", //Hypno_ArcMain_Left0
		"left1.tim", //Hypno_ArcMain_Left1
		"left2.tim", //Hypno_ArcMain_Left2
		"left3.tim", //Hypno_ArcMain_Left3
		"left4.tim", //Hypno_ArcMain_Left4
		
		"down0.tim", //Hypno_ArcMain_Down0
		"down1.tim", //Hypno_ArcMain_Down1
		"down2.tim", //Hypno_ArcMain_Down2
		"down3.tim", //Hypno_ArcMain_Down3
		"down4.tim", //Hypno_ArcMain_Down4
		
		"up0.tim", //Hypno_ArcMain_Up0
		"up1.tim", //Hypno_ArcMain_Up1
		"up2.tim", //Hypno_ArcMain_Up2
		"up3.tim", //Hypno_ArcMain_Up3
		"up4.tim", //Hypno_ArcMain_Up4
		"up5.tim", //Hypno_ArcMain_Up5
		
		"right0.tim", //Hypno_ArcMain_Right0
		"right1.tim", //Hypno_ArcMain_Right1
		"right2.tim", //Hypno_ArcMain_Right2
		"right3.tim", //Hypno_ArcMain_Right3
		"right4.tim", //Hypno_ArcMain_Right4
		
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
