#include "rigby.h"

#include "../../psx/mem.h"
#include "../../psx/archive.h"
#include "../../scenes/stage/stage.h"
#include "../../main.h"

//Rigby character structure
enum
{
  Rigby_ArcMain_Idle0,
  Rigby_ArcMain_Idle1,
  Rigby_ArcMain_Left0,
  Rigby_ArcMain_Down0,
  Rigby_ArcMain_Up0,
  Rigby_ArcMain_Up1,
  Rigby_ArcMain_Right0,
  Rigby_ArcMain_Right1,
  Rigby_ArcMain_LeftAlt0,
  Rigby_ArcMain_DownAlt0,
  Rigby_ArcMain_UpAlt0,
  Rigby_ArcMain_UpAlt1,
  Rigby_ArcMain_RightAlt0,
  Rigby_ArcMain_RightAlt1,
	
	Rigby_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Rigby_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Rigby;

//Rigby character definitions
static const CharFrame char_rigby_frame[] = {
  {Rigby_ArcMain_Idle0, {  0,  0,111,113}, {170, 75}}, //0 idle 1
  {Rigby_ArcMain_Idle0, {113,  0,111,113}, {170, 75}}, //1 idle 2
  {Rigby_ArcMain_Idle0, {  0,115,111,113}, {170, 75}}, //2 idle 3
  {Rigby_ArcMain_Idle0, {113,115,111,113}, {170, 75}}, //3 idle 4
  {Rigby_ArcMain_Idle1, {  0,  0,111,113}, {170, 75}}, //4 idle 5
  {Rigby_ArcMain_Idle1, {113,  0,111,112}, {170, 75}}, //5 idle 6
  {Rigby_ArcMain_Idle1, {  0,115,111,112}, {170, 75}}, //6 idle 7
  {Rigby_ArcMain_Idle1, {113,115,111,112}, {170, 75}}, //7 idle 8

  {Rigby_ArcMain_Left0, {  0,  0,113,112}, {186, 75}}, //8 left 1
  {Rigby_ArcMain_Left0, {115,  0,112,112}, {186, 75}}, //9 left 2
  {Rigby_ArcMain_Left0, {  0,114,113,112}, {186, 75}}, //10 left 3
  {Rigby_ArcMain_Left0, {115,114,112,112}, {186, 75}}, //11 left 4

  {Rigby_ArcMain_Down0, {  0,  0,116,107}, {174, 70}}, //12 down 1
  {Rigby_ArcMain_Down0, {118,  0,116,107}, {174, 70}}, //13 down 2
  {Rigby_ArcMain_Down0, {  0,109,116,107}, {174, 70}}, //14 down 3
  {Rigby_ArcMain_Down0, {118,109,116,107}, {174, 70}}, //15 down 4

  {Rigby_ArcMain_Up0, {  0,  0, 97,150}, {169,111}}, //16 up 1
  {Rigby_ArcMain_Up0, { 99,  0, 97,150}, {169,111}}, //17 up 2
  {Rigby_ArcMain_Up1, {  0,  0, 97,144}, {169,111+6}}, //18 up 3
  {Rigby_ArcMain_Up1, { 99,  0, 92,144}, {169,111+6}}, //19 up 4

  {Rigby_ArcMain_Right0, {  0,  0,130,112}, {173, 76}}, //20 right 1
  {Rigby_ArcMain_Right0, {  0,114,130,112}, {173, 76}}, //21 right 2
  {Rigby_ArcMain_Right1, {  0,  0,130,112}, {173, 76}}, //22 right 3
  {Rigby_ArcMain_Right1, {  0,114,130,112}, {173, 76}}, //23 right 4

  {Rigby_ArcMain_LeftAlt0, {  0,  0,113,112}, {185, 75}}, //24 leftalt 1
  {Rigby_ArcMain_LeftAlt0, {115,  0,112,112}, {185, 75}}, //25 leftalt 2
  {Rigby_ArcMain_LeftAlt0, {  0,114,113,112}, {185, 75}}, //26 leftalt 3
  {Rigby_ArcMain_LeftAlt0, {115,114,112,112}, {185, 75}}, //27 leftalt 4

  {Rigby_ArcMain_DownAlt0, {  0,  0,116,107}, {174, 70}}, //28 downalt 1
  {Rigby_ArcMain_DownAlt0, {118,  0,116,107}, {174, 70}}, //29 downalt 2
  {Rigby_ArcMain_DownAlt0, {  0,109,116,107}, {174, 70}}, //30 downalt 3
  {Rigby_ArcMain_DownAlt0, {118,109,116,107}, {174, 70}}, //31 downalt 4

  {Rigby_ArcMain_UpAlt0, {  0,  0, 84,151}, {170,110}}, //32 upalt 1
  {Rigby_ArcMain_UpAlt0, { 86,  0, 84,151}, {170,110}}, //33 upalt 2
  {Rigby_ArcMain_UpAlt1, {  0,  0, 84,151}, {170,110}}, //34 upalt 3
  {Rigby_ArcMain_UpAlt1, { 86,  0, 84,151}, {170,110}}, //35 upalt 4

  {Rigby_ArcMain_RightAlt0, {  0,  0,130,112}, {174, 75}}, //36 rightalt 1
  {Rigby_ArcMain_RightAlt0, {  0,114,130,112}, {174, 75}}, //37 rightalt 2
  {Rigby_ArcMain_RightAlt1, {  0,  0,130,112}, {174, 75}}, //38 rightalt 3
  {Rigby_ArcMain_RightAlt1, {  0,114,130,112}, {174, 75}}, //39 rightalt 4
};

static const Animation char_rigby_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  4,  5,  6,  7, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 8,  9,  10,  11,  ASCR_BACK, 1}},    //CharAnim_Left
	{2, (const u8[]){ 24,  25,  26,  27,  ASCR_BACK, 1}},       //CharAnim_LeftAlt
	{2, (const u8[]){ 12, 13, 14, 15, ASCR_BACK, 1}},     //CharAnim_Down
	{2, (const u8[]){28, 29, 30, 31, ASCR_BACK, 1}},       //CharAnim_DownAlt
	{2, (const u8[]){16, 17, 18, 19, ASCR_BACK, 1}},     //CharAnim_Up
	{2, (const u8[]){32, 33, 34, 35, ASCR_BACK, 1}},       //CharAnim_UpAlt
	{2, (const u8[]){20, 21, 22, 23, ASCR_BACK, 1}},     //CharAnim_Right
	{2, (const u8[]){36, 37, 38, 39, ASCR_BACK, 1}},       //CharAnim_RightAlt
};

