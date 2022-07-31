#define XA_LENGTH(x) (((u64)(x) * 75) / 100 * IO_SECT_SIZE) //Centiseconds to sectors in bytes (w)

typedef struct
{
	XA_File file;
	u32 length;
} XA_TrackDef;

static const XA_TrackDef xa_tracks[] = {
	{FL_Menu, XA_LENGTH(11300)}, //XA_GettinFreaky
	{FL_Menu, XA_LENGTH(3800)},      //XA_GameOver
	{FL_Bopeebo, XA_LENGTH(7700)},       //XA_Bopeebo
	{FL_Fresh, XA_LENGTH(8000)},         //XA_Fresh
	{FL_Dadbattle, XA_LENGTH(8700)},     //XA_Dadbattle
	{FL_Tutorial, XA_LENGTH(6800)},      //XA_Tutorial
	{FL_Blammed, XA_LENGTH(9900)},       //XA_Blammed
};

static const char *xa_paths[] = {
	"\\MUSIC\\MENU.XA;1",     //FL_Menu
	"\\MUSIC\\BOPEEBO.XA;1",  //FL_Bopeebo
	"\\MUSIC\\FRESH.XA;1",    //FL_Fresh
	"\\MUSIC\\DADBATTL.XA;1", //FL_Dadbattle
	"\\MUSIC\\TUTORIAL.XA;1", //FL_Tutorial
	"\\MUSIC\\BLAERECT.XA;1", //FL_Blammed
	NULL,
};

typedef struct
{
	const char *name;
	boolean vocal;
} XA_Mp3;

static const XA_Mp3 xa_mp3s[] = {
	{"freaky", false},   //XA_GettinFreaky
	{"gameover", false}, //XA_GameOver
	{"bopeebo", true},   //XA_Bopeebo
	{"fresh", true},     //XA_Fresh
	{"dadbattle", true}, //XA_Dadbattle
	{"tutorial", false}, //XA_Tutorial
	{"blammed", true},   //XA_Blammed
	
	{NULL, false}
};
