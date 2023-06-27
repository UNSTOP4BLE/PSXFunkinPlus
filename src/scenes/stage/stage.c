//Characters
//Players
#include "../../characters/bf/bf.h"

//Opponents
#include "../../characters/dad/dad.h"

//Stages
#include "../../stages/default/default.h"

//Middle Char (Girlfriend)
#include "../../characters/gf/gf.h"


//Stage Codes
#include "stage.h"
#include "debug.h"

#include "object/combo.h"
#include "object/splash.h"

#include "pause.h"
#include "../menu/menu.h"

#include "../../main.h"
#include "../../audio.h"

#include "../../events/event.h"

#include "../../psx/mem.h"
#include "../../psx/timer.h"
#include "../../psx/pad.h"
#include "../../psx/random.h"
#include "../../psx/movie.h"
#include "../../psx/mutil.h"
#include "../../psx/save.h"
#include "../../psx/trans.h"
#include "../../psx/loadscr.h"

#include "../../characters/bf/bflow.h"
#include "../../stages/default/defaultlow.h"

//Stage constants
//#define STAGE_PERFECT //Play all notes perfectly
//#define STAGE_NOHUD //Disable the HUD

//#define STAGE_FREECAM //Freecam

static int Sounds[7];

static const u8 note_anims[4][3] = {
	{CharAnim_Left,  CharAnim_LeftAlt,  PlayerAnim_LeftMiss},
	{CharAnim_Down,  CharAnim_DownAlt,  PlayerAnim_DownMiss},
	{CharAnim_Up,    CharAnim_UpAlt,    PlayerAnim_UpMiss},
	{CharAnim_Right, CharAnim_RightAlt, PlayerAnim_RightMiss},
};

static const s16 note_def[4][8] = {
	{-80,-80,-80,-80,-80,-80,-80,-80}, //Notes Y Position
	{26,60,94,128,-128,-94,-60,-26}, //Notes X Position
	{-51,-17,17,51,-128,-94,94,128}, //Notes X Position with Middle Scroll
	{-128,-94,-60,-26,26,60,94,128} //Notes X Position with Swapped
};

static const StageDef stage_defs[StageId_Max] = {
	#include "../../songs.h"
};


//Stage state
Stage stage;

//Debug state
Debug debug;

//Stage music functions
static void Stage_StartVocal(void)
{
	if (!(stage.flag & STAGE_FLAG_VOCAL_ACTIVE))
	{
		Audio_ChannelXA(stage.stage_def->music_channel);
		stage.flag |= STAGE_FLAG_VOCAL_ACTIVE;
	}
}

static void Stage_CutVocal(void)
{
	if (stage.flag & STAGE_FLAG_VOCAL_ACTIVE)
	{
		Audio_ChannelXA(stage.stage_def->music_channel + 1);
		stage.flag &= ~STAGE_FLAG_VOCAL_ACTIVE;
	}
}

//Stage camera functions
static void Stage_FocusCharacter(Character *ch)
{
	//Use character focus settings to update target position and zoom
	stage.camera.tx = ch->x + ch->focus_x;
	stage.camera.ty = ch->y + ch->focus_y;
	stage.camera.tz = FIXED_MUL(ch->focus_zoom, event.zoom);
}

static void Stage_ScrollCamera(void)
{
	if (stage.debug)
		Debug_ScrollCamera();
	else if (stage.freecam)
	{
		if (pad_state.held & PAD_LEFT)
			stage.camera.x -= FIXED_DEC(2,1);
		if (pad_state.held & PAD_UP)
			stage.camera.y -= FIXED_DEC(2,1);
		if (pad_state.held & PAD_RIGHT)
			stage.camera.x += FIXED_DEC(2,1);
		if (pad_state.held & PAD_DOWN)
			stage.camera.y += FIXED_DEC(2,1);
		if (pad_state.held & PAD_TRIANGLE)
			stage.camera.zoom -= FIXED_DEC(1,100);
		if (pad_state.held & PAD_CROSS)
			stage.camera.zoom += FIXED_DEC(1,100);
	}
	else if (!stage.paused)
	{
		//Scroll based off current divisor
		stage.camera.x = lerp(stage.camera.x, stage.camera.tx, stage.camera.speed);
		stage.camera.y = lerp(stage.camera.y, stage.camera.ty, stage.camera.speed);
		stage.camera.zoom = lerp(stage.camera.zoom, stage.camera.tz, stage.camera.speed);
		stage.camera.angle = lerp(stage.camera.angle, stage.camera.ta << FIXED_SHIFT, stage.camera.speed);
		stage.camera.hudangle = lerp(stage.camera.hudangle, stage.camera.hudta << FIXED_SHIFT, stage.camera.speed);
	}
	
	//Update other camera stuff
	stage.camera.bzoom = FIXED_MUL(stage.camera.zoom, stage.bump);
}

//Stage section functions
static void Stage_ChangeBPM(u16 bpm, u16 step)
{
	//Update last BPM
	stage.last_bpm = bpm;
	
	//Update timing base
	if (stage.step_crochet)
		stage.time_base += FIXED_DIV(((fixed_t)step - stage.step_base) << FIXED_SHIFT, stage.step_crochet);
	stage.step_base = step;
	
	//Get new crochet and times
	stage.step_crochet = ((fixed_t)bpm << FIXED_SHIFT) * 8 / 240; //15/12/24
	stage.step_time = FIXED_DIV(FIXED_DEC(12,1), stage.step_crochet);
	
	//Get new crochet based values
	stage.early_safe = stage.late_safe = stage.step_crochet / 6; //10 frames
	stage.late_sus_safe = stage.late_safe;
	stage.early_sus_safe = stage.early_safe * 2 / 5;
}

static Section *Stage_GetPrevSection(Section *section)
{
	if (section > stage.sections)
		return section - 1;
	return NULL;
}

static u16 Stage_GetSectionStart(Section *section)
{
	Section *prev = Stage_GetPrevSection(section);
	if (prev == NULL)
		return 0;
	return prev->end;
}

//Section scroll structure
typedef struct
{
	fixed_t start;   //Seconds
	fixed_t length;  //Seconds
	u16 start_step;  //Sub-steps
	u16 length_step; //Sub-steps
	
	fixed_t size; //Note height
} SectionScroll;

static void Stage_GetSectionScroll(SectionScroll *scroll, Section *section)
{
	//Get BPM
	u16 bpm = section->flag & SECTION_FLAG_BPM_MASK;
	
	//Get section step info
	scroll->start_step = Stage_GetSectionStart(section);
	scroll->length_step = section->end - scroll->start_step;
	
	//Get section time length
	scroll->length = (scroll->length_step * FIXED_DEC(15,1) / 12) * 24 / bpm;
	
	//Get note height
	scroll->size = FIXED_MUL(stage.speed, scroll->length * (12 * 150) / scroll->length_step) + FIXED_UNIT;
}

//Note hit detection
static u8 Stage_HitNote(PlayerState *this, u8 type, fixed_t offset)
{
	//Get hit type
	if (offset < 0)
		offset = -offset;
	
	u8 hit_type;
	if (offset > stage.late_safe * 10 / 11)
		hit_type = 3; //SHIT
	else if (offset > stage.late_safe * 6 / 11)
		hit_type = 2; //BAD
	else if (offset > stage.late_safe * 3 / 11)
		hit_type = 1; //GOOD
	else
		hit_type = 0; //SICK
	
	//Increment combo and score
	this->combo++;
	
	static const s32 score_inc[] = {
		35, //SICK
		20, //GOOD
		10, //BAD
		 5, //SHIT
	};
	this->score += score_inc[hit_type];
	this->min_accuracy += 3 - hit_type;
	this->refresh_score = true;
	
	//Restore vocals and health
	Stage_StartVocal();
	u16 heal = (hit_type > 1) ? -400 * hit_type : 800;
	this->health += heal - this->antispam;
	this->max_accuracy += 3;
	
	if (stage.prefs.combostack)
	{
		//Create combo object telling of our combo
		Obj_Combo *combo = Obj_Combo_New(
			this->character->focus_x,
			this->character->focus_y,
			hit_type,
			this->combo >= 10 ? this->combo : 0xFFFF
		);
		if (combo != NULL)
			ObjectList_Add(&stage.objlist_fg, (Object*)combo);
	}
	
	//Create note splashes if SICK
	if (hit_type == 0 && stage.prefs.notesplashes)
	{
		for (int i = 0; i < 3; i++)
		{
			//Create splash object
			Obj_Splash *splash = Obj_Splash_New(
				stage.note_x[type ^ stage.note_swap],
				stage.note_y[type ^ stage.note_swap] * (stage.prefs.downscroll ? -1 : 1),
				type & 0x3
			);
			if (splash != NULL)
				ObjectList_Add(&stage.objlist_splash, (Object*)splash);
		}
	}
	
	return hit_type;
}

static void Stage_MissNote(PlayerState *this, u8 type)
{
	this->max_accuracy += 3;
	this->refresh_score = true;
	this->miss += 1;
	
	if (this->character->spec & CHAR_SPEC_MISSANIM)
		this->character->set_anim(this->character, note_anims[type & 0x3][2]);
	else
		this->character->set_anim(this->character, note_anims[type & 0x3][0]);
	
	if (this->combo && stage.prefs.combostack)
	{
		//Kill combo
		if (stage.gf != NULL && this->combo > 5)
			stage.gf->set_anim(stage.gf, CharAnim_DownAlt); //Cry if we lost a large combo
		this->combo = 0;
		
		//Create combo object telling of our lost combo
		Obj_Combo *combo = Obj_Combo_New(
			this->character->focus_x,
			this->character->focus_y,
			0xFF,
			0
		);
		if (combo != NULL)
			ObjectList_Add(&stage.objlist_fg, (Object*)combo);
	}
}

