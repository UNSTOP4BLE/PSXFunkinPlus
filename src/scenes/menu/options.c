#include "options.h"
#include "menu.h"

#include "../../main.h"
#include "../../audio.h"

#include "../../psx/mem.h"
#include "../../psx/timer.h"
#include "../../psx/io.h"
#include "../../psx/gfx.h"
#include "../../psx/pad.h"
#include "../../psx/archive.h"
#include "../../psx/mutil.h"
#include "../../psx/trans.h"
#include "../../psx/loadscr.h"

#include "../../fonts/font.h"

#include "../stage/stage.h"

//Options state
static struct
{
	u8 select, select2, next_select, page;
	u8 sinus;
	
	//Controls Variables
	boolean bind;
} options;

static const u8 buttons[12][4] = {
	{157, 0, 14, 14}, //PAD_UP
	{172, 0, 14, 14}, //PAD_DOWN
	{157, 15, 14, 14}, //PAD_LEFT
	{172, 15, 14, 14}, //PAD_RIGHT
	
	{138, 17, 15, 15}, //PAD_TRIANGLE
	{140, 0, 16, 16}, //PAD_CIRCLE
	{123, 0, 16, 16}, //PAD_CROSS
	{123, 17, 14, 14}, //PAD_SQUARE
	
	{72, 16, 23, 15}, //PAD_L1
	{72, 0, 24, 15}, //PAD_L2
	{97, 16, 24, 15}, //PAD_R1
	{97, 0, 25, 15}, //PAD_R2
};

static void Controls()
{
	//Change option
	if(!options.bind)
	{
		if (pad_state.press & PAD_LEFT)
		{
			if (menu.select > 0)
				menu.select--;
			else
				menu.select = 3;
		}
		if (pad_state.press & PAD_RIGHT)
		{
			if (menu.select < 3)
				menu.select++;
			else
				menu.select = 0;
		}
		if (pad_state.press & PAD_CROSS)
			options.bind = true;
		//Return to main menu if circle is pressed
		if (pad_state.press & PAD_CIRCLE)
		{
			options.page = 0;
			menu.select = 0;
		}
	}
	else
	{
		switch (pad_state.press)
		{
			case PAD_UP:
				stage.prefs.control_keys[menu.select] = PAD_UP;
				options.bind = false;
				break;
			case PAD_DOWN:
				stage.prefs.control_keys[menu.select] = PAD_DOWN;
				options.bind = false;
				break;
			case PAD_LEFT:
				stage.prefs.control_keys[menu.select] = PAD_LEFT;
				options.bind = false;
				break;
			case PAD_RIGHT:
				stage.prefs.control_keys[menu.select] = PAD_RIGHT;
				options.bind = false;
				break;
			case PAD_TRIANGLE:
				stage.prefs.control_keys[menu.select] = PAD_TRIANGLE;
				options.bind = false;
				break;
			case PAD_CIRCLE:
				stage.prefs.control_keys[menu.select] = PAD_CIRCLE;
				options.bind = false;
				break;
			case PAD_CROSS:
				stage.prefs.control_keys[menu.select] = PAD_CROSS;
				options.bind = false;
				break;
			case PAD_SQUARE:
				stage.prefs.control_keys[menu.select] = PAD_SQUARE;
				options.bind = false;
				break;
			case PAD_L1:
				stage.prefs.control_keys[menu.select] = PAD_L1;
				options.bind = false;
				break;
			case PAD_L2:
				stage.prefs.control_keys[menu.select] = PAD_L2;
				options.bind = false;
				break;
			case PAD_R1:
				stage.prefs.control_keys[menu.select] = PAD_R1;
				options.bind = false;
				break;
			case PAD_R2:
				stage.prefs.control_keys[menu.select] = PAD_R2;
				options.bind = false;
				break;
		}
	}
	
	RECT select_src = {187, 0, 9, 11};
	RECT arrows_src = {0, 40, 128, 32};
	
	u8 order;
	
	for (u8 j = 0; j < 4; j++)
	{
		switch (stage.prefs.control_keys[j])
		{
			case PAD_UP:
				order = 0;
				break;
			case PAD_DOWN:
				order = 1;
				break;
			case PAD_LEFT:
				order = 2;
				break;
			case PAD_RIGHT:
				order = 3;
				break;
			case PAD_TRIANGLE:
				order = 4;
				break;
			case PAD_CIRCLE:
				order = 5;
				break;
			case PAD_CROSS:
				order = 6;
				break;
			case PAD_SQUARE:
				order = 7;
				break;
			case PAD_L1:
				order = 8;
				break;
			case PAD_L2:
				order = 9;
				break;
			case PAD_R1:
				order = 10;
				break;
			case PAD_R2:
				order = 11;
				break;
		}
		RECT button_src = {buttons[order][0], buttons[order][1], buttons[order][2], buttons[order][3]};
		Gfx_BlitTex(&menu.tex_options,
			&button_src,
			SCREEN_WIDTH2 + (j * 32) - (button_src.w / 2) - 64 + 16,
			SCREEN_HEIGHT2 - 80
		);
	}
	
	Gfx_BlitTex(&menu.tex_options,
		&select_src,
		(SCREEN_WIDTH2 + 12) + (menu.select * 32) - 64,
		SCREEN_HEIGHT2 + (MUtil_Sin(options.sinus) / 30)
	);

	Gfx_BlitTex(&menu.tex_options, &arrows_src, SCREEN_WIDTH2 - (arrows_src.w / 2), SCREEN_HEIGHT2 - 40);
	Gfx_BlitTex(&menu.tex_options, &arrows_src, SCREEN_WIDTH2 - (arrows_src.w / 2), SCREEN_HEIGHT2 - 40);
	
}

