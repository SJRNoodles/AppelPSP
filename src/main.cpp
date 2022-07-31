#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <img.h>
#include <stdlib.h>
#include <unistd.h>
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



//APPEL PSP - NOODLES 2022
PSP_MODULE_INFO("Appel", 0, 1, 0);
PSP_HEAP_SIZE_KB(-256);

int state = 0;
int running = 1;

double xVel=0;

double tick = 0;

int gravity = 0;

int plrWin = 0;

double multiplyer = 1;

int dir = 1;
int wall = 0;
int walljump = 0;


// level tile variables!
int levelCountX = 512;
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

int gridOff = 0;
int gridOff2 = 0;
int particleAmount = 0;

int titleSelect = 0;

bool isGrounded = false;
bool playerDie = false;

int jumpTick = 25;

int pauseGame = -1;

int spawnX = (levelCountX * 31)-64;
int spawnY = 280;

double x = spawnX;
int y = spawnY;

int nX = x;
int nY = y;

int camX = x;
int camY = y;

// modifiers
int threadDelay = false;
double walkSpeed = 2.7;
int jumpHeight = 30;
int gravityAmount = 2;
int tickSpeed = 2;

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
	double tX = 10;
	double tY = 10;
	double otX = 10;
	double otY = 10;
	int scX = 32;
	int scY = 32;
	int sX = 0;
	int sY = 0;
	int index = 0;
	int type = 0;
	int en = 1;
	double tTick = 0;
	int enemy_dir = 1;
	int defeated = 0;
	int dX;
	int dY;
};

// particles
struct Particle {
public:
	int pX = 10;
	int pY = 10;
	double pXvel = 0;
	double pYvel = 0;
	int scX2 = 32;
	int scY2 = 32;
	int sX2 = 0;
	int sY2 = 0;
	int pLife = 100;
	int pType = 0;
	int vis = 0;
	int pTick = 0;
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
	
