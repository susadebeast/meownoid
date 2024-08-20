#pragma once
#include <SFML/Graphics.hpp>

void M_NewGameMenu();
void M_MainMenu();
void M_SettingsMenu();
void M_RecordsMenu();
void M_HighscoreMenu();
void M_CloseMenu();
void M_QuitGame();

void M_ChangeSoundVolume();
void M_ChangeMusicVolume();
void M_ChangeResolution();
void M_ChangeVideoMode();

void M_NextMonster();

#define NUMMAINMENU 8
#define NUMSETTINGSMENU 7
#define MAXMENUITEMS 10
#define NUMRECORDSITEMS 5
#define NUMHIGHSCORES 10

extern int soundvolume;
extern int musicvolume;
extern int vid_width;
extern int vid_height;
extern int vid_fullscreen;
extern int vid_mode;
extern int windowstyle;
extern std::string playername;

class Menu
{
public:
	Menu();

	sf::RectangleShape background; // menu background dimming
	sf::RectangleShape rect; // menu item highlight
	std::string texname;
	sf::Font font;
	sf::Text text;

	Entity *recordentity;

	int menutime;
	bool menudir;

	int currentitem = 3;
	int menuitems = 6;
	int mainmenu_item = 3;
	int settings_item = 3;

	int records_item = 0;

	void ActivateMenuItem();
	void BackMenu();
	void NextMenuItem(bool dir);

	// function to run with the current menu item
	typedef void (*menuaction_t)();

	// if records menu should have destroyed (enemies) and (player) ship lost counters
	// and each enemy page is a menu item (displayed a bit different) then there should
	// be 2 values per menu item (or some other way to display both)?
	typedef struct {
		std::string itemname;
		int value;
		menuaction_t menuact;
	} menu_t;

	// the various menus
	// the first item is the menu name and is not displayed & not selectable
	// the second item is what happens if player pressed Escape, also not displayed & not selectable
	// the third item is the Displayed Menu name, not selectable
	// -1 means the value part is not displayed
	menu_t mainmenu[NUMMAINMENU] = {
		{"MainMenu", -1, {NULL}},
		{"CloseMenu", -1, {M_CloseMenu}},
		{"=^ MEOWNOID ^=", -1, {NULL}},
		{"   New Game", -1, {M_NewGameMenu}},
		{"   Settings", -1, {M_SettingsMenu}},
		{"   Highscores", -1, {M_HighscoreMenu}},
		{"   Records", -1, {M_RecordsMenu}},
		{"   Quit Game", -1, {M_QuitGame}},
	};

	menu_t settingsmenu[NUMSETTINGSMENU] = {
		{"SettingsMenu", -1, {NULL}},
		{"BackToMain", -1, {M_MainMenu}},
		{"   SETTINGS", -1, {NULL}},
		{"Sound Volume", soundvolume, {M_ChangeSoundVolume}},
		{"Music Volume", musicvolume, {M_ChangeMusicVolume}},
		{"Resolution", 0, {M_ChangeResolution}},
		{"Video Mode", vid_fullscreen, {M_ChangeVideoMode}},
	};

	menu_t recordsmenu[6] = {
		{"RecordsMenu", -1, {NULL}},
		{"BackToMain", -1, {M_MainMenu}},
		{"       RECORDS", -1, {NULL}},
		{"Press Arrow Keys", -1, {M_NextMonster}},
		{"", -1, {NULL}},
		{"DummyText", -1, {NULL}},
	};

	menu_t highscoremenu[5] = {
		{"HighscoreMenu", -1, {NULL}},
		{"BackToMain", -1, {M_MainMenu}},
		{"       HIGHSCORES", -1, {NULL}},
		{"", -1, {NULL}},
		{"DummyText", -1, {NULL}},
	};

	// the current active menu is set to this
	menu_t activemenu[MAXMENUITEMS];

	// records page
	typedef struct {
		//std::string classname;
		std::string pagetext;
		//int destroyed;
		//int shipslost;
	} records_t;

	// highscore list
	typedef struct {
		std::string name;
		int score;
	} score_t;
};

// threat levels:
// Descent has these: None, Minimal, (Low?), Medium, Moderate, High, (Very High?), Extreme

#define INFO_SPACEBLOB \
"Space Blob\n"\
"Monstrous Invasive Life Form\n"\
"Size:          10 - 20 meters\n"\
"Est. Armament: Toxic Surface\n"\
"Threat:        Minimal\n\n"\
"It flies in space looking\n"\
"for other life forms and\n"\
"materials to absorb into\n"\
"itself to become larger.\n"\
"Very large space blobs\n"\
"eventually divide into\n"\
"multiple smaller blobs.\n\n"

#define INFO_ALIENSHIP \
"Purple Alien Ship\n"\
"Small Scout\n"\
"Size:          15 meters\n"\
"Est. Armament: 2 Small Blasters\n"\
"Threat:        Medium\n\n"\
"The first alien ships\n"\
"encountered scouting\n"\
"Meownyan for invasion.\n"\
"Precursor to a more\n"\
"serious threat.\n\n"

#define INFO_WIDESHIP \
"Blue Warship\n"\
"Primary Assault Fleet\n"\
"Size:          30 meters\n"\
"Est. Armament: 2 Medium Laser Cannons\n"\
"Threat:        High\n\n"\
"Equipped with thick armor\n"\
"and rapid fire weapons\n"\
"this ship can cause major\n"\
"damage very quickly. Try\n"\
"to attack from sides to\n"\
"avoid getting hit.\n\n"

#define INFO_ROCKSHIP \
"Asteroid Carrier\n"\
"Mining Materials Carrier\n"\
"Size:          40 meters\n"\
"Est. Armament: Rapid Fire Blaster\n"\
"Threat:        High\n\n"\
"The alien invaders are\n"\
"doing some mining operation\n"\
"in the nearby asteroid belt.\n"\
"Don't let these space pirates\n"\
"get away with our mineral\n"\
"rich asteroids!\n\n"

#define INFO_ASTEROID \
"Small Asteroid\n"\
"Flying Space Rock\n"\
"Size:          10 meters\n"\
"Est. Armament: None\n"\
"Threat:        Minimal\n\n"\
"Piece of a larger asteroid.\n"\
"May sometimes contain valuable\n"\
"minerals. Just don't fly\n"\
"your ship right into them.\n\n"

// also could be?
//"Feline-GX\n"

#define INFO_MEOWDEFENDER \
"Neko-GX\n"\
"Meownyan Defender\n"\
"Size:          20 meters\n"\
"Est. Armament: Lasers + Plasma Blast\n"\
"Threat:        Extreme\n\n"\
"Your ship. Equipped with\n"\
"upgradeable laser cannons,\n"\
"powerful plasma blaster,\n"\
"recharging shield and strong\n"\
"meownium armor.\n\n"

#define INFO_DESTROYED "Destroyed:  "
#define INFO_SHIPSLOST "Ships Lost: "