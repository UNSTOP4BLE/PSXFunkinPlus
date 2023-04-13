#include "bg3.h"

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
	Gfx_Tex tex_agoti;
	Gfx_Tex tex_aldryx;
	
	IO_Data arc_bg3obj;
	
	u8 agoti, agoti_speed;
	u8 aldryx, aldryx_speed;
} Back_BG3;

static const BG3OBJ agoti[] = {
	{"agoti0.tim",{0,0,86,97},{45,87}}, //0 Agoti
	{"agoti0.tim",{87,0,86,98},{45,88}}, //1 Agoti
	{"agoti0.tim",{0,99,86,98},{45,88}}, //2 Agoti
	{"agoti0.tim",{87,99,86,98},{45,88}}, //3 Agoti
	{"agoti1.tim",{0,0,86,98},{45,88}}, //4 Agoti
	{"agoti1.tim",{87,0,86,98},{45,88}}, //5 Agoti
	{"agoti1.tim",{0,99,87,98},{45,88}}, //6 Agoti
	{"agoti1.tim",{88,99,86,98},{45,89}}, //7 Agoti
};
	
static const BG3OBJ aldryx[] = {
	{"aldryx0.tim",{0,0,74,180},{53,177}}, //0 Aldryx
	{"aldryx0.tim",{75,0,71,180},{49,176}}, //1 Aldryx
	{"aldryx0.tim",{147,0,72,180},{51,177}}, //2 Aldryx
	{"aldryx1.tim",{0,0,72,180},{51,177}}, //3 Aldryx
	{"aldryx1.tim",{73,0,72,180},{52,177}}, //4 Aldryx
	{"aldryx1.tim",{146,0,72,180},{51,177}}, //5 Aldryx
};

//Week 1 background functions
void Back_BG3_DrawBG(StageBack *back)
{
	Back_BG3 *this = (Back_BG3*)back;
	
	stage.opponent->size = FIXED_DEC(73,100);
	stage.player->size = FIXED_DEC(70,100);
	stage.gf->size = FIXED_DEC(70,100);
	
	fixed_t fx, fy;
	
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	u8 frame;
	
	if(this->agoti_speed == 3)
	{
		if (!(this->agoti >= 7))
			this->agoti++;
		this->agoti_speed = 0;
	}
	else
		this->agoti_speed++;
	
	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{
		if ((stage.song_step & 0x3) == 0)
			this->agoti = 0;
	}
	
	frame = this->agoti;
	
	RECT agoti_src = {agoti[frame].src[0], agoti[frame].src[1], agoti[frame].src[2], agoti[frame].src[3]};
	RECT_FIXED agoti_dst = {
		FIXED_DEC(132 - agoti[frame].off[0],1) - fx,
		FIXED_DEC(8 - agoti[frame].off[1],1) - fy,
		FIXED_DEC(agoti[frame].src[2],1),
		FIXED_DEC(agoti[frame].src[3],1)
	};
	
	Gfx_LoadTex(&this->tex_agoti, Archive_Find(this->arc_bg3obj, agoti[frame].texname), 0);
	Stage_DrawTex(&this->tex_agoti, &agoti_src, &agoti_dst, stage.camera.bzoom);
	
	if(this->aldryx_speed == 3)
	{
		if (!(this->aldryx >= 5))
			this->aldryx++;
		this->aldryx_speed = 0;
	}
	else
		this->aldryx_speed++;
	
	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{
		if ((stage.song_step & 0x7) == 0)
			this->aldryx = 0;
	}
	
	frame = this->aldryx;
	
	RECT aldryx_src = {aldryx[frame].src[0], aldryx[frame].src[1], aldryx[frame].src[2], aldryx[frame].src[3]};
	RECT_FIXED aldryx_dst = {
		FIXED_DEC(235 - aldryx[frame].off[0],1) - fx,
		FIXED_DEC(93 - aldryx[frame].off[1],1) - fy,
		FIXED_DEC(aldryx[frame].src[2],1),
		FIXED_DEC(aldryx[frame].src[3],1)
	};
	
	Gfx_LoadTex(&this->tex_aldryx, Archive_Find(this->arc_bg3obj, aldryx[frame].texname), 0);
	Stage_DrawTex(&this->tex_aldryx, &aldryx_src, &aldryx_dst, stage.camera.bzoom);
	
	RECT bg1_src = {0, 0, 256, 256};
	RECT_FIXED bg1_dst = {
		FIXED_DEC(-256,1) - fx,
		FIXED_DEC(-126,1) - fy,
		FIXED_DEC(256,1),
		FIXED_DEC(256,1)
	};
	RECT bg2_src = {0, 0, 256, 256};
	RECT_FIXED bg2_dst = {
		FIXED_DEC(-2,1) - fx,
		FIXED_DEC(-126,1) - fy,
		FIXED_DEC(256,1),
		FIXED_DEC(256,1)
	};
	
	Stage_DrawTex(&this->tex_back0, &bg1_src, &bg1_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back1, &bg2_src, &bg2_dst, stage.camera.bzoom);
}

void Back_BG3_Free(StageBack *back)
{
	Back_BG3 *this = (Back_BG3*)back;
	
	//Free structure
	Mem_Free(this->arc_bg3obj);
	Mem_Free(this);
}

StageBack *Back_BG3_New(void)
{
	//Allocate background structure
	Back_BG3 *this = (Back_BG3*)Mem_Alloc(sizeof(Back_BG3));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = NULL;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_BG3_DrawBG;
	this->back.free = Back_BG3_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\STAGES\\BG3.ARC;1");
	this->arc_bg3obj = IO_Read("\\STAGES\\BG3OBJ.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "1.tim"), 0);
	Mem_Free(arc_back);
	
	return (StageBack*)this;
}
