#include <3ds.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <libconfig.h>
#include "config.h"
#include "utility.h"

#define CONFIG_PATH "/boot.cfg"
config_t cfg;
config_setting_t *setting_root = NULL, *setting_boot = NULL, *setting_entries = NULL;

int configCreate();

void configThemeInit();

int configInit() {

    config = malloc(sizeof(boot_config_s));
    memset(config, 0, sizeof(boot_config_s));

    config->timeout = 3;
    config->autobootfix = 100;
    config->index = 0;
    config->recovery = 2;
    configThemeInit();

    config_init(&cfg);

    if (!config_read_file(&cfg, CONFIG_PATH)) {
        debug("Configuration file not found: %s\nCreating default configuration..\n", CONFIG_PATH);
        if (configCreate() != 0) {
            debug("Couldn't create configuration file..\n");
            return -1;
        }
    }

    setting_boot = config_lookup(&cfg, "boot_config");
    if (setting_boot != NULL) {
        int timeout = 3, autobootfix = 8, index = 0, recovery = 2; //SELECT

        if (config_setting_lookup_int(setting_boot, "timeout", &timeout)) {
            config->timeout = timeout;
        }
        if (config_setting_lookup_int(setting_boot, "autobootfix", &autobootfix)) {
            config->autobootfix = autobootfix;
        }
        if (config_setting_lookup_int(setting_boot, "default", &index)) {
            config->index = index;
        }
        if (config_setting_lookup_int(setting_boot, "recovery", &recovery)) {
            config->recovery = recovery;
        }
    }

    setting_entries = config_lookup(&cfg, "boot_config.entries");
    if (setting_entries != NULL) {
        int count = config_setting_length(setting_entries);
        if (count > CONFIG_MAX_ENTRIES)
            count = CONFIG_MAX_ENTRIES;

        int i;
        for (i = 0; i < count; ++i) {
            config_setting_t *entry = config_setting_get_elem(setting_entries, i);
            const char *title, *path, *offset;
            int key = -1;

            if (!(config_setting_lookup_string(entry, "title", &title)
                  && config_setting_lookup_string(entry, "path", &path)))
                continue;

            strncpy(config->entries[i].title, title, 512);
            strncpy(config->entries[i].path, path, 512);
            config->entries[i].key = -1;
            if (config_setting_lookup_int(entry, "key", &key)) {
                config->entries[i].key = key;
            }
            if (config_setting_lookup_string(entry, "offset", &offset)) {
                config->entries[i].offset = strtoul(offset, NULL, 16);
            }
            config->count++;
        }
        // prevent invalid boot index
        if (config->index >= config->count || config->index < 0) {
            config->index = 0;
        }
    }

    // "theme"
    config_setting_t *setting_theme = config_lookup(&cfg, "boot_config.theme");
    if (setting_theme != NULL) {

        const char *color;
        if (config_setting_lookup_string(setting_theme, "bgTop", &color)) {
            config->bgTop = strtoul(color, NULL, 16);
        }
        if (config_setting_lookup_string(setting_theme, "bgBottom", &color)) {
            config->bgBot = strtoul(color, NULL, 16);
        }
        if (config_setting_lookup_string(setting_theme, "highlight", &color)) {
            config->highlight = strtoul(color, NULL, 16);
        }
        if (config_setting_lookup_string(setting_theme, "borders", &color)) {
            config->borders = strtoul(color, NULL, 16);
        }
        if (config_setting_lookup_string(setting_theme, "font1", &color)) {
            config->fntDef = strtoul(color, NULL, 16);
        }
        if (config_setting_lookup_string(setting_theme, "font2", &color)) {
            config->fntSel = strtoul(color, NULL, 16);
        }
    }
    return 0;
}

