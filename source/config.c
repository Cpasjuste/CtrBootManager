#include <3ds.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include <libconfig.h>
#include "config.h"

#define BIT(n) (1U<<(n))

int configInit() {

    config_t cfg;
    config_setting_t *setting;

    boot_config = malloc(sizeof(boot_config_s));
    memset(boot_config, 0, sizeof(boot_config_s));
    boot_config->timeout = 3;
    boot_config->autobootfix = 100;
    boot_config->index = 0;

    config_init(&cfg);

    if (!config_read_file(&cfg, "/boot.cfg")) {
        printf("%s:%d - %s\n", config_error_file(&cfg),
               config_error_line(&cfg), config_error_text(&cfg));
        config_destroy(&cfg);
        return -1;
    }

    setting = config_lookup(&cfg, "boot_config");
    if (setting != NULL) {
        int timeout = 3, index = 0, autobootfix = 100;
        u32 recovery = BIT(2); //SELECT

        if (config_setting_lookup_int(setting, "timeout", &timeout)) {
            boot_config->timeout = timeout;
        }
        if (config_setting_lookup_int(setting, "autobootfix", &autobootfix)) {
            boot_config->autobootfix = autobootfix;
        }
        if (config_setting_lookup_int(setting, "default", &index)) {
            boot_config->index = index;
        }
        if (config_setting_lookup_int(setting, "recovery", &recovery)) {
            boot_config->recovery = BIT(recovery);
        }
    }

    setting = config_lookup(&cfg, "boot_config.entries");
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

    config_destroy(&cfg);
    return 0;
}

void configExit() {
    if (boot_config) {
        free(boot_config);
    }
}
