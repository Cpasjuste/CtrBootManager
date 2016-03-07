#ifdef ARM9

#include "arm9/source/common.h"
#include "arm9/source/hid.h"

#else
#include <3ds.h>
#include <time.h>
#endif

#include "loader.h"
#include "gfx.h"
#include "utility.h"
#include "menu.h"
#include "config.h"

static bool timer = true;

static void draw(int boot_index, time_t elapsed);

int key_override(int index) {

    hidScanInput();
    u32 k = hidKeysHeld();
    if (k) {
        int i = 0;
        for (i = 0; i < config->count; i++) {
            if (k & BIT(config->entries[i].key)) {
                return i;
            }
        }
    }
    return index;
}

int boot(int index) {

#ifdef ARM9
    return load(config->entries[index].path,
                config->entries[index].offset);
#else
    int delay = config->autobootfix;
    while (aptMainLoop() && delay > 0) {
        gfxSwap();
        delay--;
    }
    return load(config->entries[index].path,
                config->entries[index].offset);
#endif
}

int menu_boot() {

#ifdef ARM9
    time_t elapsed = 0;
    vu32 now = 0;
#else
    time_t start, end, elapsed = 0;
#endif
    int boot_index = config->index;

    hidScanInput();
    u32 key = hidKeysHeld();
    if (key & BIT(config->recovery) || config->timeout < 0) { // disable autoboot
        timer = false;
    } else if (config->timeout == 0) { // autoboot
        int index = key_override(boot_index);
        return boot(index);
    }

#ifndef ARM9
    time(&start);
#endif

    while (aptMainLoop()) {

        if (timer) {
#ifdef ARM9
            now++;
            elapsed = (int)(now/25); // fake/crappy timer
#else
            time(&end);
            elapsed = end - start;
#endif
            if (elapsed >= config->timeout && config->count > boot_index) {
                int index = key_override(boot_index);
                return boot(index);
            }
        }

        hidScanInput();
        u32 kDown = hidKeysDown();

        if (kDown & KEY_DOWN) {
            timer = false;
            boot_index++;
            if (boot_index > config->count)
                boot_index = 0;
        }
        else if (kDown & KEY_UP) {
            timer = false;
            boot_index--;
            if (boot_index < 0)
                boot_index = config->count;
        }
        else if (kDown & KEY_A) {
            timer = false;
            if (boot_index == config->count) {
                if(menu_more() == 0) {
                    break;
                }
            } else if (boot(boot_index)) {
                break;
            }
        }
        else if (kDown & KEY_X) {
            timer = false;
            if (boot_index != config->count) {
                if (confirm(3, "Delete boot entry: \"%s\" ?\n", config->entries[boot_index].title)) {
                    configRemoveEntry(boot_index);
                    boot_index = config->index;
                }
            }
        }

        draw(boot_index, elapsed);
    }

    return 0;
}

static void draw(int boot_index, time_t elapsed) {

    int i = 0;

    drawBg();

    if (!timer) {
        drawTitle("*** Select a boot entry ***");
    } else {
        drawTitle("*** Booting %s in %i ***", config->entries[boot_index].title, config->timeout - elapsed);
    }

    for (i = 0; i < config->count; i++) {
        drawItem(i == boot_index, 16 * i, config->entries[i].title);
        if (i == boot_index) {
            drawInfo("Name: %s\nPath: %s\nOffset: 0x%lx\n\n\nPress (A) to launch\nPress (X) to remove entry\n",
                     config->entries[i].title,
                     config->entries[i].path,
                     config->entries[i].offset);
        }
    }
    drawItem(boot_index == config->count, 16 * i, "More...");
    if (boot_index == config->count) {
        drawInfo("Show more options ...");
    }

    gfxSwap();
}
