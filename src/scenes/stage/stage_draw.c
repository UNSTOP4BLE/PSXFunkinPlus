#include "stage.h"
#include "stage_draw.h"

#include "../../psx/mem.h"
#include "../../psx/mutil.h"

void Stage_InitCamera(Camera *camera)
{
	camera->speed = FIXED_DEC(5,100);
	camera->scroll = true;
	
	camera->current.x = camera->target.x;
	camera->current.y = camera->target.y;
	camera->current.zoom = camera->target.zoom;
	camera->current.rotation = camera->target.rotation;
}

void Stage_TickCamera(Camera *camera)
{
	if(camera->scroll) {
		camera->current.x = lerp(camera->current.x, camera->target.x, camera->speed);
		camera->current.y = lerp(camera->current.y, camera->target.y, camera->speed);
		camera->current.zoom = lerp(camera->current.zoom, camera->target.zoom, camera->speed);
		camera->current.rotation = lerp(camera->current.rotation, camera->target.rotation, camera->speed);
	}
}

// Helper function to apply rotation
static void ApplyRotation(fixed_t x, fixed_t y, fixed_t *outX, fixed_t *outY, u8 rotationAngle)
{
    fixed_t cosAngle = FIXED_DEC(MUtil_Cos(rotationAngle), 256);
    fixed_t sinAngle = FIXED_DEC(MUtil_Sin(rotationAngle), 256);
    *outX = FIXED_MUL(x, cosAngle) - FIXED_MUL(y, sinAngle);
    *outY = FIXED_MUL(x, sinAngle) + FIXED_MUL(y, cosAngle);
}

// Common function for texture drawing transformations with camera
static void TransformTexCoords(const RECT_FIXED *dst, RECT *outRect, Camera *camera)
{
    fixed_t x, y;
    u8 rotationAngle = camera->current.rotation / FIXED_UNIT;
    ApplyRotation(dst->x, dst->y, &x, &y, rotationAngle);
    
    fixed_t l = (SCREEN_WIDTH2  * FIXED_UNIT) + FIXED_MUL(x, camera->current.zoom) - camera->current.x;
    fixed_t t = (SCREEN_HEIGHT2 * FIXED_UNIT) + FIXED_MUL(y, camera->current.zoom) - camera->current.y;
    fixed_t r = l + FIXED_MUL(dst->w, camera->current.zoom);
    fixed_t b = t + FIXED_MUL(dst->h, camera->current.zoom);
    
    outRect->x = l / FIXED_UNIT;
    outRect->y = t / FIXED_UNIT;
    outRect->w = (r - l) / FIXED_UNIT;
    outRect->h = (b - t) / FIXED_UNIT;
}

// Common function for texture drawing transformations with camera and offset
static void TransformTexCoordsOffset(const RECT_FIXED *dst, RECT *outRect, Camera *camera)
{
    fixed_t x, y;
    u8 rotationAngle = camera->current.rotation / FIXED_UNIT;
    ApplyRotation(dst->x, dst->y, &x, &y, rotationAngle);
    
    fixed_t l = (SCREEN_WIDTH2  * FIXED_UNIT) + FIXED_MUL(x, camera->current.zoom) + FIXED_DEC(1,2) - camera->current.x;
    fixed_t t = (SCREEN_HEIGHT2 * FIXED_UNIT) + FIXED_MUL(y, camera->current.zoom) + FIXED_DEC(1,2) - camera->current.y;
    fixed_t r = l + FIXED_MUL(dst->w, camera->current.zoom);
    fixed_t b = t + FIXED_MUL(dst->h, camera->current.zoom);
    
    outRect->x = l / FIXED_UNIT;
    outRect->y = t / FIXED_UNIT;
    outRect->w = (r - l) / FIXED_UNIT;
    outRect->h = (b - t) / FIXED_UNIT;
}

// Helper function for arbitrary point transformation with camera
static void TransformPointArb(const POINT_FIXED *src, POINT *dst, Camera *camera)
{
    u8 rotationAngle = camera->current.rotation / FIXED_UNIT;
    fixed_t cosAngle = FIXED_DEC(MUtil_Cos(rotationAngle), 256);
    fixed_t sinAngle = FIXED_DEC(MUtil_Sin(rotationAngle), 256);

    fixed_t x = FIXED_MUL(src->x, cosAngle) - FIXED_MUL(src->y, sinAngle);
    fixed_t y = FIXED_MUL(src->x, sinAngle) + FIXED_MUL(src->y, cosAngle);

    dst->x = SCREEN_WIDTH2 + (FIXED_MUL(x, camera->current.zoom) / FIXED_UNIT) - camera->current.x / FIXED_UNIT;
    dst->y = SCREEN_HEIGHT2 + (FIXED_MUL(y, camera->current.zoom) / FIXED_UNIT) - camera->current.y / FIXED_UNIT;
}

