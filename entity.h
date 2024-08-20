#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#define M_PI		3.14159265358979323846

// disable these pieces of shit warnings
#pragma warning (disable : 4244)

class Entity {
public:
	Entity();
	Entity(int x, int y, std::string name);

	sf::Sprite spr;
	sf::Texture texture;
	std::string texname;
	int sprframe = 0; // if non zero append to the texname or use a different part from the spritesheet
	int renderlayer = 0;

	sf::RectangleShape rect; // the entity bounding box size
	sf::FloatRect bbox; // the actual entity bounding box derived from the rect, huhhuh
	sf::Vector2f rendrect; // rendering rect (vector2), may be different size (smaller/bigger) than the the bbox & rect

	// would be necessary to set the origin to the center of the object if making an asteroids game with rotating entities
	// default origin is top left corner of the entity...
	// set the origin of the entity
	//entity.setOrigin(10.f, 20.f);
	// retrieve the origin of the entity
	//sf::Vector2f origin = entity.getOrigin(); // = (10, 20)

	int num = -1; // just an id number, -1 would be an error, 0 is the game world, 1 is player, other numbers are used by enemies, bullets, items
	bool freed = true; // if true, the entity can be reused when spawning a new entity
	std::string classname;
	std::string classtype;
	int flags = 0;
	int health = 0;
	int shield = 0;
	int damage = 0;
	int bombs = 0;
	int laserlevel = 0;
	int items = 0;
	int score = 0;
	int lives = 1;
	int anim_time = 0;
	int attack_time = 0;
	int bomb_time = 0;
	int shield_time = 0;
	int walk_frame = 0;
	float speed = 0.0f;
	sf::Vector2f velocity;
	sf::Vector2f pos;
	float aimang;
	Entity *owner = NULL;

	// links in the linked list of entities
	Entity *next = NULL;
	Entity *prev = NULL;

	// entity sound channels, apparently both the buffer and sound need multiple channels
	// for one entity to be able to play multiple sounds at the same time
	sf::SoundBuffer *buffer[4];
	sf::Sound *sound[4];

	void BaseMove(Entity *entity, sf::Vector2f vec);
	void BasePhysics(Entity *entity);
	void Killed(Entity *entity);

	void CreateSoundBuffers();
	void PlaySound(std::string snd, int chan);
	void PlaySound(std::string snd, int chan, float pitch);
};

class Player : public Entity {
public:
	void PlayerMove(Player &plr, sf::Vector2f vec);
	void PlayerPhysics(Player &plr);
	void Killed(Player &plr);

	bool attack_released = true;
	bool bomb_released = true;

	Player();
	Player(int x, int y);

	typedef enum {
		IT_QUADLASER = 1,
		IT_MISSILES = 2,
		IT_SUPERLASER = 4,
	} items_t;
};