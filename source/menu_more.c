#include <3ds.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "gfx.h"
#include "utility.h"
#include "picker.h"
#include "loader.h"
#include "menu.h"
#include "config.h"

static char menu_item[5][512] = {"File browser", "Netload 3dsx", "Settings", "Reboot", "PowerOff"};
static int menu_count = 5;
static int menu_index = 0;
bool imgError2 = false;
u8* bgImgTopBuff;

int menu_choose() {

    file_s picked;
    memset(&picked, 0, sizeof(file_s));
    pick_file(&picked, "/");

    if (strlen(picked.path) > 0) {
        return load(picked.path, 0);
    }
    return -1;
}

int menu_more() {

    menu_index = 0;

    //Load User set bgImgTop
    FILE *file = fopen(config->bgImgTop,"rb");
    if (file == NULL){
        imgError2 = true;
        goto imgSkip;
    }
    fseek(file,0,SEEK_END);
    off_t bgImgTopSize = ftell(file);
    fseek(file,0,SEEK_SET);
    bgImgTopBuff = malloc(bgImgTopSize);
    if(!bgImgTopBuff){
        imgError2 = true;
        goto imgSkip;
    }
    off_t bgImgTopRead = fread(bgImgTopBuff,1,bgImgTopSize,file);
    fclose(file);
    if(bgImgTopSize!=bgImgTopRead){
        imgError2 = true;
        goto imgSkip;
    }

    //On Img Error skip image code
    imgSkip:

    while (aptMainLoop()) {

        hidScanInput();
        u32 kDown = hidKeysDown();

        if (kDown & KEY_DOWN) {
            menu_index++;
            if (menu_index >= menu_count)
                menu_index = 0;
        }

        if (kDown & KEY_UP) {
            menu_index--;
            if (menu_index < 0)
                menu_index = menu_count - 1;
        }

        if (kDown & KEY_A) {
            if (menu_index == 0 && menu_choose() == 0) {
                return 0;
            } else if (menu_index == 1 && menu_netloader() == 0) {
                return 0;
            } else if (menu_index == 2) {
                menu_config();
            } else if (menu_index == 3) {
                reboot();
            } else if (menu_index == 4) {
                poweroff();
            }
        }

        if (kDown & KEY_B) {
            return -1;
        }

        gfxClear();

        if(!imgError2){
            memcpy(gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL), bgImgTopBuff, bgImgTopSize);
            memcpy(gfxGetFramebuffer(GFX_TOP, GFX_RIGHT, NULL, NULL), bgImgTopBuff, bgImgTopSize);
        }

        gfxDrawText(GFX_TOP, GFX_LEFT, &fontDefault, "*** Select an option ***", 140, 20);

        int minX = 16;
        int maxX = 400 - 16;
        int minY = 32;
        int maxY = 240 - 16;
        drawRectColor(GFX_TOP, GFX_LEFT, minX, minY, maxX, maxY, config->borders);
        minY += 20;

        int i;
        for (i = 0; i < menu_count; i++) {
            if (i >= menu_count) break;

            if (i == menu_index) {
                gfxDrawRectangle(GFX_TOP, GFX_LEFT, config->highlight, minX + 4, minY + (16 * i), maxX - 23,
                                 15);
                gfxDrawText(GFX_TOP, GFX_LEFT, &fontSelected, menu_item[i], minX + 6, minY + (16 * i));
            }
            else
                gfxDrawText(GFX_TOP, GFX_LEFT, &fontDefault, menu_item[i], minX + 6, minY + (16 * i));
        }

        // draw "help"
        gfxDrawText(GFX_BOTTOM, GFX_LEFT, &fontDefault, "Informations", minX + 6, 20);
        switch (menu_index) {
            case 0:
                gfxDrawText(GFX_BOTTOM, GFX_LEFT, &fontDefault,
                            "Browse for a file to boot or add a boot entry", minX + 12, 40);
                break;

            case 1:
                gfxDrawText(GFX_BOTTOM, GFX_LEFT, &fontDefault,
                            "Netload a file (3dsx) from the computer with 3dslink", minX + 12, 40);
                break;

            case 2:
                gfxDrawText(GFX_BOTTOM, GFX_LEFT, &fontDefault,
                            "Edit boot settings", minX + 12, 40);
                break;

            case 3:
                gfxDrawText(GFX_BOTTOM, GFX_LEFT, &fontDefault,
                            "Reboot the 3ds...", minX + 12, 40);
                break;

            case 4:
                gfxDrawText(GFX_BOTTOM, GFX_LEFT, &fontDefault,
                            "Shutdown the 3ds...", minX + 12, 40);
                break;

            default:
                break;
        }

        gfxSwap();
    }
    return -1;
}
