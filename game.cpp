#include <time.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "game.h"

extern Game game;

HUD::HUD() { };

// maybe the barsize/rect are unnecessary
HUD::HUD(std::string name, Player &plr, int x, int y, sf::Vector2f barsize, sf::Vector2f blsize)
{
	pos.x = (float)x;
	pos.y = (float)y;
	rect.setSize(barsize);
	blocksize = blsize;
	rect.setPosition(pos);
	spr.setPosition(pos);
	texname = "sprites/" + name + ".png";
	owner = &plr;
};

HUD::HUD(std::string name, Player &plr, int x, int y)
{
	pos.x = (float)x;
	pos.y = (float)y;
	text.setPosition(pos);
	texname = "fonts/" + name + ".ttf";
	owner = &plr;
};

void HUD::UpdateElement(int val)
{
	value = val;
}

Game::Game(int width, int height)
{
	this->width = width;
	this->height = height;
}

Game::Game() { };

// should there be a menu class?
void Game::MenuStuff()
{
	// the menu could also just use the entities?
	// though if there were lots of entities already active in the game
	// the pause menu wouldn't work right, or the menu entities are never removed/reset?
	menu.menutime --;
	menu.recordentity->BasePhysics(menu.recordentity);
}

// to allow player firing the plasma blast when had 5 of them
// and collected one from the level
// or should this be in the player class?
void Game::PlayerPlasmaBlast()
{
	int rx;
	plr.aimang = -90 - 75;
	for (rx = 0; rx < 21; rx++) {
		SpawnEntity(head, "plasma", plr.pos.x + 6, plr.pos.y);
		plr.aimang += 7.5f;
	}
	plr.bomb_time = 35;
	plr.PlaySound("plasma_fire", 1);
}

void Game::RespawnPlayer(bool reset)
{
	int i, j, k;
	j = -1;
	Menu::score_t tempscore[NUMHIGHSCORES+1];
	// reset the game for a new game when going to the menu
	if(plr.lives <= 0 && !reset) {
		// check if player made a new highscore
		for(i=0; i<NUMHIGHSCORES; i++) {
			tempscore[i].name = highscore[i].name;
			tempscore[i].score = highscore[i].score;
			if(plr.score > highscore[i].score) {
				j = i;
				tempscore[i].name = playername;
				tempscore[i].score = plr.score;
				break;
			}
		}
		if(j != -1) {
			for(i=j; i<NUMHIGHSCORES; i++) {
				k = i+1;
				tempscore[k].name = highscore[i].name;
				tempscore[k].score = highscore[i].score;
			}
			for(i=0; i<NUMHIGHSCORES; i++) {
				highscore[i].name = tempscore[i].name;
				highscore[i].score = tempscore[i].score;
			}
		}


		Entity *entity = head;
		do {
			// stars can continue to exist
			if(entity->freed == true || entity->classtype == "stars" || entity->classname == "catworld")
				continue;
			RemoveEntity(entity);
		} while(entity = entity->next, entity->next->classname != "catworld");
		active_monsters = 0;
		level_time = 0;
		star_spawn_time = 0;
		enemy_spawn_time = TICRATE;
		gamestate = GS_MENU;
		return;
	}
	// player had some extra lives, respawn, but decrease some weapons
	if(!reset) {
		plr.lives--;
		plr.bombs--;
		if(plr.bombs < 2)
			plr.bombs = 2;
		plr.laserlevel -= 2;
		if(plr.laserlevel < 0)
			plr.laserlevel = 0;
		plr.items -= (plr.items & plr.IT_QUADLASER);
	}
	// starting a new game
	else {
		plr.lives = 1;
		plr.score = 0;
		plr.bombs = 2;
		plr.items = 0;
		plr.laserlevel = 0;
	}
	plr.health = 10;
	plr.shield = 5;
	plr.damage = 2;
	plr.texname = "catship";
	plr.sprframe = 0;
	plr.velocity = sf::Vector2f(0, 0);
	plr.pos = sf::Vector2f(width / 2 - 48, height - 104);
}

void Game::StartNewGame()
{
	Entity *entity = head;
	do {
		// stars can continue to exist
		if(entity->freed == true || entity->classtype == "stars" || entity->classname == "catworld")
			continue;
		RemoveEntity(entity);
	} while(entity = entity->next, entity->next->classname != "catworld");
	active_monsters = 0;
	level_time = 0;
	star_spawn_time = 0;
	enemy_spawn_time = TICRATE;

	plr.lives = 1;
	plr.score = 0;
	plr.bombs = 2;
	plr.items = 0;
	plr.laserlevel = 0;

	plr.health = 10;
	plr.shield = 5;
	plr.damage = 2;
	plr.texname = "catship";
	plr.sprframe = 0;
	plr.velocity = sf::Vector2f(0, 0);
	plr.pos = sf::Vector2f(width / 2 - 48, height - 104);
}

