#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <3ds.h>
#include <stdarg.h>

#include "gfx.h"
#include "font.h"
#include "text.h"
#include "costable.h"

void drawPixel(int x, int y, char r, char g, char b, u8 *screen) {
    int height = 240;

    u32 v = (height - 1 - y + x * height) * 3;
    screen[v] = b;
    screen[v + 1] = g;
    screen[v + 2] = r;
}

void drawLine(gfxScreen_t screen, gfx3dSide_t side, int x1, int y1, int x2, int y2, char r, char g, char b) {
    u16 fbWidth, fbHeight;
    u8 *fbAdr = gfxGetFramebuffer(screen, side, &fbWidth, &fbHeight);

    int x, y;
    if (x1 == x2) {
        if (y1 < y2) for (y = y1; y < y2; y++) drawPixel(x1, y, r, g, b, fbAdr);
        else for (y = y2; y < y1; y++) drawPixel(x1, y, r, g, b, fbAdr);
    } else {
        if (x1 < x2) for (x = x1; x < x2; x++) drawPixel(x, y1, r, g, b, fbAdr);
        else for (x = x2; x < x1; x++) drawPixel(x, y1, r, g, b, fbAdr);
    }
}

void drawRect(gfxScreen_t screen, gfx3dSide_t side, int x1, int y1, int x2, int y2, char r, char g, char b) {
    drawLine(screen, side, x1, y1, x2, y1, r, g, b);
    drawLine(screen, side, x2, y1, x2, y2, r, g, b);
    drawLine(screen, side, x1, y2, x2, y2, r, g, b);
    drawLine(screen, side, x1, y1, x1, y2, r, g, b);
}

void drawRectColor(gfxScreen_t screen, gfx3dSide_t side, int x1, int y1, int x2, int y2, u8 *color) {
    drawRect(screen, side, x1, y1, x2, y2, color[0], color[1], color[2]);
}

void drawFillRect(gfxScreen_t screen, gfx3dSide_t side, int x1, int y1, int x2, int y2, char r, char g, char b) {
    int X1, X2, Y1, Y2, i, j;
    u16 fbWidth, fbHeight;
    u8 *fbAdr = gfxGetFramebuffer(screen, side, &fbWidth, &fbHeight);

    if (x1 < x2) {
        X1 = x1;
        X2 = x2;
    } else {
        X1 = x2;
        X2 = x1;
    }

    if (y1 < y2) {
        Y1 = y1;
        Y2 = y2;
    } else {
        Y1 = y2;
        Y2 = y1;
    }
    for (i = X1; i <= X2; i++) {
        for (j = Y1; j <= Y2; j++) {
            drawPixel(i, j, r, g, b, fbAdr);
        }
    }
}

void gfxDrawTextf(gfxScreen_t screen, gfx3dSide_t side, font_s *f, s16 x, s16 y, const char *fmt, ...) {
    char s[512];
    memset(s, 0, 512);
    va_list args;
    va_start(args, fmt);
    int len = vsprintf(s, fmt, args);
    va_end(args);
    if (len)
        gfxDrawText(screen, side, f, s, x, y);
}

void gfxDrawText(gfxScreen_t screen, gfx3dSide_t side, font_s *f, char *str, s16 x, s16 y) {
    if (!str)return;
    if (!f)f = &fontDefault;

    u16 fbWidth, fbHeight;
    u8 *fbAdr = gfxGetFramebuffer(screen, side, &fbWidth, &fbHeight);

    drawString(fbAdr, f, str, x, 240 - y, fbHeight, fbWidth);
}

void gfxDrawTextN(gfxScreen_t screen, gfx3dSide_t side, font_s *f, char *str, u16 length, s16 x, s16 y) {
    if (!str)return;
    if (!f)f = &fontDefault;

    u16 fbWidth, fbHeight;
    u8 *fbAdr = gfxGetFramebuffer(screen, side, &fbWidth, &fbHeight);

    drawStringN(fbAdr, f, str, length, x, 240 - y, fbHeight, fbWidth);
}

