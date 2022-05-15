/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

//thx igor for makin the offsets for this ^^

#include "cuphead.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Cuphead character structure
enum
{
	Cuphead_ArcMain_Idle0,
	Cuphead_ArcMain_Idle1,
	Cuphead_ArcMain_Idle2,
	Cuphead_ArcMain_Left,
	Cuphead_ArcMain_Down,
	Cuphead_ArcMain_Up,
	Cuphead_ArcMain_Right0,
	Cuphead_ArcMain_Right1,
	
	Cuphead_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Cuphead_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Cuphead;

//Cuphead character definitions
static const CharFrame char_cuphead_frame[] = {
	{Cuphead_ArcMain_Idle0, {  0,   0,  96, 150}, { 49, 148}}, //0 idle 1
	{Cuphead_ArcMain_Idle0, { 97,   0,  95, 152}, { 49, 150}}, //1 idle 2
	{Cuphead_ArcMain_Idle1, {  0,   0,  97, 154}, { 49, 152}}, //2 idle 3
	{Cuphead_ArcMain_Idle1, { 98,   0,  94, 155}, { 49, 153}}, //3 idle 4
	{Cuphead_ArcMain_Idle2, {  0,   0,  96, 150}, { 49, 148}}, //4 idle 5
	
	{Cuphead_ArcMain_Left, {  0,   0, 140, 112}, { 80, 107}}, //5 left 1
	{Cuphead_ArcMain_Left, {  0, 114, 121, 122}, { 73, 116}}, //6 left 2
	
	{Cuphead_ArcMain_Down, {  0,   0, 133, 113}, {56, 110}}, //7 down 1
	{Cuphead_ArcMain_Down, {135,   0, 115, 125}, {48, 122}}, //8 down 2
	
	{Cuphead_ArcMain_Up, {  0,   0, 102, 160}, { 62, 158}}, //9 up 1
	{Cuphead_ArcMain_Up, {104,   0, 114, 143}, { 70, 141}}, //10 up 2
	
	{Cuphead_ArcMain_Right0, {  0,   0, 146, 132}, { 47, 129}}, //11 right 1
	{Cuphead_ArcMain_Right1, {  0,   0, 128, 134}, { 44, 132}}, //12 right 2
};

static const Animation char_cuphead_anim[CharAnim_Max] = {
	{1, (const u8[]){ 0,  1,  2,  3,  4, ASCR_CHGANI, CharAnim_Idle, 1}}, //CharAnim_Idle
	{1, (const u8[]){ 5,  6, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{1, (const u8[]){ 7,  8, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{1, (const u8[]){ 9, 10, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{1, (const u8[]){11, 12, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Cuphead character functions
void Char_Cuphead_SetFrame(void *user, u8 frame)
{
	Char_Cuphead *this = (Char_Cuphead*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_cuphead_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Cuphead_Tick(Character *character)
{
	Char_Cuphead *this = (Char_Cuphead*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Cuphead_SetFrame);
	Character_Draw(character, &this->tex, &char_cuphead_frame[this->frame]);
}

void Char_Cuphead_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Cuphead_Free(Character *character)
{
	Char_Cuphead *this = (Char_Cuphead*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Cuphead_New(fixed_t x, fixed_t y)
{
	//Allocate cuphead object
	Char_Cuphead *this = Mem_Alloc(sizeof(Char_Cuphead));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Cuphead_New] Failed to allocate cuphead object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Cuphead_Tick;
	this->character.set_anim = Char_Cuphead_SetAnim;
	this->character.free = Char_Cuphead_Free;
	
	Animatable_Init(&this->character.animatable, char_cuphead_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;
	
	this->character.focus_x = FIXED_DEC(45,1);
	this->character.focus_y = FIXED_DEC(-95,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\CUPHEAD.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //Cuphead_ArcMain_Idle0
		"idle1.tim", //Cuphead_ArcMain_Idle1
		"idle2.tim", //Cuphead_ArcMain_Idle2
		"left.tim",  //Cuphead_ArcMain_Left
		"down.tim",  //Cuphead_ArcMain_Down
		"up.tim",    //Cuphead_ArcMain_Up
		"right0.tim", //Cuphead_ArcMain_Right0
		"right1.tim", //Cuphead_ArcMain_Right1
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
