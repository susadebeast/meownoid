#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "entity.h"
#include "menu.h"

#define MAX_SPRITES 128
#define MAX_ENTITIES 512
#define MAX_STARS 32
#define TICRATE 35

int bound(int min, int val, int max);

extern int soundvolume;
extern int musicvolume;
extern int vid_width;
extern int vid_height;
extern int vid_fullscreen;
extern int vid_mode;
extern int windowstyle;
extern std::string playername;

class HUD
{
public:
	HUD();
	HUD(std::string name, Player &plr, int x, int y, sf::Vector2f barsize, sf::Vector2f blsize);
	HUD(std::string name, Player &plr, int x, int y);
	sf::Sprite spr;
	sf::Texture texture;
	std::string texname;
	sf::Font font;
	sf::Text text;

	sf::RectangleShape rect;
	sf::Vector2f blocksize; // size of one bar block (not the whole bar)
	sf::Vector2f pos;
	int value = 0;
	// the player this hud element belongs to (not used?)
	Player *owner = NULL;

	void UpdateElement(int val);
};

class Game
{
public:
	Game();
	Game(int width, int height);
	void RunGameLoop(sf::RenderWindow &window);
	sf::RenderWindow *gwindow;

	sf::Vector2u videomodes[7] = {
		sf::Vector2u(vid_width, vid_height),
		sf::Vector2u(1280, 800),
		sf::Vector2u(1600, 1000),
		sf::Vector2u(1680, 1050),
		sf::Vector2u(1920, 1080),
		sf::Vector2u(2560, 1440),
		sf::Vector2u(3840, 2160),
	};

	Menu menu;
	Player plr;
	Entity *head = NULL;
	Entity *tail = NULL;
	Entity *touchthing = NULL;
	Entity *attackthing = NULL;
	int active_monsters = 0;
	int active_stars = 0;
	int level_time = 0;
	float enemy_spawn_time = TICRATE; // delay between spawning enemies, initially 1 second
	float star_spawn_time = 0;
	bool paused = false;
	bool special_key_released = true;

	HUD hud_health;
	HUD hud_shield;
	HUD hud_bombs;
	HUD hud_score;
	bool hud_init = false;

	int destroyed_spaceblob = 0;
	int destroyed_alienship = 0;
	int destroyed_wideship = 0;
	int destroyed_rockship = 0;
	int destroyed_astesmall = 0;

	int shipslost_spaceblob = 0;
	int shipslost_alienship = 0;
	int shipslost_wideship = 0;
	int shipslost_rockship = 0;
	int shipslost_astesmall = 0;

	Menu::score_t highscore[NUMHIGHSCORES] = {
		{"CuteBeans", 400000},
		{"GhostCat", 200000},
		{"MeowMeow", 100000},
		{"Purrur", 75000},
		{"Hissy", 50000},
		{"Booppon", 25000},
		{"Mittens", 10000},
		{"HairBall", 7500},
		{"Pumpkin", 5000},
		{"Bubbles", 2500},
	};

	// game starts in menu
	// pausemenu is if the game is running, but player pressed esc, game is paused, but also displays the menu
	// pressing pause just pauses the game, but doesn't display menu
	
	typedef enum {
		GS_MENU,
		GS_GAME,
		GS_PAUSEMENU,
	} gamestate_t;
	int gamestate = GS_MENU;

	sf::Music *music;

//private:
	int width;
	int height;
	void MenuStuff();
	void PlayerPlasmaBlast();
	void HandleInput(sf::RenderWindow &window);
	void RespawnPlayer(bool reset);
	void StartNewGame();
	void QuitGame();
	void SpawnEntity(Entity *entity, std::string name, int x, int y);
	void RemoveEntity(Entity *entity);
	void StarsBackground(Entity *entity);
	void HandleGameLogic(Entity *entity);
	void Redraw(sf::RenderWindow &window, Entity *entity);
};