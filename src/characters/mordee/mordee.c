#include "mordee.h"

#include "../../psx/mem.h"
#include "../../psx/archive.h"
#include "../../scenes/stage/stage.h"
#include "../../main.h"

//Morde character structure
enum
{
  MordeE_ArcMain_Idle0,
  MordeE_ArcMain_Idle1,
  MordeE_ArcMain_Idle2,
  MordeE_ArcMain_Left0,
  MordeE_ArcMain_Left1,
  MordeE_ArcMain_Down0,
  MordeE_ArcMain_Down1,
  MordeE_ArcMain_Up0,
  MordeE_ArcMain_Up1,
  MordeE_ArcMain_Right0,
  MordeE_ArcMain_Right1,
	
	MordeE_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[MordeE_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_MordeE;

//Morde character definitions
static const CharFrame char_mordee_frame[] = {
  {MordeE_ArcMain_Idle0, {  0,  0, 90,175}, {158,136}}, //0 idle 1
  {MordeE_ArcMain_Idle0, { 90,  0, 90,175}, {158,136}}, //1 idle 2
  {MordeE_ArcMain_Idle1, {  0,  0, 90,175}, {158,136}}, //2 idle 3
  {MordeE_ArcMain_Idle1, { 90,  0, 90,175}, {158,136}}, //3 idle 4
  {MordeE_ArcMain_Idle2, {  0,  0, 90,175}, {158,136}}, //4 idle 5

  {MordeE_ArcMain_Left0, {  0,  0, 97,175}, {181,136}}, //5 left 1
  {MordeE_ArcMain_Left0, { 97,  0, 97,175}, {181,136}}, //6 left 2
  {MordeE_ArcMain_Left1, {  0,  0, 97,175}, {181,136}}, //7 left 3
  {MordeE_ArcMain_Left1, { 97,  0, 97,175}, {181,136}}, //8 left 4

  {MordeE_ArcMain_Down0, {  0,  0, 78,162}, {153,126}}, //9 down 1
  {MordeE_ArcMain_Down0, { 78,  0, 78,162}, {153,126}}, //10 down 2
  {MordeE_ArcMain_Down0, {156,  0, 78,162}, {153,126}}, //11 down 3
  {MordeE_ArcMain_Down1, {  0,  0, 78,162}, {153,126}}, //12 down 4

  {MordeE_ArcMain_Up0, {  0,  0,100,184}, {170,146}}, //13 up 1
  {MordeE_ArcMain_Up0, {100,  0,100,184}, {170,146}}, //14 up 2
  {MordeE_ArcMain_Up1, {  0,  0,100,184}, {170,146}}, //15 up 3
  {MordeE_ArcMain_Up1, {100,  0,100,184}, {170,146}}, //16 up 4

  {MordeE_ArcMain_Right0, {  0,  0,111,174}, {156,135}}, //17 right 1
  {MordeE_ArcMain_Right0, {111,  0,111,174}, {156,135}}, //18 right 2
  {MordeE_ArcMain_Right1, {  0,  0,111,174}, {156,135}}, //19 right 3
  {MordeE_ArcMain_Right1, {111,  0,111,174}, {156,135}}, //20 right 4
};

static const Animation char_mordee_anim[CharAnim_Max] = {
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
void Char_MordeE_SetFrame(void *user, u8 frame)
{
	Char_MordeE *this = (Char_MordeE*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_mordee_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_MordeE_Tick(Character *character)
{
	Char_MordeE *this = (Char_MordeE*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_MordeE_SetFrame);
	Character_Draw(character, &this->tex, &char_mordee_frame[this->frame]);
}

void Char_MordeE_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_MordeE_Free(Character *character)
{
	Char_MordeE *this = (Char_MordeE*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_MordeE_New(fixed_t x, fixed_t y)
{
	//Allocate mordee object
	Char_MordeE *this = Mem_Alloc(sizeof(Char_MordeE));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_MordeE_New] Failed to allocate mordee object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_MordeE_Tick;
	this->character.set_anim = Char_MordeE_SetAnim;
	this->character.free = Char_MordeE_Free;
	
	Animatable_Init(&this->character.animatable, char_mordee_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i[0][0] = 0;
	this->character.health_i[0][1] = 0;
	this->character.health_i[0][2] = 46;
	this->character.health_i[0][3] = 30;
	
	this->character.health_i[1][0] = 47;
	this->character.health_i[1][1] = 0;
	this->character.health_i[1][2] = 43;
	this->character.health_i[1][3] = 35;
	
	//health bar color
	this->character.health_bar = 0xFFA5004A;
	
	this->character.focus_x = FIXED_DEC(-50,1);
	this->character.focus_y = FIXED_DEC(-40,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.size = FIXED_DEC(10,10);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\MORDEE.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "idle2.tim",
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