static void Stage_NoteCheck(PlayerState *this, u8 type)
{
	//Perform note check
	for (Note *note = stage.cur_note;; note++)
	{
		if (!(note->type & NOTE_FLAG_MINE))
		{
			//Check if note can be hit
			fixed_t note_fp = (fixed_t)note->pos << FIXED_SHIFT;
			if (note_fp - stage.early_safe > stage.note_scroll)
				break;
			if (note_fp + stage.late_safe < stage.note_scroll)
				continue;
			if ((note->type & NOTE_FLAG_HIT) || (note->type & (NOTE_FLAG_OPPONENT | 0x3)) != type || (note->type & NOTE_FLAG_SUSTAIN))
				continue;
			
			//Hit the note
			note->type |= NOTE_FLAG_HIT;
			
			NoteHitEvent(note->type);
			this->character->set_anim(this->character, note_anims[type & 0x3][(note->type & NOTE_FLAG_ALT_ANIM) != 0]);
			u8 hit_type = Stage_HitNote(this, type, stage.note_scroll - note_fp);
			this->arrow_hitan[type & 0x3] = stage.step_time;
			(void)hit_type;
			return;
		}
		else
		{
			//Check if mine can be hit
			fixed_t note_fp = (fixed_t)note->pos << FIXED_SHIFT;
			if (note_fp - (stage.late_safe * 3 / 5) > stage.note_scroll)
				break;
			if (note_fp + (stage.late_safe * 2 / 5) < stage.note_scroll)
				continue;
			if ((note->type & NOTE_FLAG_HIT) || (note->type & (NOTE_FLAG_OPPONENT | 0x3)) != type || (note->type & NOTE_FLAG_SUSTAIN))
				continue;
			
			//Hit the mine
			note->type |= NOTE_FLAG_HIT;
			
			NoteHitEvent(note->type);
			this->health -= 2000;
			if (this->character->spec & CHAR_SPEC_MISSANIM)
				this->character->set_anim(this->character, note_anims[type & 0x3][2]);
			else
				this->character->set_anim(this->character, note_anims[type & 0x3][0]);
			this->arrow_hitan[type & 0x3] = -1;
			return;
		}
	}
	
	//Missed a note
	this->arrow_hitan[type & 0x3] = -1;
	
	if (!stage.prefs.ghost)
	{
		Stage_MissNote(this, type);
		
		this->health -= 400;
		this->score -= 1;
		this->refresh_score = true;
	}
	else if(this->antispam < 1600)
		this->antispam += 400;
}

static void Stage_SustainCheck(PlayerState *this, u8 type)
{
	//Perform note check
	for (Note *note = stage.cur_note;; note++)
	{
		//Check if note can be hit
		fixed_t note_fp = (fixed_t)note->pos << FIXED_SHIFT;
		if (note_fp - stage.early_sus_safe > stage.note_scroll)
			break;
		if (note_fp + stage.late_sus_safe < stage.note_scroll)
			continue;
		if ((note->type & NOTE_FLAG_HIT) || (note->type & (NOTE_FLAG_OPPONENT | 0x3)) != type || !(note->type & NOTE_FLAG_SUSTAIN))
			continue;
		
		//Hit the note
		note->type |= NOTE_FLAG_HIT;
		
		this->character->set_anim(this->character, note_anims[type & 0x3][(note->type & NOTE_FLAG_ALT_ANIM) != 0]);
		
		Stage_StartVocal();
		this->arrow_hitan[type & 0x3] = stage.step_time;
	}
}

static void CheckNewScore()
{
	if (stage.prefs.mode == StageMode_Normal && !stage.prefs.botplay && (timer.timer / ((stage.prefs.palmode) ? 50 : 60)) == 0 && timer.timer <= 5)
	{
		if (stage.player_state[0].score >= stage.prefs.savescore[stage.stage_id][stage.stage_diff])
			stage.prefs.savescore[stage.stage_id][stage.stage_diff] = stage.player_state[0].score;
	}
}

static void Stage_ProcessPlayer(PlayerState *this, Pad *pad, boolean playing)
{
	//Handle player note presses
	if (stage.prefs.botplay == 0) {
		if (playing)
		{
			u8 i = (this->character == stage.opponent) ? NOTE_FLAG_OPPONENT : 0;
			
			this->pad_held = this->character->pad_held = pad->held;
			this->pad_press = pad->press;
			
			if (this->pad_held & stage.prefs.control_keys[0])
				Stage_SustainCheck(this, 0 | i);
			if (this->pad_held & stage.prefs.control_keys[1])
				Stage_SustainCheck(this, 1 | i);
			if (this->pad_held & stage.prefs.control_keys[2])
				Stage_SustainCheck(this, 2 | i);
			if (this->pad_held & stage.prefs.control_keys[3])
				Stage_SustainCheck(this, 3 | i);
			
			if (this->pad_press & stage.prefs.control_keys[0])
				Stage_NoteCheck(this, 0 | i);
			if (this->pad_press & stage.prefs.control_keys[1])
				Stage_NoteCheck(this, 1 | i);
			if (this->pad_press & stage.prefs.control_keys[2])
				Stage_NoteCheck(this, 2 | i);
			if (this->pad_press & stage.prefs.control_keys[3])
				Stage_NoteCheck(this, 3 | i);
		}
		else
		{
			this->pad_held = this->character->pad_held = 0;
			this->pad_press = 0;
		}
	}
	
	if (this->antispam > 0)
		this->antispam -= 50;
	
	if (stage.prefs.botplay == 1) {
		//Do perfect note checks
		if (playing)
		{
			u8 i = (this->character == stage.opponent) ? NOTE_FLAG_OPPONENT : 0;
			
			u8 hit[4] = {0, 0, 0, 0};
			for (Note *note = stage.cur_note;; note++)
			{
				//Check if note can be hit
				fixed_t note_fp = (fixed_t)note->pos << FIXED_SHIFT;
				if (note_fp - stage.early_safe - FIXED_DEC(12,1) > stage.note_scroll)
					break;
				if (note_fp + stage.late_safe < stage.note_scroll)
					continue;
				if ((note->type & NOTE_FLAG_MINE) || (note->type & NOTE_FLAG_OPPONENT) != i)
					continue;
				
				//Handle note hit
				if (!(note->type & NOTE_FLAG_SUSTAIN))
				{
					if (note->type & NOTE_FLAG_HIT)
						continue;
					if (stage.note_scroll >= note_fp)
						hit[note->type & 0x3] |= 1;
					else if (!(hit[note->type & 0x3] & 8))
						hit[note->type & 0x3] |= 2;
				}
				else if (!(hit[note->type & 0x3] & 2))
				{
					if (stage.note_scroll <= note_fp)
						hit[note->type & 0x3] |= 4;
					hit[note->type & 0x3] |= 8;
				}
			}
			
			//Handle input
			this->pad_held = 0;
			this->pad_press = 0;
			
			for (u8 j = 0; j < 4; j++)
			{
				if (hit[j] & 5)
				{
					this->pad_held |= stage.prefs.control_keys[j];
					Stage_SustainCheck(this, j | i);
				}
				if (hit[j] & 1)
				{
					this->pad_press |= stage.prefs.control_keys[j];
					Stage_NoteCheck(this, j | i);
				}
			}
			
			this->character->pad_held = this->pad_held;
		}
		else
		{
			this->pad_held = this->character->pad_held = 0;
			this->pad_press = 0;
		}
	}
}

//Stage drawing functions
void Stage_DrawRect(const RECT_FIXED *dst, fixed_t zoom, u8 cr, u8 cg, u8 cb)
{
    fixed_t xz = dst->x;
    fixed_t yz = dst->y;
    fixed_t wz = dst->w;
    fixed_t hz = dst->h;
    
    fixed_t l = (SCREEN_WIDTH2  * FIXED_UNIT) + FIXED_MUL(xz, zoom);// + FIXED_DEC(1,2);
    fixed_t t = (SCREEN_HEIGHT2 * FIXED_UNIT) + FIXED_MUL(yz, zoom);// + FIXED_DEC(1,2);
    fixed_t r = l + FIXED_MUL(wz, zoom);
    fixed_t b = t + FIXED_MUL(hz, zoom);
    
    l /= FIXED_UNIT;
    t /= FIXED_UNIT;
    r /= FIXED_UNIT;
    b /= FIXED_UNIT;
    
    RECT sdst = {
        l,
        t,
        r - l,
        b - t,
    };
    Gfx_DrawRect(&sdst, cr, cg, cb);
}

void Stage_BlendRect(const RECT_FIXED *dst, fixed_t zoom, u8 cr, u8 cg, u8 cb, int mode)
{
    fixed_t xz = dst->x;
    fixed_t yz = dst->y;
    fixed_t wz = dst->w;
    fixed_t hz = dst->h;
    
    fixed_t l = (SCREEN_WIDTH2  * FIXED_UNIT) + FIXED_MUL(xz, zoom);// + FIXED_DEC(1,2);
    fixed_t t = (SCREEN_HEIGHT2 * FIXED_UNIT) + FIXED_MUL(yz, zoom);// + FIXED_DEC(1,2);
    fixed_t r = l + FIXED_MUL(wz, zoom);
    fixed_t b = t + FIXED_MUL(hz, zoom);
    
    l /= FIXED_UNIT;
    t /= FIXED_UNIT;
    r /= FIXED_UNIT;
    b /= FIXED_UNIT;
    
    RECT sdst = {
        l,
        t,
        r - l,
        b - t,
    };
    Gfx_BlendRect(&sdst, cr, cg, cb, mode);
}

void Stage_DrawTexRotateCol(Gfx_Tex *tex, const RECT *src, const RECT_FIXED *dst, u8 angle, fixed_t hx, fixed_t hy, u8 cr, u8 cg, u8 cb, fixed_t zoom, fixed_t rotation)
{
    fixed_t xz = dst->x;
    fixed_t yz = dst->y;
    fixed_t wz = dst->w;
    fixed_t hz = dst->h;
    
    #ifdef STAGE_NOHUD
        if (tex == &stage.tex_hud0 || tex == &stage.tex_hud1)
            return;
    #endif
    
    // Calculate the rotated coordinates
    u8 rotationAngle = rotation / FIXED_UNIT;  // Specify the desired rotation angle (in degrees)
    fixed_t rotatedX = FIXED_MUL(xz,FIXED_DEC(MUtil_Cos(rotationAngle),256)) - FIXED_MUL(yz,FIXED_DEC(MUtil_Sin(rotationAngle),256));
    fixed_t rotatedY = FIXED_MUL(xz,FIXED_DEC(MUtil_Sin(rotationAngle),256)) + FIXED_MUL(yz,FIXED_DEC(MUtil_Cos(rotationAngle),256));
	
    fixed_t l = (SCREEN_WIDTH2  * FIXED_UNIT) + FIXED_MUL(rotatedX, zoom);// + FIXED_DEC(1,2);
    fixed_t t = (SCREEN_HEIGHT2 * FIXED_UNIT) + FIXED_MUL(rotatedY, zoom);// + FIXED_DEC(1,2);
    fixed_t r = l + FIXED_MUL(wz, zoom);
    fixed_t b = t + FIXED_MUL(hz, zoom);
    
    l /= FIXED_UNIT;
    t /= FIXED_UNIT;
    r /= FIXED_UNIT;
    b /= FIXED_UNIT;
    
    RECT sdst = {
        l,
        t,
        r - l,
        b - t,
    };
    Gfx_DrawTexRotateCol(tex, src, &sdst, angle + rotationAngle, hx, hy, cr, cg, cb);
}

