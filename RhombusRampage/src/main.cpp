#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <string>
#include <vector>

#include "Sprite.hpp"
#include "Instance.hpp"

bool collideRhombusV(float rhombusX, float rhombusY, float swordMin[6], float swordMax[6], float swordTrig[4]) {
	// check y=-2x for separating axis
	{
		float campMin = rhombusX + (rhombusY + 64.0f) * 0.5f;
		
		if (swordMax[2] < campMin || campMin + 64.0f < swordMin[2]) return 0;
	}

	// check y=2x for separating axis
	{
		float campMin = rhombusX - (rhombusY + 64.0f) * 0.5f;
		
		if (swordMax[3] < campMin || campMin + 64.0f < swordMin[3]) return 0;
	}

	// check if sword lateral axis is separating
	{							
		float campProj0 = ((rhombusX + 32.0f) + rhombusY * swordTrig[2]) * swordTrig[1];
		float campProj1 = ((rhombusX + 64.0f) + (rhombusY + 64.0f) * swordTrig[2]) * swordTrig[1];
		float campProj2 = ((rhombusX + 32.0f) + (rhombusY + 128.0f) * swordTrig[2]) * swordTrig[1];
		float campProj3 = (rhombusX + (rhombusY + 64.0f) * swordTrig[2]) * swordTrig[1];
		
		float campMin = SDL_min(SDL_min(campProj0, campProj1), SDL_min(campProj2, campProj3));
		float campMax = SDL_max(SDL_max(campProj0, campProj1), SDL_max(campProj2, campProj3));
		
		if (campMax < swordMin[0] || swordMax[0] < campMin) return 0;
	}

	// check if sword longitudal axis is separating
	{
		float campProj0 = ((rhombusX + 32.0f) + rhombusY * swordTrig[3]) * swordTrig[0];
		float campProj1 = ((rhombusX + 64.0f) + (rhombusY + 64.0f) * swordTrig[3]) * swordTrig[0];
		float campProj2 = ((rhombusX + 32.0f) + (rhombusY + 128.0f) * swordTrig[3]) * swordTrig[0];
		float campProj3 = (rhombusX + (rhombusY + 64.0f) * swordTrig[3]) * swordTrig[0];
		
		float campMin = SDL_min(SDL_min(campProj0, campProj1), SDL_min(campProj2, campProj3));
		float campMax = SDL_max(SDL_max(campProj0, campProj1), SDL_max(campProj2, campProj3));
		
		if (campMax < swordMin[1] || swordMax[1] < campMin) return 0;
	}

	return 1;
}

bool collideRhombusH(float rhombusX, float rhombusY, float swordMin[6], float swordMax[6], float swordTrig[4]) {
	// check y=-x/2 for separating axis
	{
		float campMin = rhombusX * 0.5f + rhombusY + 32.0f;
		
		if (swordMax[4] < campMin || campMin + 64.0f < swordMin[4]) return 0;
	}

	// check y=x/2 for separating axis
	{
		float campMin = rhombusX * 0.5f - rhombusY - 32.0f;
		
		if (swordMax[5] < campMin || campMin + 64.0f < swordMin[5]) return 0;
	}

	// check if sword lateral axis is separating
	{							
		float campProj0 = ((rhombusX + 64.0f) + rhombusY * swordTrig[2]) * swordTrig[1];
		float campProj1 = ((rhombusX + 128.0f) + (rhombusY + 32.0f) * swordTrig[2]) * swordTrig[1];
		float campProj2 = ((rhombusX + 64.0f) + (rhombusY + 64.0f) * swordTrig[2]) * swordTrig[1];
		float campProj3 = (rhombusX + (rhombusY + 32.0f) * swordTrig[2]) * swordTrig[1];
		
		float campMin = SDL_min(SDL_min(campProj0, campProj1), SDL_min(campProj2, campProj3));
		float campMax = SDL_max(SDL_max(campProj0, campProj1), SDL_max(campProj2, campProj3));
		
		if (campMax < swordMin[0] || swordMax[0] < campMin) return 0;
	}

	// check if sword longitudal axis is separating
	{
		float campProj0 = ((rhombusX + 64.0f) + rhombusY * swordTrig[3]) * swordTrig[0];
		float campProj1 = ((rhombusX + 128.0f) + (rhombusY + 32.0f) * swordTrig[3]) * swordTrig[0];
		float campProj2 = ((rhombusX + 64.0f) + (rhombusY + 64.0f) * swordTrig[3]) * swordTrig[0];
		float campProj3 = (rhombusX + (rhombusY + 32.0f) * swordTrig[3]) * swordTrig[0];
		
		float campMin = SDL_min(SDL_min(campProj0, campProj1), SDL_min(campProj2, campProj3));
		float campMax = SDL_max(SDL_max(campProj0, campProj1), SDL_max(campProj2, campProj3));
		
		if (campMax < swordMin[1] || swordMax[1] < campMin) return 0;
	}

	return 1;
}

