//
// Created by cpasjuste on 29/02/16.
//
#include <string.h>
#include <stdlib.h>
#include "ini.h"
#include "config.h"
#include "font.h"
#include "utility.h"
#include "memory.h"

#ifdef ARM9
#include "arm9/source/fatfs/ff.h"
#include "config.h"
#include "gfx.h"
boot_config_s sconf;
#else
#include <3ds.h>
#endif

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

typedef struct
{
    const char* ptr;
    int bytes_left;
} buffer_ctx;

void setColor(u8 *cfgColor, const char *color) {
    long l = strtoul(color, NULL, 16);
    cfgColor[0] = (u8) (l >> 16 & 0xFF);
    cfgColor[1] = (u8) (l >> 8 & 0xFF);
    cfgColor[2] = (u8) (l & 0xFF);
}

static char* ini_buffer_reader(char* str, int num, void* stream)
{
    buffer_ctx* ctx = (buffer_ctx*)stream;
    int idx = 0;
    char newline = 0;

    if (ctx->bytes_left <= 0)
        return NULL;

    for (idx = 0; idx < num - 1; ++idx)
    {
        if (idx == ctx->bytes_left)
            break;

        if (ctx->ptr[idx] == '\n')
            newline = '\n';
        else if (ctx->ptr[idx] == '\r')
            newline = '\r';

        if (newline)
            break;
    }

    memcpy(str, ctx->ptr, idx);
    str[idx] = 0;

    ctx->ptr += idx + 1;
    ctx->bytes_left -= idx + 1;

    if (newline && ctx->bytes_left > 0 &&
            ((newline == '\r' && ctx->ptr[0] == '\n') ||
             (newline == '\n' && ctx->ptr[0] == '\r'))) {
        ctx->bytes_left--;
        ctx->ptr++;
    }
    return str;
}

static int handler(void* user, const char* section, const char* name,
                   const char* value)
{
	// general
    if (MATCH("general", "timeout")) {
		config->timeout = atoi(value);
    } else if (MATCH("general", "recovery")) {
		config->recovery = atoi(value);
    } else if (MATCH("general", "default")) {
		config->index = atoi(value);
    } else if (MATCH("general", "autobootfix")) {
        config->autobootfix = atoi(value);
    }

    // theme
    else if (MATCH("theme", "bgTop1")) {
        setColor(config->bgTop1, value);
    } else if (MATCH("theme", "bgTop2")) {
        setColor(config->bgTop2, value);
    } else if (MATCH("theme", "bgBottom")) {
        setColor(config->bgBot, value);
    } else if (MATCH("theme", "highlight")) {
        setColor(config->highlight, value);
    } else if (MATCH("theme", "borders")) {
        setColor(config->borders, value);
    } else if (MATCH("theme", "font1")) {
        setColor(config->fntDef, value);
    } else if (MATCH("theme", "font2")) {
        setColor(config->fntSel, value);
    } else if (MATCH("theme", "bgImgTop")) {
        strncpy(config->bgImgTop, value, 128);
    } else if (MATCH("theme", "bgImgBot")) {
        strncpy(config->bgImgBot, value, 128);
    }

    // entries
    else if (MATCH("entry", "title")) {
        strncpy(config->entries[config->count].title, value, 64);
    } else if (MATCH("entry", "path")) {
        strncpy(config->entries[config->count].path, value, 128);
    } else if (MATCH("entry", "offset")) {
        config->entries[config->count].offset = strtoul(value, NULL, 16);
    } else if (MATCH("entry", "key")) {
        config->entries[config->count].key = atoi(value);
		config->count++;
    }
    else {
        return 0;
    }
    return 1;
}

void configThemeInit() {
    config->imgError = true;
    config->imgErrorBot = true;
    memcpy(config->bgTop1, (u8[3]) {0x4a, 0x00, 0x31}, sizeof(u8[3]));
    memcpy(config->bgTop2, (u8[3]) {0x6f, 0x01, 0x49}, sizeof(u8[3]));
    memcpy(config->bgBot, (u8[3]) {0x6f, 0x01, 0x49}, sizeof(u8[3]));
    memcpy(config->highlight, (u8[3]) {0xdc, 0xdc, 0xdc}, sizeof(u8[3]));
    memcpy(config->borders, (u8[3]) {0xff, 0xff, 0xff}, sizeof(u8[3]));
    memcpy(config->fntDef, (u8[3]) {0xff, 0xff, 0xff}, sizeof(u8[3]));
    memcpy(config->fntSel, (u8[3]) {0x00, 0x00, 0x00}, sizeof(u8[3]));
}

int configInit()
{
    buffer_ctx ctx;

    // init config
#ifdef ARM9
    config = &sconf;
#else
    config = malloc(sizeof(boot_config_s));
#endif
    memset(config, 0, sizeof(boot_config_s));

    config->timeout = 3;
    config->autobootfix = 100;
    config->index = 0;
    config->recovery = 2;
    config->count = 0;
    configThemeInit();

    // read config file to buffer
    size_t size = fileSize("/a9lh.cfg");
    if(!size) {
        return -1;
    }
	char buffer[size];
	memset(buffer, 0, size);
    if(fileRead("/a9lh.cfg", buffer, size) != 0) {
        return -1;
    }
    ctx.ptr = buffer;
    ctx.bytes_left = strlen(ctx.ptr);

    if (ini_parse_stream((ini_reader)ini_buffer_reader, &ctx, handler, config) < 0) {
        return -1;
    }

    //
    memcpy(fontDefault.color, config->fntDef, sizeof(u8[3]));
    loadBg(GFX_TOP);
    loadBg(GFX_BOTTOM);

    return 0;
}

int configCreate() {
    //TODO
    return 0;
}

int configAddEntry(char *title, char *path, long offset) {
    //TODO
    return 0;
}

int configRemoveEntry(int index) {
    //TODO
    return 0;
}

void configUpdateSettings() {
    //TODO
}

void configWrite() {
    //TODO
}

void configExit() {
#ifndef ARM9
    if (config) {
        if (config->bgImgTopBuff) {
            free(config->bgImgTopBuff);
        }
        if (config->bgImgBotBuff) {
            free(config->bgImgBotBuff);
        }
        free(config);
    }
#endif
}

void loadBg(gfxScreen_t screen) {

    const char *path = screen == GFX_TOP ? config->bgImgTop : config->bgImgBot;
    size_t size = fileSize(path);
    if(!size) {
        return;
    }

#ifdef ARM9
    u8 *bg = screen == GFX_TOP ? PTR_TOP_BG_BUF : PTR_BOT_BG_BUF;
#else
    u8 *bg = malloc(size);
#endif
    if(fileRead(path, bg, size) != 0) {
        return;
    }

    if(screen == GFX_TOP) {
        config->bgImgTopSize = size;
        config->bgImgTopBuff = bg;
        config->imgError = false;
    } else {
        config->bgImgBotSize = size;
        config->bgImgBotBuff = bg;
        config->imgErrorBot = false;
    }
}
