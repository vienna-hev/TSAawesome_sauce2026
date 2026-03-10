#ifndef INSTANCE_HPP
#define INSTANCE_HPP

struct CampInstance {
	SDL_FPoint pos;
	short hp;
	char lv;
	float inv;
};

struct EnemyInstance {
	SDL_FPoint pos;
	short hp;
	char lv;
	float animTime;
	SDL_FPoint target;
	float inv;
	float bias;
};

#endif