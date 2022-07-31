#ifndef PSXF_GUARD_AUDIO_H
#define PSXF_GUARD_AUDIO_H

#include "psx.h"

//XA enumerations
typedef enum
{
	FL_Menu, //Menu
	FL_Bopeebo, //Bopeebo
	FL_Fresh,   //Fresh
	FL_Dadbattle, //DadBattle
	FL_Tutorial,  //Tutorial
	FL_Blammed, //Blammed
	
	XA_Max,
} XA_File;

typedef enum
{
	XA_GettinFreaky, //Gettin' Freaky
	XA_GameOver,     //Game Over
	XA_Bopeebo, //Bopeebo
	XA_Fresh,   //Fresh
	XA_Dadbattle, //DadBattle
	XA_Tutorial,  //Tutorial
	XA_Blammed, //Blammed
	
	XA_TrackMax,
} XA_Track;

//Audio functions
u32 Audio_GetLength(XA_Track lengthtrack);
void Audio_Init(void);
void Audio_Quit(void);
void Audio_PlayXA_Track(XA_Track track, u8 volume, u8 channel, boolean loop);
void Audio_SeekXA_Track(XA_Track track);
void Audio_PauseXA(void);
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
void Audio_ClearAlloc(void);

#endif
