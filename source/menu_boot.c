#include <3ds.h>
#include <time.h>

#include "gfx.h"
#include "config.h"
#include "loader.h"
#include "menu.h"
#include "utility.h"

bool timer = true;


int autoBootFix(int index) {

    int delay = config->autobootfix;
    while (aptMainLoop() && delay > 0) {
        gfxSwap();
        delay--;
    }

    hidScanInput();
    u32 k = hidKeysHeld();
    if (k) {
        int i = 0;
        for (i = 0; i < config->count; i++) {
            if (k & BIT(config->entries[i].key)) {
                index = i;
                break;
            }
        }
    }

    return load(config->entries[index].path,
                config->entries[index].offset);
}

int menu_boot() {

    time_t start, end, elapsed = 0;
    int boot_index = config->index;
    int i = 0;

    hidScanInput();
    if (config->timeout < 0 || hidKeysHeld() & BIT(config->recovery)) { // disable autoboot
        timer = false;
    } else if (config->timeout == 0) { // autoboot
        return autoBootFix(boot_index);
    }

    time(&start);

    while (aptMainLoop()) {
        hidScanInput();
        u32 kDown = hidKeysDown();

        if (timer) {
            time(&end);
            elapsed = end - start;
            if (elapsed >= config->timeout
                && config->count > boot_index) {
                return autoBootFix(boot_index);
            }
        }

        if (kDown & KEY_DOWN) {
            timer = false;
            boot_index++;
            if (boot_index > config->count)
                boot_index = 0;
        }

        if (kDown & KEY_UP) {
            timer = false;
            boot_index--;
            if (boot_index < 0)
                boot_index = config->count;
        }

        if (kDown & KEY_A) {
            timer = false;
            if (boot_index == config->count) {
                if (menu_more() == 0) {
                    break;
                }
            } else {
                if (load(config->entries[boot_index].path,
                         config->entries[boot_index].offset) == 0) {
                    break;
                }
            }
        }

        if (kDown & KEY_X) {
            timer = false;
            if (boot_index != config->count) {
                if (confirm(3, "Delete boot entry: \"%s\" ?\n", config->entries[boot_index].title)) {
                    configRemoveEntry(boot_index);
                    boot_index--;
                }
            }
        }

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
    return 0;
}
