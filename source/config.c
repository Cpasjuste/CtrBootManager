//
// Created by cpasjuste on 29/02/16.
//
#include <string.h>
#include <stdlib.h>
#include <sys/unistd.h>
#include "ini.h"
#include "config.h"
#include "font.h"
#ifdef ARM9
#include "arm9/source/fatfs/ff.h"
#include "utility.h"
#include "memory.h"
#endif

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

typedef struct {
    const char *ptr;
    int bytes_left;
} buffer_ctx;

void setColor(u8 *cfgColor, const char *color) {
    long l = strtoul(color, NULL, 16);
    cfgColor[0] = (u8) (l >> 16 & 0xFF);
    cfgColor[1] = (u8) (l >> 8 & 0xFF);
    cfgColor[2] = (u8) (l & 0xFF);
}

static char *ini_buffer_reader(char *str, int num, void *stream) {
    buffer_ctx *ctx = (buffer_ctx *) stream;
    int idx = 0;
    char newline = 0;

    if (ctx->bytes_left <= 0)
        return NULL;

    for (idx = 0; idx < num - 1; ++idx) {
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

static int handler(void *user, const char *section, const char *name,
                   const char *value) {

    char item[256];
    memset(item, 0, 256);
    if(value[strlen(value)-1] == ';') {
        strncpy(item, value, strlen(value)-1);
    } else {
        strncpy(item, value, strlen(value));
    }

    // general
    if (MATCH("general", "timeout")) {
        config->timeout = atoi(item);
    } else if (MATCH("general", "recovery")) {
        config->recovery = atoi(item);
    } else if (MATCH("general", "default")) {
        config->index = atoi(item);
    } else if (MATCH("general", "autobootfix")) {
        config->autobootfix = atoi(item);
    }

        // theme
    else if (MATCH("theme", "bgTop1")) {
        setColor(config->bgTop1, item);
    } else if (MATCH("theme", "bgTop2")) {
        setColor(config->bgTop2, item);
    } else if (MATCH("theme", "bgBottom")) {
        setColor(config->bgBot, item);
    } else if (MATCH("theme", "highlight")) {
        setColor(config->highlight, item);
    } else if (MATCH("theme", "borders")) {
        setColor(config->borders, item);
    } else if (MATCH("theme", "font1")) {
        setColor(config->fntDef, item);
    } else if (MATCH("theme", "font2")) {
        setColor(config->fntSel, item);
    } else if (MATCH("theme", "bgImgTop")) {
        strncpy(config->bgImgTop, item, 128);
    } else if (MATCH("theme", "bgImgBot")) {
        strncpy(config->bgImgBot, item, 128);
    }

        // entries
    else if (MATCH("entry", "title")) {
        strncpy(config->entries[config->count].title, item, 64);
    } else if (MATCH("entry", "path")) {
        strncpy(config->entries[config->count].path, item, 128);
    } else if (MATCH("entry", "offset")) {
        config->entries[config->count].offset = strtoul(item, NULL, 16);
    } else if (MATCH("entry", "key")) {
        config->entries[config->count].key = atoi(item);
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

int configInit() {
    buffer_ctx ctx;

    // init config
#ifdef ARM9
    config = (boot_config_s *)PTR_CFG;
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
    size_t size = fileSize(CONFIG_PATH);
    if (!size) {
        return -1;
    }
    char buffer[size];
    memset(buffer, 0, size);
    if (fileRead(CONFIG_PATH, buffer, size) != 0) {
        configSave(); // write new config file
        return -1;
    }
    ctx.ptr = buffer;
    ctx.bytes_left = strlen(ctx.ptr);

    if (ini_parse_stream((ini_reader) ini_buffer_reader, &ctx, handler, config) < 0) {
        configSave(); // write new config file
        return -1;
    }

    memcpy(fontDefault.color, config->fntDef, sizeof(u8[3]));
    loadBg(GFX_TOP);
    loadBg(GFX_BOTTOM);

    return 0;
}

int configAddEntry(char *title, char *path, long offset) {

    strncpy(config->entries[config->count].title, title, 64);
    strncpy(config->entries[config->count].path, path, 128);
    config->entries[config->count].offset = offset;
    config->entries[config->count].key = -1;
    config->count++;
    configSave();

    return 0;
}

int configRemoveEntry(int index) {

    int i = 0;
    for(i=0; i<config->count; i++) {
        if(i > index) {
            config->entries[i-1] = config->entries[i];
        }
    }

    if (config->index >= index && config->index > 0) {
        config->index--;
    }
    config->count--;
    configSave();

    return 0;
}

void configSave() {

    int size = 0, i = 0;
    int buffSize = 256*(256*sizeof(char));
#ifdef ARM9
    char *cfg = (char*)PTR_CFG_TMP;
#else
    char *cfg = malloc(buffSize);
#endif
    memset(cfg, 0, buffSize); // 256 lines * 256 char

    // general section
    size += snprintf(cfg, 256, "[general];\n");
#ifndef ARM9
    size += snprintf(cfg+size, 256, "autobootfix=%i;\n", config->autobootfix);
#endif
    size += snprintf(cfg+size, 256, "timeout=%i;\n", config->timeout);
    size += snprintf(cfg+size, 256, "recovery=%i;\n", config->recovery);
    size += snprintf(cfg+size, 256, "default=%i;\n\n", config->index);

    // theme section
    size += snprintf(cfg+size, 256, "[theme];\n");
    size += snprintf(cfg+size, 256, "bgTop1=%02X%02X%02X;\n", config->bgTop1[0], config->bgTop1[1], config->bgTop1[2]);
    size += snprintf(cfg+size, 256, "bgTop2=%02X%02X%02X;\n", config->bgTop2[0], config->bgTop2[1], config->bgTop2[2]);
    size += snprintf(cfg+size, 256, "bgBottom=%02X%02X%02X;\n", config->bgBot[0], config->bgBot[1], config->bgBot[2]);
    size += snprintf(cfg+size, 256, "highlight=%02X%02X%02X;\n", config->highlight[0], config->highlight[1], config->highlight[2]);
    size += snprintf(cfg+size, 256, "borders=%02X%02X%02X;\n", config->borders[0], config->borders[1], config->borders[2]);
    size += snprintf(cfg+size, 256, "font1=%02X%02X%02X;\n", config->fntDef[0], config->fntDef[1], config->fntDef[2]);
    size += snprintf(cfg+size, 256, "font2=%02X%02X%02X;\n", config->fntSel[0], config->fntSel[1], config->fntSel[2]);
    size += snprintf(cfg+size, 256, "bgImgTop=%s;\n", config->bgImgTop);
    size += snprintf(cfg+size, 256, "bgImgBot=%s;\n\n", config->bgImgBot);

    for(i=0; i<config->count; i++) {
        size += snprintf(cfg+size, 256, "[entry];\n");
        size += snprintf(cfg+size, 256, "title=%s;\n", config->entries[i].title);
        size += snprintf(cfg+size, 256, "path=%s;\n", config->entries[i].path);
        size += snprintf(cfg+size, 256, "offset=%ld;\n", config->entries[i].offset);
        size += snprintf(cfg+size, 256, "key=%i;\n\n", config->entries[i].key);
    }
#ifdef ARM9
    FIL file;
    unsigned int br = 0;
    f_unlink(CONFIG_PATH);
    if(f_open(&file, CONFIG_PATH, FA_WRITE | FA_CREATE_ALWAYS) != FR_OK) {
        debug("Could not open cfg: %s", CONFIG_PATH);
        return;
    }
    f_write(&file, cfg, size, &br);
    f_close(&file);
#else
    unlink(CONFIG_PATH);
    FILE *file = fopen(CONFIG_PATH, "w");
    fwrite(cfg, 1 , size, file);
    fclose(file);
    free(cfg);
#endif
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
    if (!size) {
        return;
    }

#ifdef ARM9
    u8 *bg = screen == GFX_TOP ? PTR_TOP_BG : PTR_BOT_BG;
#else
    u8 *bg = malloc(size);
#endif
    if (fileRead(path, bg, size) != 0) {
        return;
    }

    if (screen == GFX_TOP) {
        config->bgImgTopSize = size;
        config->bgImgTopBuff = bg;
        config->imgError = false;
    } else {
        config->bgImgBotSize = size;
        config->bgImgBotBuff = bg;
        config->imgErrorBot = false;
    }
}