// ==================== STAGE DRAWING FUNCTIONS ====================

void Stage_DrawRect(const RECT_FIXED *dst, u8 cr, u8 cg, u8 cb, Camera *camera)
{
    RECT sdst;
    TransformTexCoords(dst, &sdst, camera);
    Gfx_DrawRect(&sdst, cr, cg, cb);
}

void Stage_BlendRect(const RECT_FIXED *dst, u8 cr, u8 cg, u8 cb, int mode, Camera *camera)
{
    RECT sdst;
    TransformTexCoords(dst, &sdst, camera);
    Gfx_BlendRect(&sdst, cr, cg, cb, mode);
}

void Stage_DrawTexRotateCol(Gfx_Tex *tex, const RECT *src, const RECT_FIXED *dst, u8 angle, fixed_t hx, fixed_t hy, u8 r, u8 g, u8 b, Camera *camera)
{
    RECT sdst;
    TransformTexCoords(dst, &sdst, camera);
    
    u8 totalRotation = camera->current.rotation / FIXED_UNIT;
    Gfx_DrawTexRotateCol(tex, src, &sdst, angle + totalRotation, hx, hy, r, g, b);
}

void Stage_DrawTexRotate(Gfx_Tex *tex, const RECT *src, const RECT_FIXED *dst, u8 angle, fixed_t hx, fixed_t hy, Camera *camera)
{
    Stage_DrawTexRotateCol(tex, src, dst, angle, hx, hy, 128, 128, 128, camera);
}

void Stage_DrawTexCol(Gfx_Tex *tex, const RECT *src, const RECT_FIXED *dst, u8 r, u8 g, u8 b, Camera *camera)
{
    RECT sdst;
    TransformTexCoordsOffset(dst, &sdst, camera);
    
    u8 totalRotation = camera->current.rotation / FIXED_UNIT;
    Gfx_DrawTexRotateCol(tex, src, &sdst, totalRotation, 0, 0, r, g, b);
}

void Stage_DrawTex(Gfx_Tex *tex, const RECT *src, const RECT_FIXED *dst, Camera *camera)
{
    Stage_DrawTexCol(tex, src, dst, 0x80, 0x80, 0x80, camera);
}

void Stage_DrawTexArbCol(Gfx_Tex *tex, const RECT *src, const POINT_FIXED *p0, const POINT_FIXED *p1, const POINT_FIXED *p2, const POINT_FIXED *p3, u8 r, u8 g, u8 b, Camera *camera)
{
    POINT transformed[4];
    TransformPointArb(p0, &transformed[0], camera);
    TransformPointArb(p1, &transformed[1], camera);
    TransformPointArb(p2, &transformed[2], camera);
    TransformPointArb(p3, &transformed[3], camera);

    Gfx_DrawTexArbCol(tex, src, &transformed[0], &transformed[1], &transformed[2], &transformed[3], r, g, b);
}

void Stage_DrawTexArb(Gfx_Tex *tex, const RECT *src, const POINT_FIXED *p0, const POINT_FIXED *p1, const POINT_FIXED *p2, const POINT_FIXED *p3, Camera *camera)
{
    Stage_DrawTexArbCol(tex, src, p0, p1, p2, p3, 0x80, 0x80, 0x80, camera);
}

void Stage_BlendTexArbCol(Gfx_Tex *tex, const RECT *src, const POINT_FIXED *p0, const POINT_FIXED *p1, const POINT_FIXED *p2, const POINT_FIXED *p3, u8 r, u8 g, u8 b, u8 mode, Camera *camera)
{
    POINT transformed[4];
    TransformPointArb(p0, &transformed[0], camera);
    TransformPointArb(p1, &transformed[1], camera);
    TransformPointArb(p2, &transformed[2], camera);
    TransformPointArb(p3, &transformed[3], camera);
    
    Gfx_BlendTexArbCol(tex, src, &transformed[0], &transformed[1], &transformed[2], &transformed[3], r, g, b, mode);
}

void Stage_BlendTexArb(Gfx_Tex *tex, const RECT *src, const POINT_FIXED *p0, const POINT_FIXED *p1, const POINT_FIXED *p2, const POINT_FIXED *p3, u8 mode, Camera *camera)
{
    Stage_BlendTexArbCol(tex, src, p0, p1, p2, p3, 0x80, 0x80, 0x80, mode, camera);
}

void Stage_BlendTex(Gfx_Tex *tex, const RECT *src, const RECT_FIXED *dst, u8 mode, Camera *camera)
{
    RECT sdst;
    TransformTexCoords(dst, &sdst, camera);
    
    u8 totalRotation = camera->current.rotation / FIXED_UNIT;
    Gfx_BlendTexRotate(tex, src, &sdst, totalRotation, 0, 0, mode);
}