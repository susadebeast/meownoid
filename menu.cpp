#include <time.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "game.h"
#include "menu.h"

extern Game game;

Menu::Menu() { texname = "fonts/ibmvga.ttf"; };

void Menu::ActivateMenuItem()
{
	if(activemenu[currentitem].menuact != NULL) {
		// the player plays the menu sounds
		//if(menutime > 0)
		//	game.plr.PlaySound("menu2", 0);
		activemenu[currentitem].menuact();
	}
}

void Menu::BackMenu()
{
	if(game.gamestate == game.GS_MENU && activemenu[1].itemname == "CloseMenu")
		return;
	game.plr.PlaySound("trigger1", 3);
	activemenu[1].menuact();
}

// the menuitem 0 is the menu title, so don't select that
void Menu::NextMenuItem(bool dir)
{
	if(menutime > 0)
		return;

	if(activemenu[0].itemname != "HighscoreMenu")
		game.plr.PlaySound("menu1", 3);

	if(activemenu[0].itemname == "RecordsMenu") {
		if(dir == false) {
			records_item --;
			if(records_item < 0)
				records_item = menuitems;
		}
		else {
			records_item ++;
			if(records_item > menuitems)
				records_item = 0;
		}
		currentitem = 3;
		menutime = 5;
		return;
	}

	if(dir == false) {
		currentitem --;
		if(currentitem < 3)
			currentitem = menuitems-1;
	}
	else {
		currentitem ++;
		if(currentitem > menuitems-1)
			currentitem = 3;
	}
	if(activemenu[0].itemname == "MainMenu")
		mainmenu_item = currentitem;
	if(activemenu[0].itemname == "SettingsMenu")
		settings_item = currentitem;
	if(activemenu[0].itemname == "HighscoreMenu")
		currentitem = 4;
	menutime = 5;
	//std::cout << currentitem << "\n";
	//std::cout << activemenu[currentitem].itemname << "\n";
}

// menu actions
// for now this just starts a new game, later there could be skill selection first
void M_NewGameMenu()
{
	game.plr.PlaySound("menu2", 3);
	game.gamestate = game.GS_GAME;
	game.StartNewGame();
}

void M_QuitGame()
{
	game.QuitGame();
}

// these change the menu to a different menu
// kinda wonky with having to set the activemenu with a for loop
void M_MainMenu()
{
	int i;
	game.menu.menuitems = NUMMAINMENU;
	for(i=0; i<MAXMENUITEMS; i++) {
		if(i < game.menu.menuitems)
			game.menu.activemenu[i] = game.menu.mainmenu[i];
		else {
			game.menu.activemenu[i].itemname = "";
			game.menu.activemenu[i].value = -1;
			game.menu.activemenu[i].menuact = NULL;
		}
	}
	game.menu.currentitem = game.menu.mainmenu_item;
}

void M_SettingsMenu()
{
	int i;
	game.plr.PlaySound("menu2", 3);

	game.menu.menuitems = NUMSETTINGSMENU;
	for(i=0; i<MAXMENUITEMS; i++) {
		if(i < game.menu.menuitems) {
			game.menu.activemenu[i] = game.menu.settingsmenu[i];
			// apparently need to set the values like this
			if(game.menu.activemenu[i].itemname == "Sound Volume")
				game.menu.activemenu[i].value = soundvolume;
			if(game.menu.activemenu[i].itemname == "Music Volume")
				game.menu.activemenu[i].value = musicvolume;
			if(game.menu.activemenu[i].itemname == "Resolution")
				game.menu.activemenu[i].value = vid_mode;
			if(game.menu.activemenu[i].itemname == "Video Mode")
				game.menu.activemenu[i].value = vid_fullscreen;
		}
		else {
			game.menu.activemenu[i].itemname = "";
			game.menu.activemenu[i].value = -1;
			game.menu.activemenu[i].menuact = NULL;
		}
	}
	game.menu.currentitem = game.menu.settings_item;
}

