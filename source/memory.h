//
// Created by cpasjuste on 02/03/16.
//

#ifndef CTRBOOTMANAGER_MEMORY_H
#define CTRBOOTMANAGER_MEMORY_H

#define PTR_TOP_SCREEN          (u8*)(*(u32*)0x23FFFE00)
#define PTR_BOT_SCREEN          (u8*)(*(u32*)0x23FFFE08)
#define PTR_TOP_SCREEN_BUF      (u8*)(0x2507E700)
#define PTR_BOT_SCREEN_BUF      (u8*)(0x250C4A00)
#define PTR_TOP_BG              (u8*)(0x25000000)
#define PTR_BOT_BG              (u8*)(0x25046300)
#define PTR_CFG                 (u8*)(0x25200000)
#define PTR_CFG_TMP             (u8*)(0x25230000)
#define PTR_PICKER              (u8*)(0x25210000)
#define PTR_PICKER_FILE         (u8*)(0x25220000)

#define PTR_PAYLOAD_MAIN        0x23F00000
#define PTR_PAYLOAD_MAIN_DATA   0x26000000
#define PTR_PAYLOAD_SIZE_MAX    0x000FFD00
#define PTR_PAYLOAD_STAGE2      0x08006000

#endif //CTRBOOTMANAGER_MEMORY_H
