#include <3ds.h>
#include "font.h"

font_s fontDefault =
        {
                defaultData,
                defaultDesc,
                16,
                (u8[]) {0xFF, 0xFF, 0xFF}
        };

font_s fontSelected =
        {
                defaultData,
                defaultDesc,
                16,
                (u8[]) {0x00, 0x00, 0x00}
        };
