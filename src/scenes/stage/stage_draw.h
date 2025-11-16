#ifndef PSXF_GUARD_STAGE_DRAW_H
#define PSXF_GUARD_STAGE_DRAW_H

#include "../../psx/io.h"
#include "../../psx/gfx.h"

typedef struct
{
	struct {
		fixed_t x, y;      // Position
		fixed_t zoom;      // Zoom level
		fixed_t rotation;  // Rotation in fixed-point degrees
	} current;
	struct {
		fixed_t x, y;      // Position
		fixed_t zoom;      // Zoom level
		fixed_t rotation;  // Rotation in fixed-point degrees
	} target;
	fixed_t speed;
	boolean scroll;
} Camera;

void Stage_TickCamera(Camera *camera);
void Stage_InitCamera(Camera *camera);

//Stage drawing functions
void Stage_DrawRect(const RECT_FIXED *dst, u8 cr, u8 cg, u8 cb, Camera *camera);
void Stage_BlendRect(const RECT_FIXED *dst, u8 cr, u8 cg, u8 cb, int mode, Camera *camera);
void Stage_DrawTexRotateCol(Gfx_Tex *tex, const RECT *src, const RECT_FIXED *dst, u8 angle, fixed_t hx, fixed_t hy, u8 r, u8 g, u8 b, Camera *camera);
void Stage_DrawTexRotate(Gfx_Tex *tex, const RECT *src, const RECT_FIXED *dst, u8 angle, fixed_t hx, fixed_t hy, Camera *camera);
void Stage_DrawTexCol(Gfx_Tex *tex, const RECT *src, const RECT_FIXED *dst, u8 r, u8 g, u8 b, Camera *camera);
void Stage_DrawTex(Gfx_Tex *tex, const RECT *src, const RECT_FIXED *dst, Camera *camera);
void Stage_DrawTexArbCol(Gfx_Tex *tex, const RECT *src, const POINT_FIXED *p0, const POINT_FIXED *p1, const POINT_FIXED *p2, const POINT_FIXED *p3, u8 r, u8 g, u8 b, Camera *camera);
void Stage_DrawTexArb(Gfx_Tex *tex, const RECT *src, const POINT_FIXED *p0, const POINT_FIXED *p1, const POINT_FIXED *p2, const POINT_FIXED *p3, Camera *camera);
void Stage_BlendTexArbCol(Gfx_Tex *tex, const RECT *src, const POINT_FIXED *p0, const POINT_FIXED *p1, const POINT_FIXED *p2, const POINT_FIXED *p3, u8 r, u8 g, u8 b, u8 mode, Camera *camera);
void Stage_BlendTexArb(Gfx_Tex *tex, const RECT *src, const POINT_FIXED *p0, const POINT_FIXED *p1, const POINT_FIXED *p2, const POINT_FIXED *p3, u8 mode, Camera *camera);
void Stage_BlendTex(Gfx_Tex *tex, const RECT *src, const RECT_FIXED *dst, u8 mode, Camera *camera);

#endif
