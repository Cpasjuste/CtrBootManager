//
// Created by cpasjuste on 02/03/16.
//

#ifndef CTRBOOTMANAGER_MEMORY_H
#define CTRBOOTMANAGER_MEMORY_H

#define PTR_TOP_SCREEN         (u8*)(0x18300000)
#define PTR_BOT_SCREEN             (u8*)(0x18346500)

#define PTR_PAYLOAD_MAIN        0x23F00000
#define PTR_PAYLOAD_MAIN_DATA   0x20000000
#define PTR_PAYLOAD_SIZE_MAX    0x00100000
#define PTR_PAYLOAD_STAGE2      0x08006000

#define PTR_BG_TOP              (u8*)(0x23FFFE00)
#define PTR_BG_BOT              (u8*)(0x24046300)

#endif //CTRBOOTMANAGER_MEMORY_H
