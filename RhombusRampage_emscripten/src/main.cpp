#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <emscripten/emscripten.h>

#include <string>
#include <vector>
#include <cstdlib>

#include "Sprite.hpp"
#include "Instance.hpp"

SDL_Window* rhombusrampage_window;
SDL_Renderer* rhombusrampage_renderer;
TTF_TextEngine* rhombusrampage_textEngine;
TTF_Font* rhombusrampage_statFont;
TTF_Font* rhombusrampage_upgradeFont;
const bool* rhombusrampage_keyStates;
bool rhombusrampage_isFullscreen;

short rhombusrampage_points, rhombusrampage_health;
char rhombusrampage_level, rhombusrampage_power, rhombusrampage_size, rhombusrampage_speed;
unsigned char rhombusrampage_flags;
/*  0x01 = player, sword, and ui
	0x02 = collisions and enemies
	0x04 = boss animation
	0x08 = boss fight
	0x10 = title screen
	0x20 = information textbox
	0x40 = victory screen
	0x80 = defeat screen
*/

bool rhombusrampage_bossPrev;
Uint64 rhombusrampage_bossTimer;
float rhombusrampage_bossInv;
short rhombusrampage_bossHealth;
unsigned char rhombusrampage_bossDir;

Sprite* rhombusrampage_player;
Sprite* rhombusrampage_vector;
Sprite* rhombusrampage_boss;

std::vector<CampInstance> rhombusrampage_campVec;
std::vector<EnemyInstance> rhombusrampage_enemyVec;

SDL_FRect rhombusrampage_boxBorders[2];
SDL_FRect rhombusrampage_boxes[4];
SDL_FRect rhombusrampage_upgradeSrc[3];
SDL_FRect rhombusrampage_upgradeDst[3];

SDL_Texture* rhombusrampage_bgTex;
SDL_Texture* rhombusrampage_upgradeTex;
SDL_Texture* rhombusrampage_campTex;
SDL_Texture* rhombusrampage_enemyTex;
SDL_Texture* rhombusrampage_helpTex;

Uint64 rhombusrampage_prevTick;

bool rhombusrampage_running;

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