void gfxDrawSprite(gfxScreen_t screen, gfx3dSide_t side, u8 *spriteData, u16 width, u16 height, s16 x, s16 y) {
    if (!spriteData)return;

    u16 fbWidth, fbHeight;
    u8 *fbAdr = gfxGetFramebuffer(screen, side, &fbWidth, &fbHeight);

    if (x + width < 0 || x >= fbWidth)return;
    if (y + height < 0 || y >= fbHeight)return;

    u16 xOffset = 0, yOffset = 0;
    u16 widthDrawn = width, heightDrawn = height;

    if (x < 0)xOffset = -x;
    if (y < 0)yOffset = -y;
    if (x + width >= fbWidth)widthDrawn = fbWidth - x;
    if (y + height >= fbHeight)heightDrawn = fbHeight - y;
    widthDrawn -= xOffset;
    heightDrawn -= yOffset;

    int j;
    for (j = yOffset; j < yOffset + heightDrawn; j++) {
        memcpy(&fbAdr[((x + xOffset) + (y + j) * fbWidth) * 3], &spriteData[((xOffset) + (j) * width) * 3],
               widthDrawn * 3);
    }
}

void gfxDrawDualSprite(u8 *spriteData, u16 width, u16 height, s16 x, s16 y) {
    if (!spriteData)return;

    gfxDrawSprite(GFX_TOP, GFX_LEFT, spriteData, width, height, x - 240, y);
    gfxDrawSprite(GFX_BOTTOM, GFX_LEFT, spriteData, width, height, x, y - 40);
}

void gfxDrawSpriteAlpha(gfxScreen_t screen, gfx3dSide_t side, u8 *spriteData, u16 width, u16 height, s16 x, s16 y) {
    if (!spriteData)return;

    u16 fbWidth, fbHeight;
    u8 *fbAdr = gfxGetFramebuffer(screen, side, &fbWidth, &fbHeight);

    if (x + width < 0 || x >= fbWidth)return;
    if (y + height < 0 || y >= fbHeight)return;

    u16 xOffset = 0, yOffset = 0;
    u16 widthDrawn = width, heightDrawn = height;

    if (x < 0)xOffset = -x;
    if (y < 0)yOffset = -y;
    if (x + width >= fbWidth)widthDrawn = fbWidth - x;
    if (y + height >= fbHeight)heightDrawn = fbHeight - y;
    widthDrawn -= xOffset;
    heightDrawn -= yOffset;

    //TODO : optimize
    fbAdr += (y + yOffset) * fbWidth * 3;
    spriteData += yOffset * width * 4;
    int j, i;
    for (j = yOffset; j < yOffset + heightDrawn; j++) {
        u8 *fbd = &fbAdr[(x + xOffset) * 3];
        u8 *data = &spriteData[(xOffset) * 4];
        for (i = xOffset; i < xOffset + widthDrawn; i++) {
            if (data[3]) {
                fbd[0] = data[0];
                fbd[1] = data[1];
                fbd[2] = data[2];
            }
            fbd += 3;
            data += 4;
        }
        fbAdr += fbWidth * 3;
        spriteData += width * 4;
    }
}

void gfxDrawSpriteAlphaBlend(gfxScreen_t screen, gfx3dSide_t side, u8 *spriteData, u16 width, u16 height, s16 x,
                             s16 y) {
    if (!spriteData)return;

    u16 fbWidth, fbHeight;
    u8 *fbAdr = gfxGetFramebuffer(screen, side, &fbWidth, &fbHeight);

    if (x + width < 0 || x >= fbWidth)return;
    if (y + height < 0 || y >= fbHeight)return;

    u16 xOffset = 0, yOffset = 0;
    u16 widthDrawn = width, heightDrawn = height;

    if (x < 0)xOffset = -x;
    if (y < 0)yOffset = -y;
    if (x + width >= fbWidth)widthDrawn = fbWidth - x;
    if (y + height >= fbHeight)heightDrawn = fbHeight - y;
    widthDrawn -= xOffset;
    heightDrawn -= yOffset;

    //TODO : optimize
    fbAdr += (y + yOffset) * fbWidth * 3;
    spriteData += yOffset * width * 4;
    int j, i;
    for (j = yOffset; j < yOffset + heightDrawn; j++) {
        u8 *fbd = &fbAdr[(x + xOffset) * 3];
        u8 *data = &spriteData[(xOffset) * 4];
        for (i = xOffset; i < xOffset + widthDrawn; i++) {
            if (data[3]) {
                u8 alphaSource = data[3];
                fbd[0] = ((data[0] * alphaSource) + (fbd[0] * (255 - alphaSource))) / 256;
                fbd[1] = ((data[1] * alphaSource) + (fbd[1] * (255 - alphaSource))) / 256;
                fbd[2] = ((data[2] * alphaSource) + (fbd[2] * (255 - alphaSource))) / 256;
            }
            fbd += 3;
            data += 4;
        }
        fbAdr += fbWidth * 3;
        spriteData += width * 4;
    }
}

