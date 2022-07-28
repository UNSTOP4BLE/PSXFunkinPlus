/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

//thx igor for makin the offsets for this ^^

#include "TRICKY.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"
#include "../mutil.h"


//Tricky character structure
enum
{
	Tricky_Idle,
	Tricky_Idle_Flame_1,
	Tricky_Idle_Flame_2,
	
	Tricky_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Tricky_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
	
	//Hair texture
	Gfx_Tex tex_idle;
} Char_Tricky;

//Tricky character definitions
static const CharFrame char_Tricky_frame[] = {
	{Tricky_Idle_Flame_1, {  0,   0,  236, 250}, { 200, 472}},
	{Tricky_Idle_Flame_2, {  0,   0,  240, 246}, { 200, 480}},
	
	{Tricky_Idle_Flame_1, {  0,   0,  236, 250}, { 49, 148}},
	{Tricky_Idle_Flame_2, {  0,   0,  240, 246}, { 49, 150}},
	
	{Tricky_Idle_Flame_1, {  0,   0,  236, 250}, { 49, 148}},
	{Tricky_Idle_Flame_2, {  0,   0,  240, 246}, { 49, 150}},
	
	{Tricky_Idle_Flame_1, {  0,   0,  236, 250}, { 49, 148}},
	{Tricky_Idle_Flame_2, {  0,   0,  240, 246}, { 49, 150}},
	
	{Tricky_Idle_Flame_1, {  0,   0,  236, 250}, { 49, 148}},
	{Tricky_Idle_Flame_2, {  0,   0,  240, 246}, { 49, 150}},
};

static const Animation char_Tricky_anim[CharAnim_Max] = {
	{1, (const u8[]){ 0,  1, ASCR_CHGANI, CharAnim_Idle}}, //CharAnim_Idle
	{1, (const u8[]){ 0,  1, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{1, (const u8[]){ 0,  1, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{1, (const u8[]){ 0, 1, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{1, (const u8[]){0, 1, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Tricky character functions
void Char_Tricky_SetFrame(void *user, u8 frame)
{
	Char_Tricky *this = (Char_Tricky*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_Tricky_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

s16 sinanim;
s16 sinanim2;

void Char_Tricky_Tick(Character *character)
{
	Char_Tricky *this = (Char_Tricky*)character;
	
	fixed_t fx, fy;
	
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	//Draw Head
	sinanim += 4;
	if (sinanim > 250)
		sinanim = 0;
	sinanim2 += 3;
	if (sinanim2 > 250)
		sinanim2 = 0;
	
	RECT idle_src = {1, 1, 90, 107};
	RECT_FIXED idle_dst = {
		FIXED_DEC(100 + (MUtil_Cos(sinanim) / 50),1) + character->x - fx,
		FIXED_DEC(-260 + (MUtil_Sin(sinanim) / 60),1) + character->y - fy,
		FIXED_DEC(idle_src.w * 2,1),
		FIXED_DEC(idle_src.h * 2,1)
	};
	
	Stage_DrawTexRotate(&this->tex_idle, &idle_src, &idle_dst, (MUtil_Cos(sinanim2) / 70), stage.camera.bzoom, 0, 0);
	
	//Draw Left Hand
	RECT lefthand_src = {92, 1, 89, 92};
	RECT_FIXED lefthand_dst = {
		FIXED_DEC(-120 + (MUtil_Cos(sinanim) / 40),1) + character->x - fx,
		FIXED_DEC(-80 + (MUtil_Sin(sinanim2) / 35),1) + character->y - fy,
		FIXED_DEC(lefthand_src.w * 2,1),
		FIXED_DEC(lefthand_src.h * 2,1)
	};
	
	Stage_DrawTexRotate(&this->tex_idle, &lefthand_src, &lefthand_dst, -(MUtil_Cos(sinanim) / 50), stage.camera.bzoom, 0, 0);
	
	//Draw Right Hand
	RECT righthand_src = {182, 1, 64, 80};
	RECT_FIXED righthand_dst = {
		FIXED_DEC(240 + (MUtil_Cos(sinanim2) / 50),1) + character->x - fx,
		FIXED_DEC(-80 - (MUtil_Sin(sinanim) / 50),1) + character->y - fy,
		FIXED_DEC(righthand_src.w * 2,1),
		FIXED_DEC(righthand_src.h * 2,1)
	};
	
	Stage_DrawTexRotate(&this->tex_idle, &righthand_src, &righthand_dst, (MUtil_Sin(sinanim2) / 70), stage.camera.bzoom, 0, 0);
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Tricky_SetFrame);
	Character_Draw(character, &this->tex, &char_Tricky_frame[this->frame]);
}

void Char_Tricky_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Tricky_Free(Character *character)
{
	Char_Tricky *this = (Char_Tricky*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Tricky_New(fixed_t x, fixed_t y)
{
	//Allocate Tricky object
	Char_Tricky *this = Mem_Alloc(sizeof(Char_Tricky));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Tricky_New] Failed to allocate Tricky object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Tricky_Tick;
	this->character.set_anim = Char_Tricky_SetAnim;
	this->character.free = Char_Tricky_Free;
	
	Animatable_Init(&this->character.animatable, char_Tricky_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;
	
	this->character.focus_x = FIXED_DEC(45,1);
	this->character.focus_y = FIXED_DEC(-200,1);
	this->character.focus_zoom = FIXED_DEC(5,10);
	
	this->character.size = FIXED_DEC(2,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\TRICKY.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle.tim",
		"idlef0.tim",
		"idlef1.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	Gfx_LoadTex(&this->tex_idle, this->arc_ptr[0], GFX_LOADTEX_FREE);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
