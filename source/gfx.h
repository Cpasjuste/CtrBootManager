#pragma once

#ifdef ARM9
#include "arm9/source/common.h"
u8* gfxGetFramebuffer(gfxScreen_t screen, gfx3dSide_t side, u16* width, u16* height);

#else
#include <3ds.h>
#endif
#include "font.h"

//rendering stuff
void drawLine(gfxScreen_t screen, gfx3dSide_t side, int x1, int y1, int x2, int y2, char r, char g, char b);

void drawRectColor(gfxScreen_t screen, gfx3dSide_t side, int x1, int y1, int x2, int y2, u8 *color);

void gfxDrawText(gfxScreen_t screen, gfx3dSide_t side, font_s *f, char *str, s16 x, s16 y);

void gfxDrawTextN(gfxScreen_t screen, gfx3dSide_t side, font_s *f, char *str, u16 length, s16 x, s16 y);

void gfxDrawTextf(gfxScreen_t screen, gfx3dSide_t side, font_s *f, s16 x, s16 y, const char *fmt, ...);

void gfxFillColor(gfxScreen_t screen, gfx3dSide_t side, u8 rgbColor[3]);

void gfxFillColorGradient(gfxScreen_t screen, gfx3dSide_t side, u8 rgbColorStart[3], u8 rgbColorEnd[3]);

void gfxDrawRectangle(gfxScreen_t screen, gfx3dSide_t side, u8 rgbColor[3], s16 x, s16 y, u16 width, u16 height);

void gfxClearTop(u8 top1[3], u8 top2[3]);

void gfxClearTopRight(u8 top1[3], u8 top2[3]);

void gfxClearBotRight(u8 bot[8]);

void gfxClearBot(u8 bot[8]);

void gfxSwap();
