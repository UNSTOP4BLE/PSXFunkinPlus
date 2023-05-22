#include "bf.h"

#include "../../psx/mem.h"
#include "../../psx/archive.h"
#include "../../psx/random.h"
#include "../../scenes/stage/stage.h"
#include "../../main.h"

//BF player types
enum
{
	BF_ArcMain_bf0,
	BF_ArcMain_bf1,
	BF_ArcMain_bf2,
	BF_ArcMain_bf3,
	BF_ArcMain_bf4,
	BF_ArcMain_bf5,
	BF_ArcMain_bf6,
	BF_ArcMain_dead0,
	BF_ArcMain_dead1,
	BF_ArcMain_dead2,

	
	BF_ArcMain_Retry,

	BF_ArcMain_Max,
};

#define BF_Arc_Max BF_ArcMain_Max

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main, arc_dead;
	CdlFILE file_dead_arc; //dead.arc file position
	IO_Data arc_ptr[BF_Arc_Max];
	
	Gfx_Tex tex, tex_retry;
	u8 frame, tex_id;
} Char_BF;

//BF character definitions
static const u16 char_bf_icons[2][4] = {
	{0,0,41,25},
	{42,0,41,27}
};

static const CharFrame char_bf_frame[] = {
	{BF_ArcMain_bf0,{0,0,102,99},{54,86}}, //0 Idle
	{BF_ArcMain_bf0,{103,0,102,99},{54,86}}, //1 Idle
	{BF_ArcMain_bf0,{0,100,102,101},{54,88}}, //2 Idle
	{BF_ArcMain_bf0,{103,100,103,104},{54,91}}, //3 Idle
	{BF_ArcMain_bf1,{0,0,103,104},{54,91}}, //4 Idle
	
	{BF_ArcMain_bf1,{104,0,96,102},{57,89}}, //5 Left
	{BF_ArcMain_bf1,{0,106,94,102},{55,88}}, //6 Left
	
	{BF_ArcMain_bf1,{95,103,94,89},{51,77}}, //7 Down
	{BF_ArcMain_bf2,{0,0,94,90},{51,78}}, //8 Down
	
	{BF_ArcMain_bf2,{95,0,93,112},{45,98}}, //9 Up
	{BF_ArcMain_bf2,{0,91,94,111},{46,97}}, //10 Up
	
	{BF_ArcMain_bf2,{95,113,102,102},{44,89}}, //11 Right
	{BF_ArcMain_bf3,{0,0,102,102},{44,89}}, //12 Right
	
	{BF_ArcMain_bf3,{103,0,99,105},{55,92}}, //13 Peace
	{BF_ArcMain_bf3,{0,103,104,103},{55,90}}, //14 Peace
	{BF_ArcMain_bf3,{105,106,104,104},{55,91}}, //15 Peace
	
	{BF_ArcMain_bf4,{0,0,103,104},{54,91}}, //16 Sweat
	{BF_ArcMain_bf4,{104,0,103,104},{54,91}}, //17 Sweat
	{BF_ArcMain_bf4,{0,105,103,104},{54,91}}, //18 Sweat
	{BF_ArcMain_bf4,{103,105,103,104},{54,91}}, //19 Sweat
	
	{BF_ArcMain_bf5,{0,0,93,108},{53,96}}, //20 Leftmiss
	{BF_ArcMain_bf5,{94,0,93,108},{53,96}}, //21 Leftmiss
	
	{BF_ArcMain_bf5,{0,109,95,98},{51,86}}, //22 Downmiss
	{BF_ArcMain_bf5,{96,109,95,97},{51,85}}, //23 Downmiss
	
	{BF_ArcMain_bf6,{0,0,90,107},{46,94}}, //24 Upmiss
	{BF_ArcMain_bf6,{91,0,89,108},{46,95}}, //25 Upmiss
	
	{BF_ArcMain_bf6,{0,108,99,108},{45,97}}, //26 Rightmiss
	{BF_ArcMain_bf6,{100,109,101,108},{45,97}}, //27 Rightmiss
	
	{BF_ArcMain_dead0,{0,0,128,128},{58,92}}, //28 Dead0
	{BF_ArcMain_dead0,{128,0,128,128},{58,91}}, //29 Dead0
	{BF_ArcMain_dead0,{0,128,128,128},{59,91}}, //30 Dead0
	{BF_ArcMain_dead0,{128,128,128,128},{59,90}}, //31 Dead0
	{BF_ArcMain_dead0,{128,128,128,128},{59,90}}, //32 Dead0
	{BF_ArcMain_dead0,{128,128,128,128},{59,90}}, //33 Dead0
	{BF_ArcMain_dead0,{128,128,128,128},{59,90}}, //34 Dead0
	{BF_ArcMain_dead0,{128,128,128,128},{59,90}}, //35 Dead0
	{BF_ArcMain_dead0,{128,128,128,128},{59,90}}, //36 Dead0
	{BF_ArcMain_dead0,{128,128,128,128},{59,90}}, //37 Dead0
	
	{BF_ArcMain_dead0,{128,128,128,128},{59,90}}, //38 Dead1
	
	{BF_ArcMain_dead1,{0,0,128,128},{59,90}}, //39 Dead2
	{BF_ArcMain_dead1,{128,0,128,128},{58,90}}, //40 Dead2
	{BF_ArcMain_dead1,{0,128,128,128},{58,90}}, //41 Dead2
	{BF_ArcMain_dead1,{128,128,128,128},{58,90}}, //42 Dead2
	{BF_ArcMain_dead1,{128,128,128,128},{58,90}}, //43 Dead2
	{BF_ArcMain_dead1,{128,128,128,128},{58,90}}, //44 Dead2
	{BF_ArcMain_dead1,{128,128,128,128},{58,90}}, //45 Dead2
	{BF_ArcMain_dead1,{128,128,128,128},{58,90}}, //46 Dead2
	{BF_ArcMain_dead1,{128,128,128,128},{58,90}}, //47 Dead2
	{BF_ArcMain_dead1,{128,128,128,128},{58,90}}, //48 Dead2
	{BF_ArcMain_dead1,{128,128,128,128},{58,90}}, //49 Dead2
	{BF_ArcMain_dead1,{128,128,128,128},{58,90}}, //50 Dead2
	{BF_ArcMain_dead1,{128,128,128,128},{58,90}}, //51 Dead2
	{BF_ArcMain_dead1,{128,128,128,128},{58,90}}, //52 Dead2
	{BF_ArcMain_dead1,{128,128,128,128},{58,90}}, //53 Dead2
	
	{BF_ArcMain_dead1,{128,128,128,128},{58,90}}, //54 Dead3
	
	{BF_ArcMain_dead2,{0,0,128,128},{58,90}}, //55 Dead4
	{BF_ArcMain_dead2,{128,0,128,128},{58,90}}, //56 Dead4
	{BF_ArcMain_dead1,{128,128,128,128},{58,90}}, //57 Dead4
	{BF_ArcMain_dead2,{128,0,128,128},{58,90}}, //58 Dead4
	{BF_ArcMain_dead2,{128,0,128,128},{58,90}}, //59 Dead4
	{BF_ArcMain_dead2,{128,0,128,128},{58,90}}, //60 Dead4
	{BF_ArcMain_dead2,{128,0,128,128},{58,90}}, //61 Dead4
	
	{BF_ArcMain_dead2,{0,128,128,128},{58,90}}, //62 Dead5
	{BF_ArcMain_dead2,{128,128,128,128},{58,90}}, //63 Dead5
	{BF_ArcMain_dead1,{128,128,128,128},{58,90}}, //64 Dead5
	{BF_ArcMain_dead1,{128,128,128,128},{58,90}}, //65 Dead5
	{BF_ArcMain_dead1,{128,128,128,128},{58,90}}, //66 Dead5
	{BF_ArcMain_dead1,{128,128,128,128},{58,90}}, //67 Dead5
	{BF_ArcMain_dead1,{128,128,128,128},{58,90}}, //68 Dead5
	
	{BF_ArcMain_dead1,{128,128,128,128},{58,90}}, //69 Dead6
	{BF_ArcMain_dead1,{128,128,128,128},{58,90}}, //70 Dead6
	{BF_ArcMain_dead1,{128,128,128,128},{58,90}}, //71 Dead6
	
	{BF_ArcMain_dead2,{0,128,128,128},{58,90}}, //72 Dead7
	{BF_ArcMain_dead2,{128,128,128,128},{58,90}}, //73 Dead7
	{BF_ArcMain_dead1,{128,128,128,128},{58,90}}, //74 Dead7
	

};

