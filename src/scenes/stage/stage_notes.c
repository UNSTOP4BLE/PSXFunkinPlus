#include "stage_notes.h"
#include "stage.h"
#include "object/combo.h"
#include "object/splash.h"
#include "../../psx/audio.h"
#include "../../psx/timer.h"
#include "../../psx/pad.h"

// Stage constants (needed for note functions)
static const u8 note_anims[4][3] = {
    {CharAnim_Left,  CharAnim_LeftAlt,  PlayerAnim_LeftMiss},
    {CharAnim_Down,  CharAnim_DownAlt,  PlayerAnim_DownMiss},
    {CharAnim_Up,    CharAnim_UpAlt,    PlayerAnim_UpMiss},
    {CharAnim_Right, CharAnim_RightAlt, PlayerAnim_RightMiss},
};

// Note hit detection
u8 Stage_HitNote(PlayerState *this, u8 type, fixed_t offset)
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

void Stage_MissNote(PlayerState *this, u8 type)
{
    this->max_accuracy += 3;
    this->refresh_score = true;
    this->miss += 1;
    
    if (!stage.prefs.lowgraphics)
    {
        if (this->character->spec & CHAR_SPEC_MISSANIM)
            this->character->set_anim(this->character, note_anims[type & 0x3][2]);
        else
            this->character->set_anim(this->character, note_anims[type & 0x3][0]);
    }
    
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

void Stage_NoteCheck(PlayerState *this, u8 type)
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
            if (!stage.prefs.lowgraphics)
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
            if (!stage.prefs.lowgraphics)
            {
                if (this->character->spec & CHAR_SPEC_MISSANIM)
                    this->character->set_anim(this->character, note_anims[type & 0x3][2]);
                else
                    this->character->set_anim(this->character, note_anims[type & 0x3][0]);
            }
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

void Stage_SustainCheck(PlayerState *this, u8 type)
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
        
        if (!stage.prefs.lowgraphics)
            this->character->set_anim(this->character, note_anims[type & 0x3][(note->type & NOTE_FLAG_ALT_ANIM) != 0]);
        
        Stage_StartVocal();
        this->arrow_hitan[type & 0x3] = stage.step_time;
    }
}

void Stage_ProcessPlayer(PlayerState *this, Pad *pad, boolean playing, boolean player)
{
    //Handle player note presses
    if (stage.prefs.botplay == 0) {
        if (playing)
        {
            u8 i = (!player) ? NOTE_FLAG_OPPONENT : 0;
            
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
            u8 i = (!player) ? NOTE_FLAG_OPPONENT : 0;
            
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

void Stage_DrawStrum(u8 i, RECT *note_src, RECT_FIXED *note_dst)
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

void Stage_DrawNotes(void)
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
                                Stage_BlendTex(&stage.tex_hud0, &note_src, &note_dst, 1, &stage.camera.hud);
                            else
                                Stage_DrawTex(&stage.tex_hud0, &note_src, &note_dst, &stage.camera.hud);
                        else if (!(note->type & NOTE_FLAG_OPPONENT))
                            Stage_DrawTex(&stage.tex_hud0, &note_src, &note_dst, &stage.camera.hud);
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
                                Stage_BlendTex(&stage.tex_hud0, &note_src, &note_dst, 1, &stage.camera.hud);
                            else
                                Stage_DrawTex(&stage.tex_hud0, &note_src, &note_dst, &stage.camera.hud);
                        else if (!(note->type & NOTE_FLAG_OPPONENT))
                            Stage_DrawTex(&stage.tex_hud0, &note_src, &note_dst, &stage.camera.hud);
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
                    Stage_BlendTex(&stage.tex_hud0, &note_src, &note_dst, 1, &stage.camera.hud);
                else
                    Stage_DrawTex(&stage.tex_hud0, &note_src, &note_dst, &stage.camera.hud);
                
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
                        Stage_BlendTex(&stage.tex_hud0, &note_src, &note_dst, 1, &stage.camera.hud);
                    else
                        Stage_DrawTex(&stage.tex_hud0, &note_src, &note_dst, &stage.camera.hud);
                else if (!(note->type & NOTE_FLAG_OPPONENT))
                    Stage_DrawTex(&stage.tex_hud0, &note_src, &note_dst, &stage.camera.hud);
                
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
                        Stage_BlendTex(&stage.tex_hud0, &note_src, &note_dst, 1, &stage.camera.hud);
                    else
                        Stage_DrawTex(&stage.tex_hud0, &note_src, &note_dst, &stage.camera.hud);
                else if (!(note->type & NOTE_FLAG_OPPONENT))
                    Stage_DrawTex(&stage.tex_hud0, &note_src, &note_dst, &stage.camera.hud);
            }
        }
    }
}