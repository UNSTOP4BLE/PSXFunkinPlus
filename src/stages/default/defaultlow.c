#include "defaultlow.h"

#include "../../psx/archive.h"
#include "../../psx/mem.h"
#include "../../scenes/stage/stage.h"

//Week 1 background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back0; //Stage and back
	Gfx_Tex tex_back1; //Curtains
} Back_DefaultLow;

//Week 1 background functions
void Back_DefaultLow_DrawBG(StageBack *back)
{
	Back_DefaultLow *this = (Back_DefaultLow*)back;
}

void Back_DefaultLow_Free(StageBack *back)
{
	Back_DefaultLow *this = (Back_DefaultLow*)back;
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_DefaultLow_New(void)
{
	//Allocate background structure
	Back_DefaultLow *this = (Back_DefaultLow*)Mem_Alloc(sizeof(Back_DefaultLow));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = NULL;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_DefaultLow_DrawBG;
	this->back.free = Back_DefaultLow_Free;
	
	return (StageBack*)this;
}
