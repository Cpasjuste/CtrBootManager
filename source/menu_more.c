#include <3ds.h>
#include <string.h>
#include <gui/gui.h>

#include "utility.h"
#include "picker.h"
#include "loader.h"
#include "menu.h"
#include "config.h"

static char menu_item[5][512] = {"File browser", "Netload 3dsx", "Settings", "Reboot", "PowerOff"};
static int menu_count = 5;
static int menu_index = 0;

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

        // gui top
        guiStart(GFX_TOP);
        guiDrawBg();

        guiDrawTextCenter(GFX_TOP, config->fntDef, 8, 14, "Select an option");

        int minX = 16; int maxX = 400 - 16;
        int minY = 40;

        int i;
        for (i = 0; i < menu_count; i++) {
            if (i >= menu_count) break;

            if (i == menu_index) {
                guiDrawRect(config->highlight, minX + 4, minY + (16 * i), maxX - 23, 17);
                guiDrawText(config->fntSel, minX + 6, minY + (16 * i), 14, menu_item[i]);
            }
            else {
                guiDrawText(config->fntDef, minX + 6, minY + (16 * i), 14, menu_item[i]);
            }
        }
        guiEnd();
        // gui top

        // gui bottom
        guiStart(GFX_BOTTOM);
        guiDrawTextCenter(GFX_BOTTOM, config->fntDef, 20, 14, "Informations");
        switch (menu_index) {
            case 0:
                guiDrawText(config->fntDef, minX + 6, 70, 14,
                            "Browse for a file to boot or add a boot entry");
                break;

            case 1:
                guiDrawText(config->fntDef, minX + 6, 70, 14,
                            "Netload a file (3dsx) from the computer\nwith 3dslink");
                break;

            case 2:
                guiDrawText(config->fntDef, minX + 6, 70, 14,
                            "Edit boot settings");
                break;

            case 3:
                guiDrawText(config->fntDef, minX + 6, 70, 14,
                            "Reboot the 3ds...");
                break;

            case 4:
                guiDrawText(config->fntDef, minX + 6, 70, 14,
                            "Shutdown the 3ds...");
                break;

            default:
                break;
        }
        guiEnd();
        // gui bottom

        guiSwap();
    }
    return -1;
}