void gfxDrawSpriteAlphaBlendFade(gfxScreen_t screen, gfx3dSide_t side, u8 *spriteData, u16 width, u16 height, s16 x,
                                 s16 y, u8 fadeValue) {
    if (!spriteData)return;

    u16 fbWidth, fbHeight;
    u8 *fbAdr = gfxGetFramebuffer(screen, side, &fbWidth, &fbHeight);

    if (x + width < 0 || x >= fbWidth)return;
    if (y + height < 0 || y >= fbHeight)return;

    u16 xOffset = 0, yOffset = 0;
    u16 widthDrawn = width, heightDrawn = height;

    if (x < 0)xOffset = -x;
    if (y < 0)yOffset = -y;
    if (x + width >= fbWidth)widthDrawn = fbWidth - x;
    if (y + height >= fbHeight)heightDrawn = fbHeight - y;
    widthDrawn -= xOffset;
    heightDrawn -= yOffset;

    //TODO : optimize
    fbAdr += (y + yOffset) * fbWidth * 3;
    spriteData += yOffset * width * 4;
    int j, i;
    for (j = yOffset; j < yOffset + heightDrawn; j++) {
        u8 *fbd = &fbAdr[(x + xOffset) * 3];
        u8 *data = &spriteData[(xOffset) * 4];
        for (i = xOffset; i < xOffset + widthDrawn; i++) {
            if (data[3]) {
                u8 alphaSource = (fadeValue * data[3]) / 256;
                fbd[0] = ((data[0] * alphaSource) / 256) + ((fbd[0] * (255 - alphaSource)) / 256);
                fbd[1] = ((data[1] * alphaSource) / 256) + ((fbd[1] * (255 - alphaSource)) / 256);
                fbd[2] = ((data[2] * alphaSource) / 256) + ((fbd[2] * (255 - alphaSource)) / 256);
            }
            fbd += 3;
            data += 4;
        }
        fbAdr += fbWidth * 3;
        spriteData += width * 4;
    }
}

void gfxFillColor(gfxScreen_t screen, gfx3dSide_t side, u8 rgbColor[3]) {
    u16 fbWidth, fbHeight;
    u8 *fbAdr = gfxGetFramebuffer(screen, side, &fbWidth, &fbHeight);

    //TODO : optimize; use GX command ?
    int i;
    for (i = 0; i < fbWidth * fbHeight; i++) {
        *(fbAdr++) = rgbColor[2];
        *(fbAdr++) = rgbColor[1];
        *(fbAdr++) = rgbColor[0];
    }
}

void gfxFillColorGradient(gfxScreen_t screen, gfx3dSide_t side, u8 rgbColorStart[3], u8 rgbColorEnd[3]) {
    u16 fbWidth, fbHeight;
    u8 *fbAdr = gfxGetFramebuffer(screen, side, &fbWidth, &fbHeight);
    u8 colorLine[fbWidth * 3];

    //TODO : optimize; use GX command ?
    int i;
    float n;
    float total = (float) (fbWidth - 1);
    // make slightly bigger to prevent gradients from blending around.  SHould be removed and have the gradient color be better later.
    total *= 1.5f;
    for (i = 0; i < fbWidth; i++) {
        n = (float) i / total;
        colorLine[i * 3 + 0] = (float) rgbColorStart[2] * (1.0f - n) + (float) rgbColorEnd[2] * n;
        colorLine[i * 3 + 1] = (float) rgbColorStart[1] * (1.0f - n) + (float) rgbColorEnd[1] * n;
        colorLine[i * 3 + 2] = (float) rgbColorStart[0] * (1.0f - n) + (float) rgbColorEnd[0] * n;
    }

    for (i = 0; i < fbHeight; i++) {
        memcpy(fbAdr, colorLine, fbWidth * 3);
        fbAdr += fbWidth * 3;
    }
}

