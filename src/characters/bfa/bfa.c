#include "bfa.h"

#include "../../psx/mem.h"
#include "../../psx/archive.h"
#include "../../psx/random.h"
#include "../../scenes/stage/stage.h"
#include "../../main.h"

//Boyfriend skull fragments
static SkullFragment char_bfa_skull[15] = {
	{ 1 * 8, -87 * 8, -13, -13},
	{ 9 * 8, -88 * 8,   5, -22},
	{18 * 8, -87 * 8,   9, -22},
	{26 * 8, -85 * 8,  13, -13},
	
	{-3 * 8, -82 * 8, -13, -11},
	{ 8 * 8, -85 * 8,  -9, -15},
	{20 * 8, -82 * 8,   9, -15},
	{30 * 8, -79 * 8,  13, -11},
	
	{-1 * 8, -74 * 8, -13, -5},
	{ 8 * 8, -77 * 8,  -9, -9},
	{19 * 8, -75 * 8,   9, -9},
	{26 * 8, -74 * 8,  13, -5},
	
	{ 5 * 8, -73 * 8, -5, -3},
	{14 * 8, -76 * 8,  9, -6},
	{26 * 8, -67 * 8, 15, -3},
};

//Boyfriend player types
enum
{
  BFA_ArcMain_Idle0,
  BFA_ArcMain_Idle1,
  BFA_ArcMain_Idle2,
  BFA_ArcMain_Idle3,
  BFA_ArcMain_Left0,
  BFA_ArcMain_Left1,
  BFA_ArcMain_Down0,
  BFA_ArcMain_Down1,
  BFA_ArcMain_Down2,
  BFA_ArcMain_Down3,
  BFA_ArcMain_Up0,
  BFA_ArcMain_Up1,
  BFA_ArcMain_Right0,
  BFA_ArcMain_Right1,
  BFA_ArcMain_Right2,
  BFA_ArcMain_Right3,
  BFA_ArcMain_LeftMiss0,
  BFA_ArcMain_LeftMiss1,
  BFA_ArcMain_DownMiss0,
  BFA_ArcMain_DownMiss1,
  BFA_ArcMain_DownMiss2,
  BFA_ArcMain_UpMiss0,
  BFA_ArcMain_UpMiss1,
  BFA_ArcMain_RightMiss0,
  BFA_ArcMain_RightMiss1,
  BFA_ArcMain_RightMiss2,
	
	BFA_ArcMain_Max,
};

#define BFA_Arc_Max BFA_ArcMain_Max

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main, arc_dead;
	CdlFILE file_dead_arc; //dead.arc file position
	IO_Data arc_ptr[BFA_Arc_Max];
	
	Gfx_Tex tex, tex_retry;
	u8 frame, tex_id;
	
	u8 retry_bump;
	
	SkullFragment skull[COUNT_OF(char_bfa_skull)];
	u8 skull_scale;
} Char_BFA;

