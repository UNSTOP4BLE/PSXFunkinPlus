#ifndef PSXF_GUARD_BENSON_H
#define PSXF_GUARD_BENSON_H

#include "../../scenes/stage/character.h"

//Dad character functions
Character *Char_Benson_New(fixed_t x, fixed_t y);

typedef struct
{
	boolean angry;
	boolean angry2;
} Benson;

extern Benson benson;

#endif