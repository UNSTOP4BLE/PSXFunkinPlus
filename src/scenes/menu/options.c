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

//Internal menu functions
char options_text_buffer[0x100];

static const char *Options_LowerIf(const char *text, boolean lower)
{
	//Copy text
	char *dstp = options_text_buffer;
	if (lower)
	{
		for (const char *srcp = text; *srcp != '\0'; srcp++)
		{
			if (*srcp >= 'A' && *srcp <= 'Z')
				*dstp++ = *srcp | 0x20;
			else
				*dstp++ = *srcp;
		}
	}
	else
	{
		for (const char *srcp = text; *srcp != '\0'; srcp++)
		{
			if (*srcp >= 'a' && *srcp <= 'z')
				*dstp++ = *srcp & ~0x20;
			else
				*dstp++ = *srcp;
		}
	}
	
	//Terminate text
	*dstp++ = '\0';
	return options_text_buffer;
}

static void Preferences()
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
		{OptType_Boolean, "GHOST TAP ", &stage.prefs.ghost, {.spec_boolean = {0}}},
		{OptType_Boolean, "DOWNSCROLL", &stage.prefs.downscroll, {.spec_boolean = {0}}},
		{OptType_Boolean, "MIDDLESCROLL", &stage.prefs.middlescroll, {.spec_boolean = {0}}},
		{OptType_Boolean, "INSTAKILL", &stage.instakill, {.spec_boolean = {0}}},
		{OptType_Boolean, "CAM FOLLOW CHAR", &stage.prefs.followcamera, {.spec_boolean = {0}}},
		{OptType_Boolean, "PAL REFRESH RATE", &stage.prefs.palmode, {.spec_boolean = {0}}}
	};
	
	if (menu.select == 6 && pad_state.press & (PAD_CROSS | PAD_LEFT | PAD_RIGHT))
		stage.pal_i = 1;
	
	//Initialize page
	if (menu.page_swap)
		menu.scroll = COUNT_OF(menu_options) * FIXED_DEC(24 + SCREEN_HEIGHT2,1);
	
	//Draw page label
	fonts.font_bold.draw(&fonts.font_bold,
		"OPTIONS",
		16,
		SCREEN_HEIGHT - 32,
		FontAlign_Left
	);
	
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
			menu.next_page = MenuPage_Main;
			menu.next_select = 3; //Options
			Trans_Start();
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
		fonts.font_bold.draw(&fonts.font_bold,
			Options_LowerIf(text, menu.select != i),
			48 + (y >> 2),
			SCREEN_HEIGHT2 + y - 8,
			FontAlign_Left
		);
	}
}

void Options_Tick()
{
	Preferences();
}
