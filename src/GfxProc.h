/*
 * GfxProc.h
 *
 *  Created on: Oct 31, 2020
 *      Author: camer
 */

#ifndef GFXPROC_H_
#define GFXPROC_H_
#include <SDL2/SDL.h>

class GfxProc {
private:
	int winWidth;
	int winHeight;
	int sc;
	SDL_Surface* sur;
	SDL_Window* win;


public:
	GfxProc(int width, int height, int scale);
	void renderSurface(unsigned char c[64][32]);
	~GfxProc();
};

#endif /* GFXPROC_H_ */
