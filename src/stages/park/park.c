#include "park.h"

#include "../../psx/archive.h"
#include "../../psx/mem.h"
#include "../../scenes/stage/stage.h"

//Week 1 background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back0, tex_back1, tex_back2, tex_back3;
} Back_Park;

//Week 1 background functions
void Back_Park_DrawBG(StageBack *back)
{
	Back_Park *this = (Back_Park*)back;
	
	fixed_t fx, fy;
	
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT backl_src = {0, 0, 242, 256};
	RECT_FIXED backl_dst = {
		FIXED_DEC(-242,1) - fx,
		FIXED_DEC(-112,1) - fy,
		FIXED_DEC(242,1),
		FIXED_DEC(256,1)
	};
	RECT backr_src = {0, 0, 242, 256};
	RECT_FIXED backr_dst = {
		FIXED_DEC(-1,1) - fx,
		FIXED_DEC(-112,1) - fy,
		FIXED_DEC(242,1),
		FIXED_DEC(256,1)
	};
	
	Stage_DrawTex(&this->tex_back0, &backl_src, &backl_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back1, &backr_src, &backr_dst, stage.camera.bzoom);
	
	fx = stage.camera.x / 2;
	fy = stage.camera.y / 2;
	
	RECT backl2_src = {0, 0, 242, 256};
	RECT_FIXED backl2_dst = {
		FIXED_DEC(-242,1) - fx,
		FIXED_DEC(-120,1) - fy,
		FIXED_DEC(242,1),
		FIXED_DEC(256,1)
	};
	RECT backr2_src = {0, 0, 242, 256};
	RECT_FIXED backr2_dst = {
		FIXED_DEC(-1,1) - fx,
		FIXED_DEC(-120,1) - fy,
		FIXED_DEC(242,1),
		FIXED_DEC(256,1)
	};
	
	Stage_DrawTex(&this->tex_back2, &backl2_src, &backl2_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back3, &backr2_src, &backr2_dst, stage.camera.bzoom);
}

void Back_Park_Free(StageBack *back)
{
	Back_Park *this = (Back_Park*)back;
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_Park_New(void)
{
	//Allocate background structure
	Back_Park *this = (Back_Park*)Mem_Alloc(sizeof(Back_Park));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = NULL;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Park_DrawBG;
	this->back.free = Back_Park_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\STAGES\\PARK.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "1.tim"), 0);
	Gfx_LoadTex(&this->tex_back2, Archive_Find(arc_back, "2.tim"), 0);
	Gfx_LoadTex(&this->tex_back3, Archive_Find(arc_back, "3.tim"), 0);
	Mem_Free(arc_back);
	
	return (StageBack*)this;
}
