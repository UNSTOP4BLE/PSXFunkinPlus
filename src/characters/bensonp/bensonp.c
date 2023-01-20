#include "bensonp.h"

#include "../../psx/mem.h"
#include "../../psx/archive.h"
#include "../../scenes/stage/stage.h"
#include "../../main.h"

//BensonP character structure
enum
{
	BensonP_ArcMain_Idle0,
	BensonP_ArcMain_Idle1,
	BensonP_ArcMain_Idle2,
	BensonP_ArcMain_Idle3,
	BensonP_ArcMain_Idle4,
	BensonP_ArcMain_Idle5,
	BensonP_ArcMain_Left0,
	BensonP_ArcMain_Left1,
	BensonP_ArcMain_Left2,
	BensonP_ArcMain_Left3,
	BensonP_ArcMain_Down0,
	BensonP_ArcMain_Down1,
	BensonP_ArcMain_Down2,
	BensonP_ArcMain_Down3,
	BensonP_ArcMain_Down4,
	BensonP_ArcMain_Up0,
	BensonP_ArcMain_Up1,
	BensonP_ArcMain_Up2,
	BensonP_ArcMain_Right0,
	BensonP_ArcMain_Right1,
	BensonP_ArcMain_Right2,
	BensonP_ArcMain_Right3,
	
	BensonP_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[BensonP_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_BensonP;

//BensonP character definitions
static const CharFrame char_bensonp_frame[] = {
  {BensonP_ArcMain_Idle0, {  0,  0,149,121}, {150, 84}}, //0 idle 1
  {BensonP_ArcMain_Idle0, {  0,123,150,120}, {150, 83}}, //1 idle 2
  {BensonP_ArcMain_Idle1, {  0,  0,155,122}, {150, 85}}, //2 idle 3
  {BensonP_ArcMain_Idle1, {  0,124,154,122}, {150, 85}}, //3 idle 4
  {BensonP_ArcMain_Idle2, {  0,  0,152,127}, {150, 90}}, //4 idle 5
  {BensonP_ArcMain_Idle3, {  0,  0,152,127}, {150, 90}}, //5 idle 6
  {BensonP_ArcMain_Idle4, {  0,  0,150,127}, {150, 90}}, //6 idle 7
  {BensonP_ArcMain_Idle5, {  0,  0,157,126}, {150, 89}}, //7 idle 8
  {BensonP_ArcMain_Idle5, {  0,128,146,126}, {150, 89}}, //8 idle 9

  {BensonP_ArcMain_Left0, {  0,  0,137,144}, {205,107}}, //9 left 1
  {BensonP_ArcMain_Left1, {  0,  0,133,149}, {197,112}}, //10 left 2
  {BensonP_ArcMain_Left2, {  0,  0,133,150}, {197,113}}, //11 left 3
  {BensonP_ArcMain_Left3, {  0,  0,132,149}, {196,112}}, //12 left 4

  {BensonP_ArcMain_Down0, {  0,  0,146,150}, {170, 88}}, //13 down 1
  {BensonP_ArcMain_Down1, {  0,  0,147,157}, {177, 95}}, //14 down 2
  {BensonP_ArcMain_Down2, {  0,  0,148,156}, {178, 94}}, //15 down 3
  {BensonP_ArcMain_Down3, {  0,  0,140,156}, {171, 94}}, //16 down 4
  {BensonP_ArcMain_Down4, {  0,  0,140,156}, {171, 94}}, //17 down 5

  {BensonP_ArcMain_Up0, {  0,  0,132,220}, {193,184}}, //18 up 1
  {BensonP_ArcMain_Up1, {  0,  0,133,220}, {188,184}}, //19 up 2
  {BensonP_ArcMain_Up2, {  0,  0,118,214}, {184,178}}, //20 up 3
  {BensonP_ArcMain_Up2, {120,  0,104,214}, {182,178}}, //21 up 4

  {BensonP_ArcMain_Right0, {  0,  0,172,152}, {150,115}}, //22 right 1
  {BensonP_ArcMain_Right1, {  0,  0,168,148}, {150,111}}, //23 right 2
  {BensonP_ArcMain_Right2, {  0,  0,165,149}, {150,112}}, //24 right 3
  {BensonP_ArcMain_Right3, {  0,  0,165,149}, {150,112}}, //25 right 4
};

static const Animation char_bensonp_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  4, 5, 6, 7, 8, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){10, 11, 12, 13,  ASCR_BACK, 1}},    //CharAnim_Left
	{2, (const u8[]){ 1,  5,  ASCR_BACK, 1}},       //CharAnim_LeftAlt
	{2, (const u8[]){14, 15, 16, 17,  ASCR_BACK, 1}},     //CharAnim_Down
	{2, (const u8[]){ 6,  7,  ASCR_BACK, 1}},       //CharAnim_DownAlt
	{2, (const u8[]){18, 19, 20, 21,  ASCR_BACK, 1}},     //CharAnim_Up
	{2, (const u8[]){ 8,  9,  ASCR_BACK, 1}},       //CharAnim_UpAlt
	{2, (const u8[]){22, 23, 24, 25,  ASCR_BACK, 1}},     //CharAnim_Right
	{2, (const u8[]){10, 11,  ASCR_BACK, 1}},       //CharAnim_RightAlt
};

//BensonP character functions
void Char_BensonP_SetFrame(void *user, u8 frame)
{
	Char_BensonP *this = (Char_BensonP*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_bensonp_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_BensonP_Tick(Character *character)
{
	Char_BensonP *this = (Char_BensonP*)character;
	Animatable_Init(&this->character.animatable, char_bensonp_anim);
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_BensonP_SetFrame);
	Character_Draw(character, &this->tex, &char_bensonp_frame[this->frame]);
}

void Char_BensonP_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_BensonP_Free(Character *character)
{
	Char_BensonP *this = (Char_BensonP*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_BensonP_New(fixed_t x, fixed_t y)
{
	//Allocate bensonp object
	Char_BensonP *this = Mem_Alloc(sizeof(Char_BensonP));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_BensonP_New] Failed to allocate bensonp object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_BensonP_Tick;
	this->character.set_anim = Char_BensonP_SetAnim;
	this->character.free = Char_BensonP_Free;
	
	Animatable_Init(&this->character.animatable, char_bensonp_anim);
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
	
	this->character.focus_x = FIXED_DEC(-36,1);
	this->character.focus_y = FIXED_DEC(-60,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.size = FIXED_DEC(10,10);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\BENSONP.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "idle2.tim",
  "idle3.tim",
  "idle4.tim",
  "idle5.tim",
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
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}