static void mainloop() {
	if (!rhombusrampage_running) {
		SDL_DestroyTexture(rhombusrampage_helpTex);
		SDL_DestroyTexture(rhombusrampage_campTex);
		SDL_DestroyTexture(rhombusrampage_enemyTex);
		SDL_DestroyTexture(rhombusrampage_upgradeTex);
		SDL_DestroyTexture(rhombusrampage_bgTex);
		
		delete rhombusrampage_player;
		delete rhombusrampage_vector;
		delete rhombusrampage_boss;
		
		TTF_CloseFont(rhombusrampage_upgradeFont);
		TTF_CloseFont(rhombusrampage_statFont);
		TTF_DestroyRendererTextEngine(rhombusrampage_textEngine);
		SDL_DestroyRenderer(rhombusrampage_renderer);
		SDL_DestroyWindow(rhombusrampage_window);
		
		TTF_Quit();
		SDL_Quit();

        #ifdef __EMSCRIPTEN__
        	emscripten_cancel_main_loop();
        #else
        	exit(0);
        #endif
	}
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		switch (e.type) {
			case SDL_EVENT_QUIT:
				rhombusrampage_running = 0;
				break;
			case SDL_EVENT_KEY_DOWN:
				switch (e.key.key) {
					case SDLK_F11:
						rhombusrampage_isFullscreen = !rhombusrampage_isFullscreen;
						SDL_SetWindowFullscreen(rhombusrampage_window, rhombusrampage_isFullscreen);
						break;
					case SDLK_RETURN: {
						if (rhombusrampage_enemyVec.size() || rhombusrampage_campVec.size() || rhombusrampage_flags & 0xc) break;
						rhombusrampage_level++;
						const char* newTex;
						switch (rhombusrampage_level) {
							case 1:
								newTex = "assets/textures/redBg.bmp";
								rhombusrampage_campVec.push_back({{32.0f, 32.0f}, 1, 0, 0.0f});
								rhombusrampage_campVec.push_back({{928.0f, 32.0f}, 2, 1, 0.0f});
								rhombusrampage_flags = 0x3;
								break;
							case 2:
								rhombusrampage_player->setPos(512.0f, 256.0f);
								newTex = "assets/textures/greenBg.bmp";
								rhombusrampage_campVec.push_back({{32.0f, 32.0f}, 4, 2, 0.0f});
								rhombusrampage_campVec.push_back({{32.0f, 320.0f}, 8, 3, 0.0f});
								rhombusrampage_campVec.push_back({{928.0f, 32.0f}, 16, 4, 0.0f});
								rhombusrampage_campVec.push_back({{928.0f, 320.0f}, 32, 5, 0.0f});
								break;
							case 3:
								rhombusrampage_player->setPos(512.0f, 256.0f);
								newTex = "assets/textures/blueBg.bmp";
								rhombusrampage_campVec.push_back({{32.0f, 32.0f}, 1, 0, 0.0f});
								rhombusrampage_campVec.push_back({{32.0f, 160.0f}, 2, 1, 0.0f});
								rhombusrampage_campVec.push_back({{32.0f, 288.0f}, 4, 2, 0.0f});
								rhombusrampage_campVec.push_back({{32.0f, 416.0f}, 2, 1, 0.0f});
								rhombusrampage_campVec.push_back({{224.0f, 32.0f}, 1, 0, 0.0f});
								rhombusrampage_campVec.push_back({{224.0f, 416.0f}, 8, 3, 0.0f});
								rhombusrampage_campVec.push_back({{736.0f, 32.0f}, 4, 2, 0.0f});
								rhombusrampage_campVec.push_back({{736.0f, 416.0f}, 32, 5, 0.0f});
								rhombusrampage_campVec.push_back({{928.0f, 32.0f}, 8, 3, 0.0f});
								rhombusrampage_campVec.push_back({{928.0f, 160.0f}, 16, 4, 0.0f});
								rhombusrampage_campVec.push_back({{928.0f, 288.0f}, 32, 5, 0.0f});
								rhombusrampage_campVec.push_back({{928.0f, 416.0f}, 16, 4, 0.0f});
								break;
							case 4:
								rhombusrampage_player->setPos(32.0f, 448.0f);
								newTex = "assets/textures/bossBg.bmp";
								rhombusrampage_flags = 0x7;
								break;
							case 5:
								rhombusrampage_player->setPos(512.0f, 256.0f);
								newTex = "assets/textures/victory.bmp";
								rhombusrampage_flags = 0x40;
								break;
							case 6:
								// reset game
								rhombusrampage_points = 0;
								rhombusrampage_health = 1024;
								rhombusrampage_level = 0;
								rhombusrampage_power = 0;
								rhombusrampage_size = 0;
								rhombusrampage_speed = 0;

								rhombusrampage_flags = 0x10;

								rhombusrampage_bossPrev = 0;
								rhombusrampage_bossTimer = 0;
								rhombusrampage_bossInv = 0;
								rhombusrampage_bossHealth = 256;
								rhombusrampage_bossDir = 0x3;

								rhombusrampage_player->setFrame(0);
								rhombusrampage_vector->setFrame(0);
								rhombusrampage_boss->setFrame(0);

								newTex = "assets/textures/title.bmp";
								break;
							default:
								newTex = "assets/textures/error.bmp";
								rhombusrampage_flags = 0x0;
								break;
						}
						SDL_DestroyTexture(rhombusrampage_bgTex);
						SDL_Surface* sur = SDL_LoadBMP(newTex);
						rhombusrampage_bgTex = SDL_CreateTextureFromSurface(rhombusrampage_renderer, sur);
						SDL_DestroySurface(sur);
						SDL_SetTextureScaleMode(rhombusrampage_bgTex, SDL_SCALEMODE_NEAREST);
						break;
					}
					case SDLK_Z: {
						short pointsNeeded = SDL_pow(4, rhombusrampage_power + 1);
						if (rhombusrampage_power >= 5 || rhombusrampage_points < pointsNeeded) break;
						rhombusrampage_points -= pointsNeeded;
						rhombusrampage_power++;
						rhombusrampage_vector->setFrame(rhombusrampage_power);
						break;
					}
					case SDLK_X: {
						short pointsNeeded = SDL_pow(4, rhombusrampage_size + 1);
						if (rhombusrampage_size >= 5 || rhombusrampage_points < pointsNeeded) break;
						rhombusrampage_points -= pointsNeeded;
						rhombusrampage_size++;
						break;
					}
					case SDLK_C: {
						short pointsNeeded = SDL_pow(4, rhombusrampage_speed + 1);
						if (rhombusrampage_speed >= 5 || rhombusrampage_points < pointsNeeded) break;
						rhombusrampage_points -= pointsNeeded;
						rhombusrampage_speed++;
						rhombusrampage_player->setFrame(rhombusrampage_speed);
						break;
					}
				}
				break;
			case SDL_EVENT_MOUSE_BUTTON_DOWN:
				if (e.button.button != 1) break;
				SDL_ConvertEventToRenderCoordinates(rhombusrampage_renderer, &e);
				if (rhombusrampage_flags & 0x2) {
					if (e.button.y < 480.0f || e.button.y > 544.0f) break;
					if (e.button.x >= 32.0f && e.button.x <= 96.0f && rhombusrampage_power < 5){
						short pointsNeeded = SDL_pow(4, rhombusrampage_power + 1);
						if (rhombusrampage_points < pointsNeeded) break;
						rhombusrampage_points -= pointsNeeded;
						rhombusrampage_power++;
						rhombusrampage_vector->setFrame(rhombusrampage_power);
					} else if (e.button.x >= 352.0f && e.button.x <= 416.0f && rhombusrampage_size < 5) {
						short pointsNeeded = SDL_pow(4, rhombusrampage_size + 1);
						if (rhombusrampage_points < pointsNeeded) break;
						rhombusrampage_points -= pointsNeeded;
						rhombusrampage_size++;
					} else if (e.button.x >= 672.0f && e.button.x <= 736.0f && rhombusrampage_speed < 5) {
						short pointsNeeded = SDL_pow(4, rhombusrampage_speed + 1);
						if (rhombusrampage_points < pointsNeeded) break;
						rhombusrampage_points -= pointsNeeded;
						rhombusrampage_speed++;
						rhombusrampage_player->setFrame(rhombusrampage_speed);
					}
				} else if ((rhombusrampage_flags & 0x10) && e.button.x > 880 && e.button.x < 928 && e.button.y > 448 && e.button.y < 528) rhombusrampage_flags |= 0x20;
				break;
		}
	}
	
	Uint64 crntTick = SDL_GetTicks();
	float delta = (float)(crntTick - rhombusrampage_prevTick) / 1000.0f;
	rhombusrampage_prevTick = crntTick;
	// if (delta > 1.0f / 60.0f) {
	// 	tpf--;
	// } else {
	// 	tpf++;
	// }
	
	SDL_SetRenderDrawColor(rhombusrampage_renderer, 0x00, 0x00, 0x00, 0xff);
	SDL_RenderClear(rhombusrampage_renderer);
	
	// Render Background
	if (rhombusrampage_flags & 0x10) {
		SDL_RenderTexture(rhombusrampage_renderer, rhombusrampage_bgTex, nullptr, nullptr);
	} else {
		SDL_RenderTextureTiled(rhombusrampage_renderer, rhombusrampage_bgTex, nullptr, 32.0f, nullptr);
	}
	
	// Collisions
	if ((rhombusrampage_flags & 0x2) && (SDL_fmodf(rhombusrampage_player->getAngle(), 90.0f) != 45.0f)) {
		// save frequently used information in variables
		float swordW = 17.0f + rhombusrampage_size * 4.25f;
		float swordH = 164.0f + rhombusrampage_size * 41.0f;
		SDL_FPoint playerPos = rhombusrampage_player->getPos();
		
		// calculate invincibility time
		float invTime = 2.0f / (float)(rhombusrampage_speed + 2);
		
		// precalculate trigonometric functions
		float swordRad = (rhombusrampage_player->getAngle() - 45.0f) * SDL_PI_F / 180.0f;
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
			if (rhombusrampage_flags & 0x8) {
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
		for (size_t i = 0; i < rhombusrampage_campVec.size(); i++) {
			// check if camp has invincibility
			if (rhombusrampage_campVec[i].inv > 0) {
				rhombusrampage_campVec[i].inv -= delta;
				continue;
			}

			if (!collideRhombusV(rhombusrampage_campVec[i].pos.x, rhombusrampage_campVec[i].pos.y, swordMin, swordMax, precalculations)) continue;
			
			// No separating axes have been found
			// A collision has occurred
			rhombusrampage_campVec[i].inv = invTime;
			
			rhombusrampage_campVec[i].hp -= (rhombusrampage_power + 1);
			if (rhombusrampage_campVec[i].hp <= 0) {
				rhombusrampage_enemyVec.push_back({
					{rhombusrampage_campVec[i].pos.x + 24.0f, rhombusrampage_campVec[i].pos.y + 16.0f},
					(short)SDL_pow(2, rhombusrampage_campVec[i].lv),
					rhombusrampage_campVec[i].lv,
					0.0f,
					{rhombusrampage_campVec[i].pos.x + (SDL_randf() * 64.0f) - 8.0f, rhombusrampage_campVec[i].pos.y + (SDL_randf() * 64.0f) - 16.0f},
					1.0f,
					SDL_randf() * 0.75f - 0.375f
				});
				rhombusrampage_enemyVec.push_back({
					{rhombusrampage_campVec[i].pos.x + 8.0f, rhombusrampage_campVec[i].pos.y + 48.0f},
					(short)SDL_pow(2, rhombusrampage_campVec[i].lv),
					rhombusrampage_campVec[i].lv,
					0.0f,
					{rhombusrampage_campVec[i].pos.x + (SDL_randf() * 64.0f) - 24.0f, rhombusrampage_campVec[i].pos.y + (SDL_randf() * 64.0f) + 16.0f},
					1.0f,
					SDL_randf() * 0.75f - 0.375f
				});
				rhombusrampage_enemyVec.push_back({
					{rhombusrampage_campVec[i].pos.x + 40.0f, rhombusrampage_campVec[i].pos.y + 48.0f},
					(short)SDL_pow(2, rhombusrampage_campVec[i].lv),
					rhombusrampage_campVec[i].lv,
					0.0f,
					{rhombusrampage_campVec[i].pos.x + (SDL_randf() * 64.0f) + 8.0f, rhombusrampage_campVec[i].pos.y + (SDL_randf() * 64.0f) + 16.0f},
					1.0f,
					SDL_randf() * 0.75f - 0.375f
				});
				rhombusrampage_enemyVec.push_back({
					{rhombusrampage_campVec[i].pos.x + 24.0f, rhombusrampage_campVec[i].pos.y + 80.0f},
					(short)SDL_pow(2, rhombusrampage_campVec[i].lv),
					rhombusrampage_campVec[i].lv,
					0.0f,
					{rhombusrampage_campVec[i].pos.x + (SDL_randf() * 64.0f) - 8.0f, rhombusrampage_campVec[i].pos.y + (SDL_randf() * 64.0f) + 48.0f},
					1.0f,
					SDL_randf() * 0.75f - 0.375f
				});
				rhombusrampage_campVec.erase(rhombusrampage_campVec.begin() + i);
				i--;
			}
		}
		
		// Enemy Collisison
		for (size_t i = 0; i < rhombusrampage_enemyVec.size(); i++) {
			float deltaX = playerPos.x - rhombusrampage_enemyVec[i].pos.x - 8.0f;
			float deltaY = playerPos.y - rhombusrampage_enemyVec[i].pos.y - 16.0f;
			float squareDist = (deltaX * deltaX) + (deltaY * deltaY);
			
			if (squareDist < 196.0f) {
				rhombusrampage_health -= SDL_pow(2, rhombusrampage_enemyVec[i].lv);
				rhombusrampage_enemyVec.erase(rhombusrampage_enemyVec.begin() + i);
				i--;

				if (rhombusrampage_health < 0) {
					SDL_DestroyTexture(rhombusrampage_bgTex);
					SDL_Surface* sur = SDL_LoadBMP("assets/textures/defeat.bmp");
					rhombusrampage_bgTex = SDL_CreateTextureFromSurface(rhombusrampage_renderer, sur);
					SDL_DestroySurface(sur);
					SDL_SetTextureScaleMode(rhombusrampage_bgTex, SDL_SCALEMODE_NEAREST);

					rhombusrampage_flags = 0x80;
				}

				continue;
			}
			
			if (rhombusrampage_enemyVec[i].inv > 0.0f) {
				rhombusrampage_enemyVec[i].inv -= delta;
				continue;
			}
			
			if (squareDist > (swordH * swordH)) continue; // check within an elipse
			
			float swordLeft = swordTan * (deltaX + swordW * swordSin) + swordW * swordCos;
			float swordRight = swordTan * (deltaX - swordW * swordSin) - swordW * swordCos;
			float swordBase = swordNCot * deltaX;
			
			float swordMin = SDL_min(swordLeft, swordRight);
			float swordMax = SDL_max(swordLeft, swordRight);
			
			if (deltaY < swordMin || deltaY > swordMax) continue;
			
			if (SDL_fmodf(rhombusrampage_player->getAngle() - 45.0f, 360.0f) < 180.0f) {
				if (deltaY > swordBase) continue;
			} else {
				if (deltaY < swordBase) continue;
			}
			
			// Collision has occurred
			
			rhombusrampage_enemyVec[i].hp -= (rhombusrampage_power + 1);
			if (rhombusrampage_enemyVec[i].hp > 0) {
				rhombusrampage_enemyVec[i].inv = invTime;
				continue;
			}
			
			rhombusrampage_points += SDL_pow(2, rhombusrampage_enemyVec[i].lv);
			
			if (rhombusrampage_enemyVec[i].lv < 1) {
				rhombusrampage_enemyVec.erase(rhombusrampage_enemyVec.begin() + i);
				i--;
				continue;
			}
			
			rhombusrampage_enemyVec[i].lv--;
			rhombusrampage_enemyVec[i].hp = (short)SDL_pow(2, rhombusrampage_enemyVec[i].lv);
			rhombusrampage_enemyVec[i].animTime = 0.0f;
			rhombusrampage_enemyVec[i].target.x = rhombusrampage_enemyVec[i].pos.x + (SDL_randf() * 64.0f) - 32.0f;
			rhombusrampage_enemyVec[i].target.y = rhombusrampage_enemyVec[i].pos.y + (SDL_randf() * 64.0f) - 32.0f;
			rhombusrampage_enemyVec[i].inv = 1.0f;
			
			rhombusrampage_enemyVec.push_back({
				rhombusrampage_enemyVec[i].pos,
				rhombusrampage_enemyVec[i].hp,
				rhombusrampage_enemyVec[i].lv,
				0.0f,
				{rhombusrampage_enemyVec[i].pos.x + (SDL_randf() * 64.0f) - 32.0f, rhombusrampage_enemyVec[i].pos.y + (SDL_randf() * 64.0f) - 32.0f},
				1.0f,
				SDL_randf() * 0.75f - 0.375f
			});
		}

		// Boss Collision
		// get the center position of the boss
		SDL_FPoint bossPos = rhombusrampage_boss->getPos();
		if (rhombusrampage_bossInv > 0) {
			rhombusrampage_bossInv -= delta;
		} else if (rhombusrampage_flags & 0x8) {
			if (collideRhombusV(bossPos.x - 32.0f, bossPos.y - 64.0f, swordMin, swordMax, precalculations) || collideRhombusH(bossPos.x - 64.0f, bossPos.y - 32.0f, swordMin, swordMax, precalculations)) {
				//collision code
				rhombusrampage_bossInv = invTime;
				rhombusrampage_bossHealth -= rhombusrampage_power + 1;
				if (rhombusrampage_bossHealth <= 0) {
					rhombusrampage_flags = 0x3;
				}
			}
		} else if (rhombusrampage_flags & 0x4) {
			if (collideRhombusV(bossPos.x - 32.0f, bossPos.y - 64.0f, swordMin, swordMax, precalculations)) {
				rhombusrampage_bossInv = 6.0f;
				rhombusrampage_bossTimer = crntTick + 2000u;
			}
		}

		if (rhombusrampage_flags & 0x8) {
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
				if (!rhombusrampage_bossPrev) {
					rhombusrampage_bossPrev = 1;

					rhombusrampage_health -= 256;
					if (rhombusrampage_health <= 0) {
						SDL_DestroyTexture(rhombusrampage_bgTex);
						SDL_Surface* sur = SDL_LoadBMP("assets/textures/defeat.bmp");
						rhombusrampage_bgTex = SDL_CreateTextureFromSurface(rhombusrampage_renderer, sur);
						SDL_DestroySurface(sur);
						SDL_SetTextureScaleMode(rhombusrampage_bgTex, SDL_SCALEMODE_NEAREST);

						rhombusrampage_flags = 0x80;
					} 
				}
			} else {
				rhombusrampage_bossPrev = 0;
			}
		}

		for (CampInstance& camp : rhombusrampage_campVec) {
			SDL_FRect src = {(camp.lv % 3) * 16.0f, ((camp.lv / 3) % 2) * 32.0f, 16.0f, 32.0f};
			SDL_FRect dst = {camp.pos.x, camp.pos.y, 64.0f, 128.0f};
			SDL_RenderTexture(rhombusrampage_renderer, rhombusrampage_campTex, &src, &dst);
		}
		for (size_t i = 0; i < rhombusrampage_enemyVec.size(); i++) {
			if (rhombusrampage_enemyVec[i].animTime < 1.0f) {
				rhombusrampage_enemyVec[i].animTime += delta;
			} else {
				rhombusrampage_enemyVec[i].target.x = playerPos.x - 8.0f;
				rhombusrampage_enemyVec[i].target.y = playerPos.y - 16.0f;
			}
			
			float dx = rhombusrampage_enemyVec[i].target.x - rhombusrampage_enemyVec[i].pos.x;
			float dy = rhombusrampage_enemyVec[i].target.y - rhombusrampage_enemyVec[i].pos.y;
			float dist = delta * (50.0f + 25.0f * rhombusrampage_enemyVec[i].lv);
			
			if (dx * dx + dy * dy > dist * dist) {
				float targetAngle = SDL_atanf(dy / dx);
				float dxSign = (dx < 0.0f) ? -1.0f : 1.0f;
				
				rhombusrampage_enemyVec[i].pos.x += dist * dxSign * SDL_cosf(targetAngle + rhombusrampage_enemyVec[i].bias);
				rhombusrampage_enemyVec[i].pos.y += dist * dxSign * SDL_sinf(targetAngle + rhombusrampage_enemyVec[i].bias);
			}
			
			SDL_FRect src = {(rhombusrampage_enemyVec[i].lv % 3) * 4.0f, ((rhombusrampage_enemyVec[i].lv / 3) % 2) * 8.0f, 4.0f, 8.0f};
			SDL_FRect dst = {rhombusrampage_enemyVec[i].pos.x, rhombusrampage_enemyVec[i].pos.y, 16.0f, 32.0f};
			SDL_RenderTexture(rhombusrampage_renderer, rhombusrampage_enemyTex, &src, &dst);
		}
	}
	
	// Boss
	if (rhombusrampage_flags & 0x8) {
		SDL_FPoint bossPos = rhombusrampage_boss->getPos();
		rhombusrampage_boss->changePos(delta * 320.0f * ((rhombusrampage_bossDir & 0x1) ? 1.0f : -1.0f), delta * 320.0f * ((rhombusrampage_bossDir & 0x2) ? 1.0f : -1.0f));

		if (bossPos.x < 64) {
			rhombusrampage_bossDir |= 0x1u;
		} else if (bossPos.x > 960) {
			rhombusrampage_bossDir &= 0xfeu;
		}
		if (bossPos.y < 64) {
			rhombusrampage_bossDir |= 0x2u;
		} else if (bossPos.y > 512) {
			rhombusrampage_bossDir &= 0xfd;
		}

		if (crntTick > rhombusrampage_bossTimer) {
			rhombusrampage_bossTimer = crntTick + 2000u;
			char enemyLv = SDL_rand(6);
			rhombusrampage_enemyVec.push_back({
				{bossPos.x, bossPos.y},
				(short)SDL_pow(2, enemyLv),
				enemyLv,
				0.0f,
				{bossPos.x + (SDL_randf() * 64.0f) - 8.0f, bossPos.y + (SDL_randf() * 64.0f) - 16.0f},
				1.0f,
				SDL_randf() * 0.75f - 0.375f
			});
		}
		rhombusrampage_boss->render(rhombusrampage_renderer, 4.0f);
	} else if (rhombusrampage_flags & 0x4) {
		char bossFrame = rhombusrampage_boss->getFrame();
		if (rhombusrampage_bossTimer && crntTick > rhombusrampage_bossTimer) {
			rhombusrampage_bossTimer = crntTick + 2000u;
			if (bossFrame == 2) rhombusrampage_flags = 0xb;
			else rhombusrampage_boss->setFrame(bossFrame + 1);
		}
		rhombusrampage_boss->render(rhombusrampage_renderer, 4.0f);
	}

	// Render Player, Sword, and UI
	if (rhombusrampage_flags & 0x1) {
		char dx = (rhombusrampage_keyStates[SDL_SCANCODE_D] || rhombusrampage_keyStates[SDL_SCANCODE_RIGHT]) - (rhombusrampage_keyStates[SDL_SCANCODE_A] || rhombusrampage_keyStates[SDL_SCANCODE_LEFT]);
		char dy = (rhombusrampage_keyStates[SDL_SCANCODE_S] || rhombusrampage_keyStates[SDL_SCANCODE_DOWN]) - (rhombusrampage_keyStates[SDL_SCANCODE_W] || rhombusrampage_keyStates[SDL_SCANCODE_UP]);
		
		rhombusrampage_player->changePos(delta * (float)dx * (float)(100 + 50 * rhombusrampage_speed) * (dy ? 0.75f : 1.0f), delta * (float)dy * (float)(100 + 50 * rhombusrampage_speed) * (dx ? 0.75f : 1.0f));
		SDL_FPoint plrPos = rhombusrampage_player->getPos();
		if (plrPos.x > 1008.0f) {
			rhombusrampage_player->setPosX(1008.0f);
			plrPos.x = 1008.0f;
		} else if (plrPos.x < 16.0f) {
			rhombusrampage_player->setPosX(16.0f);
			plrPos.x = 16.0f;
		}
		if (plrPos.y > 560.0f) {
			rhombusrampage_player->setPosY(560.0f);
			plrPos.y = 560.0f;
		} else if (plrPos.y < 16.0f) {
			rhombusrampage_player->setPosY(16.0f);
			plrPos.y = 16.0f;
		}
		rhombusrampage_vector->setPos(plrPos.x, plrPos.y);
		
		rhombusrampage_player->changeAngle((180.0f + 90.0f * rhombusrampage_speed) * delta);
		rhombusrampage_vector->setAngle(rhombusrampage_player->getAngle());
		rhombusrampage_vector->render(rhombusrampage_renderer, 4.0f + (float)rhombusrampage_size);
		rhombusrampage_player->render(rhombusrampage_renderer, 4.0f);

		// Render UI
		SDL_SetRenderDrawColor(rhombusrampage_renderer, 0x77, 0x77, 0x77, 0xff);
		SDL_RenderFillRects(rhombusrampage_renderer, rhombusrampage_boxBorders, 2);
		SDL_SetRenderDrawColor(rhombusrampage_renderer, 0x00, 0x00, 0x00, 0xff);
		SDL_RenderFillRects(rhombusrampage_renderer, rhombusrampage_boxes, 4);
		
		SDL_RenderTexture(rhombusrampage_renderer, rhombusrampage_upgradeTex, rhombusrampage_upgradeSrc, rhombusrampage_upgradeDst);
		SDL_RenderTexture(rhombusrampage_renderer, rhombusrampage_upgradeTex, rhombusrampage_upgradeSrc + 1, rhombusrampage_upgradeDst + 1);
		SDL_RenderTexture(rhombusrampage_renderer, rhombusrampage_upgradeTex, rhombusrampage_upgradeSrc + 2, rhombusrampage_upgradeDst + 2);
		
		// Generate and Display Text
		{
			std::string str = "Points: " + std::to_string(rhombusrampage_points) + "\nHealth: " + std::to_string(rhombusrampage_health) + "\nLevel: " + std::to_string(rhombusrampage_level);
			TTF_Text* text = TTF_CreateText(rhombusrampage_textEngine, rhombusrampage_statFont, str.c_str(), 0);
			TTF_DrawRendererText(text, 448.0f, 42.0f);
			TTF_DestroyText(text);
		}
		{
			std::string str = "Power Level: " + std::to_string(rhombusrampage_power + 1);
			TTF_Text* text = TTF_CreateText(rhombusrampage_textEngine, rhombusrampage_upgradeFont, str.c_str(), 0);
			TTF_DrawRendererText(text, 128.0f, 504.0f);
			TTF_DestroyText(text);
		}
		{
			std::string str = "Size Level: " + std::to_string(rhombusrampage_size + 1);
			TTF_Text* text = TTF_CreateText(rhombusrampage_textEngine, rhombusrampage_upgradeFont, str.c_str(), 0);
			TTF_DrawRendererText(text, 448.0f, 504.0f);
			TTF_DestroyText(text);
		}
		{
		std::string str = "Speed Level: " + std::to_string(rhombusrampage_speed + 1);
		TTF_Text* text = TTF_CreateText(rhombusrampage_textEngine, rhombusrampage_upgradeFont, str.c_str(), 0);
		TTF_DrawRendererText(text, 768.0f, 504.0f);
		TTF_DestroyText(text);
		}
	}
	
	// Render Information Textbox
	if (rhombusrampage_flags & 0x20) {
		SDL_FRect helpRect = {320.0f, 160.0f, 384.0f, 256.0f};
		SDL_RenderFillRect(rhombusrampage_renderer, &helpRect);
		helpRect.y = 164.0f;
		SDL_RenderTexture(rhombusrampage_renderer, rhombusrampage_helpTex, nullptr, &helpRect);
	}

	SDL_RenderPresent(rhombusrampage_renderer);
}

int main(int argc, char* argv[]) {
	if (!SDL_Init(SDL_INIT_VIDEO)) exit(1);
	if (!TTF_Init()) exit(1);
	
	
	// Uint64 tpf = 16;
	
	rhombusrampage_window = SDL_CreateWindow("Rhombus Rampage", 1024, 576, SDL_WINDOW_RESIZABLE);
	rhombusrampage_renderer = SDL_CreateRenderer(rhombusrampage_window, nullptr);
	rhombusrampage_textEngine = TTF_CreateRendererTextEngine(rhombusrampage_renderer);
	rhombusrampage_statFont = TTF_OpenFont("assets/fonts/RobotoMono-Bold.ttf", 12);
	rhombusrampage_upgradeFont = TTF_OpenFont("assets/fonts/RobotoMono-Bold.ttf", 16);
	rhombusrampage_keyStates = SDL_GetKeyboardState(nullptr);
	rhombusrampage_isFullscreen = 0;
	
	SDL_SetRenderLogicalPresentation(rhombusrampage_renderer, 1024, 576, SDL_LOGICAL_PRESENTATION_LETTERBOX);
	#ifndef __EMSCRIPTEN__
		SDL_SetRenderVSync(rhombusrampage_renderer, 1);
	#endif
	TTF_SetFontStyle(rhombusrampage_statFont, TTF_STYLE_BOLD);
	TTF_SetFontLineSkip(rhombusrampage_statFont, 32);
	TTF_SetFontStyle(rhombusrampage_upgradeFont, TTF_STYLE_BOLD);
	TTF_SetFontLineSkip(rhombusrampage_upgradeFont, 24);
	

	rhombusrampage_points = 0;
	rhombusrampage_health = 1024;
	rhombusrampage_level = 0;
	rhombusrampage_power = 0;
	rhombusrampage_size = 0;
	rhombusrampage_speed = 0;
	rhombusrampage_flags = 0x10;
	/*  0x01 = player, sword, and ui
		0x02 = collisions and enemies
		0x04 = boss animation
		0x08 = boss fight
		0x10 = title screen
		0x20 = information textbox
		0x40 = victory screen
		0x80 = defeat screen
	*/

	rhombusrampage_bossPrev = 0;
	rhombusrampage_bossTimer = 0;
	rhombusrampage_bossInv = 0;
	rhombusrampage_bossHealth = 256;
	rhombusrampage_bossDir = 0x3;
	
	rhombusrampage_player = new Sprite(rhombusrampage_renderer, "assets/textures/player.bmp", 11, 11, 512.0f, 256.0f, 4.5f, 6.5f);
	rhombusrampage_vector = new Sprite(rhombusrampage_renderer, "assets/textures/vector.bmp", 34, 34, 512.0f, 256.0f, 4.5f, 29.5f);
	rhombusrampage_boss = new Sprite(rhombusrampage_renderer, "assets/textures/boss.bmp", 32, 32, 512.0f, 256.0f, 16.0f, 16.0f);

	rhombusrampage_boxBorders[0] = {444.0f, 28.0f, 136.0f, 104.0f};
	rhombusrampage_boxBorders[1] = {28.0f, 476.0f, 968.0f, 72.0f};

	rhombusrampage_boxes[0] = {448.0f, 32.0f, 128.0f, 96.0f};
	rhombusrampage_boxes[1] = {32.0f, 480.0f, 316.0f, 64.0f};
	rhombusrampage_boxes[2] = {352.0f, 480.0f, 316.0f, 64.0f};
	rhombusrampage_boxes[3] = {672.0f, 480.0f, 320.0f, 64.0f};

	rhombusrampage_upgradeSrc[0] =	{0.0f, 0.0f, 16.0f, 16.0f};
	rhombusrampage_upgradeSrc[1] = {16.0f, 0.0f, 16.0f, 16.0f};
	rhombusrampage_upgradeSrc[2] = {32.0f, 0.0f, 16.0f, 16.0f};
	
	rhombusrampage_upgradeDst[0] =	{32.0f, 480.0f, 64.0f, 64.0f};
	rhombusrampage_upgradeDst[1] = {352.0f, 480.0f, 64.0f, 64.0f};
	rhombusrampage_upgradeDst[2] = {672.0f, 480.0f, 64.0f, 64.0f};
			
	
	{
		SDL_Surface* sur = SDL_LoadBMP("assets/textures/title.bmp");
		rhombusrampage_bgTex = SDL_CreateTextureFromSurface(rhombusrampage_renderer, sur);
		SDL_DestroySurface(sur);
		SDL_SetTextureScaleMode(rhombusrampage_bgTex, SDL_SCALEMODE_NEAREST);
	}
	
	{
		SDL_Surface* sur = SDL_LoadBMP("assets/textures/upgrades.bmp");
		rhombusrampage_upgradeTex = SDL_CreateTextureFromSurface(rhombusrampage_renderer, sur);
		SDL_DestroySurface(sur);
		SDL_SetTextureScaleMode(rhombusrampage_upgradeTex, SDL_SCALEMODE_NEAREST);
	}
	
	{
		SDL_Surface* sur = SDL_LoadBMP("assets/textures/enemyCamp.bmp");
		rhombusrampage_campTex = SDL_CreateTextureFromSurface(rhombusrampage_renderer, sur);
		SDL_DestroySurface(sur);
		SDL_SetTextureScaleMode(rhombusrampage_campTex, SDL_SCALEMODE_NEAREST);
	}
	
	{
		SDL_Surface* sur = SDL_LoadBMP("assets/textures/enemy.bmp");
		rhombusrampage_enemyTex = SDL_CreateTextureFromSurface(rhombusrampage_renderer, sur);
		SDL_DestroySurface(sur);
		SDL_SetTextureScaleMode(rhombusrampage_enemyTex, SDL_SCALEMODE_NEAREST);
	}
	
	{
		std::string str = "The Rhombuses are invading, and need to be stopped! You, Circle, are the only one capable. Defeat all the Rhombuses, and level up your abilities. But be warned, the Rhomboss, the king of all Rhombuses, might not appreciate your disturbance of the invasion.";
		SDL_Surface* sur = TTF_RenderText_Blended_Wrapped(rhombusrampage_upgradeFont, str.c_str(), 0, {0xff, 0xff, 0xff, 0xff}, 384);
		rhombusrampage_helpTex = SDL_CreateTextureFromSurface(rhombusrampage_renderer, sur);
		SDL_DestroySurface(sur);
	}
	
	rhombusrampage_prevTick = 0;
	rhombusrampage_running = 1;

    #ifdef __EMSCRIPTEN__
    	emscripten_set_main_loop(mainloop, 0, 1);
    #else
    	while (1) mainloop();
    #endif
}