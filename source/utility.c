#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#ifdef ARM9

#include "arm9/source/common.h"
#include "arm9/source/hid.h"
#include "arm9/source/i2c.h"
#include "arm9/source/fatfs/ff.h"

#else
#include <3ds.h>
#include <CakeBrah/source/libkhax/khax.h>
#endif

#include "gfx.h"
#include "menu.h"
#include "utility.h"
#include "memory.h"

#ifdef ARM9

void waitcycles(uint32_t val);

#else
FS_Archive sdmcArchive;
extern void __appExit();

void openSDArchive() {
    sdmcArchive = (FS_Archive) {0x00000009, (FS_Path) {PATH_EMPTY, 1, (u8 *) ""}};
    FSUSER_OpenArchive(&sdmcArchive);
}

void closeSDArchive() {
    FSUSER_CloseArchive(&sdmcArchive);
}
#endif

void svcSleep(u32 millis) {
    u64 nano = millis * 1000000;
#ifndef ARM9
    svcSleepThread(nano);
#else
    waitcycles((u32) nano);
#endif
}

bool end_with(const char *str, const char c) {
    return (str && *str && str[strlen(str) - 1] == c) ? true : false;
}

const char *get_filename_ext(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) return "";
    return dot + 1;
}

char *get_button(int button) {

    switch (BIT(button)) {
        case KEY_A:
            return "A";

        case KEY_B:
            return "B";

        case KEY_SELECT:
            return "SELECT";

        case KEY_START:
            return "START";

        case KEY_DRIGHT:
            return "D-PAD RIGHT";

        case KEY_DLEFT:
            return "D-PAD LEFT";

        case KEY_DUP:
            return "D-PAD UP";

        case KEY_DDOWN:
            return "D-PAD DOWN";

        case KEY_R:
            return "R";

        case KEY_L:
            return "L";

        case KEY_X:
            return "X";

        case KEY_Y:
            return "Y";

        default:
            return "Invalid button";
    }
}

void debug(const char *fmt, ...) {
    char s[512];
    memset(s, 0, 512);
    va_list args;
    va_start(args, fmt);
    vsprintf(s, fmt, args);
    va_end(args);

    while (aptMainLoop()) {

        hidScanInput();
        if (hidKeysDown() & KEY_A)
            break;

        drawBg();
        gfxDrawText(GFX_TOP, GFX_LEFT, &fontDefault, s, MENU_MIN_X + 16, MENU_MIN_Y + 16);
        gfxDrawText(GFX_TOP, GFX_LEFT, &fontDefault, "Press (A) key to continue...", MENU_MIN_X + 16, MENU_MIN_Y + 64);
        gfxSwap();
    }
}

bool confirm(int confirmButton, const char *fmt, ...) {
    char s[512];
    memset(s, 0, 512);
    va_list args;
    va_start(args, fmt);
    vsprintf(s, fmt, args);
    va_end(args);

    while (aptMainLoop()) {

        hidScanInput();
        u32 key = hidKeysDown();
        if (key & BIT(confirmButton)) {
            return true;
        } else if (key & KEY_B) {
            return false;
        }

        drawBg();
        gfxDrawText(GFX_TOP, GFX_LEFT, &fontDefault, s, MENU_MIN_X + 16, MENU_MIN_Y + 16);
        gfxDrawText(GFX_TOP, GFX_LEFT, &fontDefault, "Press (B) key to cancel...", MENU_MIN_X + 16, MENU_MIN_Y + 64);
        gfxDrawTextf(GFX_TOP, GFX_LEFT, &fontDefault, MENU_MIN_X + 16, MENU_MIN_Y + 84, "Press (%s) to confirm...",
                     get_button(confirmButton));
        gfxSwap();
    }
    return false;
}

bool fileExists(char *path) {
#ifdef ARM9
    FIL file;
    if (f_open(&file, path, FA_READ) != FR_OK) {
        return false;
    }
    f_close(&file);
#else
    if (!path)return false;

    Result ret;
    Handle fileHandle;

    ret = FSUSER_OpenFile(&fileHandle, sdmcArchive, fsMakePath(PATH_ASCII, path),
                          FS_OPEN_READ, 0);
    if (ret != 0) return false;

    ret = FSFILE_Close(fileHandle);
    if (ret != 0) return false;
#endif
    return true;
}

size_t fileSize(const char *path) {
    size_t size = -1;
#ifdef ARM9
    FIL file;
    if (f_open(&file, path, FA_READ | FA_OPEN_EXISTING) != FR_OK) {
        return size;
    }
    size = f_size(&file);
    f_close(&file);
#else
    FILE *file;
    if((file = fopen(path, "rt")) == NULL) {
        return size;
    }
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fclose(file);
#endif
    return size;
}

int fileReadOffset(const char *path, void *data, size_t size, u32 offset) {

#ifdef ARM9
    FIL file;
    UINT bytes_read = 0;
    if (f_open(&file, path, FA_READ | FA_OPEN_EXISTING) != FR_OK) {
        return -1;
    }
    f_lseek(&file, offset);
    u32 newSize = size - offset;
    if (f_read(&file, data, newSize, &bytes_read) != FR_OK) {
        f_close(&file);
        return -1;
    }
    f_close(&file);
#else
    FILE *file;
    if((file = fopen(path, "rt")) == NULL) {
        return -1;
    }
    fseek( file, offset, SEEK_SET );
    u32 newSize = size - offset;
    if(!fread(data, newSize, 1, file)) {
        fclose(file);
        return -1;
    }
    fclose(file);
#endif
    return 0;
}

int fileRead(const char *path, void *data, size_t size) {
    return fileReadOffset(path, data, size, 0);
}

int load_homemenu() {
#ifdef ARM9
    debug("load_homemenu not implemented");
    return -1;
#else
    Handle kill = 0;

    if (srvGetServiceHandle(&kill, "hb:kill") != 0) {
        return -1;
    }

    __appExit();
    srvExit();
    svcSignalEvent(kill);
    svcExitProcess();
#endif
}

void reboot() {
#ifdef ARM9
    i2cWriteRegister(I2C_DEV_MCU, 0x20, 1 << 2);
    while (true);
#else
    aptInit();
    aptOpenSession();
    APT_HardwareResetAsync();
    aptCloseSession();
    aptExit();
#endif
}

void poweroff() {
#ifdef ARM9
    i2cWriteRegister(I2C_DEV_MCU, 0x20, 1 << 0);
    while (true);
#else
    if (khaxInit() != 0) {
        debug("Err: khaxInit");
        return;
    }

    Handle handle;
    Result ret = srvGetServiceHandle(&handle, "ptm:s");
    if (ret != 0) {
        debug("Err: srvGetServiceHandle(ptm:s)");
        return;
    }

    u32 *cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = 0x040700C0; //ShutdownAsync
    cmdbuf[1] = 0; //?
    cmdbuf[2] = 0; //?
    cmdbuf[3] = 0; //?
    ret = svcSendSyncRequest(handle);
    if (ret != 0) {
        debug("Err: srvGetServiceHandle(ptm:s)");
    }
    svcCloseHandle(handle);
#endif
}

#ifdef ARM9

bool aptMainLoop() {
    return true;
}

#endif
