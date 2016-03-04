#ifndef ARM9
#include <string.h>
#include <3ds.h>
#include "brahma.h"
#else

#include "gfx.h"
#include "arm9/source/common.h"
#include "stage2_bin.h"

#endif

#include "utility.h"
#include "memory.h"

#ifdef ARM9

int load_bin(char *path, long offset) {

    size_t size = fileSize(path);
    if (!size) {
        return -1;
    }

    if (fileReadOffset(path, (void *) PTR_PAYLOAD_MAIN_DATA, size, offset) != 0) {
        return -1;
    }

    memcpy((void *) PTR_PAYLOAD_STAGE2, stage2_bin, stage2_bin_size);
    ((void (*)()) PTR_PAYLOAD_STAGE2)();

    return 0;
}

#else
char boot_app[512];
bool boot_app_enabled;

int load_3dsx(char *path) {
    memset(boot_app, 0, 512);
    strncpy(boot_app, path, 512);
    boot_app_enabled = true;
    return 0;
}

int load_bin(char *path, long offset) {

    gfxExit();

    if (brahma_init()) {
        if (load_arm9_payload_offset(path, (u32) offset, 0x10000) != 1) {
            debug("Err: Couldn't load arm9 payload...\n");
            return -1;
        }
        firm_reboot();
        brahma_exit();

    } else {
        debug("Err: Couldn't init brahma...\n");
        return -1;
    }

    return 0;
}
#endif

int load(char *path, long offset) {
    // check for reboot/poweroff
    if (strcasecmp(path, "reboot") == 0) {
        reboot();
    } else if (strcasecmp(path, "shutdown") == 0) {
        poweroff();
#ifndef ARM9
        } else if (strcasecmp(path, "homemenu") == 0) {
            return load_homemenu();
#endif
    } else {
        const char *ext = get_filename_ext(path);
        if (strcasecmp(ext, "bin") == 0
            || strcasecmp(ext, "dat") == 0) {
            return load_bin(path, offset);
#ifndef ARM9
            } else if (strcasecmp(ext, "3dsx") == 0) {
                return load_3dsx(path);
#endif
        } else {
            debug("Invalid file: %s\n", path);
            return -1;
        }
    }
    return 0;
}
