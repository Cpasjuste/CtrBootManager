#include <3ds.h>
#include <stdio.h>
#include <gui/gui.h>

#include "netloader.h"
#include "utility.h"
#include "loader.h"
#include "config.h"

int menu_netloader() {

    if (netloader_activate() != 0) {
        debug("Err: netloader_activate");
        return -1;
    }

    char msg[256];
    u32 ip = gethostid();
    sprintf(msg,
            "NetLoader Active - waiting for 3dslink\n\n"
                    "IP: %lu.%lu.%lu.%lu, Port: %d\n\n"
                    "Press B to cancel\n",
            ip & 0xFF, (ip >> 8) & 0xFF, (ip >> 16) & 0xFF, (ip >> 24) & 0xFF, NETLOADER_PORT);

    // gui top
    guiStart(GFX_TOP);
    guiDrawBg();
    guiDrawText(config->fntDef, 48, 48, 14, msg);
    guiEnd();
    guiSwap();

    while (aptMainLoop()) {

        hidScanInput();
        u32 kDown = hidKeysDown();

        if (kDown & KEY_B) {
            netloader_deactivate();
            return -1;
        }

        int rc = netloader_loop();
        if (rc > 0) {
            netloader_boot = true;
            return load_3dsx(netloadedPath);
        } else if (rc < 0) {
            netloader_draw_error();
            break;
        }
    }
    return -1;
}
