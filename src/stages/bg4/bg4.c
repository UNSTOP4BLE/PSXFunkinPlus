#include "bg4.h"

#include "../../psx/archive.h"
#include "../../psx/mem.h"
#include "../../scenes/stage/stage.h"

//Week 1 background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back0;
	Gfx_Tex tex_back1;
	Gfx_Tex tex_back2;
	Gfx_Tex tex_back3;
} Back_BG4;

//Week 1 background functions
void Back_BG4_DrawBG(StageBack *back)
{
	Back_BG4 *this = (Back_BG4*)back;
	
	stage.opponent->size = FIXED_DEC(100,100);
	stage.player->size = FIXED_DEC(80,100);
	
	fixed_t fx, fy;
	
	fx = stage.camera.x;
	fy = stage.camera.y + FIXED_DEC(160,1);
	
	RECT bg1_src = {0, 0, 256, 256};
	RECT_FIXED bg1_dst = {
		FIXED_DEC(-256,1) - fx,
		FIXED_DEC(2,1) - fy,
		FIXED_DEC(256,1),
		FIXED_DEC(256,1)
	};
	RECT bg2_src = {0, 0, 256, 256};
	RECT_FIXED bg2_dst = {
		FIXED_DEC(-2,1) - fx,
		FIXED_DEC(2,1) - fy,
		FIXED_DEC(256,1),
		FIXED_DEC(256,1)
	};
	RECT bg3_src = {0, 0, 256, 54};
	RECT_FIXED bg3_dst = {
		FIXED_DEC(-256,1) - fx,
		FIXED_DEC(256,1) - fy,
		FIXED_DEC(256,1),
		FIXED_DEC(54,1)
	};
	RECT bg4_src = {0, 54, 256, 54};
	RECT_FIXED bg4_dst = {
		FIXED_DEC(-2,1) - fx,
		FIXED_DEC(256,1) - fy,
		FIXED_DEC(256,1),
		FIXED_DEC(54,1)
	};
	
	Stage_DrawTex(&this->tex_back0, &bg1_src, &bg1_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back1, &bg2_src, &bg2_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back2, &bg3_src, &bg3_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back2, &bg4_src, &bg4_dst, stage.camera.bzoom);
}

void Back_BG4_Free(StageBack *back)
{
	Back_BG4 *this = (Back_BG4*)back;
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_BG4_New(void)
{
	//Allocate background structure
	Back_BG4 *this = (Back_BG4*)Mem_Alloc(sizeof(Back_BG4));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = NULL;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_BG4_DrawBG;
	this->back.free = Back_BG4_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\STAGES\\BG4.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "1.tim"), 0);
	Gfx_LoadTex(&this->tex_back2, Archive_Find(arc_back, "2.tim"), 0);
	Mem_Free(arc_back);
	
	return (StageBack*)this;
}
