#include <3ds.h>
#include <string.h>

#include "gfx.h"
#include "utility.h"
#include "picker.h"
#include "loader.h"
#include "menu.h"

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

    int i = 0;
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

        drawBg();
        drawTitle("*** Select an option ***");

        for (i = 0; i < menu_count; i++) {
            drawItem(i == menu_index, 16 * i, menu_item[i]);
        }

        // draw "help"
        switch (menu_index) {
            case 0:
                drawInfo("Browse for a file to boot or add a boot entry");
                break;
            case 1:
                drawInfo("Netload a file (3dsx) from the computer with 3dslink");
                break;
            case 2:
                drawInfo("Edit boot settings");
                break;
            case 3:
                drawInfo("Reboot the 3ds...");
                break;
            case 4:
                drawInfo("Shutdown the 3ds...");
                break;
            default:
                break;
        }

        gfxSwap();
    }
    return -1;
}
