#ifndef _config_h_
#define _config_h_

#ifdef __cplusplus
extern "C" {
#endif

#define BIT(n) (1U<<(n))
#define CONFIG_MAX_ENTRIES 11

typedef struct {
    char title[512];
    char path[512];
    long offset;
} boot_entry_s;

typedef struct {
    int timeout;
    int autobootfix;
    int index;
    int recovery;
    int count;
    boot_entry_s entries[12];
} boot_config_s;

boot_config_s *config;

int configInit();

int configAddEntry(char *title, char *path, long offset);

int configRemoveEntry(int index);

void configUpdateSettings();

void configWrite();

void configExit();

#ifdef __cplusplus
}
#endif
#endif // _config_h_
