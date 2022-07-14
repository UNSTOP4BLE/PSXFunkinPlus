/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "week1.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"

//Week 1 background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back0; //Stage and back
} Back_Week1;

//Week 1 background functions
void Back_Week1_DrawBG(StageBack *back)
{
	Back_Week1 *this = (Back_Week1*)back;
	
	fixed_t fx, fy;
	
	stage.camera.zoom -= FIXED_DEC(1,100);
	
	//Draw stage
	fx = stage.camera.x * 3 / 2;
	fy = stage.camera.y * 3 / 2;
	
	POINT_FIXED stage_d2 = {
		FIXED_DEC(-230,1) - fx,
		FIXED_DEC(50,1) + FIXED_DEC(100,1) - fy,
	};
	POINT_FIXED stage_d3 = {
		FIXED_DEC(-230,1) + FIXED_DEC(410,1) - fx,
		FIXED_DEC(50,1) + FIXED_DEC(100,1) - fy,
	};
	
	fx = stage.camera.x >> 1;
	fy = stage.camera.y >> 1;
	
	POINT_FIXED stage_d0 = {
		FIXED_DEC(-230,1) - fx,
		FIXED_DEC(50,1) - fy,
	};
	POINT_FIXED stage_d1 = {
		FIXED_DEC(-230,1) + FIXED_DEC(410,1) - fx,
		FIXED_DEC(50,1) - fy,
	};
	
	RECT stage_src = {0, 0, 255, 59};
	
	Stage_DrawTexArb(&this->tex_back0, &stage_src, &stage_d0, &stage_d1, &stage_d2, &stage_d3, stage.camera.bzoom);
	
	//Draw back
	fx = stage.camera.x * 3 / 6;
	fy = stage.camera.y * 3 / 6;
	
	RECT back_src = {1, 60, 71, 195};
	
	RECT_FIXED back1_dst = {
		FIXED_DEC(-140,1),
		FIXED_DEC(20,1),
		FIXED_DEC(back_src.w,1),
		FIXED_DEC(back_src.h,1)
	};
	
	RECT_FIXED back2_dst = {
		FIXED_DEC(70,1),
		FIXED_DEC(0,1),
		FIXED_DEC(back_src.w,1),
		FIXED_DEC(back_src.h,1)
	};
	
	Stage_DrawTexRotate(&this->tex_back0, &back_src, &back1_dst, -5, stage.camera.bzoom, fx, fy);
	Stage_DrawTexRotate(&this->tex_back0, &back_src, &back2_dst,  5, stage.camera.bzoom, fx, fy);
	
	fx = stage.camera.x * 3 / 10;
	fy = stage.camera.y * 3 / 10;
	
	RECT_FIXED back3_dst = {
		FIXED_DEC(-60,1),
		FIXED_DEC(50,1),
		FIXED_DEC(back_src.w,1),
		FIXED_DEC(back_src.h,1)
	};
	
	RECT_FIXED back4_dst = {
		FIXED_DEC(160,1),
		FIXED_DEC(60,1),
		FIXED_DEC(back_src.w,1),
		FIXED_DEC(back_src.h,1)
	};
	
	RECT_FIXED back5_dst = {
		FIXED_DEC(-160,1),
		FIXED_DEC(90,1),
		FIXED_DEC(back_src.w,1),
		FIXED_DEC(back_src.h,1)
	};
	
	Stage_DrawTexRotate(&this->tex_back0, &back_src, &back3_dst, -2, stage.camera.bzoom, fx, fy);
	Stage_DrawTexRotate(&this->tex_back0, &back_src, &back4_dst,  2, stage.camera.bzoom, fx, fy);
	Stage_DrawTexRotate(&this->tex_back0, &back_src, &back5_dst,  -2, stage.camera.bzoom, fx, fy);
}

void Back_Week1_Free(StageBack *back)
{
	Back_Week1 *this = (Back_Week1*)back;
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_Week1_New(void)
{
	//Allocate background structure
	Back_Week1 *this = (Back_Week1*)Mem_Alloc(sizeof(Back_Week1));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = NULL;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Week1_DrawBG;
	this->back.free = Back_Week1_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\WEEK1\\BACK.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Mem_Free(arc_back);
	
	return (StageBack*)this;
}
