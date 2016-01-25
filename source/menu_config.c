#include <3ds.h>
#include <time.h>

#include "config.h"
#include "gfx.h"
#include "utility.h"
#include "menu.h"

void keyLeft(int index) {

    switch (index) {
        case 0:
            if (config->timeout > -1)
                config->timeout--;
            break;
        case 1:
            if (config->index > 0)
                config->index--;
            else
                config->index = config->count - 1;
            break;
        case 2:
            if (config->autobootfix > 0)
                config->autobootfix--;
            break;
        case 3:
            if (config->recovery > 0)
                config->recovery--;
            else
                config->recovery = 11;
            break;
    }
}

void keyRight(int index) {

    switch (index) {
        case 0:
            config->timeout++;
            break;
        case 1:
            if (config->index < config->count - 1)
                config->index++;
            else
                config->index = 0;
            break;
        case 2:
            config->autobootfix++;
            break;
        case 3:
            if (config->recovery < 11)
                config->recovery++;
            else
                config->recovery = 0;
            break;
    }
}

int menu_config() {

    int menu_count = 4, menu_index = 0;
    // key repeat timer
    time_t t_start = 0, t_end = 0, t_elapsed = 0;

    while (aptMainLoop()) {

        hidScanInput();
        u32 kHeld = hidKeysHeld();
        u32 kDown = hidKeysDown();

        if (hidKeysUp()) {
            time(&t_start); // reset held timer
        }

        if (kDown & KEY_DOWN) {
            menu_index++;
            if (menu_index >= menu_count)
                menu_index = 0;
        }
        else if (kDown & KEY_UP) {
            menu_index--;
            if (menu_index < 0)
                menu_index = menu_count - 1;
        }

        if (kDown & KEY_LEFT) {
            keyLeft(menu_index);
            time(&t_start);
        } else if (kHeld & KEY_LEFT) {
            time(&t_end);
            t_elapsed = t_end - t_start;
            if (t_elapsed > 0) {
                keyLeft(menu_index);
                svcSleep(100);
            }
        }

        if (kDown & KEY_RIGHT) {
            keyRight(menu_index);
            time(&t_start);
        } else if (kHeld & KEY_RIGHT) {
            time(&t_end);
            t_elapsed = t_end - t_start;
            if (t_elapsed > 0) {
                keyRight(menu_index);
                svcSleep(100);
            }
        }

        if (kDown & KEY_B) {
            configUpdateSettings();
            return 0;
        }

        drawBg();
        drawTitle("*** Boot configuration ***");

        drawItem(menu_index == 0, 0, "Timeout:  %i", config->timeout);
        drawItem(menu_index == 1, 16, "Default:  %s", config->entries[config->index].title);
        drawItem(menu_index == 2, 32, "Bootfix:  %i", config->autobootfix);
        drawItem(menu_index == 3, 48, "Recovery key:  %s", get_button(config->recovery));

        gfxSwap();
    }
    return -1;
}