void Stage_DrawTexRotate(Gfx_Tex *tex, const RECT *src, const RECT_FIXED *dst, u8 angle, fixed_t hx, fixed_t hy, fixed_t zoom, fixed_t rotation)
{
    Stage_DrawTexRotateCol(tex, src, dst, angle, hx, hy, 128, 128, 128, zoom, rotation);
}

void Stage_DrawTexCol(Gfx_Tex *tex, const RECT *src, const RECT_FIXED *dst, fixed_t zoom, fixed_t rotation, u8 cr, u8 cg, u8 cb)
{
    fixed_t xz = dst->x;
    fixed_t yz = dst->y;
    fixed_t wz = dst->w;
    fixed_t hz = dst->h;
    
    #ifdef STAGE_NOHUD
        if (tex == &stage.tex_hud0 || tex == &stage.tex_hud1)
            return;
    #endif
    
    // Calculate the rotated coordinates
    u8 rotationAngle = rotation / FIXED_UNIT;  // Specify the desired rotation angle (in degrees)
    fixed_t rotatedX = FIXED_MUL(xz,FIXED_DEC(MUtil_Cos(rotationAngle),256)) - FIXED_MUL(yz,FIXED_DEC(MUtil_Sin(rotationAngle),256));
    fixed_t rotatedY = FIXED_MUL(xz,FIXED_DEC(MUtil_Sin(rotationAngle),256)) + FIXED_MUL(yz,FIXED_DEC(MUtil_Cos(rotationAngle),256));
	
    fixed_t l = (SCREEN_WIDTH2  * FIXED_UNIT) + FIXED_MUL(rotatedX, zoom) + FIXED_DEC(1,2);
    fixed_t t = (SCREEN_HEIGHT2 * FIXED_UNIT) + FIXED_MUL(rotatedY, zoom) + FIXED_DEC(1,2);
    fixed_t r = l + FIXED_MUL(wz, zoom);
    fixed_t b = t + FIXED_MUL(hz, zoom);
    
    l /= FIXED_UNIT;
    t /= FIXED_UNIT;
    r /= FIXED_UNIT;
    b /= FIXED_UNIT;
    
    RECT sdst = {
        l,
        t,
        r - l,
        b - t,
    };
	
    Gfx_DrawTexRotateCol(tex, src, &sdst, rotationAngle, 0, 0, cr, cg, cb);
}

void Stage_DrawTex(Gfx_Tex *tex, const RECT *src, const RECT_FIXED *dst, fixed_t zoom, fixed_t rotation)
{
    Stage_DrawTexCol(tex, src, dst, zoom, rotation, 0x80, 0x80, 0x80);
}

void Stage_DrawTexArbCol(Gfx_Tex *tex, const RECT *src, const POINT_FIXED *p0, const POINT_FIXED *p1, const POINT_FIXED *p2, const POINT_FIXED *p3, u8 r, u8 g, u8 b, fixed_t zoom, fixed_t rotation)
{
    // Don't draw if HUD and HUD is disabled
    #ifdef STAGE_NOHUD
        if (tex == &stage.tex_hud0 || tex == &stage.tex_hud1)
            return;
    #endif
    
    u8 rotationAngle = rotation / FIXED_UNIT;  // Specify the desired rotation angle (in degrees)
    fixed_t cosAngle = FIXED_DEC(MUtil_Cos(rotationAngle), 256);
    fixed_t sinAngle = FIXED_DEC(MUtil_Sin(rotationAngle), 256);

    fixed_t x0 = FIXED_MUL(p0->x, cosAngle) - FIXED_MUL(p0->y, sinAngle);
    fixed_t y0 = FIXED_MUL(p0->x, sinAngle) + FIXED_MUL(p0->y, cosAngle);
    fixed_t x1 = FIXED_MUL(p1->x, cosAngle) - FIXED_MUL(p1->y, sinAngle);
    fixed_t y1 = FIXED_MUL(p1->x, sinAngle) + FIXED_MUL(p1->y, cosAngle);
    fixed_t x2 = FIXED_MUL(p2->x, cosAngle) - FIXED_MUL(p2->y, sinAngle);
    fixed_t y2 = FIXED_MUL(p2->x, sinAngle) + FIXED_MUL(p2->y, cosAngle);
    fixed_t x3 = FIXED_MUL(p3->x, cosAngle) - FIXED_MUL(p3->y, sinAngle);
    fixed_t y3 = FIXED_MUL(p3->x, sinAngle) + FIXED_MUL(p3->y, cosAngle);

    // Get screen-space points
    POINT s0 = {SCREEN_WIDTH2 + (FIXED_MUL(x0, zoom) / FIXED_UNIT), SCREEN_HEIGHT2 + (FIXED_MUL(y0, zoom) / FIXED_UNIT)};
    POINT s1 = {SCREEN_WIDTH2 + (FIXED_MUL(x1, zoom) / FIXED_UNIT), SCREEN_HEIGHT2 + (FIXED_MUL(y1, zoom) / FIXED_UNIT)};
    POINT s2 = {SCREEN_WIDTH2 + (FIXED_MUL(x2, zoom) / FIXED_UNIT), SCREEN_HEIGHT2 + (FIXED_MUL(y2, zoom) / FIXED_UNIT)};
    POINT s3 = {SCREEN_WIDTH2 + (FIXED_MUL(x3, zoom) / FIXED_UNIT), SCREEN_HEIGHT2 + (FIXED_MUL(y3, zoom) / FIXED_UNIT)};

    Gfx_DrawTexArbCol(tex, src, &s0, &s1, &s2, &s3, r, g, b);
}

void Stage_DrawTexArb(Gfx_Tex *tex, const RECT *src, const POINT_FIXED *p0, const POINT_FIXED *p1, const POINT_FIXED *p2, const POINT_FIXED *p3, fixed_t zoom, fixed_t rotation)
{
    Stage_DrawTexArbCol(tex, src, p0, p1, p2, p3, 0x80, 0x80, 0x80, zoom, rotation);
}

void Stage_BlendTexArbCol(Gfx_Tex *tex, const RECT *src, const POINT_FIXED *p0, const POINT_FIXED *p1, const POINT_FIXED *p2, const POINT_FIXED *p3, fixed_t zoom, fixed_t rotation, u8 r, u8 g, u8 b, u8 mode)
{
    //Don't draw if HUD and HUD is disabled
    #ifdef STAGE_NOHUD
        if (tex == &stage.tex_hud0 || tex == &stage.tex_hud1)
            return;
    #endif
    
    u8 rotationAngle = rotation / FIXED_UNIT;  // Specify the desired rotation angle (in degrees)
    fixed_t cosAngle = FIXED_DEC(MUtil_Cos(rotationAngle), 256);
    fixed_t sinAngle = FIXED_DEC(MUtil_Sin(rotationAngle), 256);

    fixed_t x0 = FIXED_MUL(p0->x, cosAngle) - FIXED_MUL(p0->y, sinAngle);
    fixed_t y0 = FIXED_MUL(p0->x, sinAngle) + FIXED_MUL(p0->y, cosAngle);
    fixed_t x1 = FIXED_MUL(p1->x, cosAngle) - FIXED_MUL(p1->y, sinAngle);
    fixed_t y1 = FIXED_MUL(p1->x, sinAngle) + FIXED_MUL(p1->y, cosAngle);
    fixed_t x2 = FIXED_MUL(p2->x, cosAngle) - FIXED_MUL(p2->y, sinAngle);
    fixed_t y2 = FIXED_MUL(p2->x, sinAngle) + FIXED_MUL(p2->y, cosAngle);
    fixed_t x3 = FIXED_MUL(p3->x, cosAngle) - FIXED_MUL(p3->y, sinAngle);
    fixed_t y3 = FIXED_MUL(p3->x, sinAngle) + FIXED_MUL(p3->y, cosAngle);

    // Get screen-space points
    POINT s0 = {SCREEN_WIDTH2 + (FIXED_MUL(x0, zoom) / FIXED_UNIT), SCREEN_HEIGHT2 + (FIXED_MUL(y0, zoom) / FIXED_UNIT)};
    POINT s1 = {SCREEN_WIDTH2 + (FIXED_MUL(x1, zoom) / FIXED_UNIT), SCREEN_HEIGHT2 + (FIXED_MUL(y1, zoom) / FIXED_UNIT)};
    POINT s2 = {SCREEN_WIDTH2 + (FIXED_MUL(x2, zoom) / FIXED_UNIT), SCREEN_HEIGHT2 + (FIXED_MUL(y2, zoom) / FIXED_UNIT)};
    POINT s3 = {SCREEN_WIDTH2 + (FIXED_MUL(x3, zoom) / FIXED_UNIT), SCREEN_HEIGHT2 + (FIXED_MUL(y3, zoom) / FIXED_UNIT)};
    
    Gfx_BlendTexArbCol(tex, src, &s0, &s1, &s2, &s3, r, g, b, mode);
}

void Stage_BlendTexArb(Gfx_Tex *tex, const RECT *src, const POINT_FIXED *p0, const POINT_FIXED *p1, const POINT_FIXED *p2, const POINT_FIXED *p3, fixed_t zoom, fixed_t rotation, u8 mode)
{
    Stage_BlendTexArbCol(tex, src, p0, p1, p2, p3, zoom, rotation, 0x80, 0x80, 0x80, mode);
}

