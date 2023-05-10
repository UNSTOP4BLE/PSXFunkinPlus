#include "bg2.h"

#include "../../psx/archive.h"
#include "../../psx/mem.h"
#include "../../scenes/stage/stage.h"

#include "../../psx/mutil.h"

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
	Gfx_Tex tex_back4;
	
	u8 time;
} Back_BG2;

//Week 1 background functions
void Back_BG2_DrawBG(StageBack *back)
{
	Back_BG2 *this = (Back_BG2*)back;
	
	if (!stage.paused)
		this->time++;
	
	stage.opponent->focus_zoom = FIXED_DEC(80,100);
	stage.player->focus_zoom = FIXED_DEC(80,100);
	stage.opponent->size = FIXED_DEC(90,100);
	stage.player->size = FIXED_DEC(90,100);
	stage.gf->size = FIXED_DEC(90,100);
	
	fixed_t fx, fy;
	
	fx = stage.camera.x;
	fy = stage.camera.y - FIXED_DEC(60,1);
	
	RECT b1_src = {0, 0, 256, 128};
	RECT_FIXED b1_dst = {
		FIXED_DEC(-255,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(320,1),
		FIXED_DEC(160,1)
	};
	RECT b2_src = {0, 128, 256, 128};
	RECT_FIXED b2_dst = {
		FIXED_DEC(-1,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(320,1),
		FIXED_DEC(160,1)
	};
	
	Stage_DrawTex(&this->tex_back0, &b1_src, &b1_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back0, &b2_src, &b2_dst, stage.camera.bzoom);
	
	fy = stage.camera.y - FIXED_DEC(60,1) + FIXED_DEC(smooth(this->time) / 100,1);
	stage.gf->y = FIXED_DEC(-20,1) - FIXED_DEC(smooth(this->time) / 100,1);

	RECT rock_src = {0, 0, 176, 114};
	RECT_FIXED rock_dst = {
		FIXED_DEC(-72,1) - fx,
		FIXED_DEC(-80,1) - fy,
		FIXED_DEC(145,1),
		FIXED_DEC(93,1)
	};
	
	Stage_DrawTex(&this->tex_back4, &rock_src, &rock_dst, stage.camera.bzoom);
	
	
	fx = stage.camera.x / 2;
	fy = (stage.camera.y / 2 - FIXED_DEC(60,1)) - FIXED_DEC(smooth(this->time) / 100,1);
	

	RECT r1_src = {0, 11, 37, 30};
	RECT_FIXED r1_dst = {
		FIXED_DEC(-200,1) - fx,
		FIXED_DEC(-100,1) - fy,
		FIXED_DEC(56,1),
		FIXED_DEC(42,1)
	};
	
	RECT r2_src = {38, 6, 66, 46};
	RECT_FIXED r2_dst = {
		FIXED_DEC(20,1) - fx,
		FIXED_DEC(-20,1) - fy,
		FIXED_DEC(104,1),
		FIXED_DEC(73,1)
	};
	
	Stage_DrawTex(&this->tex_back2, &r1_src, &r1_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back2, &r2_src, &r2_dst, stage.camera.bzoom);
	
	
	fx = stage.camera.x / 3;
	fy = stage.camera.y / 3 - FIXED_DEC(60,1);
	
	RECT b3_src = {0, 0, 256, 256};
	RECT_FIXED b3_dst = {
		FIXED_DEC(40,1) - fx,
		FIXED_DEC(-280,1) - fy,
		FIXED_DEC(420,1),
		FIXED_DEC(420,1)
	};
	
	Stage_DrawTex(&this->tex_back1, &b3_src, &b3_dst, stage.camera.bzoom);
	
	RECT ba_src = {0, 0, 256, 256};
	RECT_FIXED ba_dst = {
		FIXED_DEC(-160,1),
		FIXED_DEC(-120,1),
		FIXED_DEC(320,1),
		FIXED_DEC(240,1)
	};
	
	Stage_DrawTex(&this->tex_back3, &ba_src, &ba_dst, FIXED_DEC(100,100) + (stage.camera.bzoom - FIXED_DEC(80,100)));
}

void Back_BG2_Free(StageBack *back)
{
	Back_BG2 *this = (Back_BG2*)back;
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_BG2_New(void)
{
	//Allocate background structure
	Back_BG2 *this = (Back_BG2*)Mem_Alloc(sizeof(Back_BG2));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = NULL;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_BG2_DrawBG;
	this->back.free = Back_BG2_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\STAGES\\BG2.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "1.tim"), 0);
	Gfx_LoadTex(&this->tex_back2, Archive_Find(arc_back, "2.tim"), 0);
	Gfx_LoadTex(&this->tex_back3, Archive_Find(arc_back, "3.tim"), 0);
	Gfx_LoadTex(&this->tex_back4, Archive_Find(arc_back, "4.tim"), 0);
	Mem_Free(arc_back);
	
	return (StageBack*)this;
}
