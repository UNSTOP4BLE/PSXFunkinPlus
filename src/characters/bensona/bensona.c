#include "bensona.h"

#include "../../psx/mem.h"
#include "../../psx/archive.h"
#include "../../scenes/stage/stage.h"
#include "../../main.h"

BensonA bensona;

//BensonA character structure
enum
{
  BensonA_ArcMain_Idle0,
  BensonA_ArcMain_Left0,
  BensonA_ArcMain_Down0,
  BensonA_ArcMain_Up0,
  BensonA_ArcMain_Right0,
  BensonA_ArcMain_LeftAlt0,
  BensonA_ArcMain_IdleAlt0,
  BensonA_ArcMain_UpAlt0,
  BensonA_ArcMain_RightAlt0,
	
	BensonA_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[BensonA_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_BensonA;

//BensonA character definitions
static const CharFrame char_bensona_frame[] = {
  {BensonA_ArcMain_Idle0, {  0,  0, 58, 99}, {182,183}}, //0 idle 1
  {BensonA_ArcMain_Idle0, { 60,  0, 58, 99}, {182,183}}, //1 idle 2
  {BensonA_ArcMain_Idle0, {120,  0, 58, 99}, {182,183}}, //2 idle 3
  {BensonA_ArcMain_Idle0, {180,  0, 58, 99}, {182,183}}, //3 idle 4
  {BensonA_ArcMain_Idle0, {  0,101, 58, 99}, {182,183}}, //4 idle 5
  {BensonA_ArcMain_Idle0, { 60,101, 58, 99}, {182,183}}, //5 idle 6

  {BensonA_ArcMain_Left0, {  0,  0, 64, 82}, {187,181}}, //6 left 1
  {BensonA_ArcMain_Left0, { 66,  0, 64, 82}, {187,181}}, //7 left 2
  {BensonA_ArcMain_Left0, {132,  0, 64, 82}, {187,181}}, //8 left 3
  {BensonA_ArcMain_Left0, {  0, 84, 64, 82}, {187,181}}, //9 left 4

  {BensonA_ArcMain_Down0, {  0,  0, 60, 74}, {182,178}}, //10 down 1
  {BensonA_ArcMain_Down0, { 62,  0, 61, 74}, {182,178}}, //11 down 2
  {BensonA_ArcMain_Down0, {125,  0, 60, 74}, {182,178}}, //12 down 3
  {BensonA_ArcMain_Down0, {187,  0, 61, 74}, {182,178}}, //13 down 4

  {BensonA_ArcMain_Up0, {  0,  0, 55,113}, {168,196}}, //14 up 1
  {BensonA_ArcMain_Up0, { 57,  0, 55,113}, {168,196}}, //15 up 2
  {BensonA_ArcMain_Up0, {114,  0, 55,113}, {168,196}}, //16 up 3
  {BensonA_ArcMain_Up0, {171,  0, 55,113}, {168,196}}, //17 up 4

  {BensonA_ArcMain_Right0, {  0,  0, 79, 97}, {182,185}}, //18 right 1
  {BensonA_ArcMain_Right0, { 81,  0, 79, 97}, {182,185}}, //19 right 2
  {BensonA_ArcMain_Right0, {162,  0, 79, 97}, {182,185}}, //20 right 3
  {BensonA_ArcMain_Right0, {  0, 99, 79, 97}, {182,185}}, //21 right 4

  {BensonA_ArcMain_LeftAlt0, {  0,  0, 56, 82}, {187,182}}, //22 leftalt 1
  {BensonA_ArcMain_LeftAlt0, { 58,  0, 56, 82}, {187,182}}, //23 leftalt 2
  {BensonA_ArcMain_LeftAlt0, {116,  0, 56, 82}, {187,182}}, //24 leftalt 3

  {BensonA_ArcMain_IdleAlt0, {  0,  0, 68, 99}, {183,183}}, //25 idlealt 1
  {BensonA_ArcMain_IdleAlt0, { 70,  0, 68, 99}, {183,183}}, //26 idlealt 2
  {BensonA_ArcMain_IdleAlt0, {140,  0, 68, 99}, {183,183}}, //27 idlealt 3
  {BensonA_ArcMain_IdleAlt0, {  0,101, 68, 99}, {183,183}}, //28 idlealt 4
  {BensonA_ArcMain_IdleAlt0, { 70,101, 68, 99}, {183,183}}, //29 idlealt 5
  {BensonA_ArcMain_IdleAlt0, {140,101, 68, 99}, {183,183}}, //30 idlealt 6

  {BensonA_ArcMain_UpAlt0, {  0,  0, 41,107}, {172,188}}, //31 upalt 1
  {BensonA_ArcMain_UpAlt0, { 43,  0, 41,107}, {172,188}}, //32 upalt 2
  {BensonA_ArcMain_UpAlt0, { 86,  0, 41,107}, {172,188}}, //33 upalt 3

  {BensonA_ArcMain_RightAlt0, {  0,  0, 60, 95}, {175,182}}, //34 rightalt 1
  {BensonA_ArcMain_RightAlt0, { 62,  0, 60, 95}, {175,182}}, //35 rightalt 2
  {BensonA_ArcMain_RightAlt0, {124,  0, 60, 95}, {175,182}}, //36 rightalt 3
};

static const Animation char_bensona_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  4,  5,  ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 6,  7,  8,  9,  ASCR_BACK, 1}},    //CharAnim_Left
	{2, (const u8[]){22, 23, 24,  ASCR_BACK, 1}},       //CharAnim_LeftAlt
	{2, (const u8[]){10, 11, 12, 13,  ASCR_BACK, 1}},     //CharAnim_Down
	{2, (const u8[]){10, 11, 12, 13,  ASCR_BACK, 1}},       //CharAnim_DownAlt
	{2, (const u8[]){14, 15, 16, 17,  ASCR_BACK, 1}},     //CharAnim_Up
	{2, (const u8[]){31, 32, 33,  ASCR_BACK, 1}},       //CharAnim_UpAlt
	{2, (const u8[]){18, 19, 20, 21,  ASCR_BACK, 1}},     //CharAnim_Right
	{2, (const u8[]){34, 35, 36,  ASCR_BACK, 1}},       //CharAnim_RightAlt
};