static const Animation char_bf_anim[PlayerAnim_Max] = {
	{12, (const u8[]){ 0, 1, 2, 3, 4,  ASCR_BACK, 1}},		//CharAnim_Idle
	{12, (const u8[]){ 5, 6,  ASCR_BACK, 1}},		//CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},		//CharAnim_LeftAlt
	{12, (const u8[]){ 7, 8,  ASCR_BACK, 1}},		//CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},		//CharAnim_DownAlt
	{12, (const u8[]){ 9, 10,  ASCR_BACK, 1}},		//CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},		//CharAnim_UpAlt
	{12, (const u8[]){ 11, 12,  ASCR_BACK, 1}},		//CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},		//CharAnim_RightAlt
	{12, (const u8[]){ 20, 21,  ASCR_BACK, 1}},		//PlayerAnim_LeftMiss
	{12, (const u8[]){ 22, 23,  ASCR_BACK, 1}},		//PlayerAnim_DownMiss
	{12, (const u8[]){ 24, 25,  ASCR_BACK, 1}},		//PlayerAnim_UpMiss
	{12, (const u8[]){ 26, 27,  ASCR_BACK, 1}},		//PlayerAnim_RightMiss
	{12, (const u8[]){ 13, 14, 15,  ASCR_BACK, 1}},		//PlayerAnim_Peace
	{12, (const u8[]){ 16, 17, 18, 19,  ASCR_REPEAT}},		//PlayerAnim_Sweat
	{12, (const u8[]){ 28, 29, 30, 31, 32, 33, 34, 35, 36, 37,  ASCR_CHGANI, PlayerAnim_Dead1}},		//PlayerAnim_Dead0
	{12, (const u8[]){ 38,  ASCR_REPEAT}},		//PlayerAnim_Dead1
	{12, (const u8[]){ 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53,  ASCR_CHGANI, PlayerAnim_Dead3}},		//PlayerAnim_Dead2
	{12, (const u8[]){ 54,  ASCR_REPEAT}},		//PlayerAnim_Dead3
	{12, (const u8[]){ 55, 56, 57, 58, 59, 60, 61,  ASCR_CHGANI, PlayerAnim_Dead3}},		//PlayerAnim_Dead4
	{12, (const u8[]){ 62, 63, 64, 65, 66, 67, 68,  ASCR_CHGANI, PlayerAnim_Dead3}},		//PlayerAnim_Dead5
	{3, (const u8[]){ 69, 70, 71,  ASCR_BACK, 1}},		//PlayerAnim_Dead6
	{12, (const u8[]){ 72, 73, 74,  ASCR_REPEAT}},		//PlayerAnim_Dead7

};

