#include "gf.h"

#include "../../psx/mem.h"
#include "../../psx/archive.h"
#include "../../scenes/stage/stage.h"
#include "../../main.h"

#include "speaker.h"

//GF character structure
enum
{
	GF_ArcMain_idle0,
	GF_ArcMain_idle1,
	GF_ArcMain_idle2,
	
	GF_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main, arc_scene;
	IO_Data arc_ptr[GF_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
	
	//Speaker
	Speaker speaker;
	
	//Pico test
	u16 *pico_p;
} Char_GF;

//GF character definitions
static const CharFrame char_gf_frame[] = {
	{gfone_ArcMain_idle0,{0,0,246,125},{123,122}},
	{gfone_ArcMain_idle0,{0,126,246,126},{122,123}},
	{gfone_ArcMain_idle1,{0,0,246,125},{123,122}},
	{gfone_ArcMain_idle1,{0,126,246,127},{122,124}},
	{gfone_ArcMain_idle2,{0,0,244,127},{122,124}},
	{gfone_ArcMain_idle2,{0,128,244,128},{122,125}},
};

static const Animation char_gf_anim[CharAnim_Max] = {
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                        //CharAnim_Idle
	{2, (const u8[]){0, 0, ASCR_BACK, 1}},                                 //CharAnim_Left
	{1, (const u8[]){ 0,  0,  1,  1,  2,  2,  3,  3,  ASCR_BACK, 1}}, //CharAnim_LeftAlt
	{2, (const u8[]){0, 0, ASCR_BACK, 1}},                                 //CharAnim_Down
	{1, (const u8[]){0, 0, 0, 0, 0, ASCR_REPEAT}},                      //CharAnim_DownAlt
	{2, (const u8[]){21, 22, ASCR_BACK, 1}},                                 //CharAnim_Up
	{2, (const u8[]){25, 26, ASCR_BACK, 1}},                                 //CharAnim_UpAlt
	{2, (const u8[]){23, 24, ASCR_BACK, 1}},                                 //CharAnim_Right
	{1, (const u8[]){ 4,  4,  5,  5,  6,  6, ASCR_BACK, 1}}, //CharAnim_RightAlt
};

//GF character functions
void Char_GF_SetFrame(void *user, u8 frame)
{
	Char_GF *this = (Char_GF*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_gf_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_GF_Tick(Character *character)
{
	Char_GF *this = (Char_GF*)character;
	
	if (this->pico_p != NULL)
	{
		if (stage.note_scroll >= 0)
		{
			//Scroll through Pico chart
			u16 substep = stage.note_scroll >> FIXED_SHIFT;
			while (substep >= ((*this->pico_p) & 0x7FFF))
			{
				//Play animation and bump speakers
				character->set_anim(character, ((*this->pico_p) & 0x8000) ? CharAnim_RightAlt : CharAnim_LeftAlt);
				Speaker_Bump(&this->speaker);
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
				switch (stage.stage_id)
				{
					case StageId_1_4: //Tutorial cheer
						if (stage.song_step > 64 && stage.song_step < 192 && (stage.song_step & 0x3F) == 60)
							character->set_anim(character, CharAnim_UpAlt);
						break;
					default:
						break;
				}
			}
			
			//Perform dance
			if (stage.note_scroll >= character->sing_end && (stage.song_step % stage.gf_speed) == 0)
			{
				//Switch animation
				if (character->animatable.anim == CharAnim_LeftAlt || character->animatable.anim == CharAnim_Right)
					character->set_anim(character, CharAnim_RightAlt);
				else
					character->set_anim(character, CharAnim_LeftAlt);
				
				//Bump speakers
				Speaker_Bump(&this->speaker);
			}
		}
	}
	
	//Get parallax
	fixed_t parallax;
	parallax = FIXED_UNIT;
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_GF_SetFrame);
	Character_DrawParallax(character, &this->tex, &char_gf_frame[this->frame], parallax);
	
	//Tick speakers
	Speaker_Tick(&this->speaker, character->x, character->y, parallax);
}

void Char_GF_SetAnim(Character *character, u8 anim)
{
	//Set animation
	if (anim == CharAnim_Left || anim == CharAnim_Down || anim == CharAnim_Up || anim == CharAnim_Right || anim == CharAnim_UpAlt)
		character->sing_end = stage.note_scroll + FIXED_DEC(22,1); //Nearly 2 steps
	Animatable_SetAnim(&character->animatable, anim);
}

void Char_GF_Free(Character *character)
{
	Char_GF *this = (Char_GF*)character;
	
	//Free art
	Mem_Free(this->arc_main);
	Mem_Free(this->arc_scene);
}

Character *Char_GF_New(fixed_t x, fixed_t y)
{
	//Allocate gf object
	Char_GF *this = Mem_Alloc(sizeof(Char_GF));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_GF_New] Failed to allocate gf object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_GF_Tick;
	this->character.set_anim = Char_GF_SetAnim;
	this->character.free = Char_GF_Free;
	
	Animatable_Init(&this->character.animatable, char_gf_anim);
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
	this->arc_main = IO_Read("\\CHAR\\GFONE.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //GF_ArcMain_GF0
		"idle1.tim", //GF_ArcMain_GF1
		"idle2.tim", //GF_ArcMain_GF2
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Load scene specific art
	this->arc_scene = NULL;
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	//Initialize speaker
	Speaker_Init(&this->speaker);
	
	this->pico_p = NULL;
	
	return (Character*)this;
}
