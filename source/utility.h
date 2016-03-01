#ifndef _utility_h_
#define _utility_h_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef ARM9
bool aptMainLoop();
#else
#include "descriptor.h"

typedef struct menuEntry_s {
    char executablePath[128 + 1];
    char arg[64 + 1];
    descriptor_s descriptor;
} menuEntry_s;

void openSDArchive();

void closeSDArchive();
#endif

bool fileExists(char *path);

size_t fileSize(const char *path);

int fileReadOffset(const char *path, void *data, size_t size, u32 offset);

int fileRead(const char *path, void *data, size_t size);

void svcSleep(u32 millis);

const char *get_filename_ext(const char *filename);

bool end_with(const char *str, const char c);

char *get_button(int button);

void debug(const char *fmt, ...);

bool confirm(int confirmButton, const char *fmt, ...);

int load_homemenu();

void reboot();

void poweroff();

#ifdef __cplusplus
}
#endif
#endif // _utility_h_