void Game::QuitGame()
{
	int i;
	std::ofstream cfgfile ("meownoid.cfg");
	if (cfgfile.is_open()) {
		cfgfile << "musicvolume " << musicvolume << "\n";
		cfgfile << "soundvolume " << soundvolume << "\n";
		cfgfile << "vid_width " << vid_width << "\n";
		cfgfile << "vid_height " << vid_height << "\n";
		cfgfile << "vid_mode " << vid_mode << "\n";
		cfgfile << "vid_fullscreen " << vid_fullscreen << "\n";
		cfgfile << "playername " << playername << "\n";
		cfgfile.close();
	}
	// write the new data
	std::ofstream datanew("meownoid_data.bin", std::ios::out | std::ios::binary);
	// highscores
	for(i=0; i<NUMHIGHSCORES; i++) {
		//datanew.std::ofstream::write((char *)&game.highscore[i].name, sizeof(game.highscore[i].name));
		datanew.std::ofstream::write(game.highscore[i].name.std::string::c_str(), game.highscore[i].name.std::string::size()); // write string to binary file
		datanew.std::ofstream::write("\0",sizeof(char)); // null end string for easier reading
		datanew.std::ofstream::write((char *)&game.highscore[i].score, sizeof(game.highscore[i].score));
	}
	// monsters
	datanew.std::ofstream::write((char *)&game.destroyed_spaceblob, sizeof(game.destroyed_spaceblob));
	datanew.std::ofstream::write((char *)&game.destroyed_alienship, sizeof(game.destroyed_alienship));
	datanew.std::ofstream::write((char *)&game.destroyed_wideship, sizeof(game.destroyed_wideship));
	datanew.std::ofstream::write((char *)&game.destroyed_rockship, sizeof(game.destroyed_rockship));
	datanew.std::ofstream::write((char *)&game.destroyed_astesmall, sizeof(game.destroyed_astesmall));
	datanew.std::ofstream::write((char *)&game.shipslost_spaceblob, sizeof(game.shipslost_spaceblob));
	datanew.std::ofstream::write((char *)&game.shipslost_alienship, sizeof(game.shipslost_alienship));
	datanew.std::ofstream::write((char *)&game.shipslost_wideship, sizeof(game.shipslost_wideship));
	datanew.std::ofstream::write((char *)&game.shipslost_rockship, sizeof(game.shipslost_rockship));
	datanew.std::ofstream::write((char *)&game.shipslost_astesmall, sizeof(game.shipslost_astesmall));
	datanew.std::ofstream::close();

	gwindow->close();
}

void Game::HandleInput(sf::RenderWindow &window)
{
	int rx;

	sf::Event event;
	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			QuitGame();
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
		QuitGame();
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::F10))
		QuitGame();
	if(gamestate != GS_MENU) {
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Pause)) {
			if(special_key_released == true)
				paused = !paused;
			special_key_released = false;
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
			if(special_key_released == true) {
				if(gamestate == GS_GAME)
					gamestate = GS_PAUSEMENU;
				else if(gamestate == GS_PAUSEMENU) {
					//gamestate = GS_GAME;
					//paused = false;
					menu.BackMenu();
				}
			}
			special_key_released = false;
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
			if(plr.health <= 0)
				special_key_released = false;
		}
		else
			special_key_released = true;
	}
	// in the menu
	if(gamestate != GS_GAME) {
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
			if(special_key_released == true) {
				menu.ActivateMenuItem();
			}
			special_key_released = false;
		}
		else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
			if(special_key_released == true) {
				menu.BackMenu();
			}
			special_key_released = false;
		}
		else
			special_key_released = true;
		if(menu.activemenu[0].itemname == "RecordsMenu") {
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
				menu.NextMenuItem(false);
				menu.ActivateMenuItem();
			}
			else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
				menu.NextMenuItem(true);
				menu.ActivateMenuItem();
			}
			else
				menu.menutime = 0;
		}
		else {
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
				menu.NextMenuItem(false);
			else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
				menu.NextMenuItem(true);
			else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
				menu.menudir = false;
				menu.ActivateMenuItem();
			}
			else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
				menu.menudir = true;
				menu.ActivateMenuItem();
			}
			else
				menu.menutime = 0;
		}
	}
	// if the game is in menu or paused, the arrow etc keys do menu stuff or nothing
	if (paused || gamestate != GS_GAME)
		return;
	// pressing keys
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		plr.PlayerMove(plr, sf::Vector2f(-1, 0));
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		plr.PlayerMove(plr, sf::Vector2f(1, 0));
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		plr.PlayerMove(plr, sf::Vector2f(0, -1));
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		plr.PlayerMove(plr, sf::Vector2f(0, 1));
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
		if (plr.attack_time <= 0 && plr.health > 0) {
			rx = (rand() % 4)*2;
			SpawnEntity(head, "laser", plr.pos.x - 6 + rx, plr.pos.y - 12);
			SpawnEntity(head, "laser", plr.pos.x + 22 - rx, plr.pos.y - 12);
			if(plr.items & plr.IT_QUADLASER) {
				plr.PlaySound("laser_quad_fire", 0);
				SpawnEntity(head, "laser", plr.pos.x - 10 - rx, plr.pos.y - 6);
				SpawnEntity(head, "laser", plr.pos.x + 26 + rx, plr.pos.y - 6);
			}
			else
				plr.PlaySound("laser_fire", 0);
			plr.attack_time = 4;

			
		}
		if (plr.health <= 0 && plr.walk_frame >= 20 && plr.attack_released == true)
			RespawnPlayer(false);
		plr.attack_released = false;
	}
	else
		plr.attack_released = true;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::RShift)) {
		if (plr.bomb_time <= 0 && plr.health > 0 && plr.bombs > 0) {
			plr.bombs--;
			PlayerPlasmaBlast();
		}
		if (plr.health <= 0 && plr.walk_frame >= 20 && plr.bomb_released == true)
			RespawnPlayer(false);
		plr.bomb_released = false;
	}
	else
		plr.bomb_released = true;

}

// should there be remove/spawn in the Entity class too?
// remove is resetting every variable and setting freed to true
// if there's any weirdness when spawning new entities, check that everything has been reset here!
void Game::RemoveEntity(Entity *entity)
{
	// just in case this would happen, don't remove/reset the catworld entity
	if(entity->classname == "catworld")
		return;
	// don't remove the recordentity
	if(entity == menu.recordentity)
		return;
	//std::cout << "Entity num: " << entity->num << " :: " << entity->classname << " removed\n";
	entity->freed = true;
	entity->classname = "";
	entity->classtype = "";
	entity->flags = 0;
	entity->health = 0;
	entity->shield = 0;
	entity->damage = 0;
	entity->score = 0;
	entity->velocity = sf::Vector2f(0, 0);
	entity->pos = sf::Vector2f(-100, -100);
	entity->rect.setSize(sf::Vector2f(0, 0));
	entity->rendrect = sf::Vector2f(0, 0);
	entity->spr.setOrigin(0, 0);
	entity->texname = "";
	entity->sprframe = 0;
	entity->anim_time = 0;
	entity->attack_time = 0;
	entity->walk_frame = 0;
}