void configThemeInit() {
    config->bgTop = 0x6F0149FF;
    config->bgBot = 0x6F0149FF;
    config->highlight = 0xDCDCDCFF;
    config->borders = 0xFFFFFFFF;
    config->fntDef = 0xFFFFFFFF;
    config->fntSel = 0x000000FF;
}

int configCreate() {

    setting_root = config_root_setting(&cfg);

    // create main group
    setting_boot = config_setting_add(setting_root, "boot_config", CONFIG_TYPE_GROUP);

    // create timeout setting
    config_setting_t *setting = config_setting_add(setting_boot, "timeout", CONFIG_TYPE_INT);
    config_setting_set_int(setting, 3);

    // create autobootfix setting
    setting = config_setting_add(setting_boot, "autobootfix", CONFIG_TYPE_INT);
    config_setting_set_int(setting, 8);

    // create recovery setting
    setting = config_setting_add(setting_boot, "recovery", CONFIG_TYPE_INT);
    config_setting_set_int(setting, 2);

    // create default setting
    setting = config_setting_add(setting_boot, "default", CONFIG_TYPE_INT);
    config_setting_set_int(setting, 0);

    // create entries group setting
    setting_entries = config_setting_add(setting_boot, "entries", CONFIG_TYPE_GROUP);

    if (!config_write_file(&cfg, CONFIG_PATH)) {
        return -1;
    }
    return 0;
}

int configAddEntry(char *title, char *path, long offset) {

    if (!setting_entries) {
        debug("Couldn't add entry: entries section not found\n");
        return -1;
    }

    config_setting_t *entry, *setting;
    // add group (entry)
    entry = config_setting_add(setting_entries, NULL, CONFIG_TYPE_GROUP);
    // add title
    setting = config_setting_add(entry, "title", CONFIG_TYPE_STRING);
    config_setting_set_string(setting, title);
    // add path
    setting = config_setting_add(entry, "path", CONFIG_TYPE_STRING);
    config_setting_set_string(setting, path);
    // add key
    setting = config_setting_add(entry, "key", CONFIG_TYPE_INT);
    config_setting_set_int(setting, -1);
    // add offset
    if (offset > 0) {
    }

    // write/update config file
    configWrite();

    strncpy(config->entries[config->count].title, title, 512);
    strncpy(config->entries[config->count].path, path, 512);
    config->entries[config->count].key = -1;
    config->count++;

    return 0;
}

int configRemoveEntry(int index) {

    // remove element
    if (!config_setting_remove_elem(setting_entries, index)) {
        return -1;
    }

    // update default boot index
    if (config->index >= index) {
        config->index--;
        config_setting_t *s = config_setting_lookup(setting_boot, "default");
        if (s) {
            config_setting_set_int(s, config->index);
        }
    }

    // write/update config file
    configWrite();

    // reload config
    configExit();
    configInit();

    return 0;
}

void configUpdateSettings() {

    if (setting_boot) {

        config_setting_t *s = config_setting_lookup(setting_boot, "timeout");
        if (s) {
            config_setting_set_int(s, config->timeout);
        } else {
            debug("Error: timeout setting not found\n");
        }

        s = config_setting_lookup(setting_boot, "autobootfix");
        if (s) {
            config_setting_set_int(s, config->autobootfix);
        }
        else {
            debug("Error: autobootfix setting not found\n");
        }

        s = config_setting_lookup(setting_boot, "default");
        if (s) {
            config_setting_set_int(s, config->index);
        }
        else {
            debug("Error: default setting not found\n");
        }

        s = config_setting_lookup(setting_boot, "recovery");
        if (s) {
            config_setting_set_int(s, config->recovery);
        }
        else {
            debug("Error: recovery setting not found\n");
        }

        configWrite();
    }
}

void configWrite() {
    if (!config_write_file(&cfg, "/boot.cfg")) {
        debug("Error while writing config file:\n.%s\n", "/boot.cfg");
    }
}

void configExit() {
    if (config) {
        config_destroy(&cfg);
        free(config);
    }
}
