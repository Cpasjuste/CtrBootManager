#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <CakeBrah/source/libkhax/khax.h>

#include "gfx.h"

FS_archive sdmcArchive;

void openSDArchive() {
    sdmcArchive = (FS_archive) {0x00000009, (FS_path) {PATH_EMPTY, 1, (u8 *) ""}};
    FSUSER_OpenArchive(NULL, &sdmcArchive);
}

void closeSDArchive() {
    FSUSER_CloseArchive(NULL, &sdmcArchive);
}

void svcSleep(u32 millis) {
    u64 nano = millis * 1000000;
    svcSleepThread(nano);
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
        if (hidKeysDown())
            break;

        gfxClear();
        gfxDrawText(GFX_TOP, GFX_LEFT, &fontDefault, s, 8, 32);
        gfxDrawText(GFX_TOP, GFX_LEFT, &fontDefault, "Press any key to continue...", 8, 64);
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
        } else if (key) {
            return false;
        }

        gfxClear();
        gfxDrawText(GFX_TOP, GFX_LEFT, &fontDefault, s, 8, 32);
        gfxDrawText(GFX_TOP, GFX_LEFT, &fontDefault, "Press any key to cancel...", 8, 64);
        gfxDrawTextf(GFX_TOP, GFX_LEFT, &fontDefault, 8, 80, "Press (%s) to confirm...", get_button(confirmButton));
        gfxSwap();
    }
}

bool fileExists(char *path) {
    if (!path)return false;

    Result ret;
    Handle fileHandle;

    ret = FSUSER_OpenFile(NULL, &fileHandle, sdmcArchive, FS_makePath(PATH_CHAR, path), FS_OPEN_READ,
                          FS_ATTRIBUTE_NONE);
    if (ret != 0)return false;

    ret = FSFILE_Close(fileHandle);
    if (ret != 0)return false;

    return true;
}

void load_homemenu() {

    /*
    if (nsInit() != 0) {
        debug("Err: nsInit...\n");
    } else {
        if (NS_RebootToTitle(0x0004003000009802, 0) != 0) {
            debug("Err: NS_LaunchTitle...\n");
        }
    }
    nsExit();
    */
}

void reboot() {
    aptInit();
    aptOpenSession();
    APT_HardwareResetAsync(NULL);
    aptCloseSession();
    aptExit();
}

void poweroff() {

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
}
