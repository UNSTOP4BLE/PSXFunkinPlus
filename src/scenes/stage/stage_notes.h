#ifndef STAGE_NOTES_H
#define STAGE_NOTES_H

#include "../../psx/fixed.h"
#include "../../psx/pad.h"

// Forward declarations
typedef struct PlayerState PlayerState;
typedef struct Note Note;
typedef struct Section Section;
typedef struct SectionScroll SectionScroll;

// Note hit detection functions
u8 Stage_HitNote(PlayerState *this, u8 type, fixed_t offset);
void Stage_MissNote(PlayerState *this, u8 type);
void Stage_NoteCheck(PlayerState *this, u8 type);
void Stage_SustainCheck(PlayerState *this, u8 type);

// Note drawing functions
void Stage_DrawStrum(u8 i, RECT *note_src, RECT_FIXED *note_dst);
void Stage_DrawNotes(void);

// Note processing functions
void Stage_ProcessPlayer(PlayerState *this, Pad *pad, boolean playing, boolean player);

#endif