//Boyfriend player definitions
static const CharFrame char_bfa_frame[] = {
  {BFA_ArcMain_Idle0, {  0,  0,196,116}, {211,117}}, //0 idle 1
  {BFA_ArcMain_Idle0, {  0,118,195,119}, {208,118}}, //1 idle 2
  {BFA_ArcMain_Idle1, {  0,  0,191,121}, {202,122}}, //2 idle 3
  {BFA_ArcMain_Idle1, {  0,123,188,124}, {200,126}}, //3 idle 4
  {BFA_ArcMain_Idle2, {  0,  0,187,125}, {200,128}}, //4 idle 5
  {BFA_ArcMain_Idle2, {  0,127,187,124}, {199,128}}, //5 idle 6
  {BFA_ArcMain_Idle3, {  0,  0,187,124}, {200,128}}, //6 idle 7

  {BFA_ArcMain_Left0, {  0,  0,181,117}, {224,115}}, //7 left 1
  {BFA_ArcMain_Left0, {  0,119,189,125}, {223,118}}, //8 left 2
  {BFA_ArcMain_Left1, {  0,  0,191,126}, {220,119}}, //9 left 3
  {BFA_ArcMain_Left1, {  0,128,193,127}, {220,120}}, //10 left 4

  {BFA_ArcMain_Down0, {  0,  0,201,129}, {217,112}}, //11 down 1
  {BFA_ArcMain_Down1, {  0,  0,200,127}, {216,114}}, //12 down 2
  {BFA_ArcMain_Down2, {  0,  0,202,129}, {215,119}}, //13 down 3
  {BFA_ArcMain_Down3, {  0,  0,203,130}, {215,120}}, //14 down 4

  {BFA_ArcMain_Up0, {  0,  0,175,117}, {188,135}}, //15 up 1
  {BFA_ArcMain_Up0, {  0,119,181,117}, {193,130}}, //16 up 2
  {BFA_ArcMain_Up1, {  0,  0,182,118}, {194,129}}, //17 up 3
  {BFA_ArcMain_Up1, {  0,120,181,118}, {194,129}}, //18 up 4

  {BFA_ArcMain_Right0, {  0,  0,217,139}, {196,135}}, //19 right 1
  {BFA_ArcMain_Right1, {  0,  0,212,136}, {198,132}}, //20 right 2
  {BFA_ArcMain_Right2, {  0,  0,207,134}, {199,130}}, //21 right 3
  {BFA_ArcMain_Right3, {  0,  0,207,134}, {199,130}}, //22 right 4

  {BFA_ArcMain_LeftMiss0, {  0,  0,193,124}, {227,116}}, //23 leftmiss 1
  {BFA_ArcMain_LeftMiss0, {  0,126,192,151}, {219,140}}, //24 leftmiss 2
  {BFA_ArcMain_LeftMiss1, {  0,  0,189,151}, {220,140}}, //25 leftmiss 3

  {BFA_ArcMain_DownMiss0, {  0,  0,202,129}, {215,102}}, //26 downalt 1
  {BFA_ArcMain_DownMiss1, {  0,  0,195,150}, {212,130}}, //27 downalt 2
  {BFA_ArcMain_DownMiss2, {  0,  0,195,152}, {212,130}}, //28 downalt 3

  {BFA_ArcMain_UpMiss0, {  0,  0,175,117}, {188,131}}, //29 upmiss 1
  {BFA_ArcMain_UpMiss0, {  0,119,177,131}, {194,138}}, //30 upmiss 2
  {BFA_ArcMain_UpMiss1, {  0,  0,176,133}, {194,140}}, //31 upmiss 3

  {BFA_ArcMain_RightMiss0, {  0,  0,217,139}, {192,134}}, //32 rightmiss 1
  {BFA_ArcMain_RightMiss1, {  0,  0,200,158}, {194,152}}, //33 rightmiss 2
  {BFA_ArcMain_RightMiss2, {  0,  0,201,161}, {194,154}}, //34 rightmiss 3
};

static const Animation char_bfa_anim[PlayerAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  4,  5,  6, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 5,  6,  7,  8, ASCR_BACK, 1}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{2, (const u8[]){ 9, 10, 11, 12, ASCR_BACK, 1}},             //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{2, (const u8[]){13, 14, 15, 16, ASCR_BACK, 1}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{2, (const u8[]){17, 18, 19, 20, ASCR_BACK, 1}},             //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
	
	{1, (const u8[]){ 23, 24, 25, ASCR_BACK, 1}},     //PlayerAnim_LeftMiss
	{1, (const u8[]){ 26, 27, 28, ASCR_BACK, 1}},     //PlayerAnim_DownMiss
	{1, (const u8[]){ 29, 30, 31, ASCR_BACK, 1}},     //PlayerAnim_UpMiss
	{1, (const u8[]){ 32, 33, 34, ASCR_BACK, 1}},     //PlayerAnim_RightMiss
	
	{2, (const u8[]){13, 14, 15, ASCR_BACK, 1}},         //PlayerAnim_Peace
	{2, (const u8[]){16, 17, 18, 19, ASCR_REPEAT}},      //PlayerAnim_Sweat
	
	{2, (const u8[]){28, 29, 30, 31, 31, 31, 31, 31, 31, 31, ASCR_CHGANI, PlayerAnim_Dead1}}, //PlayerAnim_Dead0
	{2, (const u8[]){31, ASCR_REPEAT}},                                                       //PlayerAnim_Dead1
	{3, (const u8[]){32, 33, 34, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, ASCR_CHGANI, PlayerAnim_Dead3}}, //PlayerAnim_Dead2
	{2, (const u8[]){35, ASCR_REPEAT}},                                                       //PlayerAnim_Dead3
	{3, (const u8[]){36, 37, 35, 35, 35, 35, 35, ASCR_CHGANI, PlayerAnim_Dead3}},             //PlayerAnim_Dead4
	{3, (const u8[]){38, 39, 35, 35, 35, 35, 35, ASCR_CHGANI, PlayerAnim_Dead3}},             //PlayerAnim_Dead5
	
	{10, (const u8[]){35, 35, 35, ASCR_BACK, 1}}, //PlayerAnim_Dead4
	{ 3, (const u8[]){38, 39, 35, ASCR_REPEAT}},  //PlayerAnim_Dead5
};

