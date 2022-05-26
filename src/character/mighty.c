/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

//thx igor for makin the offsets for this ^^

#include "Mighty.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Mighty character structure
enum
{
	Mighty_ArcMain_Idle0,
	Mighty_ArcMain_Idle1,
	Mighty_ArcMain_Left0,
	Mighty_ArcMain_Left1,
	Mighty_ArcMain_Down,
	Mighty_ArcMain_Up,
	Mighty_ArcMain_Right0,
	Mighty_ArcMain_Right1,
	
	Mighty_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Mighty_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Mighty;

//Mighty character definitions
static const CharFrame char_Mighty_frame[] = {
	{Mighty_ArcMain_Idle0, {  0,   1,  116, 144}, { 58, 144}}, //0 idle 1
	{Mighty_ArcMain_Idle0, { 116,   1,  115, 145}, { 58, 145}}, //1 idle 2
	{Mighty_ArcMain_Idle1, {  0,   0,  113, 147}, { 57, 147}}, //2 idle 3
	{Mighty_ArcMain_Idle1, { 113,   0,  114, 148}, { 57, 148}}, //3 idle 4
	
	{Mighty_ArcMain_Left0, {  0,   0, 174, 146}, { 87, 146}}, //5 left 1
	{Mighty_ArcMain_Left1, {  0, 0, 172, 146}, { 86, 146}}, //6 left 2
	
	{Mighty_ArcMain_Down, {  0,   0, 168, 117}, {84, 117}}, //7 down 1
	{Mighty_ArcMain_Down, {0,   117, 160, 121}, {80, 121}}, //8 down 2
	
	{Mighty_ArcMain_Up, {  0,   0, 104, 175}, { 52, 175}}, //9 up 1
	{Mighty_ArcMain_Up, {104,   0, 108, 175}, { 54, 175}}, //10 up 2
	
	{Mighty_ArcMain_Right0, {  0,   0, 180, 163}, { 90, 163}}, //11 right 1
	{Mighty_ArcMain_Right1, {  0, 0, 176, 164}, { 88, 164}}, //12 right 2
};

static const Animation char_Mighty_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3, ASCR_BACK, 0}}, //CharAnim_Idle
	{1, (const u8[]){ 4,  5, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{1, (const u8[]){ 6,  7, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{1, (const u8[]){ 8, 9, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{1, (const u8[]){10, 11, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Mighty character functions
void Char_Mighty_SetFrame(void *user, u8 frame)
{
	Char_Mighty *this = (Char_Mighty*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_Mighty_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Mighty_Tick(Character *character)
{
	Char_Mighty *this = (Char_Mighty*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Mighty_SetFrame);
	Character_Draw(character, &this->tex, &char_Mighty_frame[this->frame]);
}

void Char_Mighty_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Mighty_Free(Character *character)
{
	Char_Mighty *this = (Char_Mighty*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Mighty_New(fixed_t x, fixed_t y)
{
	//Allocate Mighty object
	Char_Mighty *this = Mem_Alloc(sizeof(Char_Mighty));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Mighty_New] Failed to allocate Mighty object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Mighty_Tick;
	this->character.set_anim = Char_Mighty_SetAnim;
	this->character.free = Char_Mighty_Free;
	
	Animatable_Init(&this->character.animatable, char_Mighty_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;
	
	this->character.focus_x = FIXED_DEC(45,1);
	this->character.focus_y = FIXED_DEC(-95,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\MIGHTY.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //Mighty_ArcMain_Idle0
		"idle1.tim", //Mighty_ArcMain_Idle1
		"left0.tim",  //Mighty_ArcMain_Left0
		"left1.tim",  //Mighty_ArcMain_Left1
		"down.tim",  //Mighty_ArcMain_Down
		"up.tim",    //Mighty_ArcMain_Up
		"right0.tim", //Mighty_ArcMain_Right0
		"right1.tim", //Mighty_ArcMain_Right1
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
