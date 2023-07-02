#include "<charactername>.h"

#include "../../psx/mem.h"
#include "../../psx/archive.h"
#include "../../psx/random.h"
#include "../../scenes/stage/stage.h"
#include "../../main.h"

//<CharacterName> player types
enum
{
<StructureHere>
	
	<CharacterName>_ArcMain_Retry,

	<CharacterName>_ArcMain_Max,
};

#define <CharacterName>_Arc_Max <CharacterName>_ArcMain_Max

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main, arc_dead;
	CdlFILE file_dead_arc; //dead.arc file position
	IO_Data arc_ptr[<CharacterName>_Arc_Max];
	
	Gfx_Tex tex, tex_retry;
	u8 frame, tex_id;
} Char_<CharacterName>;

//<CharacterName> character definitions
static const u16 char_<charactername>_icons[2][4] = {
<IconsHere>
};

static const CharFrame char_<charactername>_frame[] = {
<FramesHere>
};

static const Animation char_<charactername>_anim[PlayerAnim_Max] = {
<AnimationsHere>
};

//<CharacterName> player functions
void Char_<CharacterName>_SetFrame(void *user, u8 frame)
{
	Char_<CharacterName> *this = (Char_<CharacterName>*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_<charactername>_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_<CharacterName>_Tick(Character *character)
{
	Char_<CharacterName> *this = (Char_<CharacterName>*)character;
	
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
	Animatable_Animate(&character->animatable, (void*)this, Char_<CharacterName>_SetFrame);
	Character_Draw(character, &this->tex, &char_<charactername>_frame[this->frame]);
}

void Char_<CharacterName>_SetAnim(Character *character, u8 anim)
{
	Char_<CharacterName> *this = (Char_<CharacterName>*)character;
	
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
			Gfx_LoadTex(&this->tex_retry, this->arc_ptr[<CharacterName>_ArcMain_Retry], 0);
			break;
	}
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_<CharacterName>_Free(Character *character)
{
	Char_<CharacterName> *this = (Char_<CharacterName>*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_<CharacterName>_New(fixed_t x, fixed_t y, fixed_t scale)
{
	//Allocate boyfriend object
	Char_<CharacterName> *this = Mem_Alloc(sizeof(Char_<CharacterName>));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_<CharacterName>_New] Failed to allocate boyfriend object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_<CharacterName>_Tick;
	this->character.set_anim = Char_<CharacterName>_SetAnim;
	this->character.free = Char_<CharacterName>_Free;
	
	Animatable_Init(&this->character.animatable, char_<charactername>_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = CHAR_SPEC_MISSANIM;
	
	memcpy(this->character.health_i, char_bf_icons, sizeof(char_bf_icons));
	
	//health bar color
	this->character.health_bar = 0xFF<HB Color>;
	
	this->character.focus_x = FIXED_DEC(<Focus X>,1);
	this->character.focus_y = FIXED_DEC(<Focus Y>,1);
	this->character.focus_zoom = FIXED_DEC(<Focus Zoom>,100);
	
	this->character.size = FIXED_MUL(FIXED_DEC(<CharSize>,100),scale);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\<CHARACTERNAME>.ARC;1");
	this->arc_dead = NULL;
	
	const char **pathp = (const char *[]){
<TextureHere>
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
