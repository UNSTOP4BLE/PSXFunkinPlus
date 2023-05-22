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
#include "../../psx/save.h"

#include "../../fonts/font.h"

#include "../stage/stage.h"

//Options state
static struct
{
	u8 main_select, select, next_select, page;
	u8 sinus;
	
	//Controls Variables
	boolean bind, save, load;
	u8 savetext;
	boolean exit;
} options;

static const ButtonStr buttons[] = {
	{PAD_UP,{157, 0, 14, 14}}, //PAD_UP
	{PAD_DOWN,{172, 0, 14, 14}}, //PAD_DOWN
	{PAD_LEFT,{157, 15, 14, 14}}, //PAD_LEFT
	{PAD_RIGHT,{172, 15, 14, 14}}, //PAD_RIGHT
	
	{PAD_TRIANGLE,{138, 17, 15, 15}}, //PAD_TRIANGLE
	{PAD_CIRCLE,{140, 0, 16, 16}}, //PAD_CIRCLE
	{PAD_CROSS,{123, 0, 16, 16}}, //PAD_CROSS
	{PAD_SQUARE,{123, 17, 14, 14}}, //PAD_SQUARE
	
	{PAD_L1,{72, 16, 23, 15}}, //PAD_L1
	{PAD_L2,{72, 0, 24, 15}}, //PAD_L2
	{PAD_R1,{97, 16, 24, 15}}, //PAD_R1
	{PAD_R2,{97, 0, 25, 15}}, //PAD_R2
};

static void Main_Options()
{
	static const char *menu_options[] = {
		"CONTROLS",
		"ADJUST COMBO",
		"GRAPHICS",
		"VISUALS AND UI",
		"GAMEPLAY",
		"MEMORY CARD",
	};
	
	//Handle option and selection
	if (menu.trans_time > 0 && (menu.trans_time -= timer_dt) <= 0)
		Trans_Start();
	
	if (menu.next_page == menu.page && Trans_Idle())
	{
		//Change option
		if (pad_state.press & PAD_UP)
		{
			Audio_PlaySound(menu.sounds[0], 0x3fff);
			if (options.main_select > 0)
				options.main_select--;
			else
				options.main_select = COUNT_OF(menu_options) - 1;
		}
		if (pad_state.press & PAD_DOWN)
		{
			Audio_PlaySound(menu.sounds[0], 0x3fff);
			if (options.main_select < COUNT_OF(menu_options) - 1)
				options.main_select++;
			else
				options.main_select = 0;
		}
		
		//Select option if cross is pressed
		if (pad_state.press & (PAD_START | PAD_CROSS))
		{
			Audio_PlaySound(menu.sounds[0], 0x3fff);
			options.page = options.main_select + 1;
			menu.select = 0;
			menu.scroll = 0;
			if(options.main_select == 0)
				options.bind = false;
		}
		
		//Return to main menu if circle is pressed
		if (stage.prefs.autosave)
		{
			if ((options.savetext == 0) && options.exit)
			{
				options.exit = false;
				menu.next_page = MenuPage_Main;
				menu.next_select = 3; //Options
				Audio_PlaySound(menu.sounds[2], 0x3fff);
				Trans_Start();
			}
			if (pad_state.press & PAD_CIRCLE)
			{
				Save_Card();
				options.exit = true;
			}
		}
		else
		{
			if (pad_state.press & PAD_CIRCLE)
			{
				Audio_PlaySound(menu.sounds[2], 0x3fff);
				menu.next_page = MenuPage_Main;
				menu.next_select = 3; //Options
				Trans_Start();
			}
		}
	}

	//Draw all options
	for (u8 i = 0; i < COUNT_OF(menu_options); i++)
	{
		fonts.font_bold.draw_col(&fonts.font_bold,
			menu_options[i],
			SCREEN_WIDTH2,
			SCREEN_HEIGHT2 + (i * 24) - 70,
			FontAlign_Center,
			(i == options.main_select) ? 128 : 100,
			(i == options.main_select) ? 128 : 100,
			(i == options.main_select) ? 128 : 100
		);
	}
}

