//The debug system script by UNSTOP4BLE, part of the UNSTOP4BLE's fork on GitHub, is a crucial tool for game developers working on PSXFunkin.
//Check it out at https://github.com/UNSTOP4BLE/PSXFunkin-by-UNSTOP4BLE.

#include "stage.h"
#include "debug.h"
#include "../../fonts/font.h"

static void Drawdebugtxt()
{
	sprintf(debug.debugtext, "Welcome to PSXFunkin debug mode!");
	fonts.font_cdr.draw(&fonts.font_cdr,
		debug.debugtext,
		FIXED_DEC(-158, 1),
		FIXED_DEC(-105, 1),
		FontAlign_Left
	);
	if (debug.mode == 0)
	{
		sprintf(debug.debugtext, "Transform objects with dpad and face buttons, switch objects");
		fonts.font_cdr.draw(&fonts.font_cdr,
			debug.debugtext,
			FIXED_DEC(-158, 1),
			FIXED_DEC(-105 + 10, 1),
			FontAlign_Left
		);
		sprintf(debug.debugtext, "with L1 and R1, switch debug modes with L2 and R2,");
		fonts.font_cdr.draw(&fonts.font_cdr,
			debug.debugtext,
			FIXED_DEC(-158, 1),
			FIXED_DEC(-105 + 20, 1),
			FontAlign_Left
		);
		sprintf(debug.debugtext, "Focus camera with select");
		fonts.font_cdr.draw(&fonts.font_cdr,
			debug.debugtext,
			FIXED_DEC(-158, 1),
			FIXED_DEC(-105 + 30, 1),
			FontAlign_Left
		);
		sprintf(debug.debugtext, "x %d, y %d, w %d, h %d, selection %d, mode %d", debug.debugx, debug.debugy, debug.debugw, debug.debugh, debug.selection, debug.mode);
		fonts.font_cdr.draw(&fonts.font_cdr,
			debug.debugtext,
			FIXED_DEC(-158, 1),
			FIXED_DEC(-105 + 40, 1),
			FontAlign_Left
		);
		stage.freecam = 0;
	}
	else
	{
		sprintf(debug.debugtext, "Move the camera with the dpad,");
		fonts.font_cdr.draw(&fonts.font_cdr,
			debug.debugtext,
			FIXED_DEC(-158, 1),
			FIXED_DEC(-105 + 10, 1),
			FontAlign_Left
		);
		sprintf(debug.debugtext, "switch debug modes with L2 and R2");
		fonts.font_cdr.draw(&fonts.font_cdr,
			debug.debugtext,
			FIXED_DEC(-158, 1),
			FIXED_DEC(-105 + 20, 1),
			FontAlign_Left
		);
		sprintf(debug.debugtext, "x %d, y %d, zoom %d, mode %d", stage.camera.x / 1024, stage.camera.y / 1024, stage.camera.bzoom, debug.mode);
		fonts.font_cdr.draw(&fonts.font_cdr,
			debug.debugtext,
			FIXED_DEC(-158, 1),
			FIXED_DEC(-105 + 30, 1),
			FontAlign_Left
		);
		stage.freecam = 1;
	}
}

void Debug_StageDebug()
{
	if (stage.debug)
	{
		stage.prefs.botplay = 1; 
		
		if (debug.selection < 1)
			debug.selection = 0;

		if (debug.mode > 1 || debug.mode < 1)
			debug.mode = 0;
		Drawdebugtxt();

		if (debug.mode == 0)
		{
			switch(debug.selection)
			{
				case 0:
					if (debug.switchcooldown == 0)
					{
						debug.debugx = stage.opponent->x / 1024; 
						debug.debugy = stage.opponent->y / 1024; 
					}
					debug.switchcooldown ++;
					stage.opponent->x = FIXED_DEC(debug.debugx,1);
					stage.opponent->y = FIXED_DEC(debug.debugy,1);
					break;
				case 1:
					if (debug.switchcooldown == 0)
					{
						debug.debugx = stage.gf->x / 1024; 
						debug.debugy = stage.gf->y / 1024; 
					}
					debug.switchcooldown ++;
					stage.gf->x = FIXED_DEC(debug.debugx,1);
					stage.gf->y = FIXED_DEC(debug.debugy,1);
					break;
				case 2:
					if (debug.switchcooldown == 0)
					{
						debug.debugx = stage.player->x / 1024; 
						debug.debugy = stage.player->y / 1024; 
					}
					debug.switchcooldown ++;
					stage.player->x = FIXED_DEC(debug.debugx,1);
					stage.player->y = FIXED_DEC(debug.debugy,1);
					break;
			}

			switch(pad_state.held)
			{
				case PAD_LEFT: 
					debug.debugx --;
					break;
				case PAD_RIGHT: 
					debug.debugx ++;
					break;
				case PAD_UP: 
					debug.debugy --;
					break;
				case PAD_DOWN: 
					debug.debugy ++;
					break;
				case PAD_SQUARE: 
					debug.debugw --;
					break;
				case PAD_CIRCLE: 
					debug.debugw ++;
					break;
				case PAD_TRIANGLE: 
					debug.debugh --;
					break;
				case PAD_CROSS: 
					debug.debugh ++;
					break;
			}
		}
		switch(pad_state.press)
		{
			case PAD_L1: 
				debug.selection --;
				debug.switchcooldown = 0;
				break;
			case PAD_R1: 
				debug.selection ++;
				debug.switchcooldown = 0;
				break;
			case PAD_L2: 
				debug.mode --;
				break;
			case PAD_R2: 
				debug.mode ++;
				break;
		}
	}
}

void Debug_StageMoveDebug(RECT_FIXED *dst, int dacase, fixed_t fx, fixed_t fy)
{
	if (debug.mode == 0 && stage.debug)
	{
		if (debug.selection == dacase)
		{
			if (debug.switchcooldown == 0)
			{
				debug.debugx = dst->x / 1024 + fx / 1024; 
				debug.debugy = dst->y / 1024 + fy / 1024; 
				debug.debugw = dst->w / 1024; 
				debug.debugh = dst->h / 1024; 
			}
			debug.switchcooldown ++;
			dst->x = FIXED_DEC(debug.debugx,1) - fx;
			dst->y = FIXED_DEC(debug.debugy,1) - fy;
			dst->w = FIXED_DEC(debug.debugw,1);
			dst->h = FIXED_DEC(debug.debugh,1);
		}
	}
}

void Debug_GfxMoveDebug(RECT *dst, int dacase)
{
	if (debug.mode == 0 && stage.debug)
	{
		if (debug.selection == dacase)
		{
			if (debug.switchcooldown == 0)
			{
				debug.debugx = dst->x; 
				debug.debugy = dst->y; 
				debug.debugw = dst->w; 
				debug.debugh = dst->h; 
			}
			debug.switchcooldown ++;
			dst->x = debug.debugx;
			dst->y = debug.debugy;
			dst->w = debug.debugw;
			dst->h = debug.debugh;
		}
	}
}

void Debug_ScrollCamera(void)
{
	if (stage.freecam)
	{
		if (pad_state.held & PAD_LEFT)
			stage.camera.x -= FIXED_DEC(2,1);
		if (pad_state.held & PAD_UP)
			stage.camera.y -= FIXED_DEC(2,1);
		if (pad_state.held & PAD_RIGHT)
			stage.camera.x += FIXED_DEC(2,1);
		if (pad_state.held & PAD_DOWN)
			stage.camera.y += FIXED_DEC(2,1);
		if (pad_state.held & PAD_TRIANGLE)
			stage.camera.zoom -= FIXED_DEC(1,100);
		if (pad_state.held & PAD_CROSS)
			stage.camera.zoom += FIXED_DEC(1,100);
	}
	else if (pad_state.held & PAD_SELECT)
	{
		//Scroll based off current divisor
		stage.camera.x = lerp(stage.camera.x, stage.camera.tx, stage.camera.speed);
		stage.camera.y = lerp(stage.camera.y, stage.camera.ty, stage.camera.speed);
		stage.camera.zoom = lerp(stage.camera.zoom, stage.camera.tz, stage.camera.speed);
		stage.camera.angle = lerp(stage.camera.angle, stage.camera.ta << FIXED_SHIFT, stage.camera.speed);
		stage.camera.hudangle = lerp(stage.camera.hudangle, stage.camera.hudta << FIXED_SHIFT, stage.camera.speed);
	}
}