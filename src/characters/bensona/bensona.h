#ifndef PSXF_GUARD_BENSONA_H
#define PSXF_GUARD_BENSONA_H

#include "../../scenes/stage/character.h"

//Dad character functions
Character *Char_BensonA_New(fixed_t x, fixed_t y);

typedef struct
{
	boolean angry;
	boolean angry2;
} BensonA;

extern BensonA bensonA;

#endif