static void Controls()
{
	static const char *keys[] = {
		"LEFT",
		"DOWN",
		"UP",
		"RIGHT"
	};
	
	//Change option
	if(!options.bind)
	{
		if (pad_state.press & PAD_UP)
		{
			Audio_PlaySound(menu.sounds[0], 0x3fff);
			if (menu.select > 0)
				menu.select--;
			else
				menu.select = 3;
		}
		if (pad_state.press & PAD_DOWN)
		{
			Audio_PlaySound(menu.sounds[0], 0x3fff);
			if (menu.select < 3)
				menu.select++;
			else
				menu.select = 0;
		}
		if (pad_state.press & (PAD_CROSS | PAD_START))
		{
			Audio_PlaySound(menu.sounds[0], 0x3fff);
			options.bind = true;
		}
		
		if (pad_state.press & PAD_CIRCLE)
		{
			Audio_PlaySound(menu.sounds[2], 0x3fff);
			options.page = 0;
			options.main_select = 0;
		}
	}
	else if(pad_state.press && !(pad_state.press & (PAD_START | PAD_SELECT)))
	{
		Audio_PlaySound(menu.sounds[1], 0x3fff);
		stage.prefs.control_keys[menu.select] = pad_state.press;
		options.bind = false;
	}
	
	//Draw options
	s32 next_scroll = menu.select * FIXED_DEC(24,1);
	menu.scroll += (next_scroll - menu.scroll) >> 4;
	
	for (u8 i = 0; i < COUNT_OF(keys); i++)
	{	
		//Get position on screen
		s32 y = (i * 24) - 8 - (menu.scroll >> FIXED_SHIFT);
		if (y <= -SCREEN_HEIGHT2 - 8)
			continue;
		if (y >= SCREEN_HEIGHT2 + 8)
			break;
		
		//Draw text
		fonts.font_bold.draw_col(&fonts.font_bold,
			keys[i],
			SCREEN_WIDTH2 - 80,
			SCREEN_HEIGHT2 + y - 8,
			FontAlign_Left,
			(i == menu.select) ? 128 : 100,
			(i == menu.select) ? 128 : 100,
			(i == menu.select) ? 128 : 100
		);
		
		//Draw key
		if (!((i == menu.select) && options.bind))
		{
			u8 order;
			for (order = 0; order < COUNT_OF(buttons); order++)
			{
				if(stage.prefs.control_keys[i] == buttons[order].key)
					break;
			}
			RECT button_src = {buttons[order].src[0], buttons[order].src[1], buttons[order].src[2], buttons[order].src[3]};
			Gfx_BlitTex(&menu.tex_options,
				&button_src,
				SCREEN_WIDTH2 + 40 - (button_src.w / 2),
				SCREEN_HEIGHT2 + y - 8
			);
		}
	}
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
	
	if (pad_state.press & PAD_CIRCLE)
	{
		Audio_PlaySound(menu.sounds[2], 0x3fff);
		options.page = 0;
		options.main_select = 1;
	}
	
	RECT combo_src = {0, 0, 71, 39};
	Gfx_BlitTex(&menu.tex_options,
		&combo_src,
		SCREEN_WIDTH2 + stage.prefs.combox - 19,
		SCREEN_HEIGHT2 + stage.prefs.comboy - 64
	);
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
	
	//Handle option and selection
	if (menu.next_page == menu.page && Trans_Idle())
	{
		//Change option
		if (pad_state.press & PAD_UP)
		{
			Audio_PlaySound(menu.sounds[0], 0x3fff);
			if (menu.select > 0)
				menu.select--;
			else
				menu.select = COUNT_OF(menu_options) - 1;
		}
		if (pad_state.press & PAD_DOWN)
		{
			Audio_PlaySound(menu.sounds[0], 0x3fff);
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
				{
					Audio_PlaySound(menu.sounds[0], 0x3fff);
					*((boolean*)menu_options[menu.select].value) ^= 1;
				}
				break;
			case OptType_Enum:
				if (pad_state.press & PAD_LEFT)
				{
					if (--*((s32*)menu_options[menu.select].value) < 0)
						*((s32*)menu_options[menu.select].value) = menu_options[menu.select].spec.spec_enum.max - 1;
					Audio_PlaySound(menu.sounds[0], 0x3fff);
				}
				if (pad_state.press & PAD_RIGHT)
				{
					if (++*((s32*)menu_options[menu.select].value) >= menu_options[menu.select].spec.spec_enum.max)
						*((s32*)menu_options[menu.select].value) = 0;
					Audio_PlaySound(menu.sounds[0], 0x3fff);
				}
				break;
		}
		
		//Return to main menu if circle is pressed
		if (pad_state.press & PAD_CIRCLE)
		{
			options.page = 0;
			options.main_select = 2;
			Audio_PlaySound(menu.sounds[2], 0x3fff);
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
	
	//Handle option and selection
	if (menu.next_page == menu.page && Trans_Idle())
	{
		//Change option
		if (pad_state.press & PAD_UP)
		{
			Audio_PlaySound(menu.sounds[0], 0x3fff);
			if (menu.select > 0)
				menu.select--;
			else
				menu.select = COUNT_OF(menu_options) - 1;
		}
		if (pad_state.press & PAD_DOWN)
		{
			Audio_PlaySound(menu.sounds[0], 0x3fff);
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
				{
					Audio_PlaySound(menu.sounds[0], 0x3fff);
					*((boolean*)menu_options[menu.select].value) ^= 1;
				}
				break;
			case OptType_Enum:
				if (pad_state.press & PAD_LEFT)
				{
					if (--*((s32*)menu_options[menu.select].value) < 0)
						*((s32*)menu_options[menu.select].value) = menu_options[menu.select].spec.spec_enum.max - 1;
					Audio_PlaySound(menu.sounds[0], 0x3fff);
				}
				if (pad_state.press & PAD_RIGHT)
				{
					if (++*((s32*)menu_options[menu.select].value) >= menu_options[menu.select].spec.spec_enum.max)
						*((s32*)menu_options[menu.select].value) = 0;
					Audio_PlaySound(menu.sounds[0], 0x3fff);
				}
				break;
		}
		
		//Return to main menu if circle is pressed
		if (pad_state.press & PAD_CIRCLE)
		{
			Audio_PlaySound(menu.sounds[2], 0x3fff);
			options.page = 0;
			options.main_select = 3;
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
	
	if (stage.prefs.mode == 2)
	{
		if ((menu.select > 1) && (menu.select < 4))
			fonts.font_cdr.draw(&fonts.font_cdr,
				"This option cannot be changed during multiplayer mode.",
				SCREEN_WIDTH2 + 20,
				SCREEN_HEIGHT - 20,
				FontAlign_Center
			);
		stage.prefs.middlescroll = false;
		stage.prefs.opponentnotes = true;
	}
	
	//Handle option and selection
	if (menu.next_page == menu.page && Trans_Idle())
	{
		//Change option
		if (pad_state.press & PAD_UP)
		{
			Audio_PlaySound(menu.sounds[0], 0x3fff);
			if (menu.select > 0)
				menu.select--;
			else
				menu.select = COUNT_OF(menu_options) - 1;
		}
		if (pad_state.press & PAD_DOWN)
		{
			Audio_PlaySound(menu.sounds[0], 0x3fff);
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
				{
					Audio_PlaySound(menu.sounds[0], 0x3fff);
					*((boolean*)menu_options[menu.select].value) ^= 1;
				}
				break;
			case OptType_Enum:
				if (pad_state.press & PAD_LEFT)
				{
					if (--*((s32*)menu_options[menu.select].value) < 0)
						*((s32*)menu_options[menu.select].value) = menu_options[menu.select].spec.spec_enum.max - 1;
					Audio_PlaySound(menu.sounds[0], 0x3fff);
				}
				if (pad_state.press & PAD_RIGHT)
				{
					if (++*((s32*)menu_options[menu.select].value) >= menu_options[menu.select].spec.spec_enum.max)
						*((s32*)menu_options[menu.select].value) = 0;
					Audio_PlaySound(menu.sounds[0], 0x3fff);
				}
				break;
		}
		
		//Return to main menu if circle is pressed
		if (pad_state.press & PAD_CIRCLE)
		{
			Audio_PlaySound(menu.sounds[2], 0x3fff);
			options.page = 0;
			options.main_select = 4;
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
	
	if (menu.next_page == menu.page && Trans_Idle())
	{
		//Change option
		if (pad_state.press & PAD_UP)
		{
			Audio_PlaySound(menu.sounds[0], 0x3fff);
			if (menu.select > 0)
				menu.select--;
			else
				menu.select = COUNT_OF(menu_options) - 1;
		}
		if (pad_state.press & PAD_DOWN)
		{
			Audio_PlaySound(menu.sounds[0], 0x3fff);
			if (menu.select < COUNT_OF(menu_options) - 1)
				menu.select++;
			else
				menu.select = 0;
		}
		
		if((menu.select == 2) && (pad_state.press & (PAD_LEFT | PAD_RIGHT)))
		{
			stage.prefs.autosave = (stage.prefs.autosave) ? false : true;
			Audio_PlaySound(menu.sounds[0], 0x3fff);
		}
		
		//Select option if cross is pressed
		if (pad_state.press & (PAD_START | PAD_CROSS))
		{
			Audio_PlaySound(menu.sounds[0], 0x3fff);
			switch (menu.select)
			{
				case 0:
					Save_Card();
					break;
				case 1:
					Load_Card();
					break;
				case 3:
					defaultSettings();
					break;
			}
		}
		
		//Return to main menu if circle is pressed
		if (pad_state.press & PAD_CIRCLE)
		{
			options.page = 0;
			options.main_select = 5;
			Audio_PlaySound(menu.sounds[2], 0x3fff);
		}
	}
			
	//Draw options
	s32 next_scroll = menu.select * FIXED_DEC(24,1);
	menu.scroll += (next_scroll - menu.scroll) >> 4;
	
	//Draw all options
	for (u8 i = 0; i < COUNT_OF(menu_options); i++)
	{
		s32 y = (i * 24) - 8 - (menu.scroll >> FIXED_SHIFT);
		if (y <= -SCREEN_HEIGHT2 - 8)
			continue;
		if (y >= SCREEN_HEIGHT2 + 8)
			break;
		
		if(i == 2)
		{
			char text[0x80];
			sprintf(text, "%s %s", menu_options[i], (stage.prefs.autosave) ? "ON" : "OFF");
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
		else
			fonts.font_bold.draw_col(&fonts.font_bold,
				menu_options[i],
				SCREEN_WIDTH2 - 120,
				SCREEN_HEIGHT2 + y - 8,
				FontAlign_Left,
				(i == menu.select) ? 128 : 100,
				(i == menu.select) ? 128 : 100,
				(i == menu.select) ? 128 : 100
			);
	}
}

void Options_Tick()
{
	if(options.savetext == 2)
	{
		if (options.load)
		{
			options.load = false;
			readSaveFile();
		}
		if (options.save)
		{
			options.save = false;
			writeSaveFile();
		}
	}
	if(options.savetext > 0)
	{
		options.savetext--;
		RECT save_src = {197, 0, 18, 18};
		Gfx_BlitTex(&menu.tex_options, &save_src, SCREEN_WIDTH - 36, SCREEN_HEIGHT - 36);
	}
	
	//Initialize page
	if (menu.page_swap)
		options.page = 0;
	
	switch (options.page)
	{
		case 0:
			Main_Options();
			break;
		case 1:
			Controls();
			break;
		case 2:
			Adjust_Combo();
			break;
		case 3:
			Graphics();
			break;
		case 4:
			Visuals_And_UI();
			break;
		case 5:
			Gameplay();
			break;
		case 6:
			Memory_Card();
			break;
	}
}

void Load_Card()
{
	options.savetext = 4;
	options.load = true;
}

void Save_Card()
{
	options.savetext = 4;
	options.save = true;
}