void Stage_BlendTex(Gfx_Tex *tex, const RECT *src, const RECT_FIXED *dst, fixed_t zoom, fixed_t rotation, u8 mode)
{
	fixed_t xz = dst->x;
	fixed_t yz = dst->y;
	fixed_t wz = dst->w;
	fixed_t hz = dst->h;

	//Don't draw if HUD and is disabled
	if (tex == &stage.tex_hud0)
	{
		#ifdef STAGE_NOHUD
			return;
		#endif
	}
	
    // Calculate the rotated coordinates
    u8 rotationAngle = rotation / FIXED_UNIT;  // Specify the desired rotation angle (in degrees)
    fixed_t rotatedX = FIXED_MUL(xz,FIXED_DEC(MUtil_Cos(rotationAngle),256)) - FIXED_MUL(yz,FIXED_DEC(MUtil_Sin(rotationAngle),256));
    fixed_t rotatedY = FIXED_MUL(xz,FIXED_DEC(MUtil_Sin(rotationAngle),256)) + FIXED_MUL(yz,FIXED_DEC(MUtil_Cos(rotationAngle),256));
	
    fixed_t l = (SCREEN_WIDTH2  * FIXED_UNIT) + FIXED_MUL(rotatedX, zoom);// + FIXED_DEC(1,2);
    fixed_t t = (SCREEN_HEIGHT2 * FIXED_UNIT) + FIXED_MUL(rotatedY, zoom);// + FIXED_DEC(1,2);
    fixed_t r = l + FIXED_MUL(wz, zoom);
    fixed_t b = t + FIXED_MUL(hz, zoom);
    
    l /= FIXED_UNIT;
    t /= FIXED_UNIT;
    r /= FIXED_UNIT;
    b /= FIXED_UNIT;
    
    RECT sdst = {
        l,
        t,
        r - l,
        b - t,
    };
	
    Gfx_BlendTexRotate(tex, src, &sdst, rotationAngle, 0, 0, mode);
}

//Stage HUD functions
static void Stage_DrawHealth(s16 health, u16 health_i[2][4], boolean ox) 
{
    // Check if we should use 'dying' frame
    u8 status;
    if (ox)
        status = !(health > 2000);
    else
        status = (health >= 18000);
	
    if (stage.prefs.mode == StageMode_Swap)
        ox = !ox;
	
    // Get src and dst
    fixed_t hx = (105 << FIXED_SHIFT) * (10000 - health) / 10000;
    if (stage.prefs.mode == StageMode_Swap)
        hx = -hx;

    fixed_t temp = hx + ox * FIXED_DEC(32,1) - FIXED_DEC(12,1);
    RECT src = {
        health_i[status][0],
        health_i[status][1],
        health_i[status][2],
        health_i[status][3]
    };
    RECT_FIXED dst = {
        temp,
        (SCREEN_HEIGHT2 - 34) << FIXED_SHIFT,
        src.w << FIXED_SHIFT,
        src.h << FIXED_SHIFT
    };

    if (ox)
        dst.w = -dst.w;

    if (stage.prefs.downscroll)
        dst.y = -dst.y;

    // Draw health icon
    Stage_DrawTexRotate(&stage.tex_icons, &src, &dst, 0, src.w / 2, src.h / 2, FIXED_MUL(stage.bump, stage.sbump), stage.camera.hudangle);
}

static void Stage_DrawHealthBar(s16 x, s32 color)
{	
	//colors for health bar
	u8 red = (color >> 16) & 0xFF;
	u8 blue = (color >> 8) & 0xFF;
	u8 green = (color) & 0xFF;
	//Get src and dst
	RECT src = {
		0,
	    228,
		x,
		8
	};
	
	RECT_FIXED dst = {
		FIXED_DEC(-106,1), 
		(SCREEN_HEIGHT2 - 36) << FIXED_SHIFT, 
		FIXED_DEC(src.w,1), 
		FIXED_DEC(8,1)
	};
	
	if (stage.prefs.downscroll)
		dst.y = FIXED_DEC(-SCREEN_HEIGHT2 + 32,1);
	
	Stage_DrawTexCol(&stage.tex_hud0, &src, &dst, stage.bump, stage.camera.hudangle, red >> 1, blue >> 1, green >> 1);
}

static void Stage_DrawStrum(u8 i, RECT *note_src, RECT_FIXED *note_dst)
{
	(void)note_dst;
	
	PlayerState *this = &stage.player_state[(i & NOTE_FLAG_OPPONENT) != 0];
	i &= 0x3;
	
	if (this->arrow_hitan[i] > 0)
	{
		//Play hit animation
		u8 frame = ((this->arrow_hitan[i] << 1) / stage.step_time) & 1;
		note_src->x = (i + 1) << 5;
		note_src->y = 64 - (frame << 5);
		
		this->arrow_hitan[i] -= timer_dt;
		if (this->arrow_hitan[i] <= 0)
		{
			if (this->pad_held & stage.prefs.control_keys[i])
				this->arrow_hitan[i] = 1;
			else
				this->arrow_hitan[i] = 0;
		}
	}
	else if (this->arrow_hitan[i] < 0)
	{
		//Play depress animation
		note_src->x = (i + 1) << 5;
		note_src->y = 96;
		if (!(this->pad_held & stage.prefs.control_keys[i]))
			this->arrow_hitan[i] = 0;
	}
	else
	{
		note_src->x = 0;
		note_src->y = i << 5;
	}
}

static void Stage_DrawNotes(void)
{
	//Check if opponent should draw as bot
	u8 bot = (stage.prefs.mode >= StageMode_2P) ? 0 : NOTE_FLAG_OPPONENT;
	
	//Initialize scroll state
	SectionScroll scroll;
	scroll.start = stage.time_base;
	
	Section *scroll_section = stage.section_base;
	Stage_GetSectionScroll(&scroll, scroll_section);
	
	//Push scroll back until cur_note is properly contained
	while (scroll.start_step > stage.cur_note->pos)
	{
		//Look for previous section
		Section *prev_section = Stage_GetPrevSection(scroll_section);
		if (prev_section == NULL)
			break;
		
		//Push scroll back
		scroll_section = prev_section;
		Stage_GetSectionScroll(&scroll, scroll_section);
		scroll.start -= scroll.length;
	}
	
	//Draw notes
	for (Note *note = stage.cur_note; note->pos != 0xFFFF; note++)
	{
		//Update scroll
		while (note->pos >= scroll_section->end)
		{
			//Push scroll forward
			scroll.start += scroll.length;
			Stage_GetSectionScroll(&scroll, ++scroll_section);
		}
		
		//Get note information
		u8 i = (note->type & NOTE_FLAG_OPPONENT) != 0;
		PlayerState *this = &stage.player_state[i];
		
		fixed_t note_fp = (fixed_t)note->pos << FIXED_SHIFT;
		fixed_t time = (scroll.start - stage.song_time) + (scroll.length * (note->pos - scroll.start_step) / scroll.length_step);
		fixed_t y = stage.note_y[(note->type & 0x7)] + FIXED_MUL(stage.speed, time * 150);
		
		//Check if went above screen
		if (y < FIXED_DEC(-16 - SCREEN_HEIGHT2, 1))
		{
			//Wait for note to exit late time
			if (note_fp + stage.late_safe >= stage.note_scroll)
				continue;
			
			//Miss note if player's note
			if (!(note->type & (bot | NOTE_FLAG_HIT | NOTE_FLAG_MINE)))
			{
				//Missed note
				Stage_CutVocal();
				Stage_MissNote(this, note->type);
				NoteMissEvent(note->type,i);
				this->health -= 475;
			}
			
			//Update current note
			stage.cur_note++;
		}
		else
		{
			//Don't draw if below screen
			RECT note_src;
			RECT_FIXED note_dst;
			if (y > (FIXED_DEC(SCREEN_HEIGHT,2) + scroll.size) || note->pos == 0xFFFF)
				break;
			
			//Draw note
			if (note->type & NOTE_FLAG_SUSTAIN)
			{
				//Check for sustain clipping
				fixed_t clip;
				y -= scroll.size;
				if ((note->type & (bot | NOTE_FLAG_HIT)) || ((this->pad_held & stage.prefs.control_keys[note->type & 0x3]) && (note_fp + stage.late_sus_safe >= stage.note_scroll)))
				{
					clip = stage.note_y[(note->type & 0x7)] - y;
					if (clip < 0)
						clip = 0;
				}
				else
				{
					clip = 0;
				}
				
				//Draw sustain
				if (note->type & NOTE_FLAG_SUSTAIN_END)
				{
					if (clip < (24 << FIXED_SHIFT))
					{
						note_src.x = 160;
						note_src.y = ((note->type & 0x3) << 5) + 4 + (clip >> FIXED_SHIFT);
						note_src.w = 32;
						note_src.h = 28 - (clip >> FIXED_SHIFT);
						
						note_dst.x = stage.note_x[(note->type & 0x7)] - FIXED_DEC(16,1);
						note_dst.y = y + clip;
						note_dst.w = note_src.w << FIXED_SHIFT;
						note_dst.h = note_src.h << FIXED_SHIFT;
						
						if (stage.prefs.downscroll)
						{
							note_dst.y = -note_dst.y;
							note_dst.h = -note_dst.h;
						}
						//draw for opponent
						if (stage.prefs.opponentnotes && note->type & NOTE_FLAG_OPPONENT)
							if (stage.prefs.middlescroll)
								Stage_BlendTex(&stage.tex_hud0, &note_src, &note_dst, stage.bump, stage.camera.hudangle, 1);
							else
								Stage_DrawTex(&stage.tex_hud0, &note_src, &note_dst, stage.bump, stage.camera.hudangle);
						else if (!(note->type & NOTE_FLAG_OPPONENT))
							Stage_DrawTex(&stage.tex_hud0, &note_src, &note_dst, stage.bump, stage.camera.hudangle);
					}
				}
				else
				{
					//Get note height
					fixed_t next_time = (scroll.start - stage.song_time) + (scroll.length * (note->pos + 12 - scroll.start_step) / scroll.length_step);
					fixed_t next_y = stage.note_y[(note->type & 0x7)] + FIXED_MUL(stage.speed, next_time * 150) - scroll.size;
					fixed_t next_size = next_y - y;
					
					if (clip < next_size)
					{
						note_src.x = 160;
						note_src.y = ((note->type & 0x3) << 5);
						note_src.w = 32;
						note_src.h = 16;
						
						note_dst.x = stage.note_x[(note->type & 0x7)] - FIXED_DEC(16,1);
						note_dst.y = y + clip;
						note_dst.w = note_src.w << FIXED_SHIFT;
						note_dst.h = (next_y - y) - clip;
						
						if (stage.prefs.downscroll)
							note_dst.y = -note_dst.y - note_dst.h;
						//draw for opponent
						if (stage.prefs.opponentnotes && note->type & NOTE_FLAG_OPPONENT)
							if (stage.prefs.middlescroll)
								Stage_BlendTex(&stage.tex_hud0, &note_src, &note_dst, stage.bump, stage.camera.hudangle, 1);
							else
								Stage_DrawTex(&stage.tex_hud0, &note_src, &note_dst, stage.bump, stage.camera.hudangle);
						else if (!(note->type & NOTE_FLAG_OPPONENT))
							Stage_DrawTex(&stage.tex_hud0, &note_src, &note_dst, stage.bump, stage.camera.hudangle);
					}
				}
			}
			else if (note->type & NOTE_FLAG_MINE)
			{
				//Don't draw if already hit
				if (note->type & NOTE_FLAG_HIT)
					continue;
				
				//Draw note body
				note_src.x = 192 + ((note->type & 0x1) << 5);
				note_src.y = (note->type & 0x2) << 4;
				note_src.w = 32;
				note_src.h = 32;
				
				note_dst.x = stage.note_x[(note->type & 0x7)] - FIXED_DEC(16,1);
				note_dst.y = y - FIXED_DEC(16,1);
				note_dst.w = note_src.w << FIXED_SHIFT;
				note_dst.h = note_src.h << FIXED_SHIFT;
				
				if (stage.prefs.downscroll)
					note_dst.y = -note_dst.y - note_dst.h;
				//draw for opponent
				if (stage.prefs.middlescroll && note->type & NOTE_FLAG_OPPONENT)
					Stage_BlendTex(&stage.tex_hud0, &note_src, &note_dst, stage.bump, stage.camera.hudangle, 1);
				else
					Stage_DrawTex(&stage.tex_hud0, &note_src, &note_dst, stage.bump, stage.camera.hudangle);
				
				//Draw note fire
				note_src.x = 192 + ((animf_count & 0x1) << 5);
				note_src.y = 64 + ((animf_count & 0x2) * 24);
				note_src.w = 32;
				note_src.h = 48;
					
				if (stage.prefs.downscroll)
				{
					note_dst.y += note_dst.h;
					note_dst.h = note_dst.h * -3 / 2;
				}
				else
				{
					note_dst.h = note_dst.h * 3 / 2;
				}
				//draw for opponent
				if (stage.prefs.opponentnotes && note->type & NOTE_FLAG_OPPONENT)
					if (stage.prefs.middlescroll)
						Stage_BlendTex(&stage.tex_hud0, &note_src, &note_dst, stage.bump, stage.camera.hudangle, 1);
					else
						Stage_DrawTex(&stage.tex_hud0, &note_src, &note_dst, stage.bump, stage.camera.hudangle);
				else if (!(note->type & NOTE_FLAG_OPPONENT))
					Stage_DrawTex(&stage.tex_hud0, &note_src, &note_dst, stage.bump, stage.camera.hudangle);
				
			}
			else
			{
				//Don't draw if already hit
				if (note->type & NOTE_FLAG_HIT)
					continue;
				
				//Draw note
				note_src.x = 32 + ((note->type & 0x3) << 5);
				note_src.y = 0;
				note_src.w = 32;
				note_src.h = 32;
				
				note_dst.x = stage.note_x[(note->type & 0x7)] - FIXED_DEC(16,1);
				note_dst.y = y - FIXED_DEC(16,1);
				note_dst.w = note_src.w << FIXED_SHIFT;
				note_dst.h = note_src.h << FIXED_SHIFT;
				
				if (stage.prefs.downscroll)
					note_dst.y = -note_dst.y - note_dst.h;
				//draw for opponent
				if (stage.prefs.opponentnotes && note->type & NOTE_FLAG_OPPONENT)
					if (stage.prefs.middlescroll)
						Stage_BlendTex(&stage.tex_hud0, &note_src, &note_dst, stage.bump, stage.camera.hudangle, 1);
					else
						Stage_DrawTex(&stage.tex_hud0, &note_src, &note_dst, stage.bump, stage.camera.hudangle);
				else if (!(note->type & NOTE_FLAG_OPPONENT))
					Stage_DrawTex(&stage.tex_hud0, &note_src, &note_dst, stage.bump, stage.camera.hudangle);
			}
		}
	}
}