// could also add a SpawnMissile/Bullet code separate from spawning other entities
void Game::SpawnEntity(Entity *entity, std::string name, int x, int y)
{
	float r, len;
	sf::Vector2f aimvec;

	do {
		if ((entity->freed == false || entity->classname == "catworld") && entity != menu.recordentity)
			continue;
		entity->freed = false;
		entity->pos.x = (float)x;
		entity->pos.y = (float)y;
		entity->classname = name;

		if (name == "stars") {
			entity->texname = "stars";
			entity->classtype = "stars";
			entity->rect.setSize(sf::Vector2f(8.0f, 8.0f));
			entity->rendrect = entity->rect.getSize();
			entity->rect.setFillColor(sf::Color::White);
			entity->rect.setPosition(entity->pos);
			entity->bbox = entity->rect.getGlobalBounds();

			entity->health = 1;
			entity->speed = 2.0f + rand() % 4 + (rand() % 3) * 0.1f;
			entity->walk_frame = (rand() % 8) * 2;

			entity->BaseMove(entity, sf::Vector2f(0.0f, 1.0f));

			active_stars++;
		}
		if (name == "meowdefender") {
			entity->texname = "catship";
			entity->classtype = "monster";
			entity->rect.setSize(sf::Vector2f(48.0f, 48.0f));
			entity->rendrect = entity->rect.getSize();
			entity->rect.setFillColor(sf::Color::Red);
			entity->rect.setPosition(entity->pos);
			entity->bbox = entity->rect.getGlobalBounds();

			entity->health = 1;
			entity->speed = 0;
			entity->damage = 2;
			entity->score = entity->health * 25;
			entity->sprframe = rand() % 2;
		}
		if (name == "spaceblob") {
			entity->texname = "sblob";
			entity->classtype = "monster";
			entity->rect.setSize(sf::Vector2f(48.0f, 48.0f));
			entity->rendrect = entity->rect.getSize();
			entity->rect.setFillColor(sf::Color::Red);
			entity->rect.setPosition(entity->pos);
			entity->bbox = entity->rect.getGlobalBounds();

			entity->health = 3 + rand() % 2 + rand()%(1+plr.laserlevel);
			entity->speed = 4.0f + rand()%2;
			entity->damage = 2;
			entity->score = entity->health * 25;
			entity->sprframe = rand() % 4;

			r = rand() % 21;
			r *= 0.1f;
			entity->BaseMove(entity, sf::Vector2f(-1.0f + r, 1.0f));

			active_monsters++;
			//std::cout << "Enemy appeared: " << entity->num << "\n";
		}
		if (name == "alienship") {
			entity->texname = "aship";
			entity->classtype = "monster";
			entity->rect.setSize(sf::Vector2f(48.0f, 48.0f));
			entity->rendrect = entity->rect.getSize();
			entity->rect.setFillColor(sf::Color::Red);
			entity->rect.setPosition(entity->pos);
			entity->bbox = entity->rect.getGlobalBounds();

			entity->health = 2 + rand() % 2 + rand()%(1+plr.laserlevel);
			entity->speed = 2.0f + rand() % 2;
			entity->damage = 1;
			entity->score = entity->health * 50;
			entity->sprframe = rand() % 2;
			entity->attack_time = 20 + rand() % 20;

			r = rand() % 41;
			r *= 0.1f;
			entity->BaseMove(entity, sf::Vector2f(-2.0f + r, 0.75f));

			active_monsters++;
			//std::cout << "Enemy appeared: " << entity->num << "\n";
		}
		if (name == "wideship") {
			entity->texname = "wship";
			entity->classtype = "monster";
			entity->rect.setSize(sf::Vector2f(64.0f, 48.0f));
			entity->rendrect = entity->rect.getSize();
			entity->rect.setFillColor(sf::Color::Red);
			entity->rect.setPosition(entity->pos);
			entity->bbox = entity->rect.getGlobalBounds();

			entity->health = 6 + rand() % 3 + rand()%(1+plr.laserlevel);
			entity->speed = 2.0f + rand() % 2;
			entity->damage = 2;
			entity->score = entity->health * 75;
			entity->sprframe = rand() % 2;
			entity->attack_time = 10 + rand() % 20;

			r = rand() % 41;
			r *= 0.1f;
			entity->BaseMove(entity, sf::Vector2f(-2.0f + r, 1.0f));

			active_monsters++;
			//std::cout << "Enemy appeared: " << entity->num << "\n";
		}

		if (name == "rockship") {
			entity->texname = "rship";
			entity->classtype = "monster";
			entity->rect.setSize(sf::Vector2f(64, 64));
			entity->rendrect = entity->rect.getSize();
			entity->rect.setFillColor(sf::Color::Red);
			entity->rect.setPosition(entity->pos);
			entity->bbox = entity->rect.getGlobalBounds();

			entity->health = 4 + rand() % 2 + rand()%(1+plr.laserlevel);
			entity->speed = 3.0f + rand() % 3;
			entity->damage = 2;
			entity->score = entity->health * 75;
			entity->sprframe = rand() % 8;
			entity->attack_time = 20 + rand() % 20;
			entity->walk_frame = rand() % 2;

			r = rand() % 31;
			r *= 0.1f;
			entity->BaseMove(entity, sf::Vector2f(-1.5f + r, 1.0f));

			active_monsters++;
		}
		if (name == "astesmall") {
			entity->texname = "astesmall";
			entity->classtype = "monster";
			entity->rect.setSize(sf::Vector2f(24, 24));
			entity->rendrect = entity->rect.getSize();
			entity->rect.setFillColor(sf::Color::Red);
			entity->rect.setPosition(entity->pos);
			entity->bbox = entity->rect.getGlobalBounds();

			entity->health = 1 + rand() % 2 + rand()%(1+plr.laserlevel);
			entity->speed = 1.0f + rand() % 2;
			entity->damage = 1;
			entity->score = entity->health * 15;
			entity->sprframe = rand() % 4;
			entity->walk_frame = rand() % 2;

			r = rand() % 21;
			r *= 0.1f;
			len = rand() % 41;
			len *= 0.1f;

			entity->BaseMove(entity, sf::Vector2f(-1.0f + r, -1.0f + len));

			active_monsters++;
		}

		if (name == "laser") {
			entity->texname = "laser";
			entity->classtype = "playerbullet";
			entity->rect.setSize(sf::Vector2f(8.0f, 16.0f));
			entity->rendrect = sf::Vector2f(8.0f, 16.0f);
			entity->rect.setFillColor(sf::Color::Green);
			entity->rect.setPosition(entity->pos);
			entity->spr.setPosition(entity->pos);
			entity->bbox = entity->rect.getGlobalBounds();

			entity->speed = 15.0f;
			entity->health = 1;
			entity->damage = 1 + plr.laserlevel;

			entity->BaseMove(entity, sf::Vector2f(0.0f, -1.0f));
			//std::cout << "Laser x y: " << x << ", " << y << "\n";
		}
		if (name == "plasma") {
			entity->texname = "plasma";
			entity->classtype = "playerbullet";
			entity->rect.setSize(sf::Vector2f(16.0f, 16.0f));
			entity->rendrect = sf::Vector2f(16.0f, 16.0f);
			entity->rect.setFillColor(sf::Color::Green);
			entity->rect.setPosition(entity->pos);
			entity->spr.setPosition(entity->pos);
			entity->bbox = entity->rect.getGlobalBounds();

			entity->speed = 15.0f;
			entity->health = 1;
			entity->damage = 5 + plr.laserlevel*0.5f;
			entity->sprframe = rand()%2;
			// convert the angle into a vector
			aimvec.x = cos(plr.aimang * (M_PI*2 / 360));
			aimvec.y = sin(plr.aimang * (M_PI*2 / 360));

			entity->BaseMove(entity, aimvec);
		}
		if (name == "enemybullet") {
			entity->texname = "bullet";
			entity->classtype = "enemybullet";
			entity->rect.setSize(sf::Vector2f(8.0f, 8.0f));
			entity->rendrect = sf::Vector2f(10.0f, 10.0f);
			entity->rect.setFillColor(sf::Color::Red);
			entity->rect.setPosition(entity->pos);
			entity->bbox = entity->rect.getGlobalBounds();
			entity->owner = attackthing;

			entity->speed = 5.0f;
			entity->health = 1;
			entity->damage = 1;

			// aim to the center of player
			x -= attackthing->bbox.left;
			y -= attackthing->bbox.top;
			// get a normalized aiming vector
			aimvec = (plr.bbox.getPosition() + sf::Vector2f(6, 6)) - (attackthing->bbox.getPosition() + sf::Vector2f(x, y));
			len = sqrt(aimvec.x * aimvec.x + aimvec.y * aimvec.y);
			aimvec.x /= len;
			aimvec.y /= len;

			entity->BaseMove(entity, aimvec);
		}
		if (name == "rocklaser") {
			entity->texname = "rocklaser";
			entity->classtype = "enemybullet";
			entity->rect.setSize(sf::Vector2f(8.0f, 32.0f));
			entity->rendrect = sf::Vector2f(8.0f, 32.0f);
			entity->rect.setFillColor(sf::Color::Red);
			entity->rect.setPosition(entity->pos);
			entity->bbox = entity->rect.getGlobalBounds();
			entity->owner = attackthing;

			entity->speed = 15.0f;
			entity->health = 1;
			entity->damage = 2;

			entity->BaseMove(entity, sf::Vector2f(0.0f, 1.0f));
		}
		if (name == "armorbonus" || name == "shieldbonus") {
			entity->texname = name;
			entity->classtype = "bonus";
			entity->rect.setSize(sf::Vector2f(32.0f, 32.0f));
			entity->rendrect = sf::Vector2f(16.0f, 16.0f);
			entity->spr.setOrigin(-8, -8);
			entity->rect.setFillColor(sf::Color::Yellow);
			entity->rect.setPosition(entity->pos);
			entity->bbox = entity->rect.getGlobalBounds();

			entity->speed = 2.0f;
			entity->health = 1;
			r = rand() % 21;
			r *= 0.1f;
			entity->BaseMove(entity, sf::Vector2f(-1.0f + r, 1.0f));
		}
		if (name == "megabomb" || name == "quadlaser" || name == "laserup") {
			entity->texname = name;
			entity->classtype = "bonus";
			entity->rect.setSize(sf::Vector2f(40.0f, 40.0f));
			entity->rendrect = sf::Vector2f(24.0f, 24.0f);
			entity->spr.setOrigin(-8, -8);
			entity->rect.setFillColor(sf::Color::Yellow);
			entity->rect.setPosition(entity->pos);
			entity->bbox = entity->rect.getGlobalBounds();

			entity->speed = 2.0f;
			entity->health = 1;
			r = rand() % 21;
			r *= 0.1f;
			entity->BaseMove(entity, sf::Vector2f(-1.0f + r, 1.0f));
		}

		break;

	} while (entity = entity->next, entity->next->classname != "catworld");
}

