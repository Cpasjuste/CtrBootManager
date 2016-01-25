#ifndef _menu_h_
#define _menu_h_

#define MENU_MIN_X 16
#define MENU_MIN_Y 52
#define MENU_MAX_X 384
#define MENU_MAX_Y 232

void drawBg();

void drawTitle(const char *format, ...);

void drawItem(bool selected, int y, const char *format, ...);

void drawItemN(bool selected, int maxChar, int y, const char *format, ...);

void drawInfo(const char *format, ...);

int menu_more();

int menu_boot();

int menu_config();

int menu_netloader();

#endif