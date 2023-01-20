#include "gfa.h"

#include "../../psx/mem.h"
#include "../../psx/archive.h"
#include "../../scenes/stage/stage.h"
#include "../../main.h"

//GFA character structure
enum
{
  GFA_ArcMain_GFA0,
  GFA_ArcMain_GFA1,
  GFA_ArcMain_GFA3,
  GFA_ArcMain_GFA4,
  GFA_ArcMain_GFA6,
  GFA_ArcMain_GFA7,
  GFA_ArcMain_GFA9,
  GFA_ArcMain_GFA10,
  GFA_ArcMain_GFA12,
  GFA_ArcMain_GFA13,

	GFA_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main, arc_scene;
	IO_Data arc_ptr[GFA_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
	
	//Pico test
	u16 *pico_p;
} Char_GFA;

//GFA character definitions
static const CharFrame char_gfa_frame[] = {
  {GFA_ArcMain_GFA0, {  0,  0,179,242}, {150,143}}, //0 danceleft 1
  {GFA_ArcMain_GFA1, {  0,  0,178,240}, {149,143}}, //1 danceleft 2
  {GFA_ArcMain_GFA3, {  0,  0,177,242}, {149,146}}, //3 danceleft 4
  {GFA_ArcMain_GFA4, {  0,  0,177,244}, {149,148}}, //4 danceleft 5
  {GFA_ArcMain_GFA6, {  0,  0,176,244}, {148,147}}, //6 danceleft 7
  {GFA_ArcMain_GFA7, {  0,  0,177,238}, {149,140}}, //7 danceleft 8
  {GFA_ArcMain_GFA9, {  0,  0,177,238}, {149,141}}, //9 danceleft 10
  {GFA_ArcMain_GFA10, {  0,  0,177,239}, {149,142}}, //10 danceleft 11
  {GFA_ArcMain_GFA12, {  0,  0,176,244}, {148,147}}, //12 danceleft 13
  {GFA_ArcMain_GFA13, {  0,  0,177,245}, {149,148}}, //13 danceleft 14
};

static const Animation char_gfa_anim[CharAnim_Max] = {
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                           //CharAnim_Idle
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                           //CharAnim_Left
	{1, (const u8[]){ 0,  0,  1,  1,  2,  2,  3,  4,  4,  5, ASCR_BACK, 1}}, //CharAnim_LeftAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                           //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                           //CharAnim_DownAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                           //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                           //CharAnim_UpAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                           //CharAnim_Right
	{1, (const u8[]){ 6,  6,  7,  7,  8,  8,  9, 10, 10, 11, ASCR_BACK, 1}}, //CharAnim_RightAlt
};

//GFA character functions
void Char_GFA_SetFrame(void *user, u8 frame)
{
	Char_GFA *this = (Char_GFA*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_gfa_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_GFA_Tick(Character *character)
{
	Char_GFA *this = (Char_GFA*)character;
	
	if (this->pico_p != NULL)
	{
		if (stage.note_scroll >= 0)
		{
			//Scroll through Pico chart
			u16 substep = stage.note_scroll >> FIXED_SHIFT;
			while (substep >= ((*this->pico_p) & 0x7FFF))
			{
				//Play animation
				character->set_anim(character, ((*this->pico_p) & 0x8000) ? CharAnim_RightAlt : CharAnim_LeftAlt);
				this->pico_p++;
			}
		}
	}
	else
	{
		if (stage.flag & STAGE_FLAG_JUST_STEP)
		{
			//Stage specific animations
			if (stage.note_scroll >= 0)
			{
				//switch (stage.stage_id)
				//{
				//	case StageId_1_4: //Tutorial cheer
				//		if (stage.song_step > 64 && stage.song_step < 192 && (stage.song_step & 0x3F) == 60)
				//			character->set_anim(character, CharAnim_UpAlt);
				//		break;
				//	default:
				//		break;
				//}
			}
			
			//Perform dance
			if (stage.note_scroll >= character->sing_end && (stage.song_step % stage.gf_speed) == 0)
			{
				//Switch animation
				if (character->animatable.anim == CharAnim_LeftAlt || character->animatable.anim == CharAnim_Right)
					character->set_anim(character, CharAnim_RightAlt);
				else
					character->set_anim(character, CharAnim_LeftAlt);
			}
		}
	}
	
	//Get parallax
	fixed_t parallax;
	parallax = FIXED_UNIT;
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_GFA_SetFrame);
	Character_DrawParallax(character, &this->tex, &char_gfa_frame[this->frame], parallax);
}

void Char_GFA_SetAnim(Character *character, u8 anim)
{
	//Set animation
	if (anim == CharAnim_Left || anim == CharAnim_Down || anim == CharAnim_Up || anim == CharAnim_Right || anim == CharAnim_UpAlt)
		character->sing_end = stage.note_scroll + FIXED_DEC(22,1); //Nearly 2 steps
	Animatable_SetAnim(&character->animatable, anim);
}

void Char_GFA_Free(Character *character)
{
	Char_GFA *this = (Char_GFA*)character;
	
	//Free art
	Mem_Free(this->arc_main);
	Mem_Free(this->arc_scene);
}

Character *Char_GFA_New(fixed_t x, fixed_t y)
{
	//Allocate gfa object
	Char_GFA *this = Mem_Alloc(sizeof(Char_GFA));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_GFA_New] Failed to allocate gfa object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_GFA_Tick;
	this->character.set_anim = Char_GFA_SetAnim;
	this->character.free = Char_GFA_Free;
	
	Animatable_Init(&this->character.animatable, char_gfa_anim);
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
	
	this->character.focus_x = FIXED_DEC(2,1);
	this->character.focus_y = FIXED_DEC(-40,1);
	this->character.focus_zoom = FIXED_DEC(2,1);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\GFA.ARC;1");
	
	const char **pathp = (const char *[]){
  "gfa0.tim",
  "gfa1.tim",
  "gfa3.tim",
  "gfa4.tim",
  "gfa6.tim",
  "gfa7.tim",
  "gfa9.tim",
  "gfa10.tim",
  "gfa12.tim",
  "gfa13.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Load scene specific art
	switch (stage.stage_id)
	{
		//case StageId_1_4: //Tutorial
		//{
		//	this->arc_scene = IO_Read("\\CHAR\\GFTUT.ARC;1");
		//	
		//	const char **pathp = (const char *[]){
		//		"tut0.tim", //GFA_ArcScene_0
		//		"tut1.tim", //GFA_ArcScene_1
		//		NULL
		//	};
		//	IO_Data *arc_ptr = &this->arc_ptr[GF_ArcScene_0];
		//	for (; *pathp != NULL; pathp++)
		//		*arc_ptr++ = Archive_Find(this->arc_scene, *pathp);
		//	break;
		//}
		default:
			this->arc_scene = NULL;
			break;
	}
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	this->pico_p = NULL;
	
	return (Character*)this;
}
