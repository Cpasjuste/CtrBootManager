#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "utility.h"
#include "menu.h"

#ifdef ARM9

#include "arm9/source/common.h"
#include "arm9/source/hid.h"
#include "arm9/source/fatfs/ff.h"

#else
#include <3ds.h>
#include "hb_menu/netloader.h"


extern char boot_app[512];
extern bool boot_app_enabled;

extern void scanMenuEntry(menuEntry_s *me);

int bootApp(char *executablePath, executableMetadata_s *em, char *arg);

void __appInit() {
    srvInit();
    aptInit();
    fsInit();
    sdmcInit();
    openSDArchive();
    hidInit();
    acInit();
    ptmuInit();
    amInit();
    gfxInitDefault();
    gfxSet3D(false);
}

void __appExit() {
    gfxExit();
    netloader_exit();
    configExit();
    amExit();
    ptmuExit();
    acExit();
    hidExit();
    closeSDArchive();
    sdmcExit();
    fsExit();
    aptExit();
    srvExit();
}
#endif

int main() {

#ifdef ARM9
#ifndef EXEC_GATEWAY
    // TODO: Magic?
    *(u32 *) 0x10000020 = 0;
    *(u32 *) 0x10000020 = 0x340;
#endif
    FATFS fs;
    f_mount(&fs, "0:", 0);
#else
    if (netloader_init() != 0) {
        // fix SOC_Initialize
        strcpy(boot_app, "/boot.3dsx");
        boot_app_enabled = true;
    }
    osSetSpeedupEnable(true);

    // offset potential issues caused by homebrew that just ran (from hb_menu)
    aptOpenSession();
    APT_SetAppCpuTimeLimit(0);
    aptCloseSession();

    if (!boot_app_enabled) { // fix SOC_Initialize
#endif
    if (configInit() != 0 || config->count <= 0) { // recovery
#ifdef ARM9
        drawBg();
        debug("configInit failed");
        hidKeysDown();
        reboot();
#else
        while (aptMainLoop()) {
            if (menu_more() == 0)
                break;
        }
#endif
    } else {
        while (aptMainLoop()) {
            if (menu_boot() == 0)
                break;
        }
    }
#ifndef ARM9
    }

    menuEntry_s *me = malloc(sizeof(menuEntry_s));
    strncpy(me->executablePath, boot_app, 128);
    initDescriptor(&me->descriptor);
    static char xmlPath[128];
    snprintf(xmlPath, 128, "%s", boot_app);
    int l = strlen(xmlPath);
    xmlPath[l - 1] = 0;
    xmlPath[l - 2] = 'l';
    xmlPath[l - 3] = 'm';
    xmlPath[l - 4] = 'x';
    if (fileExists(xmlPath))
        loadDescriptor(&me->descriptor, xmlPath);
    scanMenuEntry(me);

    return bootApp(me->executablePath, &me->descriptor.executableMetadata, me->arg);
#else
    return 0;
#endif
}
