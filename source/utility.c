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

        gfxClear();
        gfxDrawText(GFX_TOP, GFX_LEFT, &fontTitle, s, 8, 20);
        gfxDrawText(GFX_TOP, GFX_LEFT, &fontTitle, "Press A to continue...", 8, 36);
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
