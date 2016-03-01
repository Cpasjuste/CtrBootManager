#include "hid.h"

void hidScanInput() {}


u32 hidKeysDown() {

    u32 key = 0;
    vu32 t = 0;

    while (HID_STATE != 0x00000FFF);
    while (true) {

        key = HID_STATE;
        if(key != 0x00000FFF)
            break;
    }

    for (t = 0; t < 0x200000; t++) {}

    return ~key;
}

u32 hidKeysDownTimeout(int seconds) {

    u32 key = 0;
    vu32 t = 0;
    u64 timeout = 0; // fake timer

    while (HID_STATE != 0x00000FFF);
    while (true) {

        key = HID_STATE;
        if(key != 0x00000FFF)
            break;

        if(seconds > 0) {
            timeout++;
            if ((int)(timeout/200000) > seconds)
                break;
        }
    }
    for (t = 0; t < 0x200000; t++) {}

    return ~key;
}

u32 hidKeysHeld() {
	return ~HID_STATE;
}
