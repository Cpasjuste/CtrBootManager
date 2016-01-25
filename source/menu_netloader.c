#include <3ds.h>
#include <stdio.h>
#include <string.h>

#include "hb_menu/gfx.h"
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

    gfxClear();
    if (!config->imgError){
        memcpy(gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL), config->bgImgTopBuff, config->bgImgTopSize);
        memcpy(gfxGetFramebuffer(GFX_TOP, GFX_RIGHT, NULL, NULL), config->bgImgTopBuff, config->bgImgTopSize);
    }
    if (!config->imgErrorBot){
        memcpy(gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL), config->bgImgBotBuff, config->bgImgBotSize);
    }
    gfxDrawTextf(GFX_TOP, GFX_LEFT, &fontDefault, 48, 48, msg);
    gfxSwap();

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
