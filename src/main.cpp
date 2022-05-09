#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <img.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <pspaudio.h>
#include <pspaudiolib.h>
#include <psppower.h>
#include "pgeWav.h"

#include <algorithm> 

#include <pspgu.h>
#include <glib2d.h>

// level
#include "dm1.h"



//APPEL FOR PSP BY NOODLES 2022
PSP_MODULE_INFO("appel", 0, 1, 0);
PSP_HEAP_SIZE_KB(-256);

int state = 0;
int running = 1;

int xVel=0;
int x = 2629;
int y = -180;

int tick = 0;

int gravity = 0;


int nX = x;
int nY = y;

int camX = x;
int camY = y;

int dir = 1;
int wall = 0;
int walljump = 0;


// level tile variables!
int levelCountX = 100;
int levelCountY = 40;


int cloneCountY=11;
int cloneCountX=17;
int cloneNumberX=0; // for loops are janky
int cloneNumberY=0; // for loops are janky
int cloneNumberOff=0; // for loops are janky

int tileMasterX;
int tileMasterY;
int tileMasterR1;
int tileMasterIndex = 0;

// collision (useful!)
bool collision(int x1,int y1,int x2,int y2,int w1,int h1,int w2, int h2,bool debug,int sx){
	if (debug==true) {
		if (sx>2) {
			g2dBeginRects(NULL);
			g2dSetColor(BLUE);	
			g2dSetScaleWH(w1,h1);
			g2dSetCoordXY(x1,y1); 
			g2dAdd();
			g2dEnd(); 
			g2dBeginRects(NULL);
			g2dSetColor(RED);	
			g2dSetScaleWH(w2/2,h2/2);
			g2dSetCoordXY(x2,y2); 
			g2dAdd();
			g2dEnd(); 
		}
	}
	return (x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && h1 + y1 > y2);
}

bool inRange(unsigned low, unsigned high, unsigned x){        
 return (low <= x && x <= high);         
} 

int tilePos(int x){
	return(x * 32);
}

// tiles
struct Tile {
public:
	int tX = 10;
	int tY = 10;
	int scX = 32;
	int scY = 32;
	int sX = 0;
	int sY = 0;
	int index = 0;
	int type = 0;
	int en = 1;
	int tTick = 0;
	int enemy_dir = 1;
	int defeated = 0;
};

int getTileX(int in){
	if(in == 2){
		return(tilePos(0));
	}
	if(in == 3){
		return(tilePos(1));
	}
	if(in == 4){
		return(tilePos(2));
	}
	if(in == 5){
		return(tilePos(3));
	}
	if(in == 6){
		return(tilePos(1));
	}
	if(in == 7){
		return(tilePos(0));
	}
	if(in == 8){
		return(tilePos(1));
	}
	if(in == 9){
		return(tilePos(0));
	}
	if(in == 10){
		return(tilePos(1));
	}
	if(in == 11){
		return(tilePos(2));
	}
	if(in == 12){
		return(tilePos(2));
	}
}

int getTileY(int in){
	if(in == 2){
		return(tilePos(0));
	}
	if(in == 3){
		return(tilePos(0));
	}
	if(in == 4){
		return(tilePos(0));
	}
	if(in == 5){
		return(tilePos(0));
	}
	if(in == 6){
		return(tilePos(1));
	}
	if(in == 7){
		return(tilePos(1));
	}
	if(in == 8){
		return(tilePos(1));
	}
	if(in == 9){
		return(tilePos(2));
	}
	if(in == 10){
		return(tilePos(2));
	}
	if(in == 11){
		return(tilePos(1));
	}
	if(in == 12){
		return(tilePos(2));
	}
}

int getType(int in){
	if(in == 2){
		return(0);
	}
	if(in == 3){
		return(0);
	}
	if(in == 4){
		return(0);
	}
	if(in == 5){
		return(1);
	}
	if(in == 6){
		return(2);
	}
	if(in == 7){
		return(0);
	}
	if(in == 8){
		return(0);
	}
	if(in == 9){
		return(0);
	}
	if(in == 10){
		return(0);
	}
	if(in == 11){
		return(0);
	}
	if(in == 12){
		return(0);
	}
}

auto main() -> int {
	
	//Get from map data
	int lvlData[4000];
	std::copy(mapData,mapData+4000,lvlData);
	for (int i = 0; i < cloneCountX * cloneCountY; i++) {
		if(lvlData[i] < 2){
			lvlData[i]=2;
		}
	}
	
	Tile lvl[600];
	//level stuff
	tileMasterX = cloneCountX * -16;
	tileMasterR1 = 0;
	tileMasterIndex = 1;
	for (int i = 0; i < cloneCountX; i++) {
		tileMasterY = cloneCountY * -16;
		for (int i2 = 0; i2 < cloneCountY; i2++) {
			lvl[i2+tileMasterR1].tX = tileMasterX;
			lvl[i2+tileMasterR1].tY = tileMasterY;
			lvl[i2+tileMasterR1].index = tileMasterIndex;
			lvl[i2+tileMasterR1].sX = getTileX(lvl[i2+tileMasterR1].index);
			lvl[i2+tileMasterR1].sY = getTileY(lvl[i2+tileMasterR1].index);
			lvl[i2+tileMasterR1].type = getType(lvlData[lvl[i2+tileMasterR1].index]);
			tileMasterY+=32;
			tileMasterIndex+=1;
		}
		tileMasterR1 += cloneCountY;
		tileMasterX+=32;
		tileMasterIndex += (levelCountY - cloneCountY);
	}

	SceCtrlLatch latchData;
	
	g2dTexture* appel = g2dTexLoad("assets/appel.png",G2D_SWIZZLE);
	g2dTexture* levelRes = g2dTexLoad("assets/level.png",G2D_SWIZZLE);
	g2dTexture* bg = g2dTexLoad("assets/bg.png",G2D_SWIZZLE);
	
	pgeWav* appleSn = pgeWavLoad("assets/audio/crunch.wav");
	pgeWav* apple2Sn = pgeWavLoad("assets/audio/coin.wav");
	pgeWav* enemySn = pgeWavLoad("assets/audio/blip2.wav");
	//pgeWav* musicSn = pgeWavLoad("assets/audio/music.wav");
	
	pgeWav* channelSn;
	pgeWav* channelSn2;
	pgeWav* channelSn3 = pgeWavLoad("assets/audio/music.wav");
	
	SceCtrlData ctrlData;
	
	// AUDIO INIT
	pgeWavInit();
	
	pgeWavLoop(channelSn3,1); // loops music
	pgeWavPlay(channelSn3);
	
	while(running==1){
		g2dClear(WHITE);
		g2dBeginRects(bg);
		g2dSetColor(WHITE);
		g2dSetScaleWH(480,272);
		g2dSetCoordXY(0,0);
		g2dAdd();
		g2dEnd();
		
		for (int i = 0; i < cloneCountX * cloneCountY; i++) {
			if (lvl[i].en == 1){
				lvl[i].sX = getTileX(lvlData[lvl[i].index]);
				lvl[i].sY = getTileY(lvlData[lvl[i].index]);
				lvl[i].type = getType(lvlData[lvl[i].index]);
				
				if (abs(lvl[i].tX - camX) > (cloneCountX * 16)) {
					if(lvl[i].tX < camX){
						lvl[i].tX += cloneCountX * 32;
						lvl[i].index += cloneCountX * levelCountY;
					}else{
						lvl[i].tX += cloneCountX * -32;
						lvl[i].index += 0 - (cloneCountX * levelCountY);
					}
				}
				if (abs(lvl[i].tY - camY) > (cloneCountY * 16)) {
					if(lvl[i].tY < camY){
						lvl[i].tY += cloneCountY * 32;
						lvl[i].index += cloneCountY;
					}else{
						lvl[i].tY += cloneCountY * -32;
						lvl[i].index += 0 -cloneCountY;
					}
				}
				//outline
				
				// texture
				g2dBeginRects(levelRes); 
				g2dSetCoordMode(G2D_CENTER);
				g2dSetScaleWH(lvl[i].scX,lvl[i].scY);
				//sin(round(tick / 1)) * 5
				if (lvl[i].type == 0){
					g2dSetCoordXY((camX-lvl[i].tX)+219,(camY-lvl[i].tY)+121); 
				}
				if (lvl[i].type == 1){
					g2dSetCoordXY((camX-lvl[i].tX)+219,(camY-lvl[i].tY)+121+sin((tick / 15)) * 5); 
				}
				if (lvl[i].type == 2){
					g2dSetCoordXY((camX-lvl[i].tX)+219,(camY-lvl[i].tY)+121); 
				}
				if (lvl[i].type != 1 && lvl[i].type != 0 && lvl[i].type !=2){
					//g2dSetCoordXY(-64,-64); 
				}
				g2dSetCropXY(lvl[i].sX,lvl[i].sY); 
				if (lvl[i].type == 2){
					g2dSetCropXY(226 + round(lvl[i].tTick/38) * 38,0); 
					lvl[i].tTick += 6;
					if(lvl[i].tTick > 105){
						lvl[i].tTick = 0;
						pgeWavPlay(enemySn);
						lvl[i].enemy_dir = lvl[i].enemy_dir * -1;
					}
					lvl[i].tX += 2 * lvl[i].enemy_dir;
				}
				g2dSetCropWH(lvl[i].scX,lvl[i].scY);
				if (lvl[i].type == 2){
					g2dSetScaleWH(23 * lvl[i].enemy_dir * -1,33);
					g2dSetCropWH(35,44);
				}
				g2dAdd();
				g2dEnd(); 
				//if (lvl[i].type == 2){
				//	g2dBeginRects(levelRes); 
				//	g2dSetCoordMode(G2D_CENTER);
				//	g2dSetScaleWH(17,22); 
				//	g2dSetCoordXY((camX-lvl[i].tX)+219,(camY-lvl[i].tY)+121); 
				//	g2dSetCropWH(35,44); 
				//	g2dSetCropXY(226,0); 
				//	g2dAdd(); 
				//	g2dEnd(); 
				//}
			}
		}
		if (camX > 2629){
			camX = 2629;
		}
		if (x > 2629){
			x = 2629;
		}
		if (camY < -80){
			camY = -80;
		}
		if (walljump == 0) {
			xVel += xVel * -0.3;
		}
		x += xVel;
		sceCtrlReadBufferPositive(&ctrlData, 1);
		if (walljump == 0) {
			if (ctrlData.Buttons & PSP_CTRL_LEFT) {
				dir = -1;
				xVel+=3;
			}
			if (ctrlData.Buttons & PSP_CTRL_RIGHT) {
				dir = 1;
				xVel-=3;
			}
		}
			if (ctrlData.Buttons & PSP_CTRL_UP) {
				//y+=5;
			}
			if (ctrlData.Buttons & PSP_CTRL_DOWN) {
				y-=5;
			}
			
			g2dBeginRects(appel); 
			g2dSetCoordMode(G2D_CENTER);
			g2dSetRotation(wall);
			g2dSetScaleWH(24 * dir,32); 
			g2dSetCoordXY((camX-x),(camY-y + 0)); 
			g2dSetCropWH(48,64); 
			g2dSetCropXY(0,0); 
			g2dAdd(); 
			g2dEnd(); 
			
			camX += (x - camX)/ 10 + 24;
			camY += (y - camY)/ 20 - (gravity / 4) + 5;
			if (walljump == 0) {
				wall += wall * -0.1;
			}else{
				wall+=10;
			}
			
			gravity+=1;
			if (gravity > 25) {
				gravity = 25;
			}
			y-=gravity/2;
			
			tick++;
			
			// level collisions
			for (int i = 0; i < cloneCountX * cloneCountY; i++) {
				// collisionn
				if(lvlData[lvl[i].index] > 2){
					if(collision((camX-x),(camY-y),(camX-lvl[i].tX)+219,(camY-lvl[i].tY+121),24,32,32,32,false,lvlData[lvl[i].index])){
						if(lvl[i].en == 1){
							if(lvl[i].type == 2){
								if (gravity >= 7){
									channelSn2 = pgeWavLoad("assets/audio/enemy_defeat.wav");
									pgeWavPlay(channelSn2);
									lvl[i].defeated = 1;
									lvl[i].en = 0;
									gravity=-35;
								}else{
									x = 2629;
									y = -80;
								}
							}
							if(lvl[i].type == 1){
								lvl[i].en = 0;
								pgeWavPlay(appleSn);
								pgeWavPlay(apple2Sn);
							}
							if(lvl[i].type == 0){
									if(gravity < -1){
										gravity = 1;
									}else{
										gravity = 1;
										// controls part 2 :D
										if (ctrlData.Buttons & PSP_CTRL_CROSS) {
											gravity=-25;
											y-=gravity;
											channelSn = pgeWavLoad("assets/audio/jump.wav");
											pgeWavPlay(channelSn);
										}
									}
									walljump = 0;
									y = nY;
										
									if(collision((camX-x),(camY-y),(camX-lvl[i].tX)+219,(camY-lvl[i].tY)+121,24,32,32,32,false,lvl[i].index)){
										x=nX;
										wall = 90 * dir;
										if (ctrlData.Buttons & PSP_CTRL_CROSS) {
											channelSn = pgeWavLoad("assets/audio/wall-jump.wav");
											pgeWavPlay(channelSn);
											gravity=-25;
											dir = dir * -1;
											xVel = (dir) * -5;
											walljump = 1;
											y-=gravity;
										}
									}
							}
						}
					}
				}
			}
			nX = x;
			nY = y;
			g2dFlip(G2D_VSYNC);
	}
		
	sceKernelExitGame();
	return 0;
	
	
}