#include <3ds.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include <libconfig.h>
#include "config.h"
#include "utility.h"

config_t cfg;
config_setting_t *setting_boot = NULL;

int configInit() {

    boot_config = malloc(sizeof(boot_config_s));
    memset(boot_config, 0, sizeof(boot_config_s));
    boot_config->timeout = 3;
    boot_config->autobootfix = 100;
    boot_config->index = 0;

    config_init(&cfg);

    if (!config_read_file(&cfg, "/boot.cfg")) {
        debug("%s:%d - %s\n", config_error_file(&cfg),
               config_error_line(&cfg), config_error_text(&cfg));
        boot_config = NULL;
        config_destroy(&cfg);
        return -1;
    }

    setting_boot = config_lookup(&cfg, "boot_config");
    if (setting_boot != NULL) {
        int timeout = 3, index = 0, autobootfix = 100;
        int recovery = 2; //SELECT

        if (config_setting_lookup_int(setting_boot, "timeout", &timeout)) {
            boot_config->timeout = timeout;
        }
        if (config_setting_lookup_int(setting_boot, "autobootfix", &autobootfix)) {
            boot_config->autobootfix = autobootfix;
        }
        if (config_setting_lookup_int(setting_boot, "default", &index)) {
            boot_config->index = index;
        }
        if (config_setting_lookup_int(setting_boot, "recovery", &recovery)) {
            boot_config->recovery = recovery;
        }
    }

    config_setting_t *setting = config_lookup(&cfg, "boot_config.entries");
    if (setting != NULL) {
        int count = config_setting_length(setting);
        if (count > 11) count = 11;
        int i;

        for (i = 0; i < count; ++i) {
            config_setting_t *entrie = config_setting_get_elem(setting, i);
            const char *title, *path, *offset, *payload;

            if (!(config_setting_lookup_string(entrie, "title", &title)
                  && config_setting_lookup_string(entrie, "path", &path)))
                continue;

            strncpy(boot_config->entries[i].title, title, 512);
            strncpy(boot_config->entries[i].path, path, 512);
            if (config_setting_lookup_string(entrie, "offset", &offset)) {
                boot_config->entries[i].offset = strtoul(offset, NULL, 16);
            }
            boot_config->count++;
        }
        // prevent invalid boot index
        if (boot_config->index >= boot_config->count) {
            boot_config->index = 0;
        }
    }

    return 0;
}

void configSave() {

    if (setting_boot) {

        config_setting_t *s = config_lookup(&setting_boot, "timeout");
        if (s) {
            config_setting_set_int(s, boot_config->timeout);
        } else {
            debug("Error: timeout setting not found\n");
        }

        s = config_lookup(&setting_boot, "autobootfix");
        if (s) {
            config_setting_set_int(s, boot_config->autobootfix);
        }
        else {
            debug("Error: autobootfix setting not found\n");
        }

        s = config_lookup(&setting_boot, "default");
        if (s) {
            config_setting_set_int(s, boot_config->index);
        }
        else {
            debug("Error: default setting not found\n");
        }

        s = config_lookup(&setting_boot, "recovery");
        if (s) {
            config_setting_set_int(s, boot_config->recovery);
        }
        else {
            debug("Error: recovery setting not found\n");
        }

        if (!config_write_file(&cfg, "/boot.cfg")) {
            debug("Error while writing config file:\n.%s\n", "/boot.cfg");
        }
    }
}

void configExit() {
    if (boot_config) {
        config_destroy(&cfg);
        free(boot_config);
    }
}