Menu::records_t M_RecordsMonster(int monsternum)
{
	Menu::records_t records;

	if(monsternum == 0) {
		records.pagetext = INFO_MEOWDEFENDER;
		//records.pagetext += INFO_DESTROYED + std::to_string(records.destroyed) + "\n";
		//records.pagetext += INFO_SHIPSLOST + std::to_string(records.shipslost) + "\n";
		game.SpawnEntity(game.menu.recordentity, "meowdefender", game.menu.recordentity->pos.x, game.menu.recordentity->pos.y);
	}
	else if(monsternum == 1) {
		records.pagetext = INFO_SPACEBLOB;
		records.pagetext += INFO_DESTROYED + std::to_string(game.destroyed_spaceblob) + "\n";
		records.pagetext += INFO_SHIPSLOST + std::to_string(game.shipslost_spaceblob) + "\n";
		game.SpawnEntity(game.menu.recordentity, "spaceblob", game.menu.recordentity->pos.x, game.menu.recordentity->pos.y);
	}
	else if(monsternum == 2) {
		records.pagetext = INFO_ALIENSHIP;
		records.pagetext += INFO_DESTROYED + std::to_string(game.destroyed_alienship) + "\n";
		records.pagetext += INFO_SHIPSLOST + std::to_string(game.shipslost_alienship) + "\n";
		game.SpawnEntity(game.menu.recordentity, "alienship", game.menu.recordentity->pos.x, game.menu.recordentity->pos.y);
	}
	else if(monsternum == 3) {
		records.pagetext = INFO_WIDESHIP;
		records.pagetext += INFO_DESTROYED + std::to_string(game.destroyed_wideship) + "\n";
		records.pagetext += INFO_SHIPSLOST + std::to_string(game.shipslost_wideship) + "\n";
		game.SpawnEntity(game.menu.recordentity, "wideship", game.menu.recordentity->pos.x, game.menu.recordentity->pos.y);
	}
	else if(monsternum == 4) {
		records.pagetext = INFO_ROCKSHIP;
		records.pagetext += INFO_DESTROYED + std::to_string(game.destroyed_rockship) + "\n";
		records.pagetext += INFO_SHIPSLOST + std::to_string(game.shipslost_rockship) + "\n";
		game.SpawnEntity(game.menu.recordentity, "rockship", game.menu.recordentity->pos.x, game.menu.recordentity->pos.y);
	}
	else if(monsternum == 5) {
		records.pagetext = INFO_ASTEROID;
		records.pagetext += INFO_DESTROYED + std::to_string(game.destroyed_astesmall) + "\n";
		records.pagetext += INFO_SHIPSLOST + std::to_string(game.shipslost_astesmall) + "\n";
		game.SpawnEntity(game.menu.recordentity, "astesmall", game.menu.recordentity->pos.x, game.menu.recordentity->pos.y);
	}

	return records;
}

void M_RecordsMenu()
{
	int i;
	Menu::records_t records;

	game.plr.PlaySound("menu2", 3);
	game.menu.menuitems = NUMRECORDSITEMS;
	for(i=0; i<MAXMENUITEMS; i++) {
		if(i < game.menu.menuitems +1) {
			game.menu.activemenu[i] = game.menu.recordsmenu[i];
			if(i==5) {
				records = M_RecordsMonster(game.menu.records_item);
				game.menu.activemenu[i].itemname = records.pagetext;
				game.menu.activemenu[i].value = -1;
				game.menu.activemenu[i].menuact = NULL;
			}
		}
		else {
			game.menu.activemenu[i].itemname = "";
			game.menu.activemenu[i].value = -1;
			game.menu.activemenu[i].menuact = NULL;
		}
	}
	game.menu.currentitem = 3;
}

void M_NextMonster()
{
	Menu::records_t records;
	records = M_RecordsMonster(game.menu.records_item);
	game.menu.activemenu[5].itemname = records.pagetext;
	game.menu.currentitem = 3;
}


void M_HighscoreMenu()
{
	int i, j, k, len;
	std::string stuff = "";
	std::string dottedname = "";

	game.plr.PlaySound("menu2", 3);
	game.menu.menuitems = 5;
	for(i=0; i<MAXMENUITEMS; i++) {
		if(i < game.menu.menuitems) {
			game.menu.activemenu[i] = game.menu.highscoremenu[i];
			if(i==4) {
				for(j=0; j<NUMHIGHSCORES; j++) {
					len = (int)game.highscore[j].name.length();
					dottedname = game.highscore[j].name;
					if(len > 16) {
						dottedname = "";
						for(k=0; k<16; k++)
							dottedname += game.highscore[j].name[k];
					}
					dottedname += " ";
					for(k=0; k<17-len; k++)
						dottedname += ".";
					dottedname += " ";
					stuff += dottedname + std::to_string(game.highscore[j].score) + "\n";
				}
				game.menu.activemenu[i].itemname = stuff;
				game.menu.activemenu[i].value = -1;
				game.menu.activemenu[i].menuact = NULL;
			}
		}
		else {
			game.menu.activemenu[i].itemname = "";
			game.menu.activemenu[i].value = -1;
			game.menu.activemenu[i].menuact = NULL;
		}
	}
	game.menu.currentitem = 4;
}

// close the menu and continue playing
void M_CloseMenu()
{
	game.gamestate = game.GS_GAME;
	game.paused = false;
}

