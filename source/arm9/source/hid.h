#pragma once

#include "common.h"

#define HID_STATE (*(volatile u32*)0x10146000)

#define KEY_A      (1 << 0)
#define KEY_B      (1 << 1)
#define KEY_SELECT (1 << 2)
#define KEY_START  (1 << 3)
#define KEY_RIGHT  (1 << 4)
#define KEY_LEFT   (1 << 5)
#define KEY_UP     (1 << 6)
#define KEY_DOWN    (1 << 7)
#define KEY_DRIGHT  KEY_RIGHT
#define KEY_DLEFT   KEY_LEFT
#define KEY_DUP     KEY_UP
#define KEY_DDOWN   KEY_DOWN
#define KEY_R1     (1 << 8)
#define KEY_L1     (1 << 9)
#define KEY_R     KEY_R1
#define KEY_L     KEY_L1
#define KEY_X      (1 << 10)
#define KEY_Y      (1 << 11)
#define KEY_ANY    0x00000FFF

#define BIT(n) (1U<<(n))

u32 hidKeysDown();
u32 hidKeysHeld();
u32 hidKeysDownTimeout(int seconds);
void hidScanInput();
