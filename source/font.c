#ifdef ARM9

#include "arm9/source/common.h"

#else
#include <3ds.h>
#endif

#include "font.h"

font_s fontDefault =
        {
                defaultData,
                defaultDesc,
                16,
                (u8[]) {0xFF, 0xFF, 0xFF}
        };