static void Stage_CountDown(void)
{
	static const RECT textures[] = {
		{0, 0, 189, 91},
		{0, 92, 176, 81},
		{0, 174, 77, 59}
	};

	static boolean transparent = false;

	switch (stage.song_step)
	{
	case -21:
		stage.introsound = true;
		break;
	case -20:
		if (stage.introsound)
		{
			Audio_PlaySound(Sounds[2], 0x3fff);
			stage.introsound = false;
		}
		break;
	case -16:
		transparent = true;
		stage.introsound = true;
		break;
	case -15:
		stage.frame = 0;
		if (stage.introsound)
		{
			Audio_PlaySound(Sounds[3], 0x3fff);
			stage.introsound = false;
		}
		break;
	case -11:
		transparent = true;
		stage.introsound = true;
		break;
	case -10:
		stage.frame = 1;
		if (stage.introsound)
		{
			Audio_PlaySound(Sounds[4], 0x3fff);
			stage.introsound = false;
		}
		break;
	case -6:
		transparent = true;
		stage.introsound = true;
		break;
	case -5:
		stage.frame = 2;
		if (stage.introsound)
		{
			Audio_PlaySound(Sounds[5], 0x3fff);
			stage.introsound = false;
		}
		break;
	case -1:
		transparent = true;
		break;
	}

	RECT ready_src = textures[stage.frame];
	RECT_FIXED ready_dst = {
		-(ready_src.w / 2) << FIXED_SHIFT,
		-(ready_src.h / 2) << FIXED_SHIFT,
		FIXED_DEC(ready_src.w, 1),
		FIXED_DEC(ready_src.h, 1)
	};

	if (stage.song_step > -16 && stage.song_step < 0)
	{
		if (!transparent)
			Stage_DrawTex(&stage.tex_intro, &ready_src, &ready_dst, stage.bump, stage.camera.hudangle);
		else
			Stage_BlendTex(&stage.tex_intro, &ready_src, &ready_dst, stage.bump, stage.camera.hudangle, 0);
	}

	transparent = false;
}

//Stage loads
static void Stage_SwapChars(void)
{
	if (stage.prefs.mode == StageMode_Swap)
	{
		Character *temp = stage.player;
		stage.player = stage.opponent;
		stage.opponent = temp;
	}
}

static void Stage_LoadPlayer(void)
{
	//Load player character
	Character_Free(stage.player);
	if(stage.prefs.lowgraphics)
		stage.player = Char_BFLow_New(0, 0);
	else
		stage.player = stage.stage_def->pchar.new(stage.stage_def->pchar.x, stage.stage_def->pchar.y);
}

static void Stage_LoadOpponent(void)
{
	//Load opponent character
	Character_Free(stage.opponent);
	if(stage.prefs.lowgraphics)
		stage.opponent = Char_BFLow_New(0, 0);
	else
		stage.opponent = stage.stage_def->ochar.new(stage.stage_def->ochar.x, stage.stage_def->ochar.y);
}

static void Stage_LoadGirlfriend(void)
{
	//Load girlfriend character
	Character_Free(stage.gf);
	if (stage.stage_def->gchar.new != NULL)
		stage.gf = stage.stage_def->gchar.new(stage.stage_def->gchar.x, stage.stage_def->gchar.y);
	else
		stage.gf = NULL;
}

static void Stage_LoadStage(void)
{
	//Load back
	if (stage.back != NULL)
		stage.back->free(stage.back);
	if(stage.prefs.lowgraphics)
		stage.back = Back_DefaultLow_New();
	else
		stage.back = stage.stage_def->back();
}

static void Stage_LoadChart(void)
{
	//Load stage data
	char chart_path[64];
	if (stage.stage_def->week & 0x80)
	{
		//Use custom path convention
		sprintf(chart_path, "\\SONGS\\CUSTOM.%d.CHT;1", stage.stage_def->week_song);
	}
	else
	{
		//Use standard path convention
		sprintf(chart_path, "\\SONGS\\%d.%d%c.CHT;1", stage.stage_def->week, stage.stage_def->week_song, "ENH"[stage.stage_diff]);
	}
	
	if (stage.chart_data != NULL)
		Mem_Free(stage.chart_data);
	stage.chart_data = IO_Read(chart_path);
	u8 *chart_byte = (u8*)stage.chart_data;
	
	//Directly use section and notes pointers
	stage.sections = (Section*)(chart_byte + 6);
	stage.notes = (Note*)(chart_byte + ((u16*)stage.chart_data)[2]);
		
	for (Note *note = stage.notes; note->pos != 0xFFFF; note++)
		stage.num_notes++;
	
	//Swap chart
	if (stage.prefs.mode == StageMode_Swap)
	{
		for (Note *note = stage.notes; note->pos != 0xFFFF; note++)
			note->type ^= NOTE_FLAG_OPPONENT;
		for (Section *section = stage.sections;; section++)
		{
			section->flag ^= SECTION_FLAG_OPPFOCUS;
			if (section->end == 0xFFFF)
				break;
		}
	}
	
	//Count max scores
	stage.player_state[0].max_score = 0;
	stage.player_state[1].max_score = 0;
	for (Note *note = stage.notes; note->pos != 0xFFFF; note++)
	{
		if (note->type & (NOTE_FLAG_SUSTAIN | NOTE_FLAG_MINE))
			continue;
		if (note->type & NOTE_FLAG_OPPONENT)
			stage.player_state[1].max_score += 35;
		else
			stage.player_state[0].max_score += 35;
	}
	if (stage.prefs.mode >= StageMode_2P && stage.player_state[1].max_score > stage.player_state[0].max_score)
		stage.max_score = stage.player_state[1].max_score;
	else
		stage.max_score = stage.player_state[0].max_score;
	
	stage.cur_section = stage.sections;
	stage.cur_note = stage.notes;
	
	stage.speed = *((fixed_t*)stage.chart_data);
	strcpy(stage.credits, stage.stage_def->credits);
	
	stage.step_crochet = 0;
	stage.time_base = 0;
	stage.step_base = 0;
	stage.section_base = stage.cur_section;
	Stage_ChangeBPM(stage.cur_section->flag & SECTION_FLAG_BPM_MASK, 0);
}

