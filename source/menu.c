//
// Created by cpasjuste on 25/01/16.
//

#include <stdbool.h>
#include <hb_menu/gfx.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "menu.h"

void drawBg() {

    if (!config->imgError) {
        memcpy(gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL), config->bgImgTopBuff,
               (size_t) config->bgImgTopSize);
    } else {
        gfxClearTop(config->bgTop1, config->bgTop2);
    }
    if (!config->imgErrorBot) {
        memcpy(gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL), config->bgImgBotBuff,
               (size_t) config->bgImgBotSize);
    } else {
        gfxClearBot(config->bgBot);
    }
    drawRectColor(GFX_TOP, GFX_LEFT, MENU_MIN_X, MENU_MIN_Y - 20, MENU_MAX_X, MENU_MAX_Y, config->borders);
}

void drawTitle(const char *format, ...) {

    char msg[512];
    va_list argp;
    va_start(argp, format);
    vsnprintf(msg, 512, format, argp);
    va_end(argp);

    gfxDrawText(GFX_TOP, GFX_LEFT, &fontDefault, msg, 140, 25);
}

void drawItem(bool selected, int y, const char *format, ...) {

    char msg[512];
    va_list argp;
    va_start(argp, format);
    vsnprintf(msg, 512, format, argp);
    va_end(argp);

    if (selected) {
        gfxDrawRectangle(GFX_TOP, GFX_LEFT, config->highlight, (s16) (MENU_MIN_X + 4), (s16) (y + MENU_MIN_Y), 361, 15);
    }
    gfxDrawText(GFX_TOP, GFX_LEFT, selected ? &fontSelected : &fontDefault, msg, (s16) (MENU_MIN_X + 6),
                (s16) y + (s16) MENU_MIN_Y);
}

void drawItemN(bool selected, int maxChar, int y, const char *format, ...) {

    char msg[512];
    va_list argp;
    va_start(argp, format);
    vsnprintf(msg, 512, format, argp);
    va_end(argp);

    if (selected) {
        gfxDrawRectangle(GFX_TOP, GFX_LEFT, config->highlight, (s16) (MENU_MIN_X + 4), (s16) (y + MENU_MIN_Y), 361, 15);
    }
    gfxDrawTextN(GFX_TOP, GFX_LEFT, selected ? &fontSelected : &fontDefault, msg, maxChar, (s16) (MENU_MIN_X + 6),
                 (s16) y + (s16) MENU_MIN_Y);
}

void drawInfo(const char *format, ...) {

    char msg[512];
    va_list argp;
    va_start(argp, format);
    vsnprintf(msg, 512, format, argp);
    va_end(argp);

    gfxDrawText(GFX_BOTTOM, GFX_LEFT, &fontDefault, "Informations", (s16) (MENU_MIN_X + 6), 40);
    gfxDrawText(GFX_BOTTOM, GFX_LEFT, &fontDefault, msg, (s16) (MENU_MIN_X + 12), 80);
}
