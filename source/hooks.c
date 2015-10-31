#include <3ds.h>

#include "hooks.h"
#include "utility.h"
#include "CakeBrah/source/libkhax/khax.h"

int hookSidRet = -1;

// from pasta-CFW
Result hookSigArm9() {
    __asm__ volatile("cpsid aif");

    u8 patchO0[] = {0x00, 0x20, 0x3B, 0xE0};
    u8 patchO1[] = {0x00, 0x20, 0x08, 0xE0};
    u8 patchN0[] = {0x01, 0x27, 0x1E, 0xF5};
    u8 patchN1[] = {0xB4, 0xF9, 0xD0, 0xAB};
    u8 patchN2[] = {0x6D, 0x20, 0xCE, 0x77};
    u8 patchN3[] = {0x5A, 0xC5, 0x73, 0xC1};
    u8 patchN4[] = {0xE7, 0x08, 0x3F, 0x22};
    u8 patchN5[] = {0x6F, 0x94, 0x72, 0x09};

    u8 isN3DS = 0;
    APT_CheckNew3DS(NULL, &isN3DS);
    unsigned int kversion = *(unsigned int *) 0x1FF80000;
    if (!isN3DS || kversion < 0x022C0600) {
        //-----------> OLD 3DS
        switch (kversion) {
            default:         // Unsupported
                //systemVersion = "unsupported";
                //debug("Unsupported kernel...");
                return;
            case 0x02220000: // 4.x
                memcpy((u32 *) 0x080549C4, patchO0, 4);
                memcpy((u32 *) 0x0804239C, patchO1, 4);
                //"Old 3DS V. 4.1 - 4.5";
                break;
            case 0x02230600: // 5.0
                memcpy((u32 *) 0x08051650, patchO0, 4);
                memcpy((u32 *) 0x0803C838, patchO1, 4);
                //systemVersion = "Old 3DS V. 5.0";
                break;
            case 0x02240000: // 5.1
                memcpy((u32 *) 0x0805164C, patchO0, 4);
                memcpy((u32 *) 0x0803C838, patchO1, 4);
                //systemVersion = "Old 3DS V. 5.1";
                break;
            case 0x02250000: // 6.0
                memcpy((u32 *) 0x0805235C, patchO0, 4);
                memcpy((u32 *) 0x08057FE4, patchO1, 4);
                //systemVersion = "Old 3DS V. 6.0";
                break;
            case 0x02260000: // 6.1
                memcpy((u32 *) 0x08051B5C, patchO0, 4);
                memcpy((u32 *) 0x08057FE4, patchO1, 4);
                //systemVersion = "Old 3DS V. 6.1 - 6.3";
                break;
            case 0x02270400: // 7.0-7.1
                memcpy((u32 *) 0x080521C4, patchO0, 4);
                memcpy((u32 *) 0x08057E98, patchO1, 4);
                //systemVersion = "Old 3DS V. 7.0 - 7.1";
                break;
            case 0x02280000: // 7.2
                memcpy((u32 *) 0x080521C8, patchO0, 4);
                memcpy((u32 *) 0x08057E9C, patchO1, 4);
                //systemVersion = "Old 3DS V. 7.2";
                break;
            case 0x022C0600: // 8.x
                memcpy((u32 *) 0x080523C4, patchO0, 4);
                memcpy((u32 *) 0x08058098, patchO1, 4);
                //systemVersion = "Old 3DS V. 8.0 - 8.1";
                break;
            case 0x022E0000: // 9.x
                memcpy((u32 *) 0x0805235C, patchO0, 4);
                memcpy((u32 *) 0x08058100, patchO1, 4);
                //debug("sigcheck hook applied for o3DS V. 9.0 - 9.2");
                //systemVersion = "Old 3DS V. 9.0 - 9.2";
                break;
        }
    }
    else {
        //-----------> NEW 3DS
        switch (kversion) {
            case 0x022C0600: // 8.x
                memcpy((u32 *) 0x08053114, patchN0, 4);
                memcpy((u32 *) 0x080587E0, patchN1, 4);
                //systemVersion = "New 3DS V. 8.1";
                break;
            case 0x022E0000: // 9.x
                memcpy((u32 *) 0x08052FD8, patchN2, 4);
                memcpy((u32 *) 0x08058804, patchN3, 4);
                //systemVersion = "New 3DS V. 9.0 - 9.2";
                break;
            default:         // Unsupported
                //debug("Unsupported kernel...");
                return;
        }
    }
    hookSidRet = 0;
    return 0;
}

void hookSig() {
    Result res = svcBackdoor(hookSigArm9);
    debug("sigPatch: %i", hookSidRet);
}
