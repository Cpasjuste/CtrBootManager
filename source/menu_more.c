#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "gfx.h"
#include "utility.h"
#include "picker.h"

//char menu_item[5][512] = { "Browse for a file to boot", "Netload 3dsx", "Load HomeMenu", "Reboot", "PowerOff" };
//int menu_count = 5;
char menu_item[4][512] = { "Browse for a file to boot", "Netload 3dsx", "Reboot", "PowerOff" };
int menu_count = 4;
int menu_index = 0;

int menu_choose() {

	file_s picked;
	memset(&picked, 0, sizeof(file_s));
    pick_file(&picked, "/");

    if(strlen(picked.path)>0) {
        return load(picked.path, 0);
    }
    return -1;
}

int menu_more() {

    menu_index = 0;

	while (aptMainLoop()) {

		hidScanInput();
		u32 kDown = hidKeysDown();

		if(kDown & KEY_DOWN) {
			menu_index++;
			if(menu_index >= menu_count)
				menu_index = 0;
		}

		if(kDown & KEY_UP) {
			menu_index--;
			if(menu_index < 0)
				menu_index = menu_count-1;
		}

		if(kDown & KEY_A) {
            if(menu_index == 0 && menu_choose() == 0) {
                return 0;
            } else if(menu_index == 1 && menu_netloader() == 0) {
                return 0;
            //} else if(menu_index == 2) {
            //    load_homemenu();
            } else if(menu_index == 2) {
                reboot();
            } else if(menu_index == 3) {
				poweroff();
            }
		}

        if(kDown & KEY_B) {
            return -1;
        }

		gfxClear();
        gfxDrawText(GFX_TOP, GFX_LEFT, &fontTitle, "*** Select an option ***", 120, 20);

        int minX = 16;
        int maxX = 400-16;
        int minY = 32;
        int maxY = 240-16;
		drawRect(GFX_TOP, GFX_LEFT, minX, minY, maxX, maxY, 0xFF, 0xFF, 0xFF);
        minY+=20;

		int i;
		for(i=0; i<menu_count; i++) {
			if(i >= menu_count) break;

			if(i == menu_index)
			{
                gfxDrawRectangle(GFX_TOP, GFX_LEFT, (u8[]){0xDC, 0xDC, 0xDC}, minX+4, minY+(16*i), maxX-23, 15);
				gfxDrawText(GFX_TOP, GFX_LEFT, &fontSelected, menu_item[i], minX+6, minY+(16*i));
            }
			else
				gfxDrawText(GFX_TOP, GFX_LEFT, &fontDefault, menu_item[i], minX+6, minY+(16*i));
		}
		gfxSwap();
	}
	return -1;
}
