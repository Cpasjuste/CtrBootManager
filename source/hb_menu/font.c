#include <3ds.h>
#include "font.h"

font_s fontDefault =
        {
                font2Data,
                font2Desc,
                13,
                (u8[]) {0xFF, 0xFF, 0xFF}
        };

font_s fontTitle =
        {
                font1Data,
                font1Desc,
                16,
                (u8[]) {0xFF, 0xFF, 0xFF}
        };

font_s fontSelected =
        {
                font2Data,
                font2Desc,
                14,
                (u8[]) {0x00, 0x00, 0x00}
        };
