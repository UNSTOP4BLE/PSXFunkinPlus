#ifndef PSXF_GUARD_BG3_H
#define PSXF_GUARD_BG3_H

#include "../../scenes/stage/stage.h"

//Week 1 functions
StageBack *Back_BG3_New();

//Character structures
typedef struct
{
	char texname[13];
	u16 src[4];
	s16 off[2];
} BG3OBJ;

#endif