// this background is displayed when gamestate == GS_MENU
void Game::StarsBackground(Entity *entity)
{
	star_spawn_time --;
	// spawn star background
	if (active_stars < MAX_STARS && rand() % 100 < 66 && star_spawn_time <= 0) {
		SpawnEntity(head, "stars", rand() % (width - 8), -8);
		star_spawn_time = 5 + rand() % 15;
	}
	do {
		if(entity->freed == true || entity->classname == "catworld")
			continue;
		// check screen borders, flip velocity if going outside the screen
		// If this is a vertically scrolling shmup, then enemies & bullets should be allowed
		// go outside the screen. If this an asteroids game, then entities (except bullets?)
		// should wrap around the screen
		if (entity->bbox.top > height || entity->bbox.top < -128
			|| entity->bbox.left > width || entity->bbox.left < -128) {
			if (entity->classtype == "stars")
				active_stars--;
			RemoveEntity(entity);
		}
		entity->BasePhysics(entity);
	} while(entity = entity->next, entity->next->classname != "catworld");
}

void Game::HandleGameLogic(Entity *entity)
{
	int i, r, dmg, saved;

	if (paused || gamestate == GS_PAUSEMENU)
		return;

	// increment level timer (35 equals to 1 second)
	level_time++;
	// spawn star background
	if (active_stars < MAX_STARS && rand() % 100 < 66 && level_time > star_spawn_time) {
		SpawnEntity(head, "stars", rand() % (width - 8), -8);
		star_spawn_time = level_time + 5 + rand() % 15;
	}
	
	// spawn new enemy ships
	if (active_monsters < 100 && rand() % 100 < 60 + plr.laserlevel*6 && level_time > enemy_spawn_time) {
		r = rand() % 100;
		if(r < 30)
			SpawnEntity(head, "spaceblob", rand() % (width - 48), -48);
		else if (r >= 30 && r < 60)
			SpawnEntity(head, "alienship", rand() % (width - 48), -48);
		else if (r >= 60 && r < 80)
			SpawnEntity(head, "wideship", rand() % (width - 64), -48);
		else
			SpawnEntity(head, "rockship", rand() % (width - 64), -64);
		enemy_spawn_time = level_time + 1 + rand()%(20-plr.laserlevel*3);
		//std::cout << "Level Time: " << level_time << "\n";
	}
	
	// player movement
	if(plr.health > 0)
		plr.PlayerPhysics(plr);
	else
		plr.Killed(plr);
	// don't let player get out of the screen
	if(plr.bbox.top < 0)
		plr.pos.y = 0;
	if(plr.bbox.top + plr.bbox.height > height)
		plr.pos.y = height - plr.bbox.height;
	if(plr.bbox.left < 0)
		plr.pos.x = 0;
	if(plr.bbox.left + plr.bbox.width > width)
		plr.pos.x = width - plr.bbox.width;
	// update player hud
	hud_health.UpdateElement(plr.health);
	hud_shield.UpdateElement(plr.shield);
	hud_bombs.UpdateElement(plr.bombs);
	hud_score.UpdateElement(plr.score);

	// don't know why, but player needs to start the game with the values maximized
	// otherwise the hud bar graphics can't show all the bars
	// so set the initial values here after the hud elements have been updated at least once
	/*
	if (level_time <= 1) {
		plr.shield = 5;
		plr.bombs = 2;
	}
	*/
	
	do {
		if(entity->freed == true || entity->classname == "catworld" || entity == menu.recordentity)
			continue;
		// player should be one of the entities too
		if(plr.bbox.intersects(entity->bbox) && plr.health > 0) {
			if (entity->health > 0 && entity->classtype == "monster") {
				saved = 0;
				dmg = entity->damage;
				if(plr.shield > 0) {
					plr.PlaySound("shield_hit", 2);
					saved = dmg;
					if(dmg > plr.shield)
						saved = plr.shield;
					dmg -= saved;
				}
				else {
					plr.PlaySound("armor_hit", 2);
				}
				plr.shield -= saved;
				plr.health -= dmg;

				entity->health -= plr.damage;
				if (entity->health <= 0) {
					entity->PlaySound("exp6", 0, 1.125f + (rand()%10)*0.025f);
					if(entity->classname == "spaceblob")
						destroyed_spaceblob++;
					if(entity->classname == "alienship")
						destroyed_alienship++;
					if(entity->classname == "wideship")
						destroyed_wideship++;
					if(entity->classname == "rockship") {
						destroyed_rockship++;
						r = 2 + rand()%3;
						for (i = 0; i<r; i++)
							SpawnEntity(head, "astesmall", entity->bbox.left + 24, entity->bbox.top + 24);
					}
					if(entity->classname == "astesmall")
						destroyed_astesmall++;

					plr.score += entity->score;
					entity->anim_time = 0;
					entity->walk_frame = 0;
					entity->sprframe = 4;
					entity->Killed(entity);
					active_monsters--;
				}
				if (plr.health <= 0) {
					plr.PlaySound("exp1", 0);
					if(entity->classname == "spaceblob")
						shipslost_spaceblob++;
					if(entity->classname == "alienship")
						shipslost_alienship++;
					if(entity->classname == "wideship")
						shipslost_wideship++;
					if(entity->classname == "rockship")
						shipslost_rockship++;
					if(entity->classname == "astesmall")
						shipslost_astesmall++;
					plr.anim_time = 0;
					plr.walk_frame = 0;
					plr.sprframe = 4;
					plr.Killed(plr);
				}
			}
			if (entity->health > 0 && entity->classtype == "enemybullet") {
				saved = 0;
				dmg = entity->damage;
				if(plr.shield > 0) {
					plr.PlaySound("shield_hit", 2);
					saved = dmg;
					if(dmg > plr.shield)
						saved = plr.shield;
					dmg -= saved;
				}
				else {
					plr.PlaySound("armor_hit", 2);
				}
				plr.shield -= saved;
				plr.health -= dmg;

				entity->health = 0;
				entity->anim_time = 0;
				entity->walk_frame = 0;
				entity->Killed(entity);
				if (plr.health <= 0) {
					plr.PlaySound("exp1", 0);
					if(entity->owner->classname == "alienship")
						shipslost_alienship++;
					if(entity->owner->classname == "wideship")
						shipslost_wideship++;
					if(entity->owner->classname == "rockship")
						shipslost_rockship++;
					plr.anim_time = 0;
					plr.walk_frame = 0;
					plr.sprframe = 4;
					plr.Killed(plr);
				}
			}
			if (entity->health > 0 && entity->classtype == "bonus") {
				entity->PlaySound("itemup", 0);
				if (entity->classname == "armorbonus") {
					plr.health += 2;
					plr.score += 100;
				}
				if (entity->classname == "shieldbonus") {
					plr.shield += 1;
					plr.score += 100;
				}
				if (entity->classname == "megabomb") {
					if(plr.bombs >= 5)
						PlayerPlasmaBlast();
					plr.bombs++;
					plr.score += 250;
				}
				if (entity->classname == "quadlaser") {
					plr.items |= plr.IT_QUADLASER;
					plr.score += 500;
				}
				if (entity->classname == "laserup") {
					plr.laserlevel ++;
					plr.score += 500;
				}
				if (plr.health > 10)
					plr.health = 10;
				if (plr.shield > 10)
					plr.shield = 10;
				if (plr.bombs > 5)
					plr.bombs = 5;
				if (plr.laserlevel > 5)
					plr.laserlevel = 5;
				RemoveEntity(entity);
			}
		}

		// going through every entity to check if something is colliding with another
		// maybe not a good idea if there's a lot of entities in the game?
		touchthing = entity->next;
		do {
			if(entity->classtype == "stars" || entity->classtype == "bonus" || entity == menu.recordentity)
				break;
			if(entity->health <= 0)
				break;
			if(touchthing->freed == true)
				continue;
			if (touchthing->health <= 0)
				continue;
			if (touchthing->classtype == "stars" || touchthing->classtype == "bonus" || touchthing == menu.recordentity)
				continue;
			// same classname (or classtype?) can't collide with each other
			if (entity->classname == touchthing->classname)
				continue;
			// hit something? TODO: insert some kind of action here depending on
			// what entities bumped into each other, for testing purposes, just flip the velocity
			if(entity->bbox.intersects(touchthing->bbox) && entity != touchthing) {
				if (entity->classtype == "playerbullet" && touchthing->classtype == "monster") {
					touchthing->health -= entity->damage;
					entity->anim_time = 0;
					entity->walk_frame = 0;
					entity->health = 0;
					entity->Killed(entity);                    
					if (touchthing->health <= 0) {
						touchthing->PlaySound("exp6", 0, 1.125f + (rand()%10)*0.025f);
						plr.score += touchthing->score;
						if(touchthing->classname == "spaceblob")
							destroyed_spaceblob++;
						if(touchthing->classname == "alienship")
							destroyed_alienship++;
						if(touchthing->classname == "wideship")
							destroyed_wideship++;
						if(touchthing->classname == "rockship")
							destroyed_rockship++;
						if(touchthing->classname == "astesmall")
							destroyed_astesmall++;
						if (rand() % 100 < 25 && touchthing->classname != "astesmall") {
							r = rand()%100;
							if(r < 40)
								SpawnEntity(head, "armorbonus", touchthing->bbox.left + 24, touchthing->bbox.top + 24);
							else if(r >= 40 && r < 80)
								SpawnEntity(head, "shieldbonus", touchthing->bbox.left + 24, touchthing->bbox.top + 24);
							else
								SpawnEntity(head, "megabomb", touchthing->bbox.left + 24, touchthing->bbox.top + 24);
						}
						if(touchthing->classname == "rockship") {
							r = 2 + rand()%3;
							for (i = 0; i<r; i++)
								SpawnEntity(head, "astesmall", touchthing->bbox.left + 24, touchthing->bbox.top + 24);
							if(rand() % 100 < 10)
								SpawnEntity(head, "laserup", touchthing->bbox.left + 24, touchthing->bbox.top + 24);
						}
						if(rand() % 100 < 10 && touchthing->classname == "wideship") // && !(plr.items & plr.IT_QUADLASER)) {
							SpawnEntity(head, "quadlaser", touchthing->bbox.left + 24, touchthing->bbox.top + 24);
						touchthing->anim_time = 0;
						touchthing->walk_frame = 0;
						touchthing->sprframe = 4;
						touchthing->Killed(touchthing);
						active_monsters--;
					}
				}
				break;
			}
		} while(touchthing = touchthing->next, entity != touchthing);

		// Alien Attack!
		if (entity->classname == "alienship" && entity->health > 0 && plr.health > 0) {
			if (entity->attack_time == 0) {
				entity->PlaySound("enemy_fire", 0);
				entity->attack_time = 25 + rand()%25;
				attackthing = entity;
				if(rand()%100 < 50)
					SpawnEntity(head, "enemybullet", entity->bbox.left + 6, entity->bbox.top + 42);
				else
					SpawnEntity(head, "enemybullet", entity->bbox.left + 38, entity->bbox.top + 42);
			}
		}
		if (entity->classname == "rockship" && entity->health > 0 && plr.health > 0) {
			if (entity->attack_time == 0) {
				entity->PlaySound("enemy_fire", 0);
				entity->attack_time = 5 + rand() % 30;
				attackthing = entity;
				SpawnEntity(head, "enemybullet", entity->bbox.left + 24, entity->bbox.top + 24);
			}
		}
		if (entity->classname == "wideship" && entity->health > 0 && plr.health > 0) {
			if (entity->attack_time == 0) {
				entity->PlaySound("enemy_laser_fire", 0);
				entity->attack_time = 7 + rand() % 21;
				attackthing = entity;
				SpawnEntity(head, "rocklaser", entity->bbox.left + 9, entity->bbox.top + 42);
				SpawnEntity(head, "rocklaser", entity->bbox.left + 55, entity->bbox.top + 42);
			}
		}

		// check screen borders, flip velocity if going outside the screen
		// If this is a vertically scrolling shmup, then enemies & bullets should be allowed
		// go outside the screen. If this an asteroids game, then entities (except bullets?)
		// should wrap around the screen
		if (entity->bbox.top > height || entity->bbox.top < -128
		|| entity->bbox.left > width || entity->bbox.left < -128) {
			if(entity->classtype == "monster")
				active_monsters--;
			if (entity->classtype == "stars")
				active_stars--;
			RemoveEntity(entity);
		}
		if(entity->health > 0)
			entity->BasePhysics(entity);
		else {
			entity->Killed(entity);
			if (entity->walk_frame >= 7)
				RemoveEntity(entity);
		}
	} while(entity = entity->next, entity->next->classname != "catworld");
}