//Rigby character functions
void Char_Rigby_SetFrame(void *user, u8 frame)
{
	Char_Rigby *this = (Char_Rigby*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_rigby_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Rigby_Tick(Character *character)
{
	Char_Rigby *this = (Char_Rigby*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Rigby_SetFrame);
	Character_Draw(character, &this->tex, &char_rigby_frame[this->frame]);
}

void Char_Rigby_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Rigby_Free(Character *character)
{
	Char_Rigby *this = (Char_Rigby*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Rigby_New(fixed_t x, fixed_t y)
{
	//Allocate rigby object
	Char_Rigby *this = Mem_Alloc(sizeof(Char_Rigby));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Rigby_New] Failed to allocate rigby object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Rigby_Tick;
	this->character.set_anim = Char_Rigby_SetAnim;
	this->character.free = Char_Rigby_Free;
	
	Animatable_Init(&this->character.animatable, char_rigby_anim);
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
	this->arc_main = IO_Read("\\CHAR\\RIGBY.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "left0.tim",
  "down0.tim",
  "up0.tim",
  "up1.tim",
  "right0.tim",
  "right1.tim",
  "leftalt0.tim",
  "downalt0.tim",
  "upalt0.tim",
  "upalt1.tim",
  "rightalt0.tim",
  "rightalt1.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
