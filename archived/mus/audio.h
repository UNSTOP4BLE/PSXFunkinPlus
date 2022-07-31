#ifndef PSXF_GUARD_AUDIO_H
#define PSXF_GUARD_AUDIO_H

#include "psx.h"

#include "fixed.h"

//Audio interface
void Audio_Init(void);
void Audio_Quit(void);
void Audio_LoadMusFile(CdlFILE *file);
void Audio_LoadMus(const char *path);
void Audio_PlayMus(boolean loops);
void Audio_StopMus(void);
void Audio_SetVolume(u8 i, u16 vol_left, u16 vol_right);
fixed_t Audio_GetTime(void);
boolean Audio_IsPlaying(void);
u32 Audio_LoadVAGData(u32 *sound, u32 sound_size);
void AudioPlayVAG(int channel, u32 addr);
void Audio_PlaySoundOnChannel(u32 addr, u32 channel, int volume);
void Audio_PlaySound(u32 addr, int volume);
void Audio_ClearAlloc(void);

#endif