void Game::Redraw(sf::RenderWindow &window, Entity *entity)
{
	int i;
	int rendercount = 0;
	std::string stuff;

	window.clear();

	entity = head;
	// draw stars first, maybe they could have their own list?
	// or add renderlayer, entities in renderlayer 0 are rendered first, renderlayer 1 rendered second, etc...
	// then there's no need for this mess in checking the classtypes/health
	do {
		if (entity->freed == true || entity == menu.recordentity)
			continue;
		if (rendercount > MAX_STARS)
			continue;
		if (entity->classtype != "stars")
			continue;
		stuff = "sprites/" + entity->texname + ".png";
		entity->texture.loadFromFile(stuff, sf::IntRect(0 + entity->rendrect.x * (entity->sprframe % 4),
			0 + entity->rendrect.y * (entity->sprframe / 4),
			entity->rendrect.x, entity->rendrect.y));
		entity->spr.setTexture(entity->texture, true);
		window.draw(entity->spr);
		rendercount++;
	} while (entity = entity->next, entity->next->classname != "catworld");

	rendercount = 0;
	entity = head;
	// draw other entities than stars
	do {
		if (entity->freed == true || entity == menu.recordentity)
			continue;
		if (rendercount > MAX_SPRITES)
			continue;
		if (entity->classtype == "stars")
			continue;
		if ((entity->classtype != "monster" && entity->classtype != "bonus") || entity->health <= 0)
			continue;
		if (entity->texname != "") {
			//if (entity->classname == "megabomb")
			//    window.draw(entity->rect);
			stuff = "sprites/" + entity->texname + ".png";
			entity->texture.loadFromFile(stuff, sf::IntRect(0 + entity->rendrect.x * (entity->sprframe%4), 
															0 + entity->rendrect.y * (entity->sprframe / 4), 
															entity->rendrect.x, entity->rendrect.y));
			//entity->texture.loadFromFile(stuff, sf::IntRect(0, 0, 48, 48));
			entity->spr.setTexture(entity->texture, true);
			window.draw(entity->spr);
		}
		else
			window.draw(entity->rect);
		rendercount++;
	} while (entity = entity->next, entity->next->classname != "catworld");

	// the last object to be rendered is rendered on top of others
	// if player is also in the entity list and is the first entity,
	// it would be necessary to add some Entity *plr here and store player in it
	// and not render it in the do while, but instead render here
	stuff = "sprites/" + plr.texname + ".png";
	plr.texture.loadFromFile(stuff, sf::IntRect(0 + plr.rendrect.x * (plr.sprframe % 4),
												0 + plr.rendrect.y * (plr.sprframe / 4),
												plr.rendrect.x, plr.rendrect.y));
	plr.spr.setTexture(plr.texture);
	//plr.spr.setColor(sf::Color(0.5f, 1.0f, 0.5f));
	if(gamestate != GS_MENU)
		window.draw(plr.spr);
	//window.draw(plr.rect);
	// would be nice if explosions rendered over other things
	rendercount = 0;
	entity = head;
	do {
		if (entity->freed == true || entity == menu.recordentity)
			continue;
		if (rendercount > MAX_SPRITES)
			continue;
		if (entity->classtype == "stars")
			continue;
		if (entity->classtype != "playerbullet" && entity->classtype != "enemybullet" && entity->health > 0)
			continue;
		if (entity->texname != "") {
			//if (entity->classname == "laser")
			//   window.draw(entity->rect);
			stuff = "sprites/" + entity->texname + ".png";
			entity->texture.loadFromFile(stuff, sf::IntRect(0 + entity->rendrect.x * (entity->sprframe % 4),
				0 + entity->rendrect.y * (entity->sprframe / 4),
				entity->rendrect.x, entity->rendrect.y));
			//entity->texture.loadFromFile(stuff, sf::IntRect(0, 0, 48, 48));
			entity->spr.setTexture(entity->texture, true);
			window.draw(entity->spr);
		}
		else
			window.draw(entity->rect);
		rendercount++;
	} while (entity = entity->next, entity->next->classname != "catworld");

	// hud stuff, render it over everything else
	if(gamestate != GS_MENU || hud_init == false) {
		// the bar graphics have to be drawn fully initially
		if(hud_init == false) {
			hud_health.value = 10;
			hud_shield.value = 10;
			hud_bombs.value = 5;
			hud_init = true;
		}
		if (hud_health.value > 0) {
			hud_health.texture.loadFromFile(hud_health.texname, sf::IntRect(0, 0, 0 + hud_health.blocksize.x * hud_health.value, 16));
			hud_health.spr.setTexture(hud_health.texture);
			window.draw(hud_health.spr);
		}
		if (hud_shield.value > 0) {
			hud_shield.texture.loadFromFile(hud_shield.texname, sf::IntRect(0, 0, 0 + hud_shield.blocksize.x * hud_shield.value, 16));
			hud_shield.spr.setTexture(hud_shield.texture);
			window.draw(hud_shield.spr);
		}
		if (hud_bombs.value > 0) {
			hud_bombs.texture.loadFromFile(hud_bombs.texname, sf::IntRect(0, 0, 0 + hud_bombs.blocksize.x * hud_bombs.value, 9));
			hud_bombs.spr.setTexture(hud_bombs.texture);
			window.draw(hud_bombs.spr);
		}
	}
	// score is rendered with text (could be a bitmap font later)
	if((hud_score.value > 0 && gamestate == GS_MENU) || gamestate != GS_MENU) {
		hud_score.font.loadFromFile(hud_score.texname);
		hud_score.text.setFont(hud_score.font);
		hud_score.text.setString(std::to_string(hud_score.value));
		hud_score.text.setCharacterSize(16);
		hud_score.text.setFillColor(sf::Color(160, 224, 124));
		hud_score.text.setStyle(sf::Text::Bold);
		hud_score.text.setPosition(width / 2 - hud_score.text.getGlobalBounds().width, -3);
		window.draw(hud_score.text);
	}

	if(gamestate != GS_GAME) {
		menu.background.setFillColor(sf::Color(16, 24, 32, 192));
		//menu.background.setSize(sf::Vector2f(256, 128));
		//menu.background.setPosition(sf::Vector2f(width / 2 - 128, 112));
		menu.background.setSize(sf::Vector2f(width, height));
		menu.background.setPosition(sf::Vector2f(0, 0));
		if(gamestate == GS_PAUSEMENU)
			window.draw(menu.background);

		menu.font.loadFromFile(menu.texname);
		menu.text.setFont(menu.font);
		stuff = "";
		std::string val;
		for(i=2; i<10; i++) {
			val = menu.activemenu[i].value != -1 ? std::to_string(menu.activemenu[i].value) : "";
			// resolution needs the resolution values
			if(menu.activemenu[i].itemname == "Resolution") {
				if(menu.activemenu[i].value == 0)
					val = std::to_string(vid_width) + "x" + std::to_string(vid_height); // custom resolution set in the cfg
				else if(menu.activemenu[i].value == 1)
					val = "1280x800";
				else if(menu.activemenu[i].value == 2)
					val = "1600x1000";
				else if(menu.activemenu[i].value == 3)
					val = "1680x1050";
				else if(menu.activemenu[i].value == 4)
					val = "1920x1080";
				else if(menu.activemenu[i].value == 5)
					val = "2560x1440";
				else if(menu.activemenu[i].value == 6)
					val = "3840x2160";
			}
			stuff += menu.activemenu[i].itemname + " " + val + "\n";
		}
		menu.text.setString(stuff);
		menu.text.setCharacterSize(16); // info about this function mentions bitmap fonts...
		menu.text.setFillColor(sf::Color(160, 224, 124));
		//menu.text.setStyle(sf::Text::Bold);
		if(menu.activemenu[0].itemname == "RecordsMenu") {
			menu.text.setPosition(width / 2 - 192, 32);
			menu.rect.setPosition(sf::Vector2f(width / 2 - 192, 52 + (menu.currentitem-3)*16));

			stuff = "sprites/" + menu.recordentity->texname + ".png";
			menu.recordentity->texture.loadFromFile(stuff, sf::IntRect(0 + menu.recordentity->rendrect.x * (menu.recordentity->sprframe % 4),
				0 + menu.recordentity->rendrect.y * (menu.recordentity->sprframe / 4),
				menu.recordentity->rendrect.x, menu.recordentity->rendrect.y));
			menu.recordentity->spr.setTexture(menu.recordentity->texture, true);
			window.draw(menu.recordentity->spr);
		}
		else if(menu.activemenu[0].itemname == "HighscoreMenu") {
			menu.text.setPosition(width / 2 - 104, 64);
			menu.rect.setPosition(sf::Vector2f(width / 2 - 134, 84 + (menu.currentitem-3)*16));
		}
		else {
			menu.text.setPosition(width / 2 - 64, 64);
			menu.rect.setPosition(sf::Vector2f(width / 2 - 128, 84 + (menu.currentitem-3)*16));
		}
		menu.rect.setFillColor(sf::Color(255, 220, 112, 128));
		menu.rect.setSize(sf::Vector2f(256, 16));
		
		window.draw(menu.text);
		window.draw(menu.rect);
	}

	window.display();
}


