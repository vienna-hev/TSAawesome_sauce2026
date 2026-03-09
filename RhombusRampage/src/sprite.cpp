#include "Sprite.hpp"

Sprite::Sprite(SDL_Renderer* pRenderer, const char* pFilePath, char pWidth, char pHeight, float pPosX, float pPosY, float pCtrX, float pCtrY)
: frame(0), angle(0.0f) {
	SDL_Surface* sur = SDL_LoadBMP(pFilePath);
	tex = SDL_CreateTextureFromSurface(pRenderer, sur);
	SDL_DestroySurface(sur);
	SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
	
	pos = {pPosX, pPosY};
	ctr = {pCtrX, pCtrY};
	
	src = {0.0f, 0.0f, (float)pWidth, (float)pHeight};
}

Sprite::~Sprite() {
	SDL_DestroyTexture(tex);
}

void Sprite::setPos(float pX, float pY) {
	pos.x = pX;
	pos.y = pY;
}
void Sprite::setPosX(float pX) {
	pos.x = pX;
}
void Sprite::setPosY(float pY) {
	pos.y = pY;
}
SDL_FPoint Sprite::getPos() {
	return pos;
}
void Sprite::changePos(float pX, float pY) {
	pos.x += pX;
	pos.y += pY;
}

void Sprite::setAngle(float pAngle) {
	angle = pAngle;
}
float Sprite::getAngle() {
	return angle;
}
void Sprite::changeAngle(float pAngle) {
	angle += pAngle;
}

void Sprite::setFrame(char pFrame) {
	frame = pFrame % 6;
	src.x = (float)(pFrame % 3) * src.w;
	src.y = (float)((pFrame / 3) % 2) * src.h;
}
char Sprite::getFrame() {
	return frame;
}

void Sprite::render(SDL_Renderer* pRenderer, float pScale) {
	SDL_FPoint ctrScaled = {ctr.x * pScale, ctr.y * pScale};
	SDL_FRect dst = {pos.x - ctrScaled.x, pos.y - ctrScaled.y, src.w * pScale, src.h * pScale};
	
	SDL_RenderTextureRotated(pRenderer, tex, &src, &dst, angle, &ctrScaled, SDL_FLIP_NONE);
}