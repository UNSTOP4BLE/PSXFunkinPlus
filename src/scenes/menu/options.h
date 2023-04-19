#ifndef PSXF_GUARD_OPTIONS_H
#define PSXF_GUARD_OPTIONS_H

#include "options.h"
#include "../../psx/fixed.h"

typedef struct
{
	u16 key;
	u16 src[4];
} ButtonStr;

//Menu functions
void Options_Tick();
void Load_Card();
void Save_Card();

#endif