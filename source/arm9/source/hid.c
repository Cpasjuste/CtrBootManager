#include "hid.h"

void hidScanInput() {}
u32 key_old = 0x00000000;

u32 hidKeysDown() {

    u32 key = HID_STATE;

    if(key_old != key) {
        key_old = key;
        return ~key;
    }

    return 0x00000000;
}

u32 hidKeysHeld() {
	return ~HID_STATE;
}
