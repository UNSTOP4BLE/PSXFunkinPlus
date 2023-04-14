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
	u8 select, select2, next_select;
	u8 page;
} options;

static void Controls()
{
	
}

static void Adjust_Combo()
{
	
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
		{OptType_Boolean, "LOW GRAPHICS ", &stage.prefs.lowgraphics, {.spec_boolean = {0}}},
		{OptType_Boolean, "PAL REFRESH RATE", &stage.prefs.palmode, {.spec_boolean = {0}}}
	};
	
	//if (menu.select == 2 && pad_state.press & (PAD_CROSS | PAD_LEFT | PAD_RIGHT))
	//	stage.pal_i = 1;
	
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
		{OptType_Boolean, "NOTE SPLASHES ", &stage.prefs.notesplashes, {.spec_boolean = {0}}},
		{OptType_Boolean, "HIDE HUD ", &stage.prefs.hidehud, {.spec_boolean = {0}}},
		{OptType_Boolean, "TIMER BAR ", &stage.prefs.timebar, {.spec_boolean = {0}}},
		{OptType_Boolean, "FLASHING LIGHTS ", &stage.prefs.flashinglights, {.spec_boolean = {0}}},
		{OptType_Boolean, "CAMERA ZOOMS ", &stage.prefs.camerazooms, {.spec_boolean = {0}}},
		{OptType_Boolean, "COMBO STACK ", &stage.prefs.combostack, {.spec_boolean = {0}}}
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
		//{OptType_Boolean, "LOW GRAPHICS ", &stage.prefs.lowgraphics, {.spec_boolean = {0}}},
		//{OptType_Boolean, "PAL REFRESH RATE", &stage.prefs.palmode, {.spec_boolean = {0}}},
		{OptType_Boolean, "DOWNSCROLL", &stage.prefs.downscroll, {.spec_boolean = {0}}},
		{OptType_Boolean, "MIDDLESCROLL", &stage.prefs.middlescroll, {.spec_boolean = {0}}},
		{OptType_Boolean, "OPPONENT NOTES ", &stage.prefs.opponentnotes, {.spec_boolean = {0}}},
		{OptType_Boolean, "GHOST TAPPING ", &stage.prefs.ghost, {.spec_boolean = {0}}},
		//{OptType_Boolean, "NOTE SPLASHES ", &stage.prefs.notesplashes, {.spec_boolean = {0}}},
		//{OptType_Boolean, "HIDE HUD", &stage.prefs.hidehud, {.spec_boolean = {0}}},
		//{OptType_Boolean, "HIDE SONG LENGTH ", &stage.prefs.hidetimer, {.spec_boolean = {0}}},
		//{OptType_Boolean, "FLASHING LIGHTS", &stage.flashinglights, {.spec_boolean = {0}}},
		//{OptType_Boolean, "CAMERA ZOOMS", &stage.prefs.camerazooms, {.spec_boolean = {0}}}
	};
	
	//if (menu.select == 2 && pad_state.press & (PAD_CROSS | PAD_LEFT | PAD_RIGHT))
	//	stage.pal_i = 1;
	
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
	
}

void Options_Tick()
{
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
