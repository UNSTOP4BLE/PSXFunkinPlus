#ifndef PSXF_GUARD_AUDIO_H
#define PSXF_GUARD_AUDIO_H

#include "psx.h"
#include "../audio_def.h"  // All audio definitions are here now

//Audio functions
u32 Audio_GetLength(XA_Track lengthtrack);
void Audio_Init(void);
void Audio_Quit(void);
void Audio_Reset(void);
void Audio_PlayXA_Track(XA_Track track, u8 volume, u8 channel, boolean loop);
void Audio_SeekXA_Track(XA_Track track);
void Audio_PauseXA(void);
void Audio_ResumeXA(void);
void Audio_StopXA(void);
void Audio_ChannelXA(u8 channel);
s32 Audio_TellXA_Sector(void);
s32 Audio_TellXA_Milli(void);
boolean Audio_PlayingXA(void);
void Audio_WaitPlayXA(void);
void Audio_ProcessXA(void);
void findFreeChannel(void);
u32 Audio_LoadVAGData(u32 *sound, u32 sound_size);
void AudioPlayVAG(int channel, u32 addr);
void Audio_PlaySoundOnChannel(u32 addr, u32 channel, int volume);
void Audio_PlaySound(u32 addr, int volume);
u32 VAG_IsPlaying(u32 channel);
void Audio_ClearAlloc(void);

#endif