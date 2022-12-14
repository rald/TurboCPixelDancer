#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <time.h>
#include <math.h>
#include <dos.h>
#include <mem.h>

#include "typedefs.h"

#include "stdbool.h"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200
#define SCREEN_SIZE (SCREEN_WIDTH*SCREEN_HEIGHT)

#define INPUT_STATUS_1 0x03DA
#define VRETRACE       0x08

void vsync(void);

void SetMode(unsigned char mode);
void SetPalette(byte idx,byte r,byte g,byte b);
void DrawPoint(byte *srf,int x,int y,int c);
void DrawLine(byte *srf,int x0,int y0,int x1,int y1,int c);
void FillRect(byte *srf,int x,int y,int w,int h,int c);
void DrawRect(byte *srf,int x,int y,int w,int h,int c);

void DrawCircle(byte *srf, int xc, int yc, int r, int c);
void FillCircle(byte *srf, int xc, int yc, int r, int c);

bool inrect(int x,int y,int rx,int ry,int rw,int rh);


#ifdef GRAPHICS_IMPLEMENTATION

static void DrawBres(byte *srf,int xc, int yc, int x, int y, int c);
static void FillBres(byte *srf,int xc, int yc, int x, int y, int c);



byte *VGA=(byte*)0xA0000000L;
word *myclock=(word*)0x0000046CL;

bool inrect(int x,int y,int rx,int ry,int rw,int rh) {
	return x>=rx && x<=rx+rw && y>=ry && y<=ry+rh;
}





void vsync(void) {
	while(inp(INPUT_STATUS_1) & VRETRACE);
	while(!(inp(INPUT_STATUS_1) & VRETRACE));
}

void SetMode(unsigned char mode) {
	union REGS regs;
	regs.h.ah=0x00;
	regs.h.al=mode;
	int86(0x10,&regs,&regs);
}

void SetPalette(byte idx,byte r,byte g,byte b) {
	outp(0x03c8,idx);
	outp(0x03c9,r);
	outp(0x03c9,g);
	outp(0x03c9,b);
}

void DrawPoint(byte *srf,int x,int y,int c) {
	if(x>=0 && x<SCREEN_WIDTH && y>=0 && y<SCREEN_HEIGHT) {
		srf[SCREEN_WIDTH*y+x]=c;
	}
}

void DrawLine(byte *srf,int x0,int y0,int x1,int y1,int c) {
	int dx=abs(x1-x0),sx=x0<x1?1:-1;
	int dy=abs(y1-y0),sy=y0<y1?1:-1;
	int e1=(dx>dy?dx:-dy)/2,e2;
	for(;;) {
		DrawPoint(srf,x0,y0,c);
		if(x0==x1 && y0==y1) break;
		e2=e1;
		if(e2>-dx) { e1-=dy; x0+=sx; }
		if(e2<dy)  { e1+=dx; y0+=sy; }
	}
}

void FillRect(byte *srf,int x,int y,int w,int h,int c) {
	int i,j;
	for(j=0;j<h;j++)
		for(i=0;i<w;i++)
			DrawPoint(srf,i+x,j+y,c);
}

void DrawRect(byte *srf,int x,int y,int w,int h,int c) {
	int i,j;
	for(i=0;i<w;i++) {
		DrawPoint(srf,i+x,y,c);
		DrawPoint(srf,i+x,y+h-1,c);
	}
	for(j=0;j<h;j++) {
		DrawPoint(srf,x,y+j,c);
		DrawPoint(srf,x+w-1,y+j,c);
	}
}


static void DrawBres(byte *srf,int xc, int yc, int x, int y, int c) {
	DrawPoint(srf, xc + x, yc + y, c);
	DrawPoint(srf, xc - x, yc + y, c);
	DrawPoint(srf, xc + x, yc - y, c);
	DrawPoint(srf, xc - x, yc - y, c);
	DrawPoint(srf, xc + y, yc + x, c);
	DrawPoint(srf, xc - y, yc + x, c);
	DrawPoint(srf, xc + y, yc - x, c);
	DrawPoint(srf, xc - y, yc - x, c);
}

void DrawCircle(byte *srf, int xc, int yc, int r, int c) {
	int x = 0, y = r;
	int d = 3 - 2 * r;

	DrawBres(srf, xc, yc, x, y, c);

	while (y >= x) {
		x++;
		if (d > 0) {
			y--;
			d = d + 4 * (x - y) + 10;
		} else {
			d = d + 4 * x + 6;
		}

		DrawBres(srf, xc, yc, x, y, c);
	}
}


static void FillBres(byte *srf,int xc, int yc, int x, int y, int c) {
	DrawLine(srf, xc + x, yc + y, xc - x, yc + y, c);
	DrawLine(srf, xc - x, yc + y, xc + x, yc + y, c);
	DrawLine(srf, xc + x, yc - y, xc - x, yc - y, c);
	DrawLine(srf, xc - x, yc - y, xc + x, yc - y, c);
	DrawLine(srf, xc + y, yc + x, xc - y, yc + x, c);
	DrawLine(srf, xc - y, yc + x, xc + y, yc + x, c);
	DrawLine(srf, xc + y, yc - x, xc - y, yc - x, c);
	DrawLine(srf, xc - y, yc - x, xc + y, yc - x, c);
}

void FillCircle(byte *srf, int xc, int yc, int r, int c) {
	int x = 0, y = r;
	int d = 3 - 2 * r;

	FillBres(srf, xc, yc, x, y, c);

	while (y >= x) {
		x++;
		if (d > 0) {
			y--;
			d = d + 4 * (x - y) + 10;
		} else {
			d = d + 4 * x + 6;
		}

		FillBres(srf, xc, yc, x, y, c);
	}
}


#endif /* GRAPHICS_IMPLEMENTATION */

#endif /* GRAPHICS_H */

