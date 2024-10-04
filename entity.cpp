#include <iostream>
#include <stdlib.h>
#include <string>

#include "game.h"
#include "entity.h"

extern int soundvolume;
extern Game game;

void Entity::CreateSoundBuffers()
{
	int i;
	for(i = 0; i < 4; i++) {
		buffer[i] = new sf::SoundBuffer();
		sound[i] = new sf::Sound();
	}
}

// need to create the soundbuffers / sound pointers
Entity::Entity()
{
	CreateSoundBuffers();
};

Entity::Entity(int x, int y, std::string name)
{
	CreateSoundBuffers();
	pos.x = (float)x; // this->pos.x
	pos.y = (float)y;
	classname = name;
	rect.setSize(sf::Vector2f(48.0f, 48.0f));
	rendrect = rect.getSize();
	rect.setFillColor(sf::Color::Red);
	rect.setPosition(pos);
	bbox = rect.getGlobalBounds();
};

void Entity::PlaySound(std::string snd, int chan)
{
	buffer[chan]->loadFromFile("sounds/" + snd + ".wav");
	sound[chan]->setBuffer(*buffer[chan]);
	if(classtype == "monster" && health > 0)
		sound[chan]->setVolume(soundvolume * 0.5f);
	else
		sound[chan]->setVolume(soundvolume);
	sound[chan]->setPitch(1);
	sound[chan]->play();
}

void Entity::PlaySound(std::string snd, int chan, float pitch)
{
	buffer[chan]->loadFromFile("sounds/" + snd + ".wav");
	sound[chan]->setBuffer(*buffer[chan]);
	if(classtype == "monster" && health > 0)
		sound[chan]->setVolume(soundvolume * 0.5f);
	else
		sound[chan]->setVolume(soundvolume);
	sound[chan]->setPitch(pitch);
	sound[chan]->play();
}

void Entity::BaseMove(Entity *entity, sf::Vector2f vec)
{
	velocity += vec * speed;
}

void Entity::Killed(Entity *entity)
{
	anim_time++;

	entity->renderlayer = RL_EFFECTS;

	pos.x += velocity.x;
	pos.y += velocity.y;
	rect.setPosition(pos);
	spr.setPosition(pos);

	if (classname == "spaceblob" || classname == "alienship") {
		sprframe = 4 + walk_frame;
		if (anim_time % 3 == 0 && anim_time > 0) {
			anim_time = 0;
			if (walk_frame == 0)
				anim_time-= (rand()%4);
			walk_frame++;
			velocity *= 0.85f;
		}
		texname = "sblob";
	}
	if (classname == "astesmall") {
		sprframe = 4 + walk_frame;
		if (anim_time % 3 == 0 && anim_time > 0) {
			anim_time = 0;
			if (walk_frame == 0)
				anim_time-= (rand()%4);
			walk_frame++;
			velocity *= 0.85f;
		}
		texname = "astesmall";
	}
	if (classname == "rockship" || classname == "wideship") {
		rendrect.x = 64;
		rendrect.y = 64;
		sprframe = 8 + walk_frame;
		if (anim_time % 3 == 0 && anim_time > 0) {
			anim_time = 0;
			if (walk_frame == 0)
				anim_time -= (rand() % 4);
			walk_frame++;
			velocity *= 0.85f;
		}
		texname = "rship";
	}
	if (classname == "laser" || classname == "enemybullet" || classname == "rocklaser") {
		sprframe = 0 + walk_frame;
		if (anim_time % 3 == 0 && anim_time > 0) {
			anim_time = 0;
			if (walk_frame == 0)
				anim_time -= (rand()%3);
			walk_frame++;
			velocity *= 0.75f;
		}
		rect.setSize(sf::Vector2f(16.0f, 16.0f));
		rendrect = rect.getSize();
		texname = "laserexp";
	}
	if (classname == "plasma") {
		sprframe = 0 + walk_frame;
		if (anim_time % 3 == 0 && anim_time > 0) {
			anim_time = 0;
			if (walk_frame == 0)
				anim_time -= (rand()%3);
			walk_frame++;
			velocity *= 0.75f;
		}
		rect.setSize(sf::Vector2f(24.0f, 24.0f));
		rendrect = rect.getSize();
		texname = "plasmaexp";
	}
}