static void Stage_LoadMusic(void)
{
	//Offset sing ends
	stage.player->sing_end -= stage.note_scroll;
	stage.opponent->sing_end -= stage.note_scroll;
	if (stage.gf != NULL)
		stage.gf->sing_end -= stage.note_scroll;
	
	//Find music file and begin seeking to it
	Audio_SeekXA_Track(stage.stage_def->music_track);
	
	//Initialize music state
	stage.note_scroll = FIXED_DEC(-5 * 6 * 12,1);
	stage.song_time = FIXED_DIV(stage.note_scroll, stage.step_crochet);
	stage.interp_time = 0;
	stage.interp_ms = 0;
	stage.interp_speed = 0;
	
	//Offset sing ends again
	stage.player->sing_end += stage.note_scroll;
	stage.opponent->sing_end += stage.note_scroll;
	if (stage.gf != NULL)
		stage.gf->sing_end += stage.note_scroll;
}

static void Stage_LoadState(void)
{
	//Initialize stage state
	stage.flag = STAGE_FLAG_VOCAL_ACTIVE;
	
	stage.gf_speed = 1 << 2;
	
	stage.state = StageState_Play;
	
	timer.timer = Audio_GetLength(stage.stage_def->music_track);
	stage.paused = false;
	if (!stage.debug)
		stage.freecam = 0;
	
	stage.player_state[0].character = stage.player;
	stage.player_state[1].character = stage.opponent;

	
	for (int i = 0; i < 2; i++)
	{
		memset(stage.player_state[i].arrow_hitan, 0, sizeof(stage.player_state[i].arrow_hitan));
		
		stage.player_state[i].health = 10000;
		stage.player_state[i].combo = 0;
		stage.player_state[i].refresh_score = false;
		stage.player_state[i].score = 0;
		stage.player_state[i].miss = 0;
		stage.player_state[i].accuracy = 0;
		stage.player_state[i].max_accuracy = 0;
		stage.player_state[i].min_accuracy = 0;
		if (stage.prefs.mode >= StageMode_2P)
			strcpy(stage.player_state[i].score_text, "SC: 0 | MS: 0 | AC: 0%");
		else
			strcpy(stage.player_state[i].score_text, "Score: 0 | Misses: 0 | Accuracy: 0%");
		
		stage.player_state[i].pad_held = stage.player_state[i].pad_press = 0;
	}
	
	ObjectList_Free(&stage.objlist_splash);
	ObjectList_Free(&stage.objlist_fg);
	ObjectList_Free(&stage.objlist_bg);
}

//Retry Tick
static void Retry_Tick()
{
	//Draw 'RETRY'
	u8 retry_frame = 0;
	
	if (stage.player->animatable.anim >= PlayerAnim_Dead3)
	{
		if (stage.player->animatable.anim == PlayerAnim_Dead6)
		{
			//Selected retry
			retry_frame = 3 + (stage.retry_bump >> 2);
			if (retry_frame >= 5)
				retry_frame = 5;
			
			if (stage.retry_bump <= 55)
				stage.retry_bump++;
		}
		else
		{
			//Idle
			retry_frame = 1 + (stage.retry_bump >> 2);
			if (retry_frame >= 3)
				retry_frame = 0;
			
			if (++stage.retry_bump >= 55)
				stage.retry_bump = 0;
		}
	}
	
	RECT retry_src = {
		(retry_frame & 1) ? 48 : 0,
		(retry_frame >> 1) << 5,
		48,
		32
	};
	RECT_FIXED retry_dst = {
		FIXED_DEC(12,1),
		FIXED_DEC(84,1),
		FIXED_DEC(48,1),
		FIXED_DEC(32,1),
	};
	
	retry_dst.x = stage.player->x - FIXED_MUL(retry_dst.x,stage.player->size) - stage.camera.x;
	retry_dst.y = stage.player->y - FIXED_MUL(retry_dst.y,stage.player->size) - stage.camera.y;
	retry_dst.w = FIXED_MUL(retry_dst.w,stage.player->size);
	retry_dst.h = FIXED_MUL(retry_dst.h,stage.player->size);
	
	stage.retry_dst = retry_dst;
	stage.retry_src = retry_src;
	
	if (stage.retry_visibility > 0)
		stage.retry_visibility--;
	
	(void)retry_frame;
}

static void Load_SFX(char *path, u8 offset)
{
	CdlFILE file;
	
	IO_FindFile(&file, path);
   	u32 *data = IO_ReadFile(&file);
    Sounds[offset] = Audio_LoadVAGData(data, file.size);
    Mem_Free(data);
}

//Stage functions
void Stage_Load(StageId id, StageDiff difficulty, boolean story)
{
	//Get stage definition
	stage.stage_def = &stage_defs[stage.stage_id = id];
	stage.stage_diff = difficulty;
	stage.story = story;

	//Load HUD textures
	Gfx_LoadTex(&stage.tex_hud0, IO_Read("\\STAGE\\HUD0.TIM;1"), GFX_LOADTEX_FREE);
	Gfx_LoadTex(&stage.tex_intro, IO_Read("\\STAGE\\INTRO.TIM;1"), GFX_LOADTEX_FREE);
	
	//Load stage background
	Stage_LoadStage();
	
	//Load VAG files
	Audio_ClearAlloc();
	Load_SFX("\\SOUNDS\\MICDROP.VAG;1",0);
	Load_SFX("\\SOUNDS\\CONTINUE.VAG;1",1);
	Load_SFX("\\SOUNDS\\INTRO0.VAG;1",2);
	Load_SFX("\\SOUNDS\\INTRO1.VAG;1",3);
	Load_SFX("\\SOUNDS\\INTRO2.VAG;1",4);
	Load_SFX("\\SOUNDS\\INTRO3.VAG;1",5);
	
	//Load Events
	Load_Events();
	
	//Load characters
	Stage_LoadPlayer();
	Stage_LoadOpponent();
	if(!stage.prefs.lowgraphics)
		Stage_LoadGirlfriend();
	stage.hidegf = false;
	Stage_SwapChars();
	
	//Load stage chart
	Stage_LoadChart();
	
	//Initialize stage state
	stage.story = story;
	
	Stage_LoadState();
	
	//Initialize camera
	if (stage.cur_section->flag & SECTION_FLAG_OPPFOCUS)
		Stage_FocusCharacter(stage.opponent);
	else
		Stage_FocusCharacter(stage.player);
	
	//Reset Rotations
	stage.camera.ta = 0;
	stage.camera.hudta = 0;
	
	//Initialize Camera
	stage.camera.speed = FIXED_DEC(5,100);
	stage.camera.force = false;
	
	stage.camera.x = stage.camera.tx;
	stage.camera.y = stage.camera.ty;
	stage.camera.zoom = stage.camera.tz;
	stage.camera.angle = stage.camera.ta;
	stage.camera.hudangle = stage.camera.hudta;
	
	//Initialize Bumps
	stage.bump = FIXED_UNIT;
	stage.sbump = FIXED_UNIT;
	
	//Initialize stage according to mode
	stage.note_swap = (stage.prefs.mode == StageMode_Swap) ? 4 : 0;
	
	//Load music
	stage.note_scroll = 0;
	Stage_LoadMusic();
	
	//Test offset
	stage.offset = 0;
	Gfx_SetClear(0, 0, 0);
	
	//Manage Notes Position
	u8 scroll_index = (stage.prefs.middlescroll) ? 2 : 1;
	if (stage.prefs.mode == StageMode_Swap && !stage.prefs.middlescroll) {
		scroll_index = 3;
	}
	for (int i = 0; i < 8; i++) {
		stage.note_x[i] = FIXED_DEC(note_def[scroll_index][i], 1);
		stage.note_y[i] = FIXED_DEC(note_def[0][i], 1);
	}
	
	//Initialize player state
	stage.retry_visibility = 10;
	stage.retry_bump = 0;
	stage.retry_fade = 0;
}

void Stage_Unload(void)
{
	//Unload stage background
	if (stage.back != NULL)
		stage.back->free(stage.back);
	stage.back = NULL;
	
	//Unload stage data
	Mem_Free(stage.chart_data);
	stage.chart_data = NULL;
	
	//Free objects
	ObjectList_Free(&stage.objlist_splash);
	ObjectList_Free(&stage.objlist_fg);
	ObjectList_Free(&stage.objlist_bg);
	
	//Free characters
	Character_Free(stage.player);
	stage.player = NULL;
	Character_Free(stage.opponent);
	stage.opponent = NULL;
	Character_Free(stage.gf);
	stage.gf = NULL;
}

static boolean Stage_NextLoad(void)
{
	CheckNewScore();
	if (stage.prefs.autosave)
		writeSaveFile();
	
	u8 load = stage.stage_def->next_load;
	if (load == 0)
	{
		//Do stage transition if full reload
		stage.trans = StageTrans_NextSong;
		Trans_Start();
		return false;
	}
	else
	{
		//Get stage definition
		stage.stage_def = &stage_defs[stage.stage_id = stage.stage_def->next_stage];
		
		//Load stage background
		if (load & STAGE_LOAD_STAGE)
			Stage_LoadStage();
		
		//Load characters
		Stage_SwapChars();
		if (load & STAGE_LOAD_PLAYER)
		{
			Stage_LoadPlayer();
		}
		else
		{
			stage.player->x = stage.stage_def->pchar.x;
			stage.player->y = stage.stage_def->pchar.y;
		}
		if (load & STAGE_LOAD_OPPONENT)
		{
			Stage_LoadOpponent();
		}
		Stage_SwapChars();
		if (load & STAGE_LOAD_GIRLFRIEND)
		{
			Stage_LoadGirlfriend();
		}
		else if (stage.gf != NULL)
		{
			stage.gf->x = stage.stage_def->gchar.x;
			stage.gf->y = stage.stage_def->gchar.y;
		}
		
		//Load stage chart
		Stage_LoadChart();
		
		//Initialize stage state
		Stage_LoadState();
		
		//Load music
		Stage_LoadMusic();
		
		//Reset timer
		Timer_Reset();
		return true;
	}
}