void _gfxDrawRectangle(gfxScreen_t screen, gfx3dSide_t side, u8 rgbColor[3], s16 x, s16 y, u16 width, u16 height) {
    u16 fbWidth, fbHeight;
    u8 *fbAdr = gfxGetFramebuffer(screen, side, &fbWidth, &fbHeight);

    if (x + width < 0 || x >= fbWidth)return;
    if (y + height < 0 || y >= fbHeight)return;

    if (x < 0) {
        width += x;
        x = 0;
    }
    if (y < 0) {
        height += y;
        y = 0;
    }
    if (x + width >= fbWidth)width = fbWidth - x;
    if (y + height >= fbHeight)height = fbHeight - y;

    u8 colorLine[width * 3];

    int j;
    for (j = 0; j < width; j++) {
        colorLine[j * 3 + 0] = rgbColor[2];
        colorLine[j * 3 + 1] = rgbColor[1];
        colorLine[j * 3 + 2] = rgbColor[0];
    }

    fbAdr += fbWidth * 3 * y;
    for (j = 0; j < height; j++) {
        memcpy(&fbAdr[x * 3], colorLine, width * 3);
        fbAdr += fbWidth * 3;
    }
}

void gfxDrawRectangle(gfxScreen_t screen, gfx3dSide_t side, u8 rgbColor[3], s16 x, s16 y, u16 width, u16 height) {
    _gfxDrawRectangle(screen, side, rgbColor, 240 - y, x, height, width);
}

void gfxFadeScreen(gfxScreen_t screen, gfx3dSide_t side, u32 f) {
    u16 fbWidth, fbHeight;
    u8 *fbAdr = gfxGetFramebuffer(screen, side, &fbWidth, &fbHeight);

    int i;
    for (i = 0; i < fbWidth * fbHeight / 2; i++) {
        *fbAdr = (*fbAdr * f) >> 8;
        fbAdr++;
        *fbAdr = (*fbAdr * f) >> 8;
        fbAdr++;
        *fbAdr = (*fbAdr * f) >> 8;
        fbAdr++;
        *fbAdr = (*fbAdr * f) >> 8;
        fbAdr++;
        *fbAdr = (*fbAdr * f) >> 8;
        fbAdr++;
        *fbAdr = (*fbAdr * f) >> 8;
        fbAdr++;
    }
}

void gfxDrawWave(gfxScreen_t screen, gfx3dSide_t side, u8 rgbColorStart[3], u8 rgbColorEnd[3], u16 level, u16 amplitude,
                 u16 width, gfxWaveCallback cb, void *p) {
    u16 fbWidth, fbHeight;
    u8 *fbAdr = gfxGetFramebuffer(screen, side, &fbWidth, &fbHeight);

    u8 colorLine[fbWidth * 3];

    int j;

    if (width) {
        for (j = 0; j < fbWidth; j++) {
            colorLine[j * 3 + 0] = rgbColorStart[2];
            colorLine[j * 3 + 1] = rgbColorStart[1];
            colorLine[j * 3 + 2] = rgbColorStart[0];
        }
        for (j = 0; j < fbHeight; j++) {
            u16 waveLevel = level + cb(p, j) * amplitude;
            memcpy(&fbAdr[(waveLevel - width) * 3], colorLine, width * 3);
            fbAdr += fbWidth * 3;
        }
    } else {
        int i;
        float n;
        float total = (float) (fbWidth - 1);
        // make slightly bigger to prevent gradients from blending around.  SHould be removed and have the gradient color be better later.
        total *= 1.5f;
        for (i = 0; i < fbWidth; i++) {
            n = (float) i / total;
            colorLine[i * 3 + 0] = (float) rgbColorStart[2] * (1.0f - n) + (float) rgbColorEnd[2] * n;
            colorLine[i * 3 + 1] = (float) rgbColorStart[1] * (1.0f - n) + (float) rgbColorEnd[1] * n;
            colorLine[i * 3 + 2] = (float) rgbColorStart[0] * (1.0f - n) + (float) rgbColorEnd[0] * n;
        }

        for (j = 0; j < fbHeight; j++) {
            u16 waveLevel = level + cb(p, j) * amplitude;
            memcpy(fbAdr, colorLine, waveLevel * 3);
            fbAdr += fbWidth * 3;
        }
    }
}

void gfxClearTop(u8 top1[3], u8 top2[3]) {
    gfxFillColorGradient(GFX_TOP, GFX_LEFT, top1, top2);
}

void gfxClearBot(u8 bot[8]) {
    gfxFillColor(GFX_BOTTOM, GFX_LEFT, bot);
}

void gfxSwap() {
    gfxFlushBuffers();
    gfxSwapBuffers();
    gspWaitForVBlank();
}