static const Animation char_bensonasec_anim[CharAnim_Max] = {
	{2, (const u8[]){ 25,  26,  27,  28,  29,  30,  ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 6,  7,  8,  9,  ASCR_BACK, 1}},    //CharAnim_Left
	{2, (const u8[]){22, 23, 24,  ASCR_BACK, 1}},       //CharAnim_LeftAlt
	{2, (const u8[]){10, 11, 12, 13,  ASCR_BACK, 1}},     //CharAnim_Down
	{2, (const u8[]){10, 11, 12, 13,  ASCR_BACK, 1}},       //CharAnim_DownAlt
	{2, (const u8[]){14, 15, 16, 17,  ASCR_BACK, 1}},     //CharAnim_Up
	{2, (const u8[]){31, 32, 33,  ASCR_BACK, 1}},       //CharAnim_UpAlt
	{2, (const u8[]){18, 19, 20, 21,  ASCR_BACK, 1}},     //CharAnim_Right
	{2, (const u8[]){34, 35, 36,  ASCR_BACK, 1}},       //CharAnim_RightAlt
};

//BensonA character functions
void Char_BensonA_SetFrame(void *user, u8 frame)
{
	Char_BensonA *this = (Char_BensonA*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_bensona_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_BensonA_Tick(Character *character)
{
	Char_BensonA *this = (Char_BensonA*)character;
	
	if (bensona.angry2 != bensona.angry)
	{
		if (bensona.angry)
			Animatable_Init(&this->character.animatable, char_bensonasec_anim);
		else
			Animatable_Init(&this->character.animatable, char_bensona_anim);
		bensona.angry2 = bensona.angry;
	}
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_BensonA_SetFrame);
	Character_Draw(character, &this->tex, &char_bensona_frame[this->frame]);
}

void Char_BensonA_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_BensonA_Free(Character *character)
{
	Char_BensonA *this = (Char_BensonA*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_BensonA_New(fixed_t x, fixed_t y)
{
	//Allocate bensona object
	Char_BensonA *this = Mem_Alloc(sizeof(Char_BensonA));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_BensonA_New] Failed to allocate bensona object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_BensonA_Tick;
	this->character.set_anim = Char_BensonA_SetAnim;
	this->character.free = Char_BensonA_Free;
	
	Animatable_Init(&this->character.animatable, char_bensona_anim);
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
	
	this->character.focus_x = FIXED_DEC(-140,1);
	this->character.focus_y = FIXED_DEC(-140,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.size = FIXED_DEC(10,10);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\BENSONA.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "left0.tim",
  "down0.tim",
  "up0.tim",
  "right0.tim",
  "leftalt0.tim",
  "idlealt0.tim",
  "upalt0.tim",
  "rightalt0.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	bensona.angry = false;
	bensona.angry2 = false;
	
	return (Character*)this;
}