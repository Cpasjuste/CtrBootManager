#include <3ds.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hb_menu/netloader.h"
#include "config.h"
#include "scanner.h"
#include "utility.h"
#include "menu.h"
#include "gui.h"

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
}

void __appExit() {
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

int main(int argc, char *argv[]) {

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

    guiInit();

    if (!boot_app_enabled) { // fix SOC_Initialize

        if (configInit() != 0 || config->count <= 0) { // recovery
            while (aptMainLoop()) {
                if (menu_more() == 0)
                    break;
            }
        } else {
            while (aptMainLoop()) {
                if (menu_boot() == 0)
                    break;
            }
        }
    }

    guiExit();

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
}
