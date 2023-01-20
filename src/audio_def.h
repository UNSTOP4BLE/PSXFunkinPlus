#define XA_LENGTH(x) (((u64)(x) * 75) / 100 * IO_SECT_SIZE) //Centiseconds to sectors in bytes (w)

typedef struct
{
	XA_File file;
	u32 length;
} XA_TrackDef;

static const XA_TrackDef xa_tracks[] = {
	//MENU.XA
	{XA_Menu, XA_LENGTH(11300)}, //XA_GettinFreaky
	{XA_Menu, XA_LENGTH(3800)},  //XA_GameOver
	
	{XA_Exhila, XA_LENGTH(7700)},
	{XA_Exhila, XA_LENGTH(8000)},
	
	{XA_Reckle, XA_LENGTH(8700)},
	{XA_Reckle, XA_LENGTH(6800)},
	
	{XA_Unempl, XA_LENGTH(11771)},
	{XA_Unempl, XA_LENGTH(6800)},
	
	{XA_Superc, XA_LENGTH(8700)},
	{XA_Superc, XA_LENGTH(6800)},
	
	{XA_Temper, XA_LENGTH(8700)},
	{XA_Temper, XA_LENGTH(6800)},
	
	{XA_Bonus3, XA_LENGTH(11915)},
	{XA_Bonus3, XA_LENGTH(12886)},
};

static const char *xa_paths[] = {
	"\\MUSIC\\MENU.XA;1",   //XA_Menu
	"\\MUSIC\\EXHILA.XA;1",
	"\\MUSIC\\RECKLE.XA;1",
	"\\MUSIC\\UNEMPL.XA;1",
	"\\MUSIC\\SUPERC.XA;1",
	"\\MUSIC\\TEMPER.XA;1",
	"\\MUSIC\\BONUS3.XA;1",
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
	
	{"exhilaration", true},
	{"exhilarationerect", true},
	
	{"reckless", true},
	{"recklesserect", true},
	
	{"unemployed", true},
	{"unemployederect", true},
	
	{"supercharge", true},
	{"superchargeerect", true},
	
	{"temper-break", true},
	{"temper-breakerect", true},
	
	{"cookies", true},
	{"new-management", true},
	
	{NULL, false}
};
