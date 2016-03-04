//
// Created by cpasjuste on 02/03/16.
//

#ifndef CTRBOOTMANAGER_MEMORY_H
#define CTRBOOTMANAGER_MEMORY_H

#define PTR_TOP_SCREEN          (u8*)(*(u32*)0x23FFFE00)
#define PTR_BOT_SCREEN          (u8*)(*(u32*)0x23FFFE08)
#define PTR_TOP_SCREEN_BUF      (u8*)(0x2207E700)
#define PTR_BOT_SCREEN_BUF      (u8*)(0x220C4A00)

#define PTR_TOP_BG_BUF          (u8*)(0x22000000)
#define PTR_BOT_BG_BUF          (u8*)(0x22046300)

#define PTR_PAYLOAD_MAIN        0x23F00000
#define PTR_PAYLOAD_MAIN_DATA   0x20000000
#define PTR_PAYLOAD_SIZE_MAX    0x00100000
#define PTR_PAYLOAD_STAGE2      0x21000000

#endif //CTRBOOTMANAGER_MEMORY_H