static void Adjust_Combo()
{
	char text[0x80];
	sprintf(text, "Combo Offsets:\n[%d, %d]", stage.prefs.combox, stage.prefs.comboy);
	
	fonts.font_cdr.draw(&fonts.font_cdr,
		text,
		10,
		10,
		FontAlign_Left
	);
	
	if (pad_state.held & PAD_UP)
		stage.prefs.comboy -= 1;
	if (pad_state.held & PAD_DOWN)
		stage.prefs.comboy += 1;
	if (pad_state.held & PAD_LEFT)
		stage.prefs.combox -= 1;
	if (pad_state.held & PAD_RIGHT)
		stage.prefs.combox += 1;
	
	RECT combo_src = {0, 0, 71, 39};
	Gfx_BlitTex(&menu.tex_options, &combo_src, stage.prefs.combox + SCREEN_WIDTH2, stage.prefs.comboy + SCREEN_HEIGHT2);
}

static void Graphics()
{
	static const struct
	{
		enum
		{
			OptType_Boolean,
			OptType_Enum,
		} type;
		const char *text;
		void *value;
		union
		{
			struct
			{
				int a;
			} spec_boolean;
			struct
			{
				s32 max;
				const char **strs;
			} spec_enum;
		} spec;
	} menu_options[] = {
		{OptType_Boolean, "LOW GRAPHICS", &stage.prefs.lowgraphics, {.spec_boolean = {0}}},
		{OptType_Boolean, "PAL REFRESH RATE", &stage.prefs.palmode, {.spec_boolean = {0}}}
	};
	
	if (menu.select == 1 && pad_state.press & (PAD_CROSS | PAD_LEFT | PAD_RIGHT))
		stage.pal_i = 1;
	
	//Initialize page
	if (menu.page_swap)
		menu.scroll = COUNT_OF(menu_options) * FIXED_DEC(24 + SCREEN_HEIGHT2,1);
	
	//Handle option and selection
	if (menu.next_page == menu.page && Trans_Idle())
	{
		//Change option
		if (pad_state.press & PAD_UP)
		{
			if (menu.select > 0)
				menu.select--;
			else
				menu.select = COUNT_OF(menu_options) - 1;
		}
		if (pad_state.press & PAD_DOWN)
		{
			if (menu.select < COUNT_OF(menu_options) - 1)
				menu.select++;
			else
				menu.select = 0;
		}
		
		//Handle option changing
		switch (menu_options[menu.select].type)
		{
			case OptType_Boolean:
				if (pad_state.press & (PAD_CROSS | PAD_LEFT | PAD_RIGHT))
					*((boolean*)menu_options[menu.select].value) ^= 1;
				break;
			case OptType_Enum:
				if (pad_state.press & PAD_LEFT)
					if (--*((s32*)menu_options[menu.select].value) < 0)
						*((s32*)menu_options[menu.select].value) = menu_options[menu.select].spec.spec_enum.max - 1;
				if (pad_state.press & PAD_RIGHT)
					if (++*((s32*)menu_options[menu.select].value) >= menu_options[menu.select].spec.spec_enum.max)
						*((s32*)menu_options[menu.select].value) = 0;
				break;
		}
		
		//Return to main menu if circle is pressed
		if (pad_state.press & PAD_CIRCLE)
		{
			options.page = 0;
			menu.select = 2;
		}
	}
		
	//Draw options
	s32 next_scroll = menu.select * FIXED_DEC(24,1);
	menu.scroll += (next_scroll - menu.scroll) >> 4;
	
	for (u8 i = 0; i < COUNT_OF(menu_options); i++)
	{
		//Get position on screen
		s32 y = (i * 24) - 8 - (menu.scroll >> FIXED_SHIFT);
		if (y <= -SCREEN_HEIGHT2 - 8)
			continue;
		if (y >= SCREEN_HEIGHT2 + 8)
			break;
		
		//Draw text
		char text[0x80];
		switch (menu_options[i].type)
		{
			case OptType_Boolean:
				sprintf(text, "%s %s", menu_options[i].text, *((boolean*)menu_options[i].value) ? "ON" : "OFF");
				break;
			case OptType_Enum:
				sprintf(text, "%s %s", menu_options[i].text, menu_options[i].spec.spec_enum.strs[*((s32*)menu_options[i].value)]);
				break;
		}
		fonts.font_bold.draw_col(&fonts.font_bold,
			text,
			SCREEN_WIDTH2 - 120,
			SCREEN_HEIGHT2 + y - 8,
			FontAlign_Left,
			(i == menu.select) ? 128 : 100,
			(i == menu.select) ? 128 : 100,
			(i == menu.select) ? 128 : 100
		);
	}
}

