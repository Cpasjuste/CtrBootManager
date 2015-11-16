#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/dirent.h>

#include "gfx.h"
#include "picker.h"
#include "utility.h"
#include "config.h"

#define MAX_LINE 11

picker_s *picker;

int alphasort(const void *p, const void *q) {
    const file_s *a = p;
    const file_s *b = q;

    if ((a->isDir && b->isDir)
        || (!a->isDir && !b->isDir))
        return strcasecmp(a->name, b->name);
    else
        return a->isDir ? -1 : 1;
}

void get_dir(const char *path) {
    DIR *fd;
    struct dirent *file;
    char new_path[512];
    strncpy(new_path, path, 512);

    if (NULL == (fd = opendir(new_path))) {
        printf("\xb[3;1HDirectory empty...");
        return;
    }

    memset(picker, 0, sizeof(picker_s));
    strncpy(picker->now_path, new_path, 512);

    while ((file = readdir(fd))) {
        if (!strcmp(file->d_name, ".") || !strcmp(file->d_name, ".."))
            continue;
        if (file->d_type != DT_DIR) {
            const char *ext = get_filename_ext(file->d_name);
            if (strcasecmp(ext, "bin") != 0
                && strcasecmp(ext, "dat") != 0
                && strcasecmp(ext, "3dsx") != 0)
                continue;
        }

        // file name
        strncpy(picker->files[picker->file_count].name, file->d_name, 512);

        //  build absolute path
        if (end_with(new_path, '/')) {
            snprintf(picker->files[picker->file_count].path,
                     512, "%s%s", new_path, file->d_name);
        } else {
            snprintf(picker->files[picker->file_count].path,
                     512, "%s/%s", new_path, file->d_name);
        }

        // dir vs file
        if (file->d_type != DT_DIR) {
            picker->files[picker->file_count].isDir = false;
            // file size
            struct stat st;
            stat(picker->files[picker->file_count].path, &st);
            picker->files[picker->file_count].size = (u64) st.st_size;
        } else {
            picker->files[picker->file_count].isDir = true;
        }
        picker->file_count++;
    }

    if (picker->file_count > 1) {
        qsort(picker->files, (size_t) picker->file_count,
              sizeof(*picker->files), alphasort);
    }

    closedir(fd);
}

void pick_file(file_s *picked, const char *path) {

    picker = malloc(sizeof(picker_s));
    get_dir(path);

    // key repeat timer
    static time_t t_start = 0, t_end = 0, t_elapsed = 0;

    while (aptMainLoop()) {

        hidScanInput();
        u32 kHeld = hidKeysHeld();
        u32 kDown = hidKeysDown();

        if (hidKeysUp()) {
            time(&t_start); // reset held timer
        }

        if (kDown & KEY_DOWN) {
            picker->file_index++;
            if (picker->file_index >= picker->file_count)
                picker->file_index = 0;
            time(&t_start);
        } else if (kHeld & KEY_DOWN) {
            time(&t_end);
            t_elapsed = t_end - t_start;
            if (t_elapsed > 0) {
                picker->file_index++;
                if (picker->file_index >= picker->file_count)
                    picker->file_index = 0;
                svcSleep(100);
            }
        }

        if (kDown & KEY_UP) {
            picker->file_index--;
            if (picker->file_index < 0)
                picker->file_index = picker->file_count - 1;
            time(&t_start);
        } else if (kHeld & KEY_UP) {
            time(&t_end);
            t_elapsed = t_end - t_start;
            if (t_elapsed > 0) {
                picker->file_index--;
                if (picker->file_index < 0)
                    picker->file_index = picker->file_count - 1;
                svcSleep(100);
            }
        }

        if (kDown & KEY_A) {
            if (picker->file_count > 0) {
                int index = picker->file_index;
                if (!picker->files[index].isDir) {
                    if (confirm(0, "Launch \"%s\" ?", picker->files[index].name)) {
                        strncpy(picked->name, picker->files[index].name, 512);
                        strncpy(picked->path, picker->files[index].path, 512);
                        picked->isDir = picker->files[index].isDir;
                        picked->size = picker->files[index].size;
                        break;
                    }
                }
                else {
                    get_dir(picker->files[index].path);
                }
            }
        } else if (kDown & KEY_X) {
            int index = picker->file_index;
            if (!picker->files[index].isDir) {
                const char *ext = get_filename_ext(picker->files[index].name);
                if (strcasecmp(ext, "3dsx") == 0) {
                    if (confirm(3, "Add entry to boot menu: \"%s\" ?", picker->files[index].name)) {
                        if (config->count > CONFIG_MAX_ENTRIES - 1) {
                            debug("Maximum entries reached (%i)\n", CONFIG_MAX_ENTRIES);
                        } else if (configAddEntry(picker->files[index].name, picker->files[index].path, 0) == 0) {
                            debug("Added entry: %s\n", picker->files[index].name);
                        } else {
                            debug("Error adding entry: %s\n", picker->files[index].name);
                        }
                    }
                }
            }
        }
        else if (kDown & KEY_B) {
            // exit if we can't go back
            if (strlen(picker->now_path) <= 1)
                break;

            // remove slash if needed
            if (end_with(picker->now_path, '/'))
                picker->now_path[strlen(picker->now_path) - 1] = '\0';

            // build path
            char *slash = strrchr(picker->now_path, '/');
            if (slash == NULL)
                break;
            int len = (int) (slash - picker->now_path);
            picker->now_path[len] = '\0';

            // enter new dir
            get_dir(picker->now_path);
        }

        gfxClear();
        gfxDrawText(GFX_TOP, GFX_LEFT, &fontDefault, "*** Select a file ***", 150, 20);

        int minX = 16;
        int maxX = 400 - 16;
        int minY = 32;
        int maxY = 240 - 16;
        drawRect(GFX_TOP, GFX_LEFT, minX, minY, maxX, maxY, (u8) 0xFF, (u8) 0xFF, (u8) 0xFF);
        minY += 20;

        int i, y = 0;
        int page = picker->file_index / MAX_LINE;
        for (i = page * MAX_LINE; i < page * MAX_LINE + MAX_LINE; i++) {
            if (i >= picker->file_count)
                break;

            if (i == picker->file_index) {
                gfxDrawRectangle(GFX_TOP, GFX_LEFT, (u8[]) {0xDC, 0xDC, 0xDC}, minX + 4, minY + 16 * y, maxX - 23, 15);
                gfxDrawTextN(GFX_TOP, GFX_LEFT, &fontSelected, picker->files[i].name, 47, minX + 6, minY + 16 * y);
                if (!picker->files[i].isDir) {
                    gfxDrawText(GFX_BOTTOM, GFX_LEFT, &fontDefault, "Informations", minX + 6, 20);
                    gfxDrawText(GFX_BOTTOM, GFX_LEFT, &fontDefault,
                                "Press (A) to launch\nPress (X) to add to boot menu", minX + 12, 40);
                }
            } else {
                gfxDrawTextN(GFX_TOP, GFX_LEFT, &fontDefault, picker->files[i].name, 47, minX + 6, minY + 16 * y);
            }
            y++;
        }
        gfxSwap();
    }
    free(picker);
}
