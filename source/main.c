#include <3ds.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <malloc.h>

#include "gfx.h"
#include "config.h"
#include "scanner.h"
#include "picker.h"

extern char boot_app[512];
extern bool boot_app_enabled;
extern int bootApp(char* executablePath, executableMetadata_s* em);

int main() {

    srvInit();
	aptInit();
	gfxInitDefault();
	fsInit();
	openSDArchive();
	hidInit(NULL);
	irrstInit(NULL);
	acInit();
	ptmInit();

    // offset potential issues caused by homebrew that just ran (from hb_menu)
	aptOpenSession();
	APT_SetAppCpuTimeLimit(NULL, 0);
	aptCloseSession();
    srand(svcGetSystemTick());

    /*
    if (khaxInit() != 0) {
        debug("Could not get kernel access...Rebooting to fix things...");
        reboot();
        return 0;
    }
    hookSig();
    */

	if(configInit() != 0) { // recovery .. should change to sysnand reboot
        while(aptMainLoop()) {
            if(menu_more() == 0)
                break;
        }
	} else {
        while(aptMainLoop()) {
            if(menu_boot() == 0)
                break;
        }
    }

    executableMetadata_s *em;
    if(boot_app_enabled) {
        em = malloc(sizeof(executableMetadata_s));
        scanExecutable(em, boot_app);
    }

    configExit();
	ptmExit();
	acExit();
	irrstExit();
	hidExit();
	gfxExit();
	closeSDArchive();
	fsExit();
	aptExit();
	srvExit();

	return bootApp(boot_app, em);
}