void Stage_Tick(void)
{
	//Reload song when start or cross is pressed
	if (pad_state.press & PAD_START && stage.state != StageState_Play && stage.state != StageState_DeadDecide && (stage.player->animatable.anim >= PlayerAnim_Dead2))
	{
		stage.state = StageState_DeadDecide;
		stage.player->set_anim(stage.player, PlayerAnim_Dead6);
		Audio_StopXA();
		Audio_PlaySound(Sounds[1], 0x3fff);
		stage.retry_visibility = 0;
		stage.retry_bump = 0;
	}
	
	SeamLoad:;
	
	//Tick transition
	if (Trans_Tick())
	{
		stage.pause_select = 0;
		stage.paused = false;
		
		switch (stage.trans)
		{
			case StageTrans_Menu:
				CheckNewScore();
				if (stage.prefs.autosave)
					writeSaveFile();
				
				//Load appropriate menu
				Stage_Unload();
				
				LoadScr_Start();
				if (stage.stage_id <= StageId_LastVanilla)
				{
					if (stage.story)
						Menu_Load(MenuPage_Story);
					else
						Menu_Load(MenuPage_Freeplay);
				}
				else
				{
					//Menu_Load(MenuPage_Mods); //this changed for now
					Menu_Load(MenuPage_Freeplay);
				}
				LoadScr_End();
				
				gameloop = GameLoop_Menu;
				return;
			case StageTrans_NextSong:
				//Load next song
				Stage_Unload();
				
				LoadScr_Start();
				Stage_Load(stage.stage_def->next_stage, stage.stage_diff, stage.story);
				LoadScr_End();
				break;
			case StageTrans_Reload:
				//Reload song
				Stage_Unload();
				
				LoadScr_Start();
				Stage_Load(stage.stage_id, stage.stage_diff, stage.story);
				LoadScr_End();
				break;
		}
	}
	switch (stage.state)
	{
		case StageState_Play:
		{
			if (stage.song_step >= 0)
			{
				if (!stage.paused && pad_state.press & PAD_START)
				{
					stage.pause_scroll = -1;
					Audio_PauseXA();
					stage.paused = true;
					pad_state.press = 0;
				}
			}

			if (stage.paused)
			{
				switch (stage.pause_state)
				{
					case 0:
						PausedState();
						break;
				}
			}
			
			if (stage.prefs.timebar)
				StageTimer_Draw();
			if (stage.debug)
				Debug_StageDebug();
			
            Stage_CountDown();
			
			Events();
			
			if (stage.prefs.botplay)
			{
				//Draw botplay
				RECT bot_src = {61, 178, 67, 16};
				RECT_FIXED bot_dst = {FIXED_DEC(-bot_src.w / 2,1), FIXED_DEC(-58,1), FIXED_DEC(bot_src.w,1), FIXED_DEC(bot_src.h,1)};
				
				if (!stage.debug)
					Stage_DrawTex(&stage.tex_hud0, &bot_src, &bot_dst, stage.bump, stage.camera.hudangle);
			}
			
			//Clear per-frame flags
			stage.flag &= ~(STAGE_FLAG_JUST_STEP | STAGE_FLAG_SCORE_REFRESH);
			
			//Get song position
			boolean playing;
			fixed_t next_scroll;
			
			const fixed_t interp_int = FIXED_UNIT * 8 / 75;
			
			if (!stage.paused)
			{
				if (stage.note_scroll < 0)
				{
					//Play countdown sequence
					stage.song_time += timer_dt;
						
					//Update song
					if (stage.song_time >= 0)
					{
						//Song has started
						playing = true;
						
						Audio_PlayXA_Track(stage.stage_def->music_track, 0x40, stage.stage_def->music_channel, 0);
							
						//Update song time
						fixed_t audio_time = (fixed_t)Audio_TellXA_Milli() - stage.offset;
						if (audio_time < 0)
							audio_time = 0;
						stage.interp_ms = (audio_time << FIXED_SHIFT) / 1000;
						stage.interp_time = 0;
						stage.song_time = stage.interp_ms;
					}
					else
					{
						//Still scrolling
						playing = false;
					}
					
					//Update scroll
					next_scroll = FIXED_MUL(stage.song_time, stage.step_crochet);
				}
				else if (Audio_PlayingXA())
				{
					fixed_t audio_time_pof = (fixed_t)Audio_TellXA_Milli();
					fixed_t audio_time = (audio_time_pof > 0) ? (audio_time_pof - stage.offset) : 0;
					
					if (stage.prefs.expsync)
					{
						//Get playing song position
						if (audio_time_pof > 0)
						{
							stage.song_time += timer_dt;
							stage.interp_time += timer_dt;
						}
						
						if (stage.interp_time >= interp_int)
						{
							//Update interp state
							while (stage.interp_time >= interp_int)
								stage.interp_time -= interp_int;
							stage.interp_ms = (audio_time << FIXED_SHIFT) / 1000;
						}
						
						//Resync
						fixed_t next_time = stage.interp_ms + stage.interp_time;
						if (stage.song_time >= next_time + FIXED_DEC(25,1000) || stage.song_time <= next_time - FIXED_DEC(25,1000))
						{
							stage.song_time = next_time;
						}
						else
						{
							if (stage.song_time < next_time - FIXED_DEC(1,1000))
								stage.song_time += FIXED_DEC(1,1000);
							if (stage.song_time > next_time + FIXED_DEC(1,1000))
								stage.song_time -= FIXED_DEC(1,1000);
						}
					}
					else
					{
						//Old sync
						stage.interp_ms = (audio_time << FIXED_SHIFT) / 1000;
						stage.interp_time = 0;
						stage.song_time = stage.interp_ms;
					}
					
					playing = true;
					
					//Update scroll
					next_scroll = ((fixed_t)stage.step_base << FIXED_SHIFT) + FIXED_MUL(stage.song_time - stage.time_base, stage.step_crochet);
				}
				else
				{
					//Song has ended
					playing = false;
					stage.song_time += timer_dt;
						
					//Update scroll
					next_scroll = ((fixed_t)stage.step_base << FIXED_SHIFT) + FIXED_MUL(stage.song_time - stage.time_base, stage.step_crochet);
					
					//Transition to menu or next song
					if (stage.story && stage.stage_def->next_stage != stage.stage_id)
					{
						if (Stage_NextLoad())
							goto SeamLoad;
					}
					else
					{
						stage.trans = StageTrans_Menu;
						Trans_Start();
					}
				}	
                
				RecalcScroll:;
				//Update song scroll and step
				if (next_scroll > stage.note_scroll)
				{
					if (((stage.note_scroll / 12) & FIXED_UAND) != ((next_scroll / 12) & FIXED_UAND))
						stage.flag |= STAGE_FLAG_JUST_STEP;
					stage.note_scroll = next_scroll;
					stage.song_step = (stage.note_scroll >> FIXED_SHIFT);
					if (stage.note_scroll < 0)
						stage.song_step -= 11;
					stage.song_step /= 12;
				}
				
				//Update section
				if (stage.note_scroll >= 0)
				{
					//Check if current section has ended
					u16 end = stage.cur_section->end;
					if ((stage.note_scroll >> FIXED_SHIFT) >= end)
					{
						//Increment section pointer
						stage.cur_section++;
						
						//Update BPM
						u16 next_bpm = stage.cur_section->flag & SECTION_FLAG_BPM_MASK;
						Stage_ChangeBPM(next_bpm, end);
						stage.section_base = stage.cur_section;
						
						//Recalculate scroll based off new BPM
						next_scroll = ((fixed_t)stage.step_base << FIXED_SHIFT) + FIXED_MUL(stage.song_time - stage.time_base, stage.step_crochet);
						goto RecalcScroll;
					}
				}
			}
			
			//Handle bump
			if(!stage.prefs.lowgraphics && stage.prefs.camerazooms)
			{
				if ((stage.bump = FIXED_UNIT + FIXED_MUL(stage.bump - FIXED_UNIT, FIXED_DEC(95,100))) <= FIXED_DEC(1003,1000))
					stage.bump = FIXED_UNIT;
				stage.sbump = FIXED_UNIT + FIXED_MUL(stage.sbump - FIXED_UNIT, FIXED_DEC(90,100));
				
				if (playing && (stage.flag & STAGE_FLAG_JUST_STEP))
				{
					//Check if screen should bump
					boolean is_bump_step = (stage.song_step & 0xF) == 0;
					
					//Bump screen
					if (is_bump_step)
						stage.bump += FIXED_DEC(3,100);
					
					//Bump health every 4 steps
					if ((stage.song_step & 0x3) == 0)
						stage.sbump += FIXED_DEC(8,100);
				}
			}
			
			//Scroll camera
			if (!stage.camera.force)
			{
				if (stage.cur_section->flag & SECTION_FLAG_OPPFOCUS)
					Stage_FocusCharacter(stage.opponent);
				else
					Stage_FocusCharacter(stage.player);
			}
			Stage_ScrollCamera();
			
			switch (stage.prefs.mode)
			{
				case StageMode_Normal:
				case StageMode_Swap:
				{
					//Handle player 1 inputs
					Stage_ProcessPlayer(&stage.player_state[0], &pad_state, playing);
					
					//Handle opponent notes
					u8 opponent_anote = CharAnim_Idle;
					u8 opponent_snote = CharAnim_Idle;
					
					for (Note *note = stage.cur_note;; note++)
					{
						if (note->pos > (stage.note_scroll >> FIXED_SHIFT))
							break;
						
						//Opponent note hits
						if (playing && (note->type & NOTE_FLAG_OPPONENT) && !(note->type & NOTE_FLAG_HIT))
						{
							//Opponent hits note
							stage.player_state[1].arrow_hitan[note->type & 0x3] = stage.step_time;
							Stage_StartVocal();
							if (note->type & NOTE_FLAG_SUSTAIN)
								opponent_snote = note_anims[note->type & 0x3][(note->type & NOTE_FLAG_ALT_ANIM) != 0];
							else
								opponent_anote = note_anims[note->type & 0x3][(note->type & NOTE_FLAG_ALT_ANIM) != 0];
							note->type |= NOTE_FLAG_HIT;
							NoteHitEnemyEvent(note->type);
						}
					}
					
					if (opponent_anote != CharAnim_Idle)
						stage.opponent->set_anim(stage.opponent, opponent_anote);
					else if (opponent_snote != CharAnim_Idle)
						stage.opponent->set_anim(stage.opponent, opponent_snote);
					break;
				}
				case StageMode_2P:
				{
					//Handle player 1 and 2 inputs
					Stage_ProcessPlayer(&stage.player_state[0], &pad_state, playing);
					Stage_ProcessPlayer(&stage.player_state[1], &pad_state_2, playing);
					break;
				}
			}
			
			//Tick note splashes
			ObjectList_Tick(&stage.objlist_splash);
			
			PlayerState *this;
			s32 VScore;
			
			//Draw Score
			if (stage.prefs.mode >= StageMode_2P)
			{
				for (int i = 0; i < ((stage.prefs.mode >= StageMode_2P) ? 2 : 1); i++)
				{
					this = &stage.player_state[i];
					
					this->accuracy = ((this->min_accuracy * 100) / (this->max_accuracy));
					
					if (this->refresh_score)
					{
						VScore = (this->score * stage.max_score / this->max_score) * 10;
						sprintf(this->score_text, "SC: %d | MS: %d | AC: %d%%", VScore, this->miss, this->accuracy);
						this->refresh_score = false;
					}
					
					if (!event.hidehud && !stage.prefs.hidehud)
					{
						if (i == 0)
							fonts.font_cdr.draw(&fonts.font_cdr,
								this->score_text,
								FIXED_DEC(85,1), 
								FIXED_DEC(100,1),
								FontAlign_Center
							);
						else
							fonts.font_cdr.draw(&fonts.font_cdr,
								this->score_text,
								FIXED_DEC(-65,1),
								FIXED_DEC(100,1),
								FontAlign_Center
							);
					}
				}
			}
			else
			{
				this = &stage.player_state[0];
				
				this->accuracy = (this->min_accuracy * 100) / (this->max_accuracy);
				
				if (this->refresh_score)
				{
					VScore = (this->score * stage.max_score / this->max_score) * 10;
					sprintf(this->score_text, "Score: %d | Misses: %d | Accuracy: %d%%", VScore, this->miss, this->accuracy);
					this->refresh_score = false;
				}
			
				s32 texty = 100;
			
				if (stage.prefs.downscroll)
					texty = -70;
				
				if (!event.hidehud && !stage.prefs.hidehud)
					fonts.font_cdr.draw(&fonts.font_cdr,
						this->score_text,
						FIXED_DEC(20,1), 
						FIXED_DEC(texty,1),
						FontAlign_Center
					);
			}
			
			if (stage.prefs.mode < StageMode_2P)
			{
				//Perform health checks
				if (stage.player_state[0].health <= 0)
				{
					//Player has died
					stage.player_state[0].health = 0;
					stage.state = StageState_Dead;
				}
				if (stage.player_state[0].health > 20000)
					stage.player_state[0].health = 20000;
				
				if (!event.hidehud && !stage.prefs.hidehud)
				{
					if (stage.prefs.lowgraphics)
					{
						//Draw health bar
						Stage_DrawHealthBar(214 - (107 * stage.player_state[0].health / 10000), 0xFFFF0000);
						Stage_DrawHealthBar(214, 0xFF00FF00);
					}
					else
					{
						//Draw health heads
						Stage_DrawHealth(stage.player_state[0].health, stage.player->health_i, true);
						Stage_DrawHealth(stage.player_state[0].health, stage.opponent->health_i, false);
						
						//Draw health bar
						if (stage.prefs.mode == StageMode_Swap)
						{
							Stage_DrawHealthBar(107 * stage.player_state[0].health / 10000, stage.player->health_bar);
							Stage_DrawHealthBar(214, stage.opponent->health_bar);
						}
						else
						{
							Stage_DrawHealthBar(214 - (107 * stage.player_state[0].health / 10000), stage.opponent->health_bar);
							Stage_DrawHealthBar(214, stage.player->health_bar);
						}
					}
				}
			}
			
			//Draw stage notes
			Stage_DrawNotes();
			
			//Draw note HUD
			RECT note_src = {0, 0, 32, 32};
			RECT_FIXED note_dst = {0, 0, FIXED_DEC(32,1), FIXED_DEC(32,1)};
				
			for (u8 i = 0; i < 4; i++)
			{
				//BF
				note_dst.x = stage.note_x[i] - FIXED_DEC(16,1);
				note_dst.y = stage.note_y[i] - FIXED_DEC(16,1);
				if (stage.prefs.downscroll)
					note_dst.y = -note_dst.y - note_dst.h;
				
				Stage_DrawStrum(i, &note_src, &note_dst);
				
				Stage_DrawTex(&stage.tex_hud0, &note_src, &note_dst, stage.bump, stage.camera.hudangle);
				
				//Opponent
				note_dst.x = stage.note_x[(i | 0x4)] - FIXED_DEC(16,1);
				note_dst.y = stage.note_y[(i | 0x4)] - FIXED_DEC(16,1);
				
				if (stage.prefs.downscroll)
					note_dst.y = -note_dst.y - note_dst.h;
				Stage_DrawStrum(i | 4, &note_src, &note_dst);
				
				if (stage.prefs.opponentnotes)
				{
					if (stage.prefs.middlescroll)
						Stage_BlendTex(&stage.tex_hud0, &note_src, &note_dst, stage.bump, stage.camera.hudangle, 1);
					else
						Stage_DrawTex(&stage.tex_hud0, &note_src, &note_dst, stage.bump, stage.camera.hudangle);
				}
			}
			
			Events_Back();
			
			//Tick characters
			if(!stage.prefs.lowgraphics)
			{
				//Draw stage foreground
				if (stage.back->draw_fg != NULL)
					stage.back->draw_fg(stage.back);
				
				//Tick foreground objects
				ObjectList_Tick(&stage.objlist_fg);
			
				stage.player->tick(stage.player);
				stage.opponent->tick(stage.opponent);
			
				//Draw stage middle
				if (stage.back->draw_md != NULL)
					stage.back->draw_md(stage.back);
				
				//Tick girlfriend
				if (!stage.hidegf)
					if (stage.gf != NULL )
						stage.gf->tick(stage.gf);

				//Tick background objects
				ObjectList_Tick(&stage.objlist_bg);
				
				//Draw stage background
				if (stage.back->draw_bg != NULL)
					stage.back->draw_bg(stage.back);
			}
			
			if (stage.song_step >= 0)
				StageTimer_Tick();
			
			break;
		}
		case StageState_Dead: //Start BREAK animation and reading extra data from CD
		{
			//Stop music immediately
			Audio_StopXA();
			Audio_PlaySound(Sounds[0], 0x3fff);
			
			//Unload stage data
			Mem_Free(stage.chart_data);
			stage.chart_data = NULL;
			
			//Free background
			stage.back->free(stage.back);
			stage.back = NULL;
			
			//Free objects
			ObjectList_Free(&stage.objlist_fg);
			ObjectList_Free(&stage.objlist_bg);
			
			//Free opponent and girlfriend
			Stage_SwapChars();
			Character_Free(stage.opponent);
			stage.opponent = NULL;
			Character_Free(stage.gf);
			stage.gf = NULL;
			
			//Reset stage state
			stage.flag = 0;
			stage.bump = stage.sbump = FIXED_UNIT;
			
			//Change background colour to black
			Gfx_SetClear(0, 0, 0);
			
			//Run death animation, focus on player, and change state
			stage.player->set_anim(stage.player, PlayerAnim_Dead0);
			
			Stage_FocusCharacter(stage.player);
			stage.song_time = 0;
			
			stage.state = StageState_DeadLoad;
		}
	//Fallthrough
		case StageState_DeadLoad:
		{
			//Scroll camera and tick player
			if (stage.song_time < FIXED_UNIT)
				stage.song_time += FIXED_UNIT / 60;
			Stage_ScrollCamera();
			stage.player->tick(stage.player);
			
			stage.state = StageState_DeadDrop;
			break;
		}
		case StageState_DeadDrop:
		{
			//Scroll camera and tick player
			Stage_ScrollCamera();
			if (stage.player->animatable.anim >= PlayerAnim_Dead2)
				Retry_Tick();
			stage.player->tick(stage.player);
			
			//Enter next state once mic has been dropped
			if (stage.player->animatable.anim == PlayerAnim_Dead1)
				stage.player->set_anim(stage.player, PlayerAnim_Dead2);
			//Enter next state once mic has been dropped
			if (stage.player->animatable.anim == PlayerAnim_Dead3)
			{
				stage.state = StageState_DeadRetry;
				Audio_PlayXA_Track(XA_GameOver, 0x40, 1, true);
			}
			break;
		}
		case StageState_DeadRetry:
		{
			//Randomly twitch
			if (stage.player->animatable.anim == PlayerAnim_Dead3)
			{
				if (RandomRange(0, 29) == 0)
					stage.player->set_anim(stage.player, PlayerAnim_Dead4);
				if (RandomRange(0, 29) == 0)
					stage.player->set_anim(stage.player, PlayerAnim_Dead5);
			}
			
			//Scroll camera and tick player
			Stage_ScrollCamera();
			if (stage.player->animatable.anim >= PlayerAnim_Dead2)
				Retry_Tick();
			stage.player->tick(stage.player);
			break;
		}
		case StageState_DeadDecide:
		{
			//Fade
			static const RECT fade = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
			Gfx_BlendRect(&fade, stage.retry_fade, stage.retry_fade, stage.retry_fade, 2);
			
			if (stage.retry_fade < 252)
				stage.retry_fade += 4;
			if (Trans_Idle() && (stage.retry_fade > 200))
			{
				stage.trans = StageTrans_Reload;
				Trans_Start();
			}
			
			//Scroll camera and tick player
			Stage_ScrollCamera();
			if (stage.player->animatable.anim >= PlayerAnim_Dead2)
				Retry_Tick();
			stage.player->tick(stage.player);
			break;
		}
		default:
			break;
	}
}