int main(int argc, char* argv[]) {
	if (!SDL_Init(SDL_INIT_VIDEO)) return 1;
	if (!TTF_Init()) return 1;
	
	SDL_Window* window;
	SDL_Renderer* renderer;
	TTF_TextEngine* textEngine;
	TTF_Font* statFont;
	TTF_Font* upgradeFont;
	const bool* keyStates;
	bool isFullscreen = 0;
	// Uint64 tpf = 16;
	
	window = SDL_CreateWindow("Rhombus Rampage", 1024, 576, SDL_WINDOW_RESIZABLE);
	renderer = SDL_CreateRenderer(window, nullptr);
	textEngine = TTF_CreateRendererTextEngine(renderer);
	statFont = TTF_OpenFont("assets/fonts/RobotoMono-Bold.ttf", 12);
	upgradeFont = TTF_OpenFont("assets/fonts/RobotoMono-Bold.ttf", 16);
	keyStates = SDL_GetKeyboardState(nullptr);
	
	SDL_SetRenderLogicalPresentation(renderer, 1024, 576, SDL_LOGICAL_PRESENTATION_LETTERBOX);
	SDL_SetRenderVSync(renderer, 1);
	TTF_SetFontStyle(statFont, TTF_STYLE_BOLD);
	TTF_SetFontLineSkip(statFont, 32);
	TTF_SetFontStyle(upgradeFont, TTF_STYLE_BOLD);
	TTF_SetFontLineSkip(upgradeFont, 24);
	
	{
		short points = 0;
		short health = 1024;
		char level = 0;
		char power = 0;
		char size = 0;
		char speed = 0;
		unsigned char flags = 0x10;
		/*  0x01 = player, sword, and ui
			0x02 = collisions and enemies
			0x04 = boss animation
			0x08 = boss fight
			0x10 = title screen
			0x20 = information textbox
			0x40 = victory screen
			0x80 = defeat screen
		*/

		bool bossPrev = 0;
		Uint64 bossTimer = 0;
		float bossInv = 0;
		short bossHealth = 256;
		unsigned char bossDir = 0x3;
		
		Sprite player(renderer, "assets/textures/player.bmp", 11, 11, 512.0f, 256.0f, 4.5f, 6.5f);
		Sprite sword(renderer, "assets/textures/sword.bmp", 34, 34, 512.0f, 256.0f, 4.5f, 29.5f);
		Sprite boss(renderer, "assets/textures/boss.bmp", 32, 32, 512.0f, 256.0f, 16.0f, 16.0f);
		
		std::vector<CampInstance> campVec;
		std::vector<EnemyInstance> enemyVec;
		
		SDL_FRect boxBorders[] {
			{444.0f, 28.0f, 136.0f, 104.0f},
			{28.0f, 476.0f, 968.0f, 72.0f},
		};
		SDL_FRect boxes[] {
			{448.0f, 32.0f, 128.0f, 96.0f},
			{32.0f, 480.0f, 316.0f, 64.0f},
			{352.0f, 480.0f, 316.0f, 64.0f},
			{672.0f, 480.0f, 320.0f, 64.0f}
		};
		SDL_FRect upgradeSrc[] {
			{0.0f, 0.0f, 16.0f, 16.0f},
			{16.0f, 0.0f, 16.0f, 16.0f},
			{32.0f, 0.0f, 16.0f, 16.0f}
		};
		SDL_FRect upgradeDst[] {
			{32.0f, 480.0f, 64.0f, 64.0f},
			{352.0f, 480.0f, 64.0f, 64.0f},
			{672.0f, 480.0f, 64.0f, 64.0f}
		};
				
		SDL_Texture* bgTex = nullptr;
		{
			SDL_Surface* sur = SDL_LoadBMP("assets/textures/title.bmp");
			bgTex = SDL_CreateTextureFromSurface(renderer, sur);
			SDL_DestroySurface(sur);
			SDL_SetTextureScaleMode(bgTex, SDL_SCALEMODE_NEAREST);
		}
		
		SDL_Texture* upgradeTex = nullptr;
		{
			SDL_Surface* sur = SDL_LoadBMP("assets/textures/upgrades.bmp");
			upgradeTex = SDL_CreateTextureFromSurface(renderer, sur);
			SDL_DestroySurface(sur);
			SDL_SetTextureScaleMode(upgradeTex, SDL_SCALEMODE_NEAREST);
		}
		
		SDL_Texture* campTex = nullptr;
		{
			SDL_Surface* sur = SDL_LoadBMP("assets/textures/enemyCamp.bmp");
			campTex = SDL_CreateTextureFromSurface(renderer, sur);
			SDL_DestroySurface(sur);
			SDL_SetTextureScaleMode(campTex, SDL_SCALEMODE_NEAREST);
		}
		
		SDL_Texture* enemyTex = nullptr;
		{
			SDL_Surface* sur = SDL_LoadBMP("assets/textures/enemy.bmp");
			enemyTex = SDL_CreateTextureFromSurface(renderer, sur);
			SDL_DestroySurface(sur);
			SDL_SetTextureScaleMode(enemyTex, SDL_SCALEMODE_NEAREST);
		}
		
		SDL_Texture* helpTex = nullptr;
		{
			std::string str = "The Rhombuses are invading, and need to be stopped! You, Circle, are the only one capable. Defeat all the Rhombuses, and level up your abilities. But be warned, the Rhomboss, the king of all Rhombuses, might not appreciate your disturbance of the invasion.";
			SDL_Surface* sur = TTF_RenderText_Blended_Wrapped(upgradeFont, str.c_str(), 0, {0xff, 0xff, 0xff, 0xff}, 384);
			helpTex = SDL_CreateTextureFromSurface(renderer, sur);
			SDL_DestroySurface(sur);
		}
		
		SDL_Event e;
		Uint64 prevTick = 0;
		bool running = 1;
		while (running) {
			// SDL_Delay(tpf);
			
			while (SDL_PollEvent(&e)) {
				switch (e.type) {
					case SDL_EVENT_QUIT:
						running = 0;
						break;
					case SDL_EVENT_KEY_DOWN:
						switch (e.key.key) {
							case SDLK_F11:
								isFullscreen = !isFullscreen;
								SDL_SetWindowFullscreen(window, isFullscreen);
								break;
							case SDLK_RETURN: {
								if (enemyVec.size() || campVec.size() || flags & 0xc) break;
								level++;
								const char* newTex;
								switch (level) {
									case 1:
										newTex = "assets/textures/redBg.bmp";
										campVec.push_back({{32.0f, 32.0f}, 1, 0, 0.0f});
										campVec.push_back({{928.0f, 32.0f}, 2, 1, 0.0f});
										flags = 0x3;
										break;
									case 2:
										player.setPos(512.0f, 256.0f);
										newTex = "assets/textures/greenBg.bmp";
										campVec.push_back({{32.0f, 32.0f}, 4, 2, 0.0f});
										campVec.push_back({{32.0f, 320.0f}, 8, 3, 0.0f});
										campVec.push_back({{928.0f, 32.0f}, 16, 4, 0.0f});
										campVec.push_back({{928.0f, 320.0f}, 32, 5, 0.0f});
										break;
									case 3:
										player.setPos(512.0f, 256.0f);
										newTex = "assets/textures/blueBg.bmp";
										campVec.push_back({{32.0f, 32.0f}, 1, 0, 0.0f});
										campVec.push_back({{32.0f, 160.0f}, 2, 1, 0.0f});
										campVec.push_back({{32.0f, 288.0f}, 4, 2, 0.0f});
										campVec.push_back({{32.0f, 416.0f}, 2, 1, 0.0f});
										campVec.push_back({{224.0f, 32.0f}, 1, 0, 0.0f});
										campVec.push_back({{224.0f, 416.0f}, 8, 3, 0.0f});
										campVec.push_back({{736.0f, 32.0f}, 4, 2, 0.0f});
										campVec.push_back({{736.0f, 416.0f}, 32, 5, 0.0f});
										campVec.push_back({{928.0f, 32.0f}, 8, 3, 0.0f});
										campVec.push_back({{928.0f, 160.0f}, 16, 4, 0.0f});
										campVec.push_back({{928.0f, 288.0f}, 32, 5, 0.0f});
										campVec.push_back({{928.0f, 416.0f}, 16, 4, 0.0f});
										break;
									case 4:
										player.setPos(32.0f, 448.0f);
										newTex = "assets/textures/bossBg.bmp";
										flags = 0x7;
										break;
									case 5:
										player.setPos(512.0f, 256.0f);
										newTex = "assets/textures/victory.bmp";
										flags = 0x40;
										break;
									case 6:
										// reset game
										points = 0;
										health = 1024;
										level = 0;
										power = 0;
										size = 0;
										speed = 0;

										flags = 0x10;

										bossPrev = 0;
										bossTimer = 0;
										bossInv = 0;
										bossHealth = 256;
										bossDir = 0x3;

										player.setFrame(0);
										sword.setFrame(0);
										boss.setFrame(0);

										newTex = "assets/textures/title.bmp";
										break;
									default:
										newTex = "assets/textures/error.bmp";
										flags = 0x0;
										break;
								}
								SDL_DestroyTexture(bgTex);
								SDL_Surface* sur = SDL_LoadBMP(newTex);
								bgTex = SDL_CreateTextureFromSurface(renderer, sur);
								SDL_DestroySurface(sur);
								SDL_SetTextureScaleMode(bgTex, SDL_SCALEMODE_NEAREST);
								break;
							}
							case SDLK_Z: {
								short pointsNeeded = SDL_pow(4, power + 1);
								if (power >= 5 || points < pointsNeeded) break;
								points -= pointsNeeded;
								power++;
								sword.setFrame(power);
								break;
							}
							case SDLK_X: {
								short pointsNeeded = SDL_pow(4, size + 1);
								if (size >= 5 || points < pointsNeeded) break;
								points -= pointsNeeded;
								size++;
								break;
							}
							case SDLK_C: {
								short pointsNeeded = SDL_pow(4, speed + 1);
								if (speed >= 5 || points < pointsNeeded) break;
								points -= pointsNeeded;
								speed++;
								player.setFrame(speed);
								break;
							}
						}
						break;
					case SDL_EVENT_MOUSE_BUTTON_DOWN:
						if (e.button.button != 1) break;
						SDL_ConvertEventToRenderCoordinates(renderer, &e);
						if (flags & 0x2) {
							if (e.button.y < 480.0f || e.button.y > 544.0f) break;
							if (e.button.x >= 32.0f && e.button.x <= 96.0f && power < 5){
								short pointsNeeded = SDL_pow(4, power + 1);
								if (points < pointsNeeded) break;
								points -= pointsNeeded;
								power++;
								sword.setFrame(power);
							} else if (e.button.x >= 352.0f && e.button.x <= 416.0f && size < 5) {
								short pointsNeeded = SDL_pow(4, size + 1);
								if (points < pointsNeeded) break;
								points -= pointsNeeded;
								size++;
							} else if (e.button.x >= 672.0f && e.button.x <= 736.0f && speed < 5) {
								short pointsNeeded = SDL_pow(4, speed + 1);
								if (points < pointsNeeded) break;
								points -= pointsNeeded;
								speed++;
								player.setFrame(speed);
							}
						} else if ((flags & 0x10) && e.button.x > 880 && e.button.x < 928 && e.button.y > 448 && e.button.y < 528) flags |= 0x20;
						break;
				}
			}
			
			Uint64 crntTick = SDL_GetTicks();
			float delta = (float)(crntTick - prevTick) / 1000.0f;
			prevTick = crntTick;
			// if (delta > 1.0f / 60.0f) {
			// 	tpf--;
			// } else {
			// 	tpf++;
			// }
			
			SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
			SDL_RenderClear(renderer);
			
			// Render Background
			if (flags & 0x10) {
				SDL_RenderTexture(renderer, bgTex, nullptr, nullptr);
			} else {
				SDL_RenderTextureTiled(renderer, bgTex, nullptr, 32.0f, nullptr);
			}
			
			// Collisions
			if ((flags & 0x2) && (SDL_fmodf(player.getAngle(), 90.0f) != 45.0f)) {
				// save frequently used information in variables
				float swordW = 17.0f + size * 4.25f;
				float swordH = 164.0f + size * 41.0f;
				SDL_FPoint playerPos = player.getPos();
				
				// calculate invincibility time
				float invTime = 2.0f / (float)(speed + 2);
				
				// precalculate trigonometric functions
				float swordRad = (player.getAngle() - 45.0f) * SDL_PI_F / 180.0f;
				float swordSin = SDL_sinf(swordRad);
				float swordSin2 = swordSin * swordSin;
				float swordCos = SDL_cosf(swordRad);
				float swordCos2 = swordCos * swordCos;
				float swordTan = SDL_tanf(swordRad);
				float swordNCot = -1.0f / swordTan;
				
				float swordMin[6],
					  swordMax[6];
				// project sword vertices along lateral axis of sword
				{
					float swordProj0 = (playerPos.x + playerPos.y * swordTan) * swordCos2;
					float swordProj1 = swordProj0 + swordH * swordCos;
					
					swordMin[0] = SDL_min(swordProj0, swordProj1);
					swordMax[0] = SDL_max(swordProj0, swordProj1);
				}
				
				// project sword vertices along longitudal axis of sword
				{
					float swordProjBase = (playerPos.x + playerPos.y * swordNCot) * swordSin2;
					float swordProjOffset = SDL_abs(swordW * swordSin);
					
					swordMin[1] = swordProjBase - swordProjOffset;
					swordMax[1] = swordProjBase + swordProjOffset;
				}

				// project sword vertices along rhombus axes
				{
					float swordProjRX = playerPos.x + swordW * swordSin;
					float swordProjLX = playerPos.x - swordW * swordSin;
					float swordProjTX = swordH * swordCos;

					float swordProjRY = (playerPos.y - swordW * swordCos);
					float swordProjLY = (playerPos.y + swordW * swordCos);
					float swordProjTY = swordH * swordSin;

					// project along y=-2x
					{
						float swordProj0 = swordProjRX + swordProjRY * 0.5f;
						float swordProj1 = swordProjRX + swordProjTX + (swordProjRY + swordProjTY) * 0.5f;
						float swordProj2 = swordProjLX + swordProjTX + (swordProjLY + swordProjTY) * 0.5f;
						float swordProj3 = swordProjLX + swordProjLY * 0.5f;
						
						swordMin[2] = SDL_min(SDL_min(swordProj0, swordProj1), SDL_min(swordProj2, swordProj3));
						swordMax[2] = SDL_max(SDL_max(swordProj0, swordProj1), SDL_max(swordProj2, swordProj3));
					}

					// project along y=2x
					{
						float swordProj0 = swordProjRX - swordProjRY * 0.5f;
						float swordProj1 = swordProjRX + swordProjTX - (swordProjRY + swordProjTY) * 0.5f;
						float swordProj2 = swordProjLX + swordProjTX - (swordProjLY + swordProjTY) * 0.5f;
						float swordProj3 = swordProjLX - swordProjLY * 0.5f;
						
						swordMin[3] = SDL_min(SDL_min(swordProj0, swordProj1), SDL_min(swordProj2, swordProj3));
						swordMax[3] = SDL_max(SDL_max(swordProj0, swordProj1), SDL_max(swordProj2, swordProj3));
					}

					// project sword vertices along additional rhomboss axes
					if (flags & 0x8) {
						// project along y=-x/2
						{
							float swordProj0 = swordProjRX * 0.5f + swordProjRY;
							float swordProj1 = (swordProjRX + swordProjTX) * 0.5f + swordProjRY + swordProjTY;
							float swordProj2 = (swordProjLX + swordProjTX) * 0.5f + swordProjLY + swordProjTY;
							float swordProj3 = swordProjLX * 0.5f + swordProjLY;
							
							swordMin[4] = SDL_min(SDL_min(swordProj0, swordProj1), SDL_min(swordProj2, swordProj3));
							swordMax[4] = SDL_max(SDL_max(swordProj0, swordProj1), SDL_max(swordProj2, swordProj3));
						}
						
						// project along y=x/2
						{
							float swordProj0 = swordProjRX * 0.5f - swordProjRY;
							float swordProj1 = (swordProjRX + swordProjTX) * 0.5f - swordProjRY - swordProjTY;
							float swordProj2 = (swordProjLX + swordProjTX) * 0.5f - swordProjLY - swordProjTY;
							float swordProj3 = swordProjLX * 0.5f - swordProjLY;
							
							swordMin[5] = SDL_min(SDL_min(swordProj0, swordProj1), SDL_min(swordProj2, swordProj3));
							swordMax[5] = SDL_max(SDL_max(swordProj0, swordProj1), SDL_max(swordProj2, swordProj3));
						}
					}
				}

				float precalculations[] = {swordSin2, swordCos2, swordTan, swordNCot};
				
				// Camp Collisions
				for (size_t i = 0; i < campVec.size(); i++) {
					// check if camp has invincibility
					if (campVec[i].inv > 0) {
						campVec[i].inv -= delta;
						continue;
					}

					if (!collideRhombusV(campVec[i].pos.x, campVec[i].pos.y, swordMin, swordMax, precalculations)) continue;
					
					// No separating axes have been found
					// A collision has occurred
					campVec[i].inv = invTime;
					
					campVec[i].hp -= (power + 1);
					if (campVec[i].hp <= 0) {
						enemyVec.push_back({
							{campVec[i].pos.x + 24.0f, campVec[i].pos.y + 16.0f},
							(short)SDL_pow(2, campVec[i].lv),
							campVec[i].lv,
							0.0f,
							{campVec[i].pos.x + (SDL_randf() * 64.0f) - 8.0f, campVec[i].pos.y + (SDL_randf() * 64.0f) - 16.0f},
							1.0f,
							SDL_randf() * 0.75f - 0.375f
						});
						enemyVec.push_back({
							{campVec[i].pos.x + 8.0f, campVec[i].pos.y + 48.0f},
							(short)SDL_pow(2, campVec[i].lv),
							campVec[i].lv,
							0.0f,
							{campVec[i].pos.x + (SDL_randf() * 64.0f) - 24.0f, campVec[i].pos.y + (SDL_randf() * 64.0f) + 16.0f},
							1.0f,
							SDL_randf() * 0.75f - 0.375f
						});
						enemyVec.push_back({
							{campVec[i].pos.x + 40.0f, campVec[i].pos.y + 48.0f},
							(short)SDL_pow(2, campVec[i].lv),
							campVec[i].lv,
							0.0f,
							{campVec[i].pos.x + (SDL_randf() * 64.0f) + 8.0f, campVec[i].pos.y + (SDL_randf() * 64.0f) + 16.0f},
							1.0f,
							SDL_randf() * 0.75f - 0.375f
						});
						enemyVec.push_back({
							{campVec[i].pos.x + 24.0f, campVec[i].pos.y + 80.0f},
							(short)SDL_pow(2, campVec[i].lv),
							campVec[i].lv,
							0.0f,
							{campVec[i].pos.x + (SDL_randf() * 64.0f) - 8.0f, campVec[i].pos.y + (SDL_randf() * 64.0f) + 48.0f},
							1.0f,
							SDL_randf() * 0.75f - 0.375f
						});
						campVec.erase(campVec.begin() + i);
						i--;
					}
				}
				
				// Enemy Collisison
				for (size_t i = 0; i < enemyVec.size(); i++) {
					float deltaX = playerPos.x - enemyVec[i].pos.x - 8.0f;
					float deltaY = playerPos.y - enemyVec[i].pos.y - 16.0f;
					float squareDist = (deltaX * deltaX) + (deltaY * deltaY);
					
					if (squareDist < 196.0f) {
						health -= SDL_pow(2, enemyVec[i].lv);
						enemyVec.erase(enemyVec.begin() + i);
						i--;

						if (health < 0) {
							SDL_DestroyTexture(bgTex);
							SDL_Surface* sur = SDL_LoadBMP("assets/textures/defeat.bmp");
							bgTex = SDL_CreateTextureFromSurface(renderer, sur);
							SDL_DestroySurface(sur);
							SDL_SetTextureScaleMode(bgTex, SDL_SCALEMODE_NEAREST);

							flags = 0x80;
						}

						continue;
					}
					
					if (enemyVec[i].inv > 0.0f) {
						enemyVec[i].inv -= delta;
						continue;
					}
					
					if (squareDist > (swordH * swordH)) continue; // check within an elipse
					
					float swordLeft = swordTan * (deltaX + swordW * swordSin) + swordW * swordCos;
					float swordRight = swordTan * (deltaX - swordW * swordSin) - swordW * swordCos;
					float swordBase = swordNCot * deltaX;
					
					float swordMin = SDL_min(swordLeft, swordRight);
					float swordMax = SDL_max(swordLeft, swordRight);
					
					if (deltaY < swordMin || deltaY > swordMax) continue;
					
					if (SDL_fmodf(player.getAngle() - 45.0f, 360.0f) < 180.0f) {
						if (deltaY > swordBase) continue;
					} else {
						if (deltaY < swordBase) continue;
					}
					
					// Collision has occurred
					
					enemyVec[i].hp -= (power + 1);
					if (enemyVec[i].hp > 0) {
						enemyVec[i].inv = invTime;
						continue;
					}
					
					points += SDL_pow(2, enemyVec[i].lv);
					
					if (enemyVec[i].lv < 1) {
						enemyVec.erase(enemyVec.begin() + i);
						i--;
						continue;
					}
					
					enemyVec[i].lv--;
					enemyVec[i].hp = (short)SDL_pow(2, enemyVec[i].lv);
					enemyVec[i].animTime = 0.0f;
					enemyVec[i].target.x = enemyVec[i].pos.x + (SDL_randf() * 64.0f) - 32.0f;
					enemyVec[i].target.y = enemyVec[i].pos.y + (SDL_randf() * 64.0f) - 32.0f;
					enemyVec[i].inv = 1.0f;
					
					enemyVec.push_back({
						enemyVec[i].pos,
						enemyVec[i].hp,
						enemyVec[i].lv,
						0.0f,
						{enemyVec[i].pos.x + (SDL_randf() * 64.0f) - 32.0f, enemyVec[i].pos.y + (SDL_randf() * 64.0f) - 32.0f},
						1.0f,
						SDL_randf() * 0.75f - 0.375f
					});
				}

				// Boss Collision
				// get the center position of the boss
				SDL_FPoint bossPos = boss.getPos();
				if (bossInv > 0) {
					bossInv -= delta;
				} else if (flags & 0x8) {
					if (collideRhombusV(bossPos.x - 32.0f, bossPos.y - 64.0f, swordMin, swordMax, precalculations) || collideRhombusH(bossPos.x - 64.0f, bossPos.y - 32.0f, swordMin, swordMax, precalculations)) {
						//collision code
						bossInv = invTime;
						bossHealth -= power + 1;
						if (bossHealth <= 0) {
							flags = 0x3;
						}
					}
				} else if (flags & 0x4) {
					if (collideRhombusV(bossPos.x - 32.0f, bossPos.y - 64.0f, swordMin, swordMax, precalculations)) {
						bossInv = 6.0f;
						bossTimer = crntTick + 2000u;
					}
				}

				if (flags & 0x8) {
					float playerMin[4], bossMin[4];

					playerMin[0] = 2.0f * playerPos.x - playerPos.y - 32.0f;
					playerMin[1] = 2.0f * playerPos.x + playerPos.y - 32.0f;
					playerMin[2] = playerPos.x + 2.0f * playerPos.y - 32.0f;
					playerMin[3] = playerPos.x - 2.0f * playerPos.y - 32.0f;

					bossMin[0] = 2.0f * bossPos.x - bossPos.y - 64.0f;
					bossMin[1] = 2.0f * bossPos.x + bossPos.y - 64.0f;
					bossMin[2] = bossPos.x + 2.0f * bossPos.y - 64.0f;
					bossMin[3] = bossPos.x - 2.0f * bossPos.y - 64.0f;

					if ((
						(playerMin[0] < bossMin[0] + 128.0f && playerMin[0] + 64.0f > bossMin[0]) &&
						(playerMin[1] < bossMin[1] + 128.0f && playerMin[1] + 64.0f > bossMin[1])
					) || (
						(playerMin[2] < bossMin[2] + 128.0f && playerMin[2] + 64.0f > bossMin[2]) &&
						(playerMin[3] < bossMin[3] + 128.0f && playerMin[3] + 64.0f > bossMin[3])
					)) {
						if (!bossPrev) {
							bossPrev = 1;

							health -= 256;
							if (health <= 0) {
								SDL_DestroyTexture(bgTex);
								SDL_Surface* sur = SDL_LoadBMP("assets/textures/defeat.bmp");
								bgTex = SDL_CreateTextureFromSurface(renderer, sur);
								SDL_DestroySurface(sur);
								SDL_SetTextureScaleMode(bgTex, SDL_SCALEMODE_NEAREST);

								flags = 0x80;
							} 
						}
					} else {
						bossPrev = 0;
					}
				}

				for (CampInstance& camp : campVec) {
					SDL_FRect src = {(camp.lv % 3) * 16.0f, ((camp.lv / 3) % 2) * 32.0f, 16.0f, 32.0f};
					SDL_FRect dst = {camp.pos.x, camp.pos.y, 64.0f, 128.0f};
					SDL_RenderTexture(renderer, campTex, &src, &dst);
				}
				for (size_t i = 0; i < enemyVec.size(); i++) {
					if (enemyVec[i].animTime < 1.0f) {
						enemyVec[i].animTime += delta;
					} else {
						enemyVec[i].target.x = playerPos.x - 8.0f;
						enemyVec[i].target.y = playerPos.y - 16.0f;
					}
					
					float dx = enemyVec[i].target.x - enemyVec[i].pos.x;
					float dy = enemyVec[i].target.y - enemyVec[i].pos.y;
					float dist = delta * (50.0f + 25.0f * enemyVec[i].lv);
					
					if (dx * dx + dy * dy > dist * dist) {
						float targetAngle = SDL_atanf(dy / dx);
						float dxSign = (dx < 0.0f) ? -1.0f : 1.0f;
						
						enemyVec[i].pos.x += dist * dxSign * SDL_cosf(targetAngle + enemyVec[i].bias);
						enemyVec[i].pos.y += dist * dxSign * SDL_sinf(targetAngle + enemyVec[i].bias);
					}
					
					SDL_FRect src = {(enemyVec[i].lv % 3) * 4.0f, ((enemyVec[i].lv / 3) % 2) * 8.0f, 4.0f, 8.0f};
					SDL_FRect dst = {enemyVec[i].pos.x, enemyVec[i].pos.y, 16.0f, 32.0f};
					SDL_RenderTexture(renderer, enemyTex, &src, &dst);
				}
			}
			
			// Boss
			if (flags & 0x8) {
				SDL_FPoint bossPos = boss.getPos();
				boss.changePos(delta * 320.0f * ((bossDir & 0x1) ? 1.0f : -1.0f), delta * 320.0f * ((bossDir & 0x2) ? 1.0f : -1.0f));

				if (bossPos.x < 64) {
					bossDir |= 0x1u;
				} else if (bossPos.x > 960) {
					bossDir &= 0xfeu;
				}
				if (bossPos.y < 64) {
					bossDir |= 0x2u;
				} else if (bossPos.y > 512) {
					bossDir &= 0xfd;
				}

				if (crntTick > bossTimer) {
					bossTimer = crntTick + 2000u;
					char enemyLv = SDL_rand(6);
					enemyVec.push_back({
						{bossPos.x, bossPos.y},
						(short)SDL_pow(2, enemyLv),
						enemyLv,
						0.0f,
						{bossPos.x + (SDL_randf() * 64.0f) - 8.0f, bossPos.y + (SDL_randf() * 64.0f) - 16.0f},
						1.0f,
						SDL_randf() * 0.75f - 0.375f
					});
				}
				boss.render(renderer, 4.0f);
			} else if (flags & 0x4) {
				char bossFrame = boss.getFrame();
				if (bossTimer && crntTick > bossTimer) {
					bossTimer = crntTick + 2000u;
					if (bossFrame == 2) flags = 0xb;
					else boss.setFrame(bossFrame + 1);
				}
				boss.render(renderer, 4.0f);
			}

			// Render Player, Sword, and UI
			if (flags & 0x1) {
				char dx = (keyStates[SDL_SCANCODE_D] || keyStates[SDL_SCANCODE_RIGHT]) - (keyStates[SDL_SCANCODE_A] || keyStates[SDL_SCANCODE_LEFT]);
				char dy = (keyStates[SDL_SCANCODE_S] || keyStates[SDL_SCANCODE_DOWN]) - (keyStates[SDL_SCANCODE_W] || keyStates[SDL_SCANCODE_UP]);
				
				player.changePos(delta * (float)dx * (float)(100 + 50 * speed) * (dy ? 0.75f : 1.0f), delta * (float)dy * (float)(100 + 50 * speed) * (dx ? 0.75f : 1.0f));
				SDL_FPoint plrPos = player.getPos();
				if (plrPos.x > 1008.0f) {
					player.setPosX(1008.0f);
					plrPos.x = 1008.0f;
				} else if (plrPos.x < 16.0f) {
					player.setPosX(16.0f);
					plrPos.x = 16.0f;
				}
				if (plrPos.y > 560.0f) {
					player.setPosY(560.0f);
					plrPos.y = 560.0f;
				} else if (plrPos.y < 16.0f) {
					player.setPosY(16.0f);
					plrPos.y = 16.0f;
				}
				sword.setPos(plrPos.x, plrPos.y);
				
				player.changeAngle((180.0f + 90.0f * speed) * delta);
				sword.setAngle(player.getAngle());
				sword.render(renderer, 4.0f + (float)size);
				player.render(renderer, 4.0f);

				// Render UI
				SDL_SetRenderDrawColor(renderer, 0x77, 0x77, 0x77, 0xff);
				SDL_RenderFillRects(renderer, boxBorders, 2);
				SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
				SDL_RenderFillRects(renderer, boxes, 4);
				
				SDL_RenderTexture(renderer, upgradeTex, upgradeSrc, upgradeDst);
				SDL_RenderTexture(renderer, upgradeTex, upgradeSrc + 1, upgradeDst + 1);
				SDL_RenderTexture(renderer, upgradeTex, upgradeSrc + 2, upgradeDst + 2);
				
				// Generate and Display Text
				{
					std::string str = "Points: " + std::to_string(points) + "\nHealth: " + std::to_string(health) + "\nLevel: " + std::to_string(level);
					TTF_Text* text = TTF_CreateText(textEngine, statFont, str.c_str(), 0);
					TTF_DrawRendererText(text, 448.0f, 42.0f);
					TTF_DestroyText(text);
				}
				{
					std::string str = "Power Level: " + std::to_string(power + 1);
					TTF_Text* text = TTF_CreateText(textEngine, upgradeFont, str.c_str(), 0);
					TTF_DrawRendererText(text, 128.0f, 504.0f);
					TTF_DestroyText(text);
				}
				{
					std::string str = "Size Level: " + std::to_string(size + 1);
					TTF_Text* text = TTF_CreateText(textEngine, upgradeFont, str.c_str(), 0);
					TTF_DrawRendererText(text, 448.0f, 504.0f);
					TTF_DestroyText(text);
				}
				{
				std::string str = "Speed Level: " + std::to_string(speed + 1);
				TTF_Text* text = TTF_CreateText(textEngine, upgradeFont, str.c_str(), 0);
				TTF_DrawRendererText(text, 768.0f, 504.0f);
				TTF_DestroyText(text);
				}
			}
			
			// Render Information Textbox
			if (flags & 0x20) {
				SDL_FRect helpRect = {320.0f, 160.0f, 384.0f, 256.0f};
				SDL_RenderFillRect(renderer, &helpRect);
				helpRect.y = 164.0f;
				SDL_RenderTexture(renderer, helpTex, nullptr, &helpRect);
			}

			SDL_RenderPresent(renderer);
		}
		
		SDL_DestroyTexture(helpTex);
		SDL_DestroyTexture(campTex);
		SDL_DestroyTexture(enemyTex);
		SDL_DestroyTexture(upgradeTex);
		SDL_DestroyTexture(bgTex);
	}
	
	TTF_CloseFont(upgradeFont);
	TTF_CloseFont(statFont);
	TTF_DestroyRendererTextEngine(textEngine);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	
	TTF_Quit();
	SDL_Quit();
	
	return 0;
}
