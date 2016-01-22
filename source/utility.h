#ifndef _utility_h_
#define _utility_h_

#ifdef __cplusplus
extern "C" {
#endif

#include "descriptor.h"

typedef struct menuEntry_s {
    char executablePath[128 + 1];
    char arg[64 + 1];
    descriptor_s descriptor;
} menuEntry_s;

void openSDArchive();

void closeSDArchive();

void gfxClear();

bool fileExists(char *path);

void svcSleep(u32 millis);

const char *get_filename_ext(const char *filename);

bool end_with(const char *str, const char c);

char *get_button(int button);

void debug(const char *fmt, ...);

bool confirm(int confirmButton, const char *fmt, ...);

void reboot();

void poweroff();

#ifdef __cplusplus
}
#endif
#endif // _utility_h_