void Game::RunGameLoop(sf::RenderWindow &window)
{
	int i;
	int entitycount = MAX_ENTITIES;

	srand(time(NULL)); // init the rand()

	// create the music before creating the entities and their sounds/buffers
	music = new sf::Music();
	music->openFromFile("music/meownoid_001.ogg");
	music->setVolume(musicvolume);
	music->setLoop(true);
	music->play();

	plr = Player(width / 2 - 48, height - 104);

	menu = Menu();
	menu.menuitems = NUMMAINMENU;
	for(i=0; i<MAXMENUITEMS; i++) {
		if(i < menu.menuitems)
			menu.activemenu[i] = menu.mainmenu[i];
		else {
			menu.activemenu[i].itemname = "";
			menu.activemenu[i].value = -1;
			menu.activemenu[i].menuact = NULL;
		}
	}
	//menu.recordentity = Entity(width / 2 + 128, height / 2 + 128, "spaceblob");

	hud_health = HUD("hhealth", plr, 2, height - 18, sf::Vector2f(69,16), sf::Vector2f(7, 16));
	hud_shield = HUD("hshield", plr, 2, height - 18, sf::Vector2f(69, 16), sf::Vector2f(7, 16));
	hud_bombs = HUD("hbombs", plr, 2, height - 18 - 10, sf::Vector2f(55, 9), sf::Vector2f(14, 9));
	hud_score = HUD("ibmvga", plr, width/2, -4);

	// spawn a linked list of entities
	Entity *entity = new Entity();
	head = entity;
	tail = entity;

	// immediately spawn them all as freed entities
	for (i = 0; i < entitycount; i++) {
		entity->prev = tail;
		if (i == 0) {
			head = entity;
			entity->classname = "catworld";
			entity->freed = false;
			entity->health = 999999999;
		}
		else {
			entity->freed = true;
		}
		entity->num = i;
		tail = entity;
		if (i == entitycount - 1) {
			entity->next = head;
			head->prev = entity;
			entity->flags = 1;
			entity->freed = false;
			entity->pos = sf::Vector2f(width/2 + 64, height/2 + 64);
			menu.recordentity = entity;
			
		}
		else {
			entity->next = new Entity();
			entity = entity->next;
		}
	}
	entity = head;

	while (window.isOpen())
	{
		// 1) Get/Handle Input (mouse, keyboard, joystick)
		// 2) Gamelogic
		// 3) Clear/Draw/Display

		HandleInput(window);
		entity = head;
		if(gamestate == GS_GAME) {
			HandleGameLogic(entity);
			window.setMouseCursorVisible(false);
		}
		else if(gamestate == GS_PAUSEMENU) {
			HandleGameLogic(entity); // need to pause this logic
			MenuStuff();
			window.setMouseCursorVisible(true);
		}
		else {
			StarsBackground(entity);
			MenuStuff();
			window.setMouseCursorVisible(true);
		}
		entity = head;
		Redraw(window, entity);
	}
}