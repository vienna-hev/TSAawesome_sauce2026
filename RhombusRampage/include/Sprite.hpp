#ifndef SPRITE_HPP
#define SPRITE_HPP

#include <SDL3/SDL.h>

class Sprite {
		SDL_Texture* tex;
		SDL_FPoint pos;
		SDL_FPoint ctr;
		SDL_FRect src;
		char frame;
		float angle;
		
	public:
		Sprite(SDL_Renderer* pRenderer, const char* pFilePath, char pWidth, char pHeight, float pPosX, float pPosY, float pCtrX, float pCtrY);
		~Sprite();
		
		void setPos(float pX, float pY);
		void setPosX(float pX);
		void setPosY(float pY);
		SDL_FPoint getPos();
		void changePos(float pX, float pY);
		
		void setAngle(float pAngle);
		float getAngle();
		void changeAngle(float pAngle);
		
		void setFrame(char pFrame);
		char getFrame();
		
		void render(SDL_Renderer* pRenderer, float pScale);
};

#endif