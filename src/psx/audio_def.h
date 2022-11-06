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
	//WEEK1A.XA
	{XA_Week1A, XA_LENGTH(7700)}, //XA_Bopeebo
	{XA_Week1A, XA_LENGTH(8000)}, //XA_Fresh
	//WEEK1B.XA
	{XA_Week1B, XA_LENGTH(8700)}, //XA_Dadbattle
	{XA_Week1B, XA_LENGTH(6800)}, //XA_Tutorial
	//BONUS1.XA
	{XA_Bonus1, XA_LENGTH(13143)}, //XA_LordX
	{XA_Bonus1, XA_LENGTH(15148)}, //XA_Lullaby
	//BONUS2.XA
	{XA_Bonus2, XA_LENGTH(22375)}, //XA_Bygone_Purpose
};

static const char *xa_paths[] = {
	"\\MUSIC\\MENU.XA;1",   //XA_Menu
	"\\MUSIC\\WEEK1A.XA;1", //XA_Week1A
	"\\MUSIC\\WEEK1B.XA;1", //XA_Week1B
	"\\MUSIC\\BONUS1.XA;1", //XA_Bonus1
	"\\MUSIC\\BONUS2.XA;1", //XA_Bonus2
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
	//WEEK1A.XA
	{"bopeebo", true}, //XA_Bopeebo
	{"fresh", true},   //XA_Fresh
	//WEEK1B.XA
	{"dadbattle", true}, //XA_Dadbattle
	{"tutorial", false}, //XA_Tutorial
	//BONUS1.XA
	{"lordx", true}, //XA_LordX
	{"lullaby", true}, //XA_Lullaby
	//BONUS2.XA
	{"bygone_purpose", true}, //XA_LordX
	
	{NULL, false}
};
