#include <3ds.h>
#include <time.h>

#include "gfx.h"
#include "config.h"
#include "loader.h"
#include "menu.h"
#include "utility.h"
#include "font.h"

bool timer = true;

int autoBootFix(int index) {

    int delay = config->autobootfix;
    while (aptMainLoop() && delay > 0) {
        gfxSwap();
        delay--;
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
    } else if (config->timeout == 0
               && config->count > boot_index) { // autoboot
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

        gfxClear();

        if (!timer) {
            gfxDrawText(GFX_TOP, GFX_LEFT, &fontTitle, "*** Select a boot entry ***", 120, 20);
        } else {
            gfxDrawTextf(GFX_TOP, GFX_LEFT, &fontTitle, 100, 20,
                         "*** Booting %s in %i ***", config->entries[boot_index].title,
                         config->timeout - elapsed);
        }

        int minX = 16, maxX = 400 - 16;
        int minY = 32, maxY = 240 - 8;
        drawRect(GFX_TOP, GFX_LEFT, minX, minY, maxX, maxY, 0xFF, 0xFF, 0xFF);
        minY += 20;

        int i;
        for (i = 0; i < config->count; i++) {
            if (i >= config->count)
                break;

            if (i == boot_index) {
                gfxDrawRectangle(GFX_TOP, GFX_LEFT, (u8[]) {0xDC, 0xDC, 0xDC}, minX + 4, minY + (16 * i), maxX - 23,
                                 15);
                gfxDrawTextf(GFX_TOP, GFX_LEFT, &fontSelected, minX + 6, minY + (16 * i), "%s",
                             config->entries[i].title);

                gfxDrawText(GFX_BOTTOM, GFX_LEFT, &fontTitle, "Informations", minX + 6, 20);
                gfxDrawTextf(GFX_BOTTOM, GFX_LEFT, &fontDefault, minX + 12, 40,
                             "Name: %s\nPath: %s\nOffset: 0x%lx\n\n\nPress (A) to launch\nPress (X) to remove entry\n",
                             config->entries[i].title,
                             config->entries[i].path,
                             config->entries[i].offset);
            }
            else
                gfxDrawText(GFX_TOP, GFX_LEFT, &fontDefault, config->entries[i].title, minX + 6, minY + (16 * i));
        }

        if (boot_index == config->count) {
            gfxDrawRectangle(GFX_TOP, GFX_LEFT, (u8[]) {0xDC, 0xDC, 0xDC}, minX + 4, minY + (16 * i), maxX - 23, 15);
            gfxDrawText(GFX_TOP, GFX_LEFT, &fontSelected, "More...", minX + 6, minY + (16 * i));
            gfxDrawText(GFX_BOTTOM, GFX_LEFT, &fontTitle, "Informations", minX + 6, 20);
            gfxDrawText(GFX_BOTTOM, GFX_LEFT, &fontDefault, "Show more options ...", minX + 12, 40);
        }
        else
            gfxDrawText(GFX_TOP, GFX_LEFT, &fontDefault, "More...", minX + 6, minY + (16 * i));

        gfxSwap();
    }
    return 0;
}