//Boyfriend player functions
void Char_BFA_SetFrame(void *user, u8 frame)
{
	Char_BFA *this = (Char_BFA*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_bfa_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_BFA_Tick(Character *character)
{
	Char_BFA *this = (Char_BFA*)character;
	
	//Handle animation updates
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0 ||
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
		
		//Stage specific animations
		if (stage.note_scroll >= 0)
		{
			switch (stage.stage_id)
			{
				case StageId_1_4: //Tutorial peace
					if (stage.song_step > 64 && stage.song_step < 192 && (stage.song_step & 0x3F) == 60)
						character->set_anim(character, PlayerAnim_Peace);
					break;
				default:
					break;
			}
		}
	}
	
	//Retry screen
	if (character->animatable.anim >= PlayerAnim_Dead3)
	{
		//Tick skull fragments
		if (this->skull_scale)
		{
			SkullFragment *frag = this->skull;
			for (size_t i = 0; i < COUNT_OF_MEMBER(Char_BFA, skull); i++, frag++)
			{
				//Draw fragment
				RECT frag_src = {
					(i & 1) ? 112 : 96,
					(i >> 1) << 4,
					16,
					16
				};
				fixed_t skull_dim = (FIXED_DEC(16,1) * this->skull_scale) >> 6;
				fixed_t skull_rad = skull_dim >> 1;
				RECT_FIXED frag_dst = {
					character->x + (((fixed_t)frag->x << FIXED_SHIFT) >> 3) - skull_rad - stage.camera.x,
					character->y + (((fixed_t)frag->y << FIXED_SHIFT) >> 3) - skull_rad - stage.camera.y,
					skull_dim,
					skull_dim,
				};
				Stage_DrawTex(&this->tex_retry, &frag_src, &frag_dst, FIXED_MUL(stage.camera.zoom, stage.bump));
				
				//Move fragment
				frag->x += frag->xsp;
				frag->y += ++frag->ysp;
			}
			
			//Decrease scale
			this->skull_scale--;
		}
		
		//Draw input options
		u8 input_scale = 16 - this->skull_scale;
		if (input_scale > 16)
			input_scale = 0;
		
		RECT button_src = {
			 0, 96,
			16, 16
		};
		RECT_FIXED button_dst = {
			character->x - FIXED_DEC(32,1) - stage.camera.x,
			character->y - FIXED_DEC(88,1) - stage.camera.y,
			(FIXED_DEC(16,1) * input_scale) >> 4,
			FIXED_DEC(16,1),
		};
		
		//Cross - Retry
		Stage_DrawTex(&this->tex_retry, &button_src, &button_dst, FIXED_MUL(stage.camera.zoom, stage.bump));
		
		//Circle - Blueball
		button_src.x = 16;
		button_dst.y += FIXED_DEC(56,1);
		Stage_DrawTex(&this->tex_retry, &button_src, &button_dst, FIXED_MUL(stage.camera.zoom, stage.bump));
		
		//Draw 'RETRY'
		u8 retry_frame;
		
		if (character->animatable.anim == PlayerAnim_Dead6)
		{
			//Selected retry
			retry_frame = 2 - (this->retry_bump >> 3);
			if (retry_frame >= 3)
				retry_frame = 0;
			if (this->retry_bump & 2)
				retry_frame += 3;
			
			if (++this->retry_bump == 0xFF)
				this->retry_bump = 0xFD;
		}
		else
		{
			//Idle
			retry_frame = 1 +  (this->retry_bump >> 2);
			if (retry_frame >= 3)
				retry_frame = 0;
			
			if (++this->retry_bump >= 55)
				this->retry_bump = 0;
		}
		
		RECT retry_src = {
			(retry_frame & 1) ? 48 : 0,
			(retry_frame >> 1) << 5,
			48,
			32
		};
		RECT_FIXED retry_dst = {
			character->x -  FIXED_DEC(7,1) - stage.camera.x,
			character->y - FIXED_DEC(92,1) - stage.camera.y,
			FIXED_DEC(48,1),
			FIXED_DEC(32,1),
		};
		Stage_DrawTex(&this->tex_retry, &retry_src, &retry_dst, FIXED_MUL(stage.camera.zoom, stage.bump));
	}
	
	//Animate and draw character
	Animatable_Animate(&character->animatable, (void*)this, Char_BFA_SetFrame);
	Character_Draw(character, &this->tex, &char_bfa_frame[this->frame]);
}

void Char_BFA_SetAnim(Character *character, u8 anim)
{
	Char_BFA *this = (Char_BFA*)character;
	
	//Perform animation checks
	switch (anim)
	{
		case PlayerAnim_Dead0:
			character->focus_x = FIXED_DEC(0,1);
			character->focus_y = FIXED_DEC(-40,1);
			character->focus_zoom = FIXED_DEC(125,100);
			break;
		case PlayerAnim_Dead2:
			//Load retry art
			Gfx_LoadTex(&this->tex_retry, this->arc_ptr[BFA_ArcMain_Idle0], 0);
			break;
	}
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_BFA_Free(Character *character)
{
	Char_BFA *this = (Char_BFA*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_BFA_New(fixed_t x, fixed_t y)
{
	//Allocate boyfriend object
	Char_BFA *this = Mem_Alloc(sizeof(Char_BFA));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_BFA_New] Failed to allocate boyfriend object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_BFA_Tick;
	this->character.set_anim = Char_BFA_SetAnim;
	this->character.free = Char_BFA_Free;
	
	Animatable_Init(&this->character.animatable, char_bfa_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = CHAR_SPEC_MISSANIM;
	
	this->character.health_i[0][0] = 0;
	this->character.health_i[0][1] = 0;
	this->character.health_i[0][2] = 46;
	this->character.health_i[0][3] = 30;
	
	this->character.health_i[1][0] = 47;
	this->character.health_i[1][1] = 0;
	this->character.health_i[1][2] = 43;
	this->character.health_i[1][3] = 35;
	
	//health bar color
	this->character.health_bar = 0xFF29B5D6;
	
	this->character.focus_x = FIXED_DEC(-30,1);
	this->character.focus_y = FIXED_DEC(-80,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\BFA.ARC;1");
	this->arc_dead = NULL;
	//IO_FindFile(&this->file_dead_arc, "\\CHAR\\BFADEAD.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "idle2.tim",
  "idle3.tim",
  "left0.tim",
  "left1.tim",
  "down0.tim",
  "down1.tim",
  "down2.tim",
  "down3.tim",
  "up0.tim",
  "up1.tim",
  "right0.tim",
  "right1.tim",
  "right2.tim",
  "right3.tim",
  "leftmiss0.tim",
  "leftmiss1.tim",
  "downalt0.tim",
  "downalt1.tim",
  "downalt2.tim",
  "upmiss0.tim",
  "upmiss1.tim",
  "rightmiss0.tim",
  "rightmiss1.tim",
  "rightmiss2.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	//Initialize player state
	this->retry_bump = 0;
	
	//Copy skull fragments
	memcpy(this->skull, char_bfa_skull, sizeof(char_bfa_skull));
	this->skull_scale = 64;
	
	SkullFragment *frag = this->skull;
	for (size_t i = 0; i < COUNT_OF_MEMBER(Char_BFA, skull); i++, frag++)
	{
		//Randomize trajectory
		frag->xsp += RandomRange(-4, 4);
		frag->ysp += RandomRange(-2, 2);
	}
	
	return (Character*)this;
}