static void Visuals_And_UI()
{
	static const struct
	{
		enum
		{
			OptType_Boolean,
			OptType_Enum,
		} type;
		const char *text;
		void *value;
		union
		{
			struct
			{
				int a;
			} spec_boolean;
			struct
			{
				s32 max;
				const char **strs;
			} spec_enum;
		} spec;
	} menu_options[] = {
		{OptType_Boolean, "NOTE SPLASHES", &stage.prefs.notesplashes, {.spec_boolean = {0}}},
		{OptType_Boolean, "HIDE HUD", &stage.prefs.hidehud, {.spec_boolean = {0}}},
		{OptType_Boolean, "TIMER BAR", &stage.prefs.timebar, {.spec_boolean = {0}}},
		{OptType_Boolean, "FLASHING LIGHTS", &stage.prefs.flashinglights, {.spec_boolean = {0}}},
		{OptType_Boolean, "CAMERA ZOOMS", &stage.prefs.camerazooms, {.spec_boolean = {0}}},
		{OptType_Boolean, "COMBO STACK", &stage.prefs.combostack, {.spec_boolean = {0}}}
	};
	
	//Initialize page
	if (menu.page_swap)
		menu.scroll = COUNT_OF(menu_options) * FIXED_DEC(24 + SCREEN_HEIGHT2,1);
	
	//Handle option and selection
	if (menu.next_page == menu.page && Trans_Idle())
	{
		//Change option
		if (pad_state.press & PAD_UP)
		{
			if (menu.select > 0)
				menu.select--;
			else
				menu.select = COUNT_OF(menu_options) - 1;
		}
		if (pad_state.press & PAD_DOWN)
		{
			if (menu.select < COUNT_OF(menu_options) - 1)
				menu.select++;
			else
				menu.select = 0;
		}
		
		//Handle option changing
		switch (menu_options[menu.select].type)
		{
			case OptType_Boolean:
				if (pad_state.press & (PAD_CROSS | PAD_LEFT | PAD_RIGHT))
					*((boolean*)menu_options[menu.select].value) ^= 1;
				break;
			case OptType_Enum:
				if (pad_state.press & PAD_LEFT)
					if (--*((s32*)menu_options[menu.select].value) < 0)
						*((s32*)menu_options[menu.select].value) = menu_options[menu.select].spec.spec_enum.max - 1;
				if (pad_state.press & PAD_RIGHT)
					if (++*((s32*)menu_options[menu.select].value) >= menu_options[menu.select].spec.spec_enum.max)
						*((s32*)menu_options[menu.select].value) = 0;
				break;
		}
		
		//Return to main menu if circle is pressed
		if (pad_state.press & PAD_CIRCLE)
		{
			options.page = 0;
			menu.select = 3;
		}
	}
		
	//Draw options
	s32 next_scroll = menu.select * FIXED_DEC(24,1);
	menu.scroll += (next_scroll - menu.scroll) >> 4;
	
	for (u8 i = 0; i < COUNT_OF(menu_options); i++)
	{
		//Get position on screen
		s32 y = (i * 24) - 8 - (menu.scroll >> FIXED_SHIFT);
		if (y <= -SCREEN_HEIGHT2 - 8)
			continue;
		if (y >= SCREEN_HEIGHT2 + 8)
			break;
		
		//Draw text
		char text[0x80];
		switch (menu_options[i].type)
		{
			case OptType_Boolean:
				sprintf(text, "%s %s", menu_options[i].text, *((boolean*)menu_options[i].value) ? "ON" : "OFF");
				break;
			case OptType_Enum:
				sprintf(text, "%s %s", menu_options[i].text, menu_options[i].spec.spec_enum.strs[*((s32*)menu_options[i].value)]);
				break;
		}
		fonts.font_bold.draw_col(&fonts.font_bold,
			text,
			SCREEN_WIDTH2 - 120,
			SCREEN_HEIGHT2 + y - 8,
			FontAlign_Left,
				(i == menu.select) ? 128 : 100,
				(i == menu.select) ? 128 : 100,
				(i == menu.select) ? 128 : 100
		);
	}
}

