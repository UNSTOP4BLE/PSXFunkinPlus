/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "goon.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Goon character structure
enum
{
	Goon_ArcMain_Idle0,
	Goon_ArcMain_Idle1,
	Goon_ArcMain_Idle2,
	Goon_ArcMain_Idle3,
	Goon_ArcMain_Idle4,
	Goon_ArcMain_Idle5,
	Goon_ArcMain_Left0,
	Goon_ArcMain_Left1,
	Goon_ArcMain_Down0,
	Goon_ArcMain_Down1,
	Goon_ArcMain_Down2,
	Goon_ArcMain_Up0,
	Goon_ArcMain_Up1,
	Goon_ArcMain_Up2,
	Goon_ArcMain_Up3,
	Goon_ArcMain_Right0,
	Goon_ArcMain_Right1,
	Goon_ArcMain_Right2,
	
	Goon_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Goon_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Goon;

//Goon character definitions
static const CharFrame char_goon_frame[] = {
	{Goon_ArcMain_Idle0, {  0,   0, 147, 222}, { 74, 222}}, 
	{Goon_ArcMain_Idle1, {  0,   0, 140, 222}, { 70, 222}}, 
	{Goon_ArcMain_Idle2, {  0,   3, 139, 225}, { 70, 225}},
	{Goon_ArcMain_Idle3, {  0,   2, 141, 226}, { 70, 226}}, 
	{Goon_ArcMain_Idle4, {  0,   2, 142, 226}, { 71, 226}}, 
	{Goon_ArcMain_Idle5, {  0,   2, 142, 226}, { 71, 226}},
	
	{Goon_ArcMain_Left0, {  0,   4, 125, 236}, { 40, 236}},
	{Goon_ArcMain_Left0, {126,   3, 126, 237}, { 40, 237}},
	{Goon_ArcMain_Left1, {  0,   0, 124, 237}, { 40, 237}},
	{Goon_ArcMain_Left1, {125,   0, 124, 237}, { 40, 237}}, 
	
	{Goon_ArcMain_Down0, {  0,   0, 118, 183}, { 43, 174+4}}, 
	{Goon_ArcMain_Down0, {  0,   0, 118, 183}, { 43, 174+4}}, 
	{Goon_ArcMain_Down1, {119,   0, 117, 183}, { 43, 175+4}}, 
	{Goon_ArcMain_Down1, {119,   0, 117, 183}, { 43, 175+4}}, 
	{Goon_ArcMain_Down2, {119,   0, 117, 183}, { 43, 175+4}},
	
	{Goon_ArcMain_Up0, {  0,   0, 102, 205}, { 40, 196+4}},
	{Goon_ArcMain_Up1, {103,   0, 103, 203}, { 40, 194+4}}, 
	{Goon_ArcMain_Up2, {  0,   0, 102, 205}, { 40, 196+4}},
	{Goon_ArcMain_Up3, {103,   0, 103, 203}, { 40, 194+4}},
	
	{Goon_ArcMain_Right0, {  0,   0, 117, 199}, { 43, 189+4}}, 
	{Goon_ArcMain_Right1, {118,   0, 114, 199}, { 42, 189+4}}, 
	{Goon_ArcMain_Right2, {118,   0, 114, 199}, { 42, 189+4}},
};

static const Animation char_goon_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  4,  5,  0, ASCR_BACK}}, //CharAnim_Idle
	{1, (const u8[]){ 6,  7,  8,  9, ASCR_BACK}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{1, (const u8[]){ 6,  7, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{1, (const u8[]){ 8,  9, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{1, (const u8[]){10, 11, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Goon character functions
void Char_Goon_SetFrame(void *user, u8 frame)
{
	Char_Goon *this = (Char_Goon*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_goon_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Goon_Tick(Character *character)
{
	Char_Goon *this = (Char_Goon*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Goon_SetFrame);
	Character_Draw(character, &this->tex, &char_goon_frame[this->frame]);
}

void Char_Goon_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Goon_Free(Character *character)
{
	Char_Goon *this = (Char_Goon*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Goon_New(fixed_t x, fixed_t y)
{
	//Allocate goon object
	Char_Goon *this = Mem_Alloc(sizeof(Char_Goon));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Goon_New] Failed to allocate goon object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Goon_Tick;
	this->character.set_anim = Char_Goon_SetAnim;
	this->character.free = Char_Goon_Free;
	
	Animatable_Init(&this->character.animatable, char_goon_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-115,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\GOON.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //Goon_ArcMain_Idle0
		"idle1.tim", //Goon_ArcMain_Idle1
		"idle2.tim", //Goon_ArcMain_Idle2
		"idle3.tim", //Goon_ArcMain_Idle3
		"idle4.tim", //Goon_ArcMain_Idle4
		"idle5.tim", //Goon_ArcMain_Idle5
		"left0.tim", //Goon_ArcMain_Left0
		"left1.tim", //Goon_ArcMain_Left1
		"down0.tim", //Goon_ArcMain_Down0
		"down1.tim", //Goon_ArcMain_Down1
		"down2.tim", //Goon_ArcMain_Down2
		"up0.tim",    //Goon_ArcMain_Up0
		"up1.tim",    //Goon_ArcMain_Up1
		"up2.tim",    //Goon_ArcMain_Up2
		"up3.tim",    //Goon_ArcMain_Up3
		"right0.tim", //Goon_ArcMain_Right0
		"right1.tim", //Goon_ArcMain_Right1
		"right2.tim", //Goon_ArcMain_Right2
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
