#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "gfx.h"
#include "config.h"

bool timer = true;

int autoBootFix() {

    int delay = boot_config->autobootfix;
    while(aptMainLoop() && delay>0) {
        //gfxClearColor((u8[]){0x00, 0x00, 0x00});
        gfxSwap();
        delay--;
    }
    return load(boot_config->entries[boot_config->index].path,
                boot_config->entries[boot_config->index].offset);
}

int menu_boot() {

	int start, end;
	int elapsed;
	time(&start);

    //debug("timeout: %i", boot_config->timeout);

	if(boot_config->timeout < 0) { // disable autoboot
        timer = false;
	} else if(boot_config->timeout == 0) { // autoboot
        hidScanInput();
        if(hidKeysHeld() & boot_config->recovery) {  // recovery key
            timer = false;
        } else {
            if(autoBootFix() == 0) {
                return 0;
            }
        }
    }

	while (aptMainLoop())
	{
		hidScanInput();
		u32 kDown = hidKeysDown();

		if(timer) {
            time(&end);
            elapsed = end - start;
            if(elapsed >= boot_config->timeout) {
                if(autoBootFix() == 0) {
                    break;
                }
            }
        }

		if(kDown & KEY_DOWN)
		{
			timer = false;
			boot_config->index++;
			if(boot_config->index > boot_config->count)
				boot_config->index = 0;
		}

		if(kDown & KEY_UP)
		{
			timer = false;
			boot_config->index--;
			if(boot_config->index < 0)
				boot_config->index = boot_config->count;
		}

		if(kDown & KEY_A) {
            timer = false;
            if(boot_config->index == boot_config->count) {
                if(menu_more() == 0) {
                    break;
                }
            } else {
                if(load(boot_config->entries[boot_config->index].path,
                        boot_config->entries[boot_config->index].offset) == 0) {
                    break;
                }
            }
        }

        gfxClear();

		if(!timer) {
			gfxDrawText(GFX_TOP, GFX_LEFT, &fontTitle, "*** Select a boot entry ***", 120, 20);
		} else {
            gfxDrawTextf(GFX_TOP, GFX_LEFT, &fontTitle, 100, 20,
                "*** Booting %s in %i ***", boot_config->entries[boot_config->index].title,
                boot_config->timeout-elapsed );
		}

        int minX = 16;
        int maxX = 400-16;
        int minY = 32;
        int maxY = 240-16;
		drawRect(GFX_TOP, GFX_LEFT, minX, minY, maxX, maxY, 0xFF, 0xFF, 0xFF);
        minY+=20;

		int i;
		for(i=0; i<boot_config->count; i++)
		{
			if(i >= boot_config->count)
                break;

			if(i==boot_config->index)
			{
                gfxDrawRectangle(GFX_TOP, GFX_LEFT, (u8[]){0xDC, 0xDC, 0xDC}, minX+4, minY+(16*i), maxX-23, 15);
				gfxDrawTextf(GFX_TOP, GFX_LEFT, &fontSelected, minX+6, minY+(16*i), "%s",
                    boot_config->entries[i].title);

                gfxDrawText(GFX_BOTTOM, GFX_LEFT, &fontTitle, "Informations", minX+6, 20);
                gfxDrawTextf(GFX_BOTTOM, GFX_LEFT, &fontDefault, minX+12, 40,
                    "Name: %s\nPath: %s\nOffset: 0x%lx",
                    boot_config->entries[i].title,
                    boot_config->entries[i].path,
                    boot_config->entries[i].offset);
            }
			else
				gfxDrawText(GFX_TOP, GFX_LEFT, &fontDefault, boot_config->entries[i].title, minX+6, minY+(16*i));
		}

		if(boot_config->index == boot_config->count)
		{
            gfxDrawRectangle(GFX_TOP, GFX_LEFT, (u8[]){0xDC, 0xDC, 0xDC}, minX+4, minY+(16*i), maxX-23, 15);
			gfxDrawText(GFX_TOP, GFX_LEFT, &fontSelected, "More...", minX+6, minY+(16*i));
        }
		else
			gfxDrawText(GFX_TOP, GFX_LEFT, &fontDefault, "More...", minX+6, minY+(16*i));

		gfxSwap();
	}
	return 0;
}