// change sound volume
void M_ChangeSoundVolume()
{
	if(game.menu.menutime > 0)
		return;
	if(!game.menu.menudir)
		soundvolume -= 5;
	else
		soundvolume += 5;
	game.plr.PlaySound("talk", 3);
	soundvolume = bound(0, soundvolume, 100);
	game.menu.activemenu[game.menu.currentitem].value = soundvolume;
	game.menu.menutime = 5;
}

// change music volume
void M_ChangeMusicVolume()
{
	if(game.menu.menutime > 0)
		return;
	if(!game.menu.menudir)
		musicvolume -= 5;
	else
		musicvolume += 5;
	game.plr.PlaySound("talk", 3);
	musicvolume = bound(0, musicvolume, 100);
	game.music->setVolume(musicvolume);
	game.menu.activemenu[game.menu.currentitem].value = musicvolume;
	game.menu.menutime = 5;
}

void M_ChangeResolution()
{
	if(game.menu.menutime > 0)
		return;
	game.plr.PlaySound("menu2", 3);
	if(!game.menu.menudir)
		vid_mode --;
	else
		vid_mode ++;
	if(vid_mode < 0)
		vid_mode = 6;
	if(vid_mode > 6)
		vid_mode = 0;
	game.menu.activemenu[game.menu.currentitem].value = vid_mode;
	if(vid_mode > 0) {
		game.gwindow->setSize(game.videomodes[vid_mode]);
		if(sf::VideoMode::getDesktopMode().width == game.videomodes[vid_mode].x && sf::VideoMode::getDesktopMode().height == game.videomodes[vid_mode].y)
			game.gwindow->setPosition(sf::Vector2i(0, 0));
		else
			game.gwindow->setPosition(sf::Vector2i(sf::VideoMode::getDesktopMode().width/2 - game.videomodes[vid_mode].x/2,
				sf::VideoMode::getDesktopMode().height/2 - game.videomodes[vid_mode].y/2 - !(vid_fullscreen)*31));
	}
	else {
		game.gwindow->setSize(sf::Vector2u(vid_width, vid_height));
		if(sf::VideoMode::getDesktopMode().width == vid_width && sf::VideoMode::getDesktopMode().height == vid_height)
			game.gwindow->setPosition(sf::Vector2i(0, 0));
		else
			game.gwindow->setPosition(sf::Vector2i(sf::VideoMode::getDesktopMode().width/2 - vid_width/2,
				sf::VideoMode::getDesktopMode().height/2 - vid_height/2 - !(vid_fullscreen)*31));
	}

	game.menu.menutime = 5;
}

void M_ChangeVideoMode()
{
	if(game.menu.menutime > 0)
		return;
	game.plr.PlaySound("menu2", 3);
	if(!game.menu.menudir)
		vid_fullscreen --;
	else
		vid_fullscreen ++;
	if(vid_fullscreen < 0)
		vid_fullscreen = 1;
	if(vid_fullscreen > 1)
		vid_fullscreen = 0;
	game.menu.activemenu[game.menu.currentitem].value = vid_fullscreen;

	if(vid_fullscreen == 0)
		windowstyle = sf::Style::Default;
	else
		windowstyle = sf::Style::None;
	game.gwindow->create(sf::VideoMode(640, 400), "Meownoid", windowstyle);
	game.gwindow->setFramerateLimit(TICRATE);
	if(vid_mode > 0) {
		game.gwindow->setSize(game.videomodes[vid_mode]);
		if(sf::VideoMode::getDesktopMode().width == game.videomodes[vid_mode].x && sf::VideoMode::getDesktopMode().height == game.videomodes[vid_mode].y)
			game.gwindow->setPosition(sf::Vector2i(0, 0));
		else
			game.gwindow->setPosition(sf::Vector2i(sf::VideoMode::getDesktopMode().width/2 - game.videomodes[vid_mode].x/2,
				sf::VideoMode::getDesktopMode().height/2 - game.videomodes[vid_mode].y/2 - !(vid_fullscreen)*31));
	}
	else {
		game.gwindow->setSize(sf::Vector2u(vid_width, vid_height));
		if(sf::VideoMode::getDesktopMode().width == vid_width && sf::VideoMode::getDesktopMode().height == vid_height)
			game.gwindow->setPosition(sf::Vector2i(0, 0));
		else
			game.gwindow->setPosition(sf::Vector2i(sf::VideoMode::getDesktopMode().width/2 - vid_width/2,
				sf::VideoMode::getDesktopMode().height/2 - vid_height/2 - !(vid_fullscreen)*31));
	}

	game.menu.menutime = 5;
}