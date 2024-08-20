//#include <stdarg.h>
//#include <stdio.h>
//#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>

#include "game.h"

int soundvolume = 50;
int musicvolume = 50;
int vid_width = 1280;
int vid_height = 800;
int vid_fullscreen = 0;
int vid_mode = 0;
int windowstyle;
std::string playername = "MeownyanDefender";

Game game;

int main()
{
    int i, j, len;
    std::string line;
    std::string inter[2];

    // check the config file
    std::ifstream cfgfile ("meownoid.cfg");
    if (cfgfile.is_open()) {
        while (getline(cfgfile, line)) {
            len = (int)line.length();
            inter[0] = "";
            inter[1] = "";
            for(i=0; i<len+1; i++) {
                if(line[i] == ' ') {
                    for(j=0; j<i; j++)
                        inter[0] += line[j];
                    for(j=i+1; j<len; j++)
                        inter[1] += line[j];
                    if(inter[0] == "soundvolume")
                        soundvolume = stoi(inter[1]);
                    if(inter[0] == "musicvolume")
                        musicvolume = stoi(inter[1]);
                    if(inter[0] == "vid_width")
                        vid_width = stoi(inter[1]);
                    if(inter[0] == "vid_height")
                        vid_height = stoi(inter[1]);
					if(inter[0] == "vid_mode")
						vid_mode = stoi(inter[1]);
                    if(inter[0] == "vid_fullscreen")
                        vid_fullscreen = stoi(inter[1]);
					if(inter[0] == "playername")
						playername = inter[1];
                }
            }
        }
        cfgfile.close();
    }

    // the game resolution is 640x400, borderless windowed
    if(vid_fullscreen == 0)
        windowstyle = sf::Style::Default; // resizeable window with titlebar
    //else if(vid_fullscreen == 1)
    //   windowstyle = sf::Style::Fullscreen; // fullscreen... doesn't work nicely with the videomode 640x400 first, then resized to the proper resolution
    else
        windowstyle = sf::Style::None; // borderless windowed, can't be resized, may appear as if fullscreen if resolution == desktop resolution

    sf::RenderWindow window(sf::VideoMode(640, 400), "Meownoid", windowstyle);
    //Game game(window.getSize().x, window.getSize().y);
	game = Game(window.getSize().x, window.getSize().y);

    // this didn't work for the fullscreen, the game render area was 640x400 with lots of black borders instead of getting scaled to fill the entire screen
    //sf::RenderWindow window(sf::VideoMode(vid_width, vid_height), "Meownoid", windowstyle);
    //Game game(640, 400);

    // scale to vid_width and vid_height, could check if the vid_width and vid_height match desktop resolution and the SetPosition is 0, 0 if matching res
    // otherwise center the window to the screen
	if(vid_mode > 0) {
		window.setSize(game.videomodes[vid_mode]);
		if(sf::VideoMode::getDesktopMode().width == game.videomodes[vid_mode].x && sf::VideoMode::getDesktopMode().height == game.videomodes[vid_mode].y)
			window.setPosition(sf::Vector2i(0, 0));
		else
			window.setPosition(sf::Vector2i(sf::VideoMode::getDesktopMode().width/2 - game.videomodes[vid_mode].x/2,
				sf::VideoMode::getDesktopMode().height/2 - game.videomodes[vid_mode].y/2 - !(vid_fullscreen)*31));
	}
	else {
		window.setSize(sf::Vector2u(vid_width, vid_height));
		if(sf::VideoMode::getDesktopMode().width == vid_width && sf::VideoMode::getDesktopMode().height == vid_height)
			window.setPosition(sf::Vector2i(0, 0));
		else
			window.setPosition(sf::Vector2i(sf::VideoMode::getDesktopMode().width/2 - vid_width/2,
				sf::VideoMode::getDesktopMode().height/2 - vid_height/2 - !(vid_fullscreen)*31));
	}

    // don't display mouse cursor
    //window.setMouseCursorVisible(false);
    // set the ticrate and start running
    window.setFramerateLimit(TICRATE);

	// check the data file
	std::ifstream datafile;
	datafile.std::ifstream::open("meownoid_data.bin", std::ios::in | std::ios::binary);
	// doesn't exist, make the file
	if(!datafile) {
		datafile.std::ifstream::close();
		std::ofstream datanew("meownoid_data.bin", std::ios::out | std::ios::binary);
		// highscores
		for(i=0; i<NUMHIGHSCORES; i++) {
			//datanew.std::ofstream::write((char *)&game.highscore[i].name, sizeof(std::string));
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
	}
	// read the data / set the variables
	else {
		// highscores
		for(i=0; i<NUMHIGHSCORES; i++) {
			//datafile.std::ifstream::read((char *)&game.highscore[i].name, sizeof(std::string));
			std::getline(datafile,game.highscore[i].name,'\0'); // get player name (remember we null ternimated in binary)
			datafile.std::ifstream::read((char *)&game.highscore[i].score, sizeof(game.highscore[i].score));
		}
		// monsters
		datafile.std::ifstream::read((char *)&game.destroyed_spaceblob, sizeof(game.destroyed_spaceblob));
		datafile.std::ifstream::read((char *)&game.destroyed_alienship, sizeof(game.destroyed_alienship));
		datafile.std::ifstream::read((char *)&game.destroyed_wideship, sizeof(game.destroyed_wideship));
		datafile.std::ifstream::read((char *)&game.destroyed_rockship, sizeof(game.destroyed_rockship));
		datafile.std::ifstream::read((char *)&game.destroyed_astesmall, sizeof(game.destroyed_astesmall));
		datafile.std::ifstream::read((char *)&game.shipslost_spaceblob, sizeof(game.shipslost_spaceblob));
		datafile.std::ifstream::read((char *)&game.shipslost_alienship, sizeof(game.shipslost_alienship));
		datafile.std::ifstream::read((char *)&game.shipslost_wideship, sizeof(game.shipslost_wideship));
		datafile.std::ifstream::read((char *)&game.shipslost_rockship, sizeof(game.shipslost_rockship));
		datafile.std::ifstream::read((char *)&game.shipslost_astesmall, sizeof(game.shipslost_astesmall));
		datafile.std::ifstream::close();
	}
	// Start the game loop
	game.gwindow = &window;
    game.RunGameLoop(window);
}
