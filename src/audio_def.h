#define XA_LENGTH(x) (((u64)(x) * 75) / 100 * IO_SECT_SIZE) //Centiseconds to sectors in bytes (w)

typedef struct
{
	XA_File file;
	u32 length;
} XA_TrackDef;

static const XA_TrackDef xa_tracks[] = {
	//MENU.XA
	{XA_Menu, XA_LENGTH(14034)}, //XA_GettinFreaky
	{XA_Menu, XA_LENGTH(3800)},  //XA_GameOver
	
	{XA_1, XA_LENGTH(13076)},
	{XA_1, XA_LENGTH(12599)},
	
	{XA_2, XA_LENGTH(13019)},
	{XA_2, XA_LENGTH(14624)},
};

static const char *xa_paths[] = {
	"\\MUSIC\\MENU.XA;1",   //XA_Menu
	"\\MUSIC\\1.XA;1",
	"\\MUSIC\\2.XA;1",
	NULL,
};

typedef struct
{
	const char *name;
	boolean vocal;
} XA_Mp3;

static const XA_Mp3 xa_mp3s[] = {
	//MENU.XA
	{"freakymenu", false},   //XA_GettinFreaky
	{"gameover", false}, //XA_GameOver
	
	{"shotgun-shell", true},
	{"parasite", true},
	
	{"godrays", true},
	{"promenade", true},
	
	{NULL, false}
};
