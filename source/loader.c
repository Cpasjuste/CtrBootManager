#include <3ds.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "brahma.h"

#include "gfx.h"
#include "picker.h"
#include "config.h"
#include "utility.h"
#include "menu_netloader.h"

char boot_app[512];
bool boot_app_enabled;

int load_3dsx(const char *path) {
    memset(boot_app, 0, 512);
    strncpy(boot_app, path, 512);
    boot_app_enabled = true;
    return 0;
}

int load_bin(const char *path, long offset) {

    // Memory for the arm9 payload
    u32 payload_size = 0x50000;
    void *payload = malloc(payload_size);
    if (!payload) {
        debug("Err: Couldn't allocate payload...\n");
        return -1;
    }

    int rc;
    // Load the arm9 payload into memory
    FILE *file = fopen(path, "r");
    if (!file) {
        debug("Err: Couldn't open %s...\n", path);
        return -1;
    }

    if (offset > 0) {
        rc = fseek(file, offset, SEEK_SET);
        if (rc != 0) {
            debug("Err: Couldn't seek %s...\n", path);
            return -1;
        }
    }

    fread(payload, payload_size, 1, file);
    if (ferror(file) != 0) {
        debug("Err: Couldn't read %s...\n", path);
        return -1;
    }
    fclose(file);

    if (brahma_init()) {
        rc = load_arm9_payload_from_mem(payload, payload_size);
        if (rc != 1) {
            debug("Err: Couldn't load arm9 payload...\n");
            return -1;
        }

        configExit();
        netloader_exit();
        amExit();
        ptmExit();
        acExit();
        hidExit();
        //gfxExit();
        closeSDArchive();
        sdmcExit();
        fsExit();
        aptExit();
        srvExit();

        firm_reboot();
        brahma_exit();

    } else {
        debug("Err: Couldn't init brahma...\n");
        return -1;
    }
    free(payload);
    return 0;
}

int load(const char *path, long offset) {
    const char *ext = get_filename_ext(path);
    if (strcasecmp(ext, "bin") == 0
        || strcasecmp(ext, "dat") == 0) {
        return load_bin(path, offset);
    } else if (strcasecmp(ext, "3dsx") == 0) {
        return load_3dsx(path);
    } else {
        debug("Invalid file: %s\n", path);
        return -1;
    }
}