//BF player functions
void Char_BF_SetFrame(void *user, u8 frame)
{
	Char_BF *this = (Char_BF*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_bf_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_BF_Tick(Character *character)
{
	Char_BF *this = (Char_BF*)character;
	
	//Handle animation updates
	if ((character->pad_held & (stage.prefs.control_keys[0] | stage.prefs.control_keys[1] | stage.prefs.control_keys[2] | stage.prefs.control_keys[3])) == 0 ||
	    (character->animatable.anim != CharAnim_Left &&
	     character->animatable.anim != CharAnim_LeftAlt &&
	     character->animatable.anim != CharAnim_Down &&
	     character->animatable.anim != CharAnim_DownAlt &&
	     character->animatable.anim != CharAnim_Up &&
	     character->animatable.anim != CharAnim_UpAlt &&
	     character->animatable.anim != CharAnim_Right &&
	     character->animatable.anim != CharAnim_RightAlt))
		Character_CheckEndSing(character);
	
	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{
		//Perform idle dance
		if (Animatable_Ended(&character->animatable) &&
			(character->animatable.anim != CharAnim_Left &&
		     character->animatable.anim != CharAnim_LeftAlt &&
		     character->animatable.anim != PlayerAnim_LeftMiss &&
		     character->animatable.anim != CharAnim_Down &&
		     character->animatable.anim != CharAnim_DownAlt &&
		     character->animatable.anim != PlayerAnim_DownMiss &&
		     character->animatable.anim != CharAnim_Up &&
		     character->animatable.anim != CharAnim_UpAlt &&
		     character->animatable.anim != PlayerAnim_UpMiss &&
		     character->animatable.anim != CharAnim_Right &&
		     character->animatable.anim != CharAnim_RightAlt &&
		     character->animatable.anim != PlayerAnim_RightMiss) &&
			(stage.song_step & 0x7) == 0)
			character->set_anim(character, CharAnim_Idle);
	}
	
	//Retry screen
	if (character->animatable.anim >= PlayerAnim_Dead2)
	{
		if(stage.retry_visibility == 0)
			Stage_DrawTex(&this->tex_retry, &stage.retry_src, &stage.retry_dst, FIXED_MUL(stage.camera.zoom, stage.bump), stage.camera.angle);
		else
			Stage_BlendTex(&this->tex_retry, &stage.retry_src, &stage.retry_dst, FIXED_MUL(stage.camera.zoom, stage.bump), stage.camera.angle, 0);
	}
	
	//Animate and draw character
	Animatable_Animate(&character->animatable, (void*)this, Char_BF_SetFrame);
	Character_Draw(character, &this->tex, &char_bf_frame[this->frame]);
}

void Char_BF_SetAnim(Character *character, u8 anim)
{
	Char_BF *this = (Char_BF*)character;
	
	//Perform animation checks
	switch (anim)
	{
		case PlayerAnim_Dead0:
			character->focus_x = FIXED_DEC(0,1);
			character->focus_y = FIXED_DEC(-40,1);
			character->focus_zoom = FIXED_DEC(100,100);
			break;
		case PlayerAnim_Dead2:
			//Load retry art
			Gfx_LoadTex(&this->tex_retry, this->arc_ptr[BF_ArcMain_Retry], 0);
			break;
	}
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_BF_Free(Character *character)
{
	Char_BF *this = (Char_BF*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_BF_New(fixed_t x, fixed_t y)
{
	//Allocate boyfriend object
	Char_BF *this = Mem_Alloc(sizeof(Char_BF));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_BF_New] Failed to allocate boyfriend object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_BF_Tick;
	this->character.set_anim = Char_BF_SetAnim;
	this->character.free = Char_BF_Free;
	
	Animatable_Init(&this->character.animatable, char_bf_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = CHAR_SPEC_MISSANIM;
	
	memcpy(this->character.health_i, char_bf_icons, sizeof(char_bf_icons));
	
	//health bar color
	this->character.health_bar = 0xFF31B0D1;
	
	this->character.focus_x = FIXED_DEC(-30,1);
	this->character.focus_y = FIXED_DEC(-90,1);
	this->character.focus_zoom = FIXED_DEC(100,100);
	
	this->character.size = FIXED_DEC(100,100);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\BF.ARC;1");
	this->arc_dead = NULL;
	
	const char **pathp = (const char *[]){
		"bf0.tim",
		"bf1.tim",
		"bf2.tim",
		"bf3.tim",
		"bf4.tim",
		"bf5.tim",
		"bf6.tim",
		"dead0.tim",
		"dead1.tim",
		"dead2.tim",

		"retry.tim",

		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
