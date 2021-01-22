/*
 * GfxProc.cpp
 *
 *  Created on: Oct 31, 2020
 *      Author: camer
 */

#include "GfxProc.h"
#include <stdio.h>

GfxProc::GfxProc(int width, int height, int scale) {
	winWidth = width;
	winHeight = height;
	sc = scale;
	//Start SDL and create window from paramaters
	SDL_Init(SDL_INIT_VIDEO);
	win = SDL_CreateWindow("CHIP-8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, winWidth, winHeight, SDL_WINDOW_SHOWN);
	//global variable to edit the surface within other methods
	sur = SDL_GetWindowSurface(win);
}

/**
 * Take an array as an input
 * go through each array element and draw a 1x1 rectangle multiplied by scale
 * elements define coordintaes
 * 1 = draw
 * 0 = clear
 */
void GfxProc::renderSurface(unsigned char c[64][32]){
	//SDL_FillRect(sur, NULL, 0x000000);
	for(int x = 0; x < 64; x++){
		for(int y =0; y < 32; y++){
			if(c[x][y] > 0){
				SDL_Rect rect;
				rect.x = x*sc;
				rect.y = y*sc;
				rect.w = sc;
				rect.h = sc;
				//this is where to change color of the pixels
				SDL_FillRect(sur, &rect, SDL_MapRGB(sur->format,255,255,0));
			}
			else{
				SDL_Rect rect;
				rect.x = x*sc;
				rect.y = y*sc;
				rect.w = sc;
				rect.h = sc;
				SDL_FillRect(sur, &rect, SDL_MapRGB(sur->format,0,0,0));
			}
		}
	}
	//update the surface
	SDL_UpdateWindowSurface(win);
}

GfxProc::~GfxProc(){
	//exit SDL
	SDL_DestroyWindow(win);
	SDL_Quit();
}
