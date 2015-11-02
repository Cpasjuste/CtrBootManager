#ifndef _config_h_
#define _config_h_

#ifdef __cplusplus
extern "C" {
#endif

#define BIT(n) (1U<<(n))

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
    boot_entry_s entries[16];
} boot_config_s;

boot_config_s *boot_config;

int configInit();
void configSave();
void configExit();

#ifdef __cplusplus
}
#endif
#endif // _config_h_