static void Gameplay()
{
	static const char *gamemode_strs[] = {"NORMAL", "SWAP", "MULTIPLAYER"};
	static const struct
	{
		enum
		{
			OptType_Boolean,
			OptType_Enum,
		} type;
		const char *text;
		void *value;
		union
		{
			struct
			{
				int a;
			} spec_boolean;
			struct
			{
				s32 max;
				const char **strs;
			} spec_enum;
		} spec;
	} menu_options[] = {
		{OptType_Enum,    "GAMEMODE", &stage.prefs.mode, {.spec_enum = {COUNT_OF(gamemode_strs), gamemode_strs}}},
		{OptType_Boolean, "DOWNSCROLL", &stage.prefs.downscroll, {.spec_boolean = {0}}},
		{OptType_Boolean, "MIDDLESCROLL", &stage.prefs.middlescroll, {.spec_boolean = {0}}},
		{OptType_Boolean, "OPPONENT NOTES", &stage.prefs.opponentnotes, {.spec_boolean = {0}}},
		{OptType_Boolean, "GHOST TAPPING", &stage.prefs.ghost, {.spec_boolean = {0}}}
	};
	
	//Initialize page
	if (menu.page_swap)
		menu.scroll = COUNT_OF(menu_options) * FIXED_DEC(24 + SCREEN_HEIGHT2,1);
	
	//Handle option and selection
	if (menu.next_page == menu.page && Trans_Idle())
	{
		//Change option
		if (pad_state.press & PAD_UP)
		{
			if (menu.select > 0)
				menu.select--;
			else
				menu.select = COUNT_OF(menu_options) - 1;
		}
		if (pad_state.press & PAD_DOWN)
		{
			if (menu.select < COUNT_OF(menu_options) - 1)
				menu.select++;
			else
				menu.select = 0;
		}
		
		//Handle option changing
		switch (menu_options[menu.select].type)
		{
			case OptType_Boolean:
				if (pad_state.press & (PAD_CROSS | PAD_LEFT | PAD_RIGHT))
					*((boolean*)menu_options[menu.select].value) ^= 1;
				break;
			case OptType_Enum:
				if (pad_state.press & PAD_LEFT)
					if (--*((s32*)menu_options[menu.select].value) < 0)
						*((s32*)menu_options[menu.select].value) = menu_options[menu.select].spec.spec_enum.max - 1;
				if (pad_state.press & PAD_RIGHT)
					if (++*((s32*)menu_options[menu.select].value) >= menu_options[menu.select].spec.spec_enum.max)
						*((s32*)menu_options[menu.select].value) = 0;
				break;
		}
		
		//Return to main menu if circle is pressed
		if (pad_state.press & PAD_CIRCLE)
		{
			options.page = 0;
			menu.select = 4;
		}
	}
		
	//Draw options
	s32 next_scroll = menu.select * FIXED_DEC(24,1);
	menu.scroll += (next_scroll - menu.scroll) >> 4;
	
	for (u8 i = 0; i < COUNT_OF(menu_options); i++)
	{
		//Get position on screen
		s32 y = (i * 24) - 8 - (menu.scroll >> FIXED_SHIFT);
		if (y <= -SCREEN_HEIGHT2 - 8)
			continue;
		if (y >= SCREEN_HEIGHT2 + 8)
			break;
		
		//Draw text
		char text[0x80];
		switch (menu_options[i].type)
		{
			case OptType_Boolean:
				sprintf(text, "%s %s", menu_options[i].text, *((boolean*)menu_options[i].value) ? "ON" : "OFF");
				break;
			case OptType_Enum:
				sprintf(text, "%s %s", menu_options[i].text, menu_options[i].spec.spec_enum.strs[*((s32*)menu_options[i].value)]);
				break;
		}
		fonts.font_bold.draw_col(&fonts.font_bold,
			text,
			SCREEN_WIDTH2 - 120,
			SCREEN_HEIGHT2 + y - 8,
			FontAlign_Left,
				(i == menu.select) ? 128 : 100,
				(i == menu.select) ? 128 : 100,
				(i == menu.select) ? 128 : 100
		);
	}
}

