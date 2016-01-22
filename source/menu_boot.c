#include <3ds.h>
#include <time.h>
#include <string.h>
#include <stdio.h>

#include "config.h"
#include "loader.h"
#include "menu.h"
#include "utility.h"
#include "gui.h"

bool timer = true;

int autoBootFix(int index) {

    int delay = config->autobootfix;
    while (aptMainLoop() && delay > 0) {
        guiSwap();
        delay--;
    }

    hidScanInput();
    u32 k = hidKeysHeld();
    if(k) {
        int i = 0;
        for(i=0; i<config->count; i++) {
            if(k & BIT(config->entries[i].key)) {
                index = i;
                break;
            }
        }
    }

    return load(config->entries[index].path,
                config->entries[index].offset);
}

int menu_boot() {

    time_t start, end, elapsed;
    int boot_index = config->index;

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

        // gui top
        guiStart(GFX_TOP);
        guiDrawBg();

        if (!timer) {
            guiDrawTextCenter(GFX_TOP, config->fntDef, 8, 14, "Select a boot entry");
        } else {
            guiDrawTextCenter(GFX_TOP, config->fntDef, 8, 14, "Booting %s in %i",
                    config->entries[boot_index].title, config->timeout - elapsed);
        }

        char info[1024]; memset(info, 0, 1024);
        int minX = 16, maxX = 400 - 16;
        int minY = 40;

        int i;
        for (i = 0; i < config->count; i++) {
            if (i >= config->count)
                break;

            if (i == boot_index) {
                guiDrawRect(config->highlight, minX + 4, minY + (16 * i), maxX - 23, 17);
                guiDrawText(config->fntSel, minX + 6, minY + (16 * i), 14, "%s",
                            config->entries[i].title);
                snprintf(info, 1024,
                         "Name: %s\nPath: %s\nOffset: 0x%lx\n\n\nPress (A) to launch\nPress (X) to remove entry\n",
                        config->entries[i].title,
                        config->entries[i].path,
                        config->entries[i].offset);
            }
            else
                guiDrawText(config->fntDef, minX + 6, minY + (16 * i), 14, config->entries[i].title);
        }

        if (boot_index == config->count) {
            guiDrawRect(config->highlight, minX + 4, minY + (16 * i), maxX - 23, 17);
            guiDrawText(config->fntSel, minX + 6, minY + (16 * i), 14, "More...");
            strncpy(info, "Show more options ...", 1024);
        }
        else {
            guiDrawText(config->fntDef, minX + 6, minY + (16 * i), 14, "More...");
        }
        guiEnd();
        // gui top

        // gui bottom
        guiStart(GFX_BOTTOM);
        guiDrawTextCenter(GFX_BOTTOM, config->fntDef, 20, 14, "Informations");
        guiDrawText(config->fntDef, minX + 6, 70, 14, info);
        guiEnd();
        // gui bottom

        guiSwap();
    }
    return 0;
}
