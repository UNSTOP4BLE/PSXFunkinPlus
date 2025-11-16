#ifndef PSXF_GUARD_AUDIO_DEF_H
#define PSXF_GUARD_AUDIO_DEF_H

#include "psx/psx.h"
#include "psx/io.h"  // For IO_SECT_SIZE

// Number of unique XA files
#define XA_Max 3

//XA track enumerations
typedef enum
{
    // Menu tracks
    XA_GettinFreaky, // 0
    XA_GameOver,     // 1
    
    // Week 1 tracks  
    XA_Bopeebo,      // 2
    XA_Fresh,        // 3
    
    // Week 2 tracks
    XA_Dadbattle,    // 4
    XA_Tutorial,     // 5
    
    XA_TrackMax,     // 6
} XA_Track;

#define XA_LENGTH(x) (((u64)(x) * 75) / 100 * IO_SECT_SIZE) //Centiseconds to sectors in bytes (w)

typedef struct
{
    const char* file_path;  // Path to the XA file
    const char* name;       // Track name for identification
    u32 length;             // Track length in sectors
    boolean vocal;          // Whether the track has vocals
} XA_TrackDef;

// Single array containing all track information
static const XA_TrackDef xa_tracks[] = {
    // Menu tracks (both in MENU.XA)
    [XA_GettinFreaky] = {"\\MUSIC\\MENU.XA;1", "freakymenu", XA_LENGTH(11300), false},
    [XA_GameOver]     = {"\\MUSIC\\MENU.XA;1", "gameover",   XA_LENGTH(3800),  false},
    
    // Week 1 tracks (both in 1.XA)
    [XA_Bopeebo] = {"\\MUSIC\\1.XA;1", "bopeebo", XA_LENGTH(7700), true},
    [XA_Fresh]   = {"\\MUSIC\\1.XA;1", "fresh",   XA_LENGTH(8000), true},
    
    // Week 2 tracks (both in 2.XA)  
    [XA_Dadbattle] = {"\\MUSIC\\2.XA;1", "dadbattle", XA_LENGTH(8700), true},
    [XA_Tutorial]  = {"\\MUSIC\\2.XA;1", "tutorial",  XA_LENGTH(6800), false},
};

#endif