static void Memory_Card()
{
			static const char *menu_options[] = {
				"CARD SAVE",
				"CARD LOAD",
				"AUTO SAVE",
				"RESET ALL DEFAULT"
			};
			
			//Initialize page
			if (menu.page_swap)
				menu.scroll = menu.select * FIXED_DEC(12,1);
			
			//Handle option and selection
			if (menu.trans_time > 0 && (menu.trans_time -= timer_dt) <= 0)
				Trans_Start();
			
			if (menu.next_page == menu.page && Trans_Idle())
			{
				//Change option
				if (pad_state.press & PAD_UP)
				{
					if (menu.select > 0)
						menu.select--;
					else
						menu.select = COUNT_OF(menu_options) - 1;
				}
				if (pad_state.press & PAD_DOWN)
				{
					if (menu.select < COUNT_OF(menu_options) - 1)
						menu.select++;
					else
						menu.select = 0;
				}
				
				if((menu.select == 2) && (pad_state.press & (PAD_LEFT | PAD_RIGHT)))
					stage.prefs.autosave = (stage.prefs.autosave) ? false : true;
				
				//Select option if cross is pressed
				if (pad_state.press & (PAD_START | PAD_CROSS))
				{
					switch (menu.select)
					{
						case 0: //Story Mode
							break;
					}
					menu.select = 0;
				}
				
				//Return to main menu if circle is pressed
				if (pad_state.press & PAD_CIRCLE)
				{
					options.page = 0;
					menu.select = 5;
				}
			}
			
			//Draw options
			s32 next_scroll = menu.select * FIXED_DEC(12,1);
			menu.scroll += (next_scroll - menu.scroll) >> 2;
			
				//Draw all options
				for (u8 i = 0; i < COUNT_OF(menu_options); i++)
				{
					if(i == 2)
					{
						char text[64];
						sprintf(text, "%s %s", menu_options[i], (stage.prefs.autosave) ? "ON" : "OFF");
						fonts.font_bold.draw_col(&fonts.font_bold,
							text,
							SCREEN_WIDTH2,
							SCREEN_HEIGHT2 + (i * 24) - 50,
							FontAlign_Center,
							(i == menu.select) ? 128 : 100,
							(i == menu.select) ? 128 : 100,
							(i == menu.select) ? 128 : 100
						);
					}
					else
						fonts.font_bold.draw_col(&fonts.font_bold,
							menu_options[i],
							SCREEN_WIDTH2,
							SCREEN_HEIGHT2 + (i * 24) - 50,
							FontAlign_Center,
							(i == menu.select) ? 128 : 100,
							(i == menu.select) ? 128 : 100,
							(i == menu.select) ? 128 : 100
						);
				}
}

