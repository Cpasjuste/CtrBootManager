#pragma once

#include <3ds.h>
#include "font.h"

typedef float (*gfxWaveCallback)(void *p, u16 x);

//rendering stuff
void drawLine(gfxScreen_t screen, gfx3dSide_t side, int x1, int y1, int x2, int y2, char r, char g, char b);

//void drawRect(gfxScreen_t screen, gfx3dSide_t side, int x1, int y1, int x2, int y2, char r, char g, char b);

void drawRectColor(gfxScreen_t screen, gfx3dSide_t side, int x1, int y1, int x2, int y2, u8 *color);

void drawFillRect(gfxScreen_t screen, gfx3dSide_t side, int x1, int y1, int x2, int y2, char r, char g, char b);

void gfxFadeScreen(gfxScreen_t screen, gfx3dSide_t side, u32 f);

void gfxDrawSprite(gfxScreen_t screen, gfx3dSide_t side, u8 *spriteData, u16 width, u16 height, s16 x, s16 y);

void gfxDrawDualSprite(u8 *spriteData, u16 width, u16 height, s16 x, s16 y);

void gfxDrawSpriteAlpha(gfxScreen_t screen, gfx3dSide_t side, u8 *spriteData, u16 width, u16 height, s16 x, s16 y);

void gfxDrawSpriteAlphaBlend(gfxScreen_t screen, gfx3dSide_t side, u8 *spriteData, u16 width, u16 height, s16 x, s16 y);

void gfxDrawSpriteAlphaBlendFade(gfxScreen_t screen, gfx3dSide_t side, u8 *spriteData, u16 width, u16 height, s16 x,
                                 s16 y, u8 fadeValue);

void gfxDrawText(gfxScreen_t screen, gfx3dSide_t side, font_s *f, char *str, s16 x, s16 y);

void gfxDrawTextN(gfxScreen_t screen, gfx3dSide_t side, font_s *f, char *str, u16 length, s16 x, s16 y);

void gfxDrawTextf(gfxScreen_t screen, gfx3dSide_t side, font_s *f, s16 x, s16 y, const char *fmt, ...);

void gfxFillColor(gfxScreen_t screen, gfx3dSide_t side, u8 rgbColor[3]);

void gfxFillColorGradient(gfxScreen_t screen, gfx3dSide_t side, u8 rgbColorStart[3], u8 rgbColorEnd[3]);

void gfxDrawRectangle(gfxScreen_t screen, gfx3dSide_t side, u8 rgbColor[3], s16 x, s16 y, u16 width, u16 height);

void gfxDrawWave(gfxScreen_t screen, gfx3dSide_t side, u8 rgbColorStart[3], u8 rgbColorEnd[3], u16 level, u16 amplitude,
                 u16 width, gfxWaveCallback cb, void *p);

void gfxClearTop(u8 top1[3], u8 top2[3]);

void gfxClearBot(u8 bot[8]);

void gfxSwap();
