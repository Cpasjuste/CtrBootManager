//
// Created by cpasjuste on 21/01/16.
//

#ifndef CTRBOOTMANAGER_GUI_H
#define CTRBOOTMANAGER_GUI_H

#include <3ds.h>

void guiInit();
void guiDrawBg();
void guiDrawText(u32 color, int x, int y, int size, const char *format, ...);
void guiDrawTextCenter(gfxScreen_t screen, u32 color, int y, int size, const char *format, ...);
void guiDrawRect(u32 color, int x, int y, int w, int h);
void guiStart(gfxScreen_t screen);
void guiEnd();
void guiSwap();
void guiExit();

#endif //CTRBOOTMANAGER_GUI_H