void Options_Tick()
{
	options.sinus += 5;
	
	//Initialize page
	if (menu.page_swap)
		options.page = 0;
	
	if (options.page == 0)
	{
			static const char *menu_options[] = {
				"CONTROLS",
				"ADJUST COMBO",
				"GRAPHICS",
				"VISUALS AND UI",
				"GAMEPLAY",
				"MEMORY CARD",
			};
			
			//Initialize page
			if (menu.page_swap)
				menu.scroll = menu.select * FIXED_DEC(12,1);
			
			//Handle option and selection
			if (menu.trans_time > 0 && (menu.trans_time -= timer_dt) <= 0)
				Trans_Start();
			
			if (menu.next_page == menu.page && Trans_Idle())
			{
				//Change option
				if (pad_state.press & PAD_UP)
				{
					if (menu.select > 0)
						menu.select--;
					else
						menu.select = COUNT_OF(menu_options) - 1;
				}
				if (pad_state.press & PAD_DOWN)
				{
					if (menu.select < COUNT_OF(menu_options) - 1)
						menu.select++;
					else
						menu.select = 0;
				}
				
				//Select option if cross is pressed
				if (pad_state.press & (PAD_START | PAD_CROSS))
				{
					switch (menu.select)
					{
						case 0: //Story Mode
							options.page = 1;
							options.bind = false;
							break;
						case 1: //Freeplay
							options.page = 2;
							break;
						case 2: //Mods
							options.page = 3;
							break;
						case 3: //Story Mode
							options.page = 4;
							break;
						case 4: //Freeplay
							options.page = 5;
							break;
						case 5: //Mods
							options.page = 6;
							break;
					}
					menu.select = 0;
				}
				
				//Return to main menu if circle is pressed
				if (pad_state.press & PAD_CIRCLE)
				{
					menu.next_page = MenuPage_Main;
					menu.next_select = 3; //Options
					Trans_Start();
				}
			}
			
			//Draw options
			s32 next_scroll = menu.select * FIXED_DEC(12,1);
			menu.scroll += (next_scroll - menu.scroll) >> 2;
			
				//Draw all options
				for (u8 i = 0; i < COUNT_OF(menu_options); i++)
				{
					fonts.font_bold.draw_col(&fonts.font_bold,
						menu_options[i],
						SCREEN_WIDTH2,
						SCREEN_HEIGHT2 + (i * 24) - 70,
						FontAlign_Center,
						(i == menu.select) ? 128 : 100,
						(i == menu.select) ? 128 : 100,
						(i == menu.select) ? 128 : 100
					);
				}
	}
	if (options.page == 1)
		Controls();
	if (options.page == 2)
		Adjust_Combo();
	if (options.page == 3)
		Graphics();
	if (options.page == 4)
		Visuals_And_UI();
	if (options.page == 5)
		Gameplay();
	if (options.page == 6)
		Memory_Card();
}