	if(in == 13){
		return(tilePos(0));
	}
	if(in == 14){
		return(tilePos(1));
	}
	if(in == 15){
		return(tilePos(0));
	}
	if(in == 16){
		return(tilePos(1));
	}
	if(in == 17){
		return(tilePos(4));
	}
	if(in == 20){
		return(tilePos(2));
	}
	if(in == 21){
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
	
	if(in == 13){
		return(tilePos(3));
	}
	if(in == 14){
		return(tilePos(3));
	}
	if(in == 15){
		return(tilePos(4));
	}
	if(in == 16){
		return(tilePos(4));
	}
	if(in == 17){
		return(tilePos(0));
	}
	if(in == 20){
		return(tilePos(2));
	}
	if(in == 21){
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
		return(3);
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
		return(7);
	}
	if(in == 12){
		return(3);
	}
	if(in == 13){
		return(0);
	}
	if(in == 14){
		return(0);
	}
	if(in == 15){
		return(0);
	}
	if(in == 16){
		return(0);
	}
	if(in == 17){
		return(4);
	}
	if(in == 18){
		return(6);
	}
	if(in == 19){
		return(5);
	}
	if(in == 20){
		return(3);
	}
	if(in == 21){
		return(3);
	}
	if(in == 22){
		return(8);
	}
	if(in == 23){
		return(9);
	}
	if(in == 24){
		return(10);
	}
}

double optionsTest = 0;

int poptions = 0;

Tile lvl[600];
Particle parts[128];

pgeWav* channelSn;
pgeWav* channelSn2;
pgeWav* channelSn3;

auto main() -> int {
	
	//Get from map data
	int lvlData[20480];
	//std::copy(levelDat1,levelDat1+20480,lvlData);
	//level stuff
	tileMasterX = cloneCountX * -16;
	tileMasterR1 = 0;
	tileMasterIndex = 1;
	for (int i = 0; i < cloneCountX; i++) {
		tileMasterY = cloneCountY * -16;
		for (int i2 = 0; i2 < cloneCountY; i2++) {
			lvl[i2+tileMasterR1].tX = tileMasterX;
			lvl[i2+tileMasterR1].tY = tileMasterY;
			lvl[i2+tileMasterR1].otX = tileMasterX;
			lvl[i2+tileMasterR1].otY = tileMasterY;
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
	
	int rootX;
	int rootY;
	int dx;
	int dy;
	int tempE;

	SceCtrlLatch latchData;
	
	scePowerSetClockFrequency(333, 333, 166);
	
	g2dTexture* appel = g2dTexLoad("assets/appel.png",G2D_SWIZZLE);
	g2dTexture* logo = g2dTexLoad("assets/title.png",G2D_SWIZZLE);
	g2dTexture* particleData = g2dTexLoad("assets/particles.png",G2D_SWIZZLE);
	g2dTexture* guiRes = g2dTexLoad("assets/gui.png",G2D_SWIZZLE);
	g2dTexture* levelRes = g2dTexLoad("assets/level.png",G2D_SWIZZLE);
	g2dTexture* mbtRes = g2dTexLoad("assets/menu_bottomText.png",G2D_SWIZZLE);
	g2dTexture* bg = g2dTexLoad("assets/bg.png",G2D_SWIZZLE);
	g2dTexture* bg2 = g2dTexLoad("assets/grid.png",G2D_SWIZZLE);
	g2dTexture* pauseRes = g2dTexLoad("assets/pause_screen.png",G2D_SWIZZLE);
	
	pgeWav* appleSn = pgeWavLoad("assets/audio/crunch.wav");
	pgeWav* apple2Sn = pgeWavLoad("assets/audio/coin.wav");
	//pgeWav* musicSn = pgeWavLoad("assets/audio/music.wav");
	
	// AUDIO INIT
	pgeWavInit();

	channelSn3 = pgeWavLoad("assets/audio/music.wav");
	
	
	SceCtrlData ctrlData;
	
	pgeWavLoop(channelSn3,1); // loops music
	pgeWavPlay(channelSn3);
	
	for (int i = 0; i < cloneCountX * cloneCountY; i++) {
		lvl[i].otX = lvl[i].tX;
		lvl[i].otY = lvl[i].tY;
	}
	
	while(running==1){
		g2dClear(WHITE);
		// bg
		gridOff=-1024;
		gridOff2=1;
		for (int i = 0; i < 4; i++) {
			g2dBeginRects(bg);
			g2dSetColor(WHITE);
			g2dSetScaleWH(512,512);
			g2dSetCoordXY(0 - (round(((int)camX / 4) / 512) * 512 - ((int)camX / 4) + (i * 512)),0 - (round(((int)camY / 4) / 512) * 512 - ((int)camY / 4)));
			g2dAdd();
			g2dEnd();
			gridOff+=512;
		}
		for (int i = 0; i < 4; i++) {
			g2dBeginRects(bg);
			g2dSetColor(WHITE);
			g2dSetScaleWH(512,512);
			g2dSetCoordXY(0 - (round(((int)camX / 4) / 512) * 512 - ((int)camX / 4) + (i * 512)),0 - (round(((int)camY / 4) / 512) * 512 - ((int)camY / 4) + 512));
			g2dAdd();
			g2dEnd();
			gridOff+=512;
		}
		gridOff=-1024;
		gridOff2=1;
		
		if (state == 0) {
			camX++;
			if (camX > 511) {
				camX=0;
			}
			plrWin = 0;
			g2dBeginRects(mbtRes);
			g2dSetColor(WHITE);
			g2dSetScaleWH(480,272);
			g2dSetCoordXY(0,0);
			g2dAdd();
			g2dEnd();
			
			g2dBeginRects(logo);
			g2dSetCoordMode(G2D_CENTER);
			g2dSetColor(WHITE);
			g2dSetScaleWH(256,53);
			g2dSetCoordXY(240,64);
			g2dAdd();
			g2dEnd();
			
			// play button
			g2dBeginRects(guiRes); 
			g2dSetCoordMode(G2D_CENTER);
			if (titleSelect == 0) {
				g2dSetColor(YELLOW);
			}else{
				g2dSetColor(WHITE);
			}
			g2dSetScaleWH(128,35); 
			g2dSetCoordXY(240,164);
			g2dSetCropWH(256,69); 
			g2dSetCropXY(0,0); 
			g2dAdd(); 
			g2dEnd(); 
			
			// options button
			g2dBeginRects(guiRes); 
			g2dSetCoordMode(G2D_CENTER);
			if (titleSelect == 1) {
				g2dSetColor(YELLOW);
			}else{
				g2dSetColor(WHITE);
			}
			g2dSetScaleWH(128,35); 
			g2dSetCoordXY(240,216);
			g2dSetCropWH(256,69); 
			g2dSetCropXY(0,69); 
			g2dAdd(); 
			g2dEnd(); 
			
			sceCtrlReadLatch(&latchData);
			sceCtrlReadBufferPositive(&ctrlData, 1);
			
			if (latchData.uiBreak & PSP_CTRL_DOWN){
				titleSelect++;
				if (titleSelect > 1) {
					titleSelect=0;
				}
			}
			if (latchData.uiBreak & PSP_CTRL_UP){
				titleSelect--;
				if (titleSelect < 0) {
					titleSelect=1;
				}
			}
			if (latchData.uiBreak & PSP_CTRL_CROSS){
				if (titleSelect == 0) {
					std::copy(levelDat2,levelDat2+20480,lvlData);
					gravity = 0;
					xVel = 0;
					//spawnX = (levelCountX * 31)-64;/
					spawnX = 16260;
					spawnY = 280;
					x = spawnX;
					y = spawnY;
					camX = x;
					camY = y;
					state=1;
				}
				if (titleSelect == 1) {
					state=2;
				}
			}
			if (ctrlData.Buttons & PSP_CTRL_SELECT) {
					running = 0;
			}
		}
		if (state == 2) {
			camX=512;

			g2dBeginRects(guiRes);
			g2dSetCoordMode(G2D_CENTER);
			g2dSetColor(WHITE);
			g2dSetScaleWH(256,128);
			g2dSetCoordXY(240,131);
			g2dSetCropWH(256,128); 
			g2dSetCropXY(0,384); 
			g2dAdd();
			g2dEnd();
			
			// options text
			g2dBeginRects(guiRes);
			g2dSetCoordMode(G2D_CENTER);
			g2dSetColor(WHITE);
			g2dSetScaleWH(232,16);
			g2dSetCoordXY(232,108);
			g2dSetCropWH(306,16); 
			g2dSetCropXY(0,352); 
			g2dAdd();
			g2dEnd();
			
			g2dBeginRects(guiRes);
			g2dSetCoordMode(G2D_CENTER);
			g2dSetColor(WHITE);
			g2dSetScaleWH(32,16);
			if (threadDelay) {
				g2dSetCoordXY(0 + 2 * 128,148);
			}else{
				g2dSetCoordXY(0 + 1 * 128,148);
			}
			g2dSetCropWH(32,16); 
			g2dSetCropXY(0,368); 
			g2dAdd();
			g2dEnd();
			
			optionsTest++;
			if (optionsTest>512) {
				optionsTest=0;
			}
			
			sceCtrlReadLatch(&latchData);
			sceCtrlReadBufferPositive(&ctrlData, 1);
			if (latchData.uiBreak & PSP_CTRL_RIGHT){
				threadDelay = true;
			}
			if (latchData.uiBreak & PSP_CTRL_LEFT){
				threadDelay = false;
			}
			if (latchData.uiBreak & PSP_CTRL_CIRCLE){
				state = 0;
			}
			
			g2dBeginRects(appel); 
			g2dSetCoordMode(G2D_CENTER);
			g2dSetScaleWH(12,16); 
			g2dSetCoordXY(164 + sin((optionsTest/16)*multiplyer) * 32,128); 
			g2dSetCropWH(48,64); 
			g2dSetCropXY(0,0); 
			g2dAdd(); 
			g2dEnd();
		}
		if (state == 1) {
			
			for (int i = 0; i < 4; i++) {
				g2dBeginRects(bg2);
				g2dSetColor(WHITE);
				g2dSetScaleWH(512,512);
				g2dSetCoordXY(0 - (round(((int)camX / 2) / 512) * 512 - ((int)camX / 2) + (i * 512)),((int)camY / 2));
				g2dAdd();
				g2dEnd();
				gridOff+=512;
			}
			// LEVEL OUTLINE
			for (int i = 0; i < cloneCountX * cloneCountY; i++) {
				if (lvl[i].en == 1){
					lvl[i].sX = getTileX(lvlData[lvl[i].index]);
					lvl[i].sY = getTileY(lvlData[lvl[i].index]);
					lvl[i].type = getType(lvlData[lvl[i].index]);
						
					// texture
					g2dBeginRects(levelRes); 
					g2dSetCoordMode(G2D_CENTER);
					g2dSetColor(BLACK);
					g2dSetScaleWH(lvl[i].scX + 4,lvl[i].scY + 4);
					//sin(round(tick / 1)) * 5
					if (lvl[i].type != 0){
						g2dSetCoordXY(-64,-64); 
					}
					if (lvl[i].type == 0 || lvl[i].type == 7){
						g2dSetCoordXY((camX-lvl[i].tX)+219,(camY-lvl[i].tY)+121); 
					}
					g2dSetCropXY(lvl[i].sX,lvl[i].sY); 
					g2dSetCropWH(lvl[i].scX,lvl[i].scY);
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
			// LEVEL TEXTURE
			for (int i = 0; i < cloneCountX * cloneCountY; i++) {
					lvl[i].sX = getTileX(lvlData[lvl[i].index]);
					lvl[i].sY = getTileY(lvlData[lvl[i].index]);
					lvl[i].type = getType(lvlData[lvl[i].index]);
					
					if (lvl[i].type == 0 || lvl[i].type == 3){
						lvl[i].en = 1;
						lvl[i].defeated = 0;
					}
					
					if (abs(lvl[i].tX - camX) > (cloneCountX * 16)) {
						if(lvl[i].tX < camX){
							lvl[i].otX += cloneCountX * 32;
							lvl[i].tX = lvl[i].otX;
							lvl[i].index += cloneCountX * levelCountY;
						}else{
							lvl[i].otX += cloneCountX * -32;
							lvl[i].tX = lvl[i].otX;
							lvl[i].index += ((0 - cloneCountX) * levelCountY);
						}
					}
					if (abs(lvl[i].tY - camY) > (cloneCountY * 16)) {
						if(lvl[i].tY < camY){
							lvl[i].otY += cloneCountY * 32;
							lvl[i].tY = lvl[i].otY;
							lvl[i].index += cloneCountY;
						}else{
							lvl[i].otY += cloneCountY * -32;
							lvl[i].tY = lvl[i].otY;
							lvl[i].index += 0 -cloneCountY;
						}
					}
					
					// texture
					if (lvl[i].en == 1){
						g2dBeginRects(levelRes); 
						g2dSetCoordMode(G2D_CENTER);
						g2dSetColor(WHITE);
						g2dSetScaleWH(lvl[i].scX,lvl[i].scY);
						//sin(round(tick / 1)) * 5
						if (lvl[i].type == 1){
							g2dSetCoordXY((camX-lvl[i].tX)+219,(camY-lvl[i].tY)+121+sin((tick / 15)) * 5); 
						}
						if (lvl[i].type == 2){
							g2dSetCoordXY((camX-lvl[i].tX)+219,(camY-lvl[i].tY)+121); 
						}
						if (lvl[i].type != 1 && lvl[i].type != 0 && lvl[i].type !=2 && lvl[i].type !=3){
							g2dSetCoordXY(-64,-64); 
						}
						if (lvl[i].type == 0 || lvl[i].type == 3 || lvl[i].type == 7){
							g2dSetCoordXY((camX-lvl[i].tX)+219,(camY-lvl[i].tY)+121); 
						}
						g2dSetCropXY(lvl[i].sX,lvl[i].sY); 
						g2dSetCropWH(lvl[i].scX,lvl[i].scY);
						if (lvl[i].type == 2){
							g2dSetScaleWH(23 * lvl[i].enemy_dir * -1,33);
							g2dSetCropWH(35,44);
						}
						if (lvl[i].type == 2){
							g2dSetCropXY(226 + round(lvl[i].tTick/38) * 38,0); 
							if (lvl[i].defeated == 0) {
								if (pauseGame == -1) {
									lvl[i].tTick += 12 * multiplyer;
									if(collision((camX-x),(camY-y),(camX-lvl[i].tX)-(128*lvl[i].enemy_dir)+219,(camY-lvl[i].tY)+121-32,24,32,256,128,false,lvl[i].index)){
										lvl[i].tTick += 6 * multiplyer;
										lvl[i].tX += 1 * lvl[i].enemy_dir;
									}
								}
								if(lvl[i].tTick > 105){
									lvl[i].tTick = 0;
									channelSn2 = pgeWavLoad("assets/audio/blip2.wav");
									pgeWavPlay(channelSn2);
									if(!collision((camX-x),(camY-y),(camX-lvl[i].tX)-(128*lvl[i].enemy_dir)+219,(camY-lvl[i].tY)+121-32,24,32,256,128,false,lvl[i].index)){
										lvl[i].enemy_dir = lvl[i].enemy_dir * -1;
									}
								}
								if (pauseGame == -1) {
									lvl[i].tX += 2 * lvl[i].enemy_dir;
								}
							}else{
								g2dSetCropXY(342,0); 
								g2dSetCropWH(44,44);
								if (pauseGame == -1) {
									lvl[i].tTick += 1;
								}
								if (lvl[i].tTick > 12){
									channelSn2 = pgeWavLoad("assets/audio/squish.wav");
									pgeWavPlay(channelSn2);
									
									if (particleAmount <= 127){
										for (int i2 = 0; i2 < 6; i2++) {
											parts[particleAmount].pType = 2;
											parts[particleAmount].pLife = 32;
											parts[particleAmount].pYvel = 5 * multiplyer;
											parts[particleAmount].pXvel = -2+ (rand() % 4 );
											parts[particleAmount].pX = lvl[i].dX;
											parts[particleAmount].pY = lvl[i].dY;
											parts[particleAmount].vis = 1;
											particleAmount++;
										}
									}
									
									if (lvl[i].type == 2){
										//lvl[i].tY = lvl[i].otY;
										//lvl[i].tX = lvl[i].otX;
										lvlData[lvl[i].index] = 2;
										lvl[i].en = 0;
									}
								}
							}
						}
						if (lvl[i].type == 4){
							g2dSetCoordXY((camX-lvl[i].tX)+219,(camY-lvl[i].tY)+121); 
							g2dSetCropXY(lvl[i].sX,lvl[i].sY); 
							g2dSetCropWH(32,47);
							g2dSetScaleWH(32,47);
						}
						if (lvl[i].type == 5){
							g2dSetCoordXY((camX-lvl[i].tX)+219+16,(camY-lvl[i].tY)+121); 
							g2dSetCropXY(0,160); 
							g2dSetCropWH(64,32);
							g2dSetScaleWH(64,32);
							if (pauseGame == -1) {
								lvl[i].tTick += 1 * multiplyer;
								lvl[i].tY = (lvl[i].otY + sin(lvl[i].tTick / (16 / multiplyer))*32);
							}
						}
						if (lvl[i].type == 6){
							g2dSetCoordXY((camX-lvl[i].tX)+219,(camY-lvl[i].tY)+121); 
							g2dSetCropXY(160,0); 
							g2dSetCropWH(32,47);
							g2dSetScaleWH(32,47);
						}
						if (lvlData[lvl[i].index] == 20){
							g2dSetRotation(90);
						}
						if (lvlData[lvl[i].index] == 21){
							g2dSetRotation(-90);
						}
						if (lvl[i].type == 8){
							g2dSetCoordXY((camX-lvl[i].tX)+219+16,(camY-lvl[i].tY)+121); 
							g2dSetCropXY(0,160); 
							g2dSetCropWH(64,32);
							g2dSetScaleWH(64,32);
							
						}
						if (lvl[i].type == 10){
							g2dSetCoordXY((camX-lvl[i].tX)+219+16,(camY-lvl[i].tY)+121); 
							g2dSetCropXY(0,160); 
							g2dSetCropWH(64,32);
							g2dSetScaleWH(64,32);
						}
						g2dAdd();
						g2dEnd(); 
					}
					
					if (lvlData[lvl[i].index] == 13){
						rootX = lvl[i].tX - 219 - 24;
						rootY = lvl[i].tY - 121 - 24;
						dx = x - rootX;
						dy = y - rootY;
						tempE = 1;
						if (abs(dx)>abs(dy)) {
							tempE = (abs(dx)/10)+1;
						}else{
							tempE = (abs(dy)/10)+1;
						}
						g2dBeginRects(levelRes); 
						g2dSetCoordMode(G2D_CENTER);
						g2dSetScaleWH(32,32); 
						g2dSetCoordXY((camX- (rootX + (dx/tempE) ) ),(camY-(rootY + (dy/tempE)) )); 
						g2dSetCropWH(32,32); 
						g2dSetCropXY(64,96); 
						g2dAdd(); 
						g2dEnd();
					}
				///
			}

			if (walljump == 0) {
				xVel += xVel * -0.3;
			}
			if (pauseGame == -1) {
				if (plrWin == 0) {
					x += xVel * multiplyer;
				}
			}
			sceCtrlReadBufferPositive(&ctrlData, 1);
			if (pauseGame == -1) {
				if (playerDie == false){
					if (walljump == 0) {
							if (ctrlData.Buttons & PSP_CTRL_LEFT) {
								dir = -1;
								xVel+=walkSpeed;
								if (isGrounded == true) {
									if (particleAmount <= 127){
										parts[particleAmount].pType = 1;
										parts[particleAmount].pLife = 8;
										parts[particleAmount].pYvel = 1 * multiplyer;
										parts[particleAmount].pXvel = (rand() % 5 - 10) * 1;
										parts[particleAmount].pX = x;
										parts[particleAmount].pY = y - 4;
										parts[particleAmount].vis = 1;
										particleAmount++;
									}
								}
							}
							if (ctrlData.Buttons & PSP_CTRL_RIGHT) {
								dir = 1;
								xVel-=walkSpeed;
								if (isGrounded == true) {
									if (particleAmount <= 127){
										parts[particleAmount].pType = 1;
										parts[particleAmount].pLife = 8;
										parts[particleAmount].pYvel = 1 * multiplyer;
										parts[particleAmount].pXvel = (rand() % 2 - 4) * -1;
										parts[particleAmount].pX = x;
										parts[particleAmount].pY = y - 4;
										parts[particleAmount].vis = 1;
										particleAmount++;
									}
								}
							}
					}
						if (ctrlData.Buttons & PSP_CTRL_UP) {
							//y+=5;
						}
						if (ctrlData.Buttons & PSP_CTRL_DOWN) {
							//y-=5;
						}
				}
			}
				//particles
				if (particleAmount >= 1) {
					for (int i = 0; i < 128; i++) {
						if (parts[i].vis == 1) {
							parts[i].pX += parts[i].pXvel;
							parts[i].pY += parts[i].pYvel;
							if(parts[i].pType == 1){
								g2dBeginRects(particleData); 
								g2dSetCoordMode(G2D_CENTER);
								g2dSetScaleWH(12,12); 
								g2dSetCoordXY((camX-parts[i].pX),(camY-parts[i].pY)); 
								g2dSetCropWH(24,24); 
								g2dSetCropXY(0,0); 
								g2dAdd(); 
								g2dEnd(); 
							}
							
							if(parts[i].pType == 2){
								g2dBeginRects(particleData); 
								g2dSetCoordMode(G2D_CENTER);
								g2dSetScaleWH(12,12); 
								g2dSetCoordXY((camX-parts[i].pX),(camY-parts[i].pY)); 
								g2dSetCropWH(24,24); 
								g2dSetCropXY(24,0); 
								g2dAdd(); 
								g2dEnd(); 
								
								parts[i].pYvel += -0.5 * multiplyer;
							}
							
							// player stuff
							if(parts[i].pType == 3){
								g2dBeginRects(particleData); 
								g2dSetCoordMode(G2D_CENTER);
								g2dSetScaleWH(12,12); 
								g2dSetCoordXY((camX-parts[i].pX),(camY-parts[i].pY)); 
								g2dSetCropWH(24,24); 
								g2dSetCropXY(72,0); 
								g2dAdd(); 
								g2dEnd(); 
								
								parts[i].pYvel += -0.5 * multiplyer;
							}
							if(parts[i].pType == 4){
								g2dBeginRects(particleData); 
								g2dSetCoordMode(G2D_CENTER);
								g2dSetScaleWH(12,12); 
								g2dSetCoordXY((camX-parts[i].pX),(camY-parts[i].pY)); 
								g2dSetCropWH(24,24); 
								g2dSetCropXY(48,0); 
								g2dAdd(); 
								g2dEnd(); 
								
								parts[i].pYvel += -0.5 * multiplyer;
							}
							if(parts[i].pType == 5){
								g2dBeginRects(particleData); 
								g2dSetCoordMode(G2D_CENTER);
								g2dSetScaleWH(12,12); 
								g2dSetCoordXY((camX-parts[i].pX),(camY-parts[i].pY)); 
								g2dSetCropWH(24,24); 
								g2dSetCropXY(96,0); 
								g2dAdd(); 
								g2dEnd(); 
								
								parts[i].pYvel += -0.5 * multiplyer;
							}
							if(parts[i].pType == 6){
								g2dBeginRects(levelRes); 
								g2dSetCoordMode(G2D_CENTER);
								g2dSetScaleWH(16,24); 
								g2dSetCoordXY((camX-parts[i].pX),(camY-parts[i].pY)); 
								g2dSetCropWH(32,47); 
								g2dSetCropXY(160,0); 
								g2dAdd(); 
								g2dEnd(); 
								
								parts[i].pYvel += -0.5 * multiplyer;
				
							}
							
							parts[i].pTick +=1;
							if (parts[i].pTick >= parts[i].pLife){
								particleAmount-=1;
								parts[i].pTick = 0;
								parts[i].vis = 0;
							}
						}
					}
				}
				// player
				if (playerDie == false){
					g2dBeginRects(appel); 
					g2dSetCoordMode(G2D_CENTER);
					g2dSetRotation(wall);
					g2dSetScaleWH(24 * dir,32); 
					g2dSetCoordXY((camX-x),(camY-y + 0)); 
					g2dSetCropWH(48,64); 
					g2dSetCropXY(0,0); 
					g2dAdd(); 
					g2dEnd();
				}			
				
				camX += ((x - camX)/ 10 + 24)*multiplyer;
				camY += ((y - camY)/ 20 - (gravity / 4) + 5)+2*multiplyer;
				if (camX > (levelCountX * 31)-32){
					camX = (levelCountX * 31)-32;
				}
				if (x > (levelCountX * 31)-64){
					x = (levelCountX * 31)-64;
				}
				if (camY < -80){
					camY = -80;
				}
				//camX = x + 219;
				//camY = y + 121;
				if (walljump == 0) {
					wall += wall * -0.1;
				}else{
					wall+=10;
				}
				
				if (pauseGame == -1) {
					gravity+=gravityAmount;
					if (gravity > 25) {
						gravity = 25;
					}
					y-=gravity/2;
				}
				
				tick++;
				
				jumpTick +=1*tickSpeed;
				if (jumpTick >= 15){
					jumpTick = 15;
				}else{
					if (playerDie == false){
						if (ctrlData.Buttons & PSP_CTRL_CROSS) {
							gravity=0-(jumpHeight/1.1)*multiplyer;
						}
					}
				}
				
				if (tick >= 64) {
					if (playerDie == true) {
						channelSn2 = pgeWavLoad("assets/audio/Start.wav");
						pgeWavPlay(channelSn2);
						x = spawnX;
						y = spawnY;
						playerDie = false;
						camX = x;
						camY = y;
					}
				}
				
				if (particleAmount > 126) {
					particleAmount = 0;
				}
				sceCtrlReadLatch(&latchData);
				if (pauseGame == 1 || plrWin == 1){ // if game is paused or won
					g2dSetColor(WHITE);
					g2dBeginRects(pauseRes);
					g2dSetScaleWH(480,272);
					g2dSetCoordXY(0,0);
					g2dAdd();
					g2dEnd();
					
					// play button
					g2dBeginRects(guiRes); 
					g2dSetCoordMode(G2D_CENTER);
					if (titleSelect == 0) {
						g2dSetColor(YELLOW);
					}else{
						g2dSetColor(WHITE);
					}
					g2dSetScaleWH(128,35); 
					g2dSetCoordXY(240,164);
					g2dSetCropWH(256,69); 
					g2dSetCropXY(0,69); 
					g2dAdd(); 
					g2dEnd(); 
					
					// options button
					g2dBeginRects(guiRes); 
					g2dSetCoordMode(G2D_CENTER);
					if (titleSelect == 1) {
						g2dSetColor(YELLOW);
					}else{
						g2dSetColor(WHITE);
					}
					g2dSetScaleWH(128,35); 
					g2dSetCoordXY(240,216);
					g2dSetCropWH(256,69); 
					g2dSetCropXY(0,137); 
					g2dAdd(); 
					g2dEnd(); 
					
					if (latchData.uiBreak & PSP_CTRL_CROSS) {
						if (titleSelect == 0) {
							poptions = 1;
						}
						if (titleSelect == 1) {
							pauseGame = -1;
							state = 0;
						}
					}
					
					if (poptions == 1) {
						g2dBeginRects(guiRes);
						g2dSetCoordMode(G2D_CENTER);
						g2dSetColor(WHITE);
						g2dSetScaleWH(256,128);
						g2dSetCoordXY(240,131);
						g2dSetCropWH(256,128); 
						g2dSetCropXY(0,384); 
						g2dAdd();
						g2dEnd();
						
						// options text
						g2dBeginRects(guiRes);
						g2dSetCoordMode(G2D_CENTER);
						g2dSetColor(WHITE);
						g2dSetScaleWH(232,16);
						g2dSetCoordXY(232,108);
						g2dSetCropWH(306,16); 
						g2dSetCropXY(0,352); 
						g2dAdd();
						g2dEnd();
						
						g2dBeginRects(guiRes);
						g2dSetCoordMode(G2D_CENTER);
						g2dSetColor(WHITE);
						g2dSetScaleWH(32,16);
						if (threadDelay) {
							g2dSetCoordXY(0 + 2 * 128,148);
						}else{
							g2dSetCoordXY(0 + 1 * 128,148);
						}
						g2dSetCropWH(32,16); 
						g2dSetCropXY(0,368); 
						g2dAdd();
						g2dEnd();
						
						optionsTest++;
						if (optionsTest>512) {
							optionsTest=0;
						}
						if (latchData.uiBreak & PSP_CTRL_RIGHT){
							threadDelay = true;
						}
						if (latchData.uiBreak & PSP_CTRL_LEFT){
							threadDelay = false;
						}
						if (latchData.uiBreak & PSP_CTRL_CIRCLE){
							poptions = 0;
						}
						
						g2dBeginRects(appel); 
						g2dSetCoordMode(G2D_CENTER);
						g2dSetScaleWH(12,16); 
						g2dSetCoordXY(164 + sin((optionsTest/16)*multiplyer) * 32,128); 
						g2dSetCropWH(48,64); 
						g2dSetCropXY(0,0); 
						g2dAdd(); 
						g2dEnd();
					}
				}
				
				// level collisions (walls and floors and enemys and stuff) :3
				isGrounded = false;
				for (int i = 0; i < cloneCountX * cloneCountY; i++) {
					// collisionn
					if(lvlData[lvl[i].index] > 2 ){
						
						// 1 block collisions
						if(collision((camX-x),(camY-y),(camX-lvl[i].tX)+219,(camY-lvl[i].tY+121),24,32,32,32,false,lvlData[lvl[i].index])){
							if(lvl[i].en == 1){
								if(lvl[i].type == 4){
									if (plrWin == 0) {
										channelSn2 = pgeWavLoad("assets/audio/Complete.wav");
										pgeWavPlay(channelSn2);
										plrWin = 1;
									}
								}
								if(lvl[i].type == 3){
									if (playerDie == false && plrWin == 0) {
										if(collision((camX-x),(camY-y),(camX-lvl[i].tX)+219,(camY-lvl[i].tY+121),24,32,16,16,false,lvlData[lvl[i].index])){
											channelSn2 = pgeWavLoad("assets/audio/boom.wav");
											pgeWavPlay(channelSn2);
											tick = 0;
											playerDie = true;
											for (int i2 = 0; i2 < 2; i2++) {
												if (particleAmount <= 127){
													parts[particleAmount].pType = 4;
													parts[particleAmount].pLife = 128;
													parts[particleAmount].pYvel = 5+ (rand() % 7 );
													parts[particleAmount].pXvel = -4+ (rand() % 8 );
													parts[particleAmount].pX = x;
													parts[particleAmount].pY = y;
													parts[particleAmount].vis = 1;
													particleAmount++;
												}
											}
											for (int i2 = 0; i2 < 5; i2++) {
												if (particleAmount <= 127){
													parts[particleAmount].pType = 3;
													parts[particleAmount].pLife = 128;
													parts[particleAmount].pYvel = 5+ (rand() % 7 );
													parts[particleAmount].pXvel = -4+ (rand() % 8 );
													parts[particleAmount].pX = x;
													parts[particleAmount].pY = y;
													parts[particleAmount].vis = 1;
													particleAmount++;
												}
											}
										}
									}
								}
								if(lvl[i].type == 2){
									if (gravity >= 4){
										if(lvl[i].type == 2){
											lvl[i].tTick = 0;
											lvl[i].dX = x;
											lvl[i].dY = y;
											channelSn2 = pgeWavLoad("assets/audio/enemy_defeat.wav");
											pgeWavPlay(channelSn2);
											lvl[i].defeated = 1;
											y-=16;
											
											gravity=-32*multiplyer;
											if (ctrlData.Buttons & PSP_CTRL_CROSS) {
												jumpTick = 0;
												gravity=-32*multiplyer;
												y-=gravity;
											}
											jumpTick=0;
										}
									}else{
										if (playerDie == false && plrWin == 0) {
											if (lvl[i].defeated == 0) {
												channelSn2 = pgeWavLoad("assets/audio/boom.wav");
												pgeWavPlay(channelSn2);
												tick = 0;
												playerDie = true;
												for (int i2 = 0; i2 < 2; i2++) {
													if (particleAmount <= 127){
														parts[particleAmount].pType = 4;
														parts[particleAmount].pLife = 128;
														parts[particleAmount].pYvel = 5+ (rand() % 7 );
														parts[particleAmount].pXvel = -4+ (rand() % 8 );
														parts[particleAmount].pX = x;
														parts[particleAmount].pY = y;
														parts[particleAmount].vis = 1;
														particleAmount++;
													}
												}
												for (int i2 = 0; i2 < 5; i2++) {
													if (particleAmount <= 127){
														parts[particleAmount].pType = 3;
														parts[particleAmount].pLife = 128;
														parts[particleAmount].pYvel = 5+ (rand() % 7 );
														parts[particleAmount].pXvel = -4+ (rand() % 8 );
														parts[particleAmount].pX = x;
														parts[particleAmount].pY = y;
														parts[particleAmount].vis = 1;
														particleAmount++;
													}
												}
											}
										}
									}
								}
								if(lvl[i].type == 6){ // checkpoint
									if (lvl[i].defeated == 0) {
										channelSn2 = pgeWavLoad("assets/audio/Flag.wav");
										pgeWavPlay(channelSn2);
										lvl[i].defeated = 1;
										spawnX = x;
										spawnY = y + 8;
										for (int i2 = 0; i2 < 8; i2++) {
											if (particleAmount <= 127){
												parts[particleAmount].pType = 6;
												parts[particleAmount].pLife = 32;
												parts[particleAmount].pYvel = 5+ (rand() % 7 );
												parts[particleAmount].pXvel = -4+ (rand() % 8 );
												parts[particleAmount].pX = x;
												parts[particleAmount].pY = y;
												parts[particleAmount].vis = 1;
												particleAmount++;
											}
										}
									}
								}
								if(lvl[i].type == 1){
									if(lvl[i].type == 1){
										
										lvl[i].en = 0;
										lvlData[lvl[i].index] = 2;
										pgeWavPlay(appleSn);
										pgeWavPlay(apple2Sn);
									}
								}
								// || lvl[i].type == 5 || lvl[i].type == 8
								if(lvl[i].type == 0 ){
									isGrounded = true;
										if(gravity < -1){
											gravity = 1;
										}else{
											gravity = 1;
											// controls part 2 :D
											if (ctrlData.Buttons & PSP_CTRL_CROSS) {
												jumpTick = 0;
												gravity=-24*multiplyer;
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
											if (playerDie == false){
												if (ctrlData.Buttons & PSP_CTRL_CROSS) {
													channelSn = pgeWavLoad("assets/audio/wall-jump.wav");
													pgeWavPlay(channelSn);
													gravity=2-jumpHeight*multiplyer;
													dir = dir * -1;
													xVel = (dir) * -7;
													x += xVel;
													walljump = 1;
													y-=gravity+6;
												}
											}
										}
								}
							}
						}else{
							if (lvl[i].type == 8){
								lvl[i].tY +=2;
								if (lvl[i].tY >= lvl[i].otY) {
									lvl[i].tY = lvl[i].otY; //
								}
							}
							if (lvl[i].type == 10){
								lvl[i].tX +=2;
								if (lvl[i].tX >= lvl[i].otX) {
									lvl[i].tX = lvl[i].otX; //
								}
							}
						}
						if(lvl[i].type == 8){
							// |
							// v
							if(collision((camX-x),(camY-y),(camX-lvl[i].tX)+219,(camY-lvl[i].tY+121+8),24,32,64,16,false,lvlData[lvl[i].index])){
								lvl[i].tY -=12;
								camY-=12;
								for (int i2 = 0; i2 < cloneCountX * cloneCountY; i2++) {
									if(lvlData[lvl[i2].index] > 2 ){
										if(lvl[i2].type == 9 ){
											if(collision((camX-lvl[i2].tX)+219,(camY-lvl[i2].tY+121),(camX-lvl[i].tX)+219-8,(camY-lvl[i].tY+121),32,32,32,16,false,lvlData[lvl[i2].index])){
												lvl[i].tY +=12;
											}
										}
									}
								}
								
								nY = lvl[i].tY - 96;
								nY += 4;
								isGrounded = true;
								if(gravity < -1){
									gravity = 1;
								}else{
									gravity = 1;
									// controls part 2 :D
									if (ctrlData.Buttons & PSP_CTRL_CROSS) {
										jumpTick = 0;
										gravity=-14*multiplyer;
										y-=gravity;
										channelSn = pgeWavLoad("assets/audio/jump.wav");
										pgeWavPlay(channelSn);
									}
								}
								y = nY;
							}
						}
						if(lvl[i].type == 10){
							// ->
							if(collision((camX-x),(camY-y),(camX-lvl[i].tX)+219,(camY-lvl[i].tY+121+8),24,32,64,16,false,lvlData[lvl[i].index])){
								lvl[i].tX -=12;
								x -=12;
								camX-=12;
								for (int i2 = 0; i2 < cloneCountX * cloneCountY; i2++) {
									if(lvlData[lvl[i2].index] > 2 ){
										if(lvl[i2].type == 9 ){
											if(collision((camX-lvl[i2].tX)+219,(camY-lvl[i2].tY+121),(camX-lvl[i].tX)+219-8,(camY-lvl[i].tY+121),32,32,32,16,false,lvlData[lvl[i2].index])){
												lvl[i].tX +=12;
											}
										}
									}
								}
								//camX = x+219;
								nY = lvl[i].tY - 96;
								nY += 0;
								isGrounded = true;
								if(gravity < -1){
									gravity = 1;
								}else{
									gravity = 1;
									// controls part 2 :D
									if (ctrlData.Buttons & PSP_CTRL_CROSS) {
										jumpTick = 0;
										gravity=-14*multiplyer;
										y-=gravity;
										channelSn = pgeWavLoad("assets/audio/jump.wav");
										pgeWavPlay(channelSn);
									}
								}
								y = nY;
							}
						}
						if(lvl[i].type == 5){
							if(collision((camX-x),(camY-y),(camX-lvl[i].tX)+219,(camY-lvl[i].tY+121),24,32,128,32,true,lvlData[lvl[i].index])){
								nY = lvl[i].tY - 96;
								nY += 8;
								isGrounded = true;
								if(gravity < -1){
									gravity = 1;
								}else{
									gravity = 1;
									// controls part 2 :D
									if (ctrlData.Buttons & PSP_CTRL_CROSS) {
										jumpTick = 0;
										gravity=-10*multiplyer;
										y-=gravity;
										channelSn = pgeWavLoad("assets/audio/jump.wav");
										pgeWavPlay(channelSn);
									}
								}
								y = nY;
							}
						}
					}
				}

				if (latchData.uiBreak & PSP_CTRL_START) {
					titleSelect = 0;
					pauseGame = pauseGame * -1;
				}
				if (latchData.uiBreak & PSP_CTRL_DOWN){
				titleSelect++;
				if (titleSelect > 1) {
					titleSelect=0;
				}
			}
				if (latchData.uiBreak & PSP_CTRL_UP){
					titleSelect--;
					if (titleSelect < 0) {
						titleSelect=1;
					}
				}
				
				nX = x;
				nY = y;
		}
			g2dFlip(G2D_VSYNC);
			if (threadDelay) {
				sceKernelDelayThread(15 * 1000);
			}
	}
	// wrap up code
	pgeWavStopAll();
	sceKernelExitGame();
	return 0;
}