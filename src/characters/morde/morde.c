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
  Morde_ArcMain_Idle2,
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
  {Morde_ArcMain_Idle0, {  0,  0, 88,169}, {148,122}}, //0 idle 1
  {Morde_ArcMain_Idle0, { 90,  0, 86,171}, {148,123}}, //1 idle 2
  {Morde_ArcMain_Idle1, {  0,  0, 84,173}, {148,125}}, //2 idle 3
  {Morde_ArcMain_Idle1, { 86,  0, 84,172}, {147,125}}, //3 idle 4
  {Morde_ArcMain_Idle2, {  0,  0, 84,172}, {147,125}}, //4 idle 5

  {Morde_ArcMain_Left0, {  0,  0, 89,171}, {171,124}}, //5 left 1
  {Morde_ArcMain_Left0, { 91,  0, 83,172}, {163,125}}, //6 left 2
  {Morde_ArcMain_Left1, {  0,  0, 80,173}, {160,125}}, //7 left 3
  {Morde_ArcMain_Left1, { 82,  0, 80,173}, {159,125}}, //8 left 4

  {Morde_ArcMain_Down0, {  0,  0, 74,155}, {142,111}}, //9 down 1
  {Morde_ArcMain_Down0, { 76,  0, 76,160}, {142,116}}, //10 down 2
  {Morde_ArcMain_Down0, {154,  0, 75,160}, {141,116}}, //11 down 3
  {Morde_ArcMain_Down1, {  0,  0, 76,160}, {142,116}}, //12 down 4

  {Morde_ArcMain_Up0, {  0,  0, 92,182}, {160,136}}, //13 up 1
  {Morde_ArcMain_Up0, { 94,  0, 95,175}, {158,129}}, //14 up 2
  {Morde_ArcMain_Up1, {  0,  0, 93,176}, {158,130}}, //15 up 3
  {Morde_ArcMain_Up1, { 95,  0, 91,177}, {157,130}}, //16 up 4

  {Morde_ArcMain_Right0, {  0,  0,110,170}, {146,123}}, //17 right 1
  {Morde_ArcMain_Right0, {112,  0,104,172}, {146,125}}, //18 right 2
  {Morde_ArcMain_Right1, {  0,  0,104,172}, {146,125}}, //19 right 3
  {Morde_ArcMain_Right1, {106,  0,104,172}, {146,125}}, //20 right 4
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
	this->arc_main = IO_Read("\\CHAR\\MORDE.ARC;1");
	
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