void Entity::BasePhysics(Entity *entity)
{
	anim_time++;

	// this is the recordentity that should just stay still
	if(flags == 1)
		velocity = sf::Vector2f(0,0);

	pos.x += velocity.x;
	pos.y += velocity.y;
	rect.setPosition(pos);
	spr.setPosition(pos);
	bbox = rect.getGlobalBounds();

	if(game.plr.health <= 0) {
		attack_time = 15 + rand()%15;
	}

	if(classname == "meowdefender") {
		if (anim_time % 8 == 0) {
			walk_frame ++;
			if (walk_frame > 1)
				walk_frame = 0;
			sprframe = 0;
			sprframe += walk_frame;
			anim_time = 0;
		}
		texname = "catship";
	}

	if (classname == "spaceblob") {
		if (anim_time % 5 == 0) {
			sprframe = rand() % 4;
			anim_time = 0;
		}
		entity->texname = "sblob";
	}
	if (classname == "alienship") {
		if (attack_time > 0)
			attack_time--;
		if (anim_time % 8 == 0) {
			sprframe++;
			if (sprframe > 1)
				sprframe = 0;
			anim_time = 0;
		}
		entity->texname = "aship";
	}
	if (classname == "wideship") {
		if (attack_time > 0)
			attack_time--;
		if (anim_time % 8 == 0) {
			sprframe++;
			if (sprframe > 1)
				sprframe = 0;
			anim_time = 0;
		}
		entity->texname = "wship";
	}
	if (classname == "rockship") {
		if (attack_time > 0)
			attack_time--;
		if (anim_time % 5 == 0) {
			if (walk_frame == 0) {
				sprframe++;
				if (sprframe > 7)
					sprframe = 0;
			}
			else {
				sprframe--;
				if (sprframe < 0)
					sprframe = 7;
			}
			anim_time = 0;
		}
		entity->texname = "rship";
	}
	if (classname == "astesmall") {
		if (attack_time > 0)
			attack_time--;
		if (anim_time % 5 == 0) {
			if (walk_frame == 0) {
				sprframe++;
				if (sprframe > 3)
					sprframe = 0;
			}
			else {
				sprframe--;
				if (sprframe < 0)
					sprframe = 3;
			}
			anim_time = 0;
		}
		entity->texname = "astesmall";
	}
	if (classname == "armorbonus") {
		if (anim_time % 8 == 0) {
			sprframe++;
			if (sprframe > 2)
				sprframe = 0;
			anim_time = 0;
		}
		entity->texname = "armorbonus";
	}
	if (classname == "shieldbonus") {
		if (anim_time % 8 == 0) {
			sprframe++;
			if (sprframe > 3)
				sprframe = 0;
			anim_time = 0;
		}
		entity->texname = "shieldbonus";
	}
	if (classname == "megabomb") {
		if (anim_time % 8 == 0) {
			sprframe++;
			if (sprframe > 3)
				sprframe = 0;
			anim_time = 0;
		}
		entity->texname = "megabomb";
	}
	if (classname == "quadlaser")
		texname = "quadlaser";

	if (classname == "laser") {
		texname = "laser";
		sprframe = damage - 1;
	}
	if (classname == "plasma") {
		if (anim_time % 5 == 0) {
			sprframe++;
			if (sprframe > 1)
				sprframe = 0;
			anim_time = 0;
		}
		texname = "plasma";
	}
	if (classname == "enemybullet")
		texname = "bullet";
	if (classname == "rocklaser")
		texname = "rocklaser";

	if (classname == "stars") {
		if (anim_time % 5 == 0) {
			sprframe = walk_frame + rand() % 2;
			anim_time = 0;
		}
		entity->texname = "stars";
	}
}

Player::Player() { };

Player::Player(int x, int y)
{
	pos.x = (float)x;
	pos.y = (float)y;
	rendrect = sf::Vector2f(48.0f, 48.0f);
	rect.setSize(sf::Vector2f(24.0f, 24.0f));
	rect.setFillColor(sf::Color::Green);
	rect.setPosition(pos);
	texname = "catship";
	spr.setOrigin(12, 12);
	spr.setPosition(pos);
	speed = 7.5f;
	health = 10;
	shield = 5;
	shield_time = 35;
	bombs = 2;
	lives = 1;
	bbox = rect.getGlobalBounds();
};

// the plr isn't necessary...
void Player::PlayerMove(Player &plr, sf::Vector2f vec)
{
	if(health > 0)
		velocity += vec * speed;
}

void Player::PlayerPhysics(Player &plr)
{
	anim_time++;

	pos.x += velocity.x;
	pos.y += velocity.y;
	rect.setPosition(pos);
	spr.setPosition(pos);
	bbox = rect.getGlobalBounds();
	
	if (anim_time % 8 == 0) {
		walk_frame ++;
		if (walk_frame > 1)
			walk_frame = 0;
		sprframe = 0;
		if (velocity.x < -5.0f)
			sprframe = 2;
		if (velocity.x > 5.0f)
			sprframe = 4;
		sprframe += walk_frame;
		anim_time = 0;
	}
	texname = "catship";
	if (attack_time > 0)
		attack_time--;
	if (bomb_time > 0)
		bomb_time--;

	if (shield < 5)
		shield_time--;
	else
		shield_time = 35;
	if (shield_time <= 0) {
		//if(shield < 5)
			shield++;
		shield_time = 35;
	}

	velocity *= 0.5f;
}

void Player::Killed(Player &plr)
{
	if(walk_frame >= 7) {
		walk_frame++;
		return;
	}
	anim_time++;

	pos.x += velocity.x;
	pos.y += velocity.y;
	rect.setPosition(pos);
	spr.setPosition(pos);

	if (anim_time % 3 == 0 && anim_time > 0) {
		anim_time = 0;
		if (walk_frame == 0)
			anim_time -= (rand() % 4);
		sprframe = 4 + walk_frame;
		walk_frame++;
		velocity *= 0.85f;
	}
	texname = "sblob";
}