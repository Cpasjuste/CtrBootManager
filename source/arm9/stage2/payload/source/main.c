#include "common.h"

#define PTR_TOP_SCREEN				0x18300000
#define PTR_BOT_SCREEN				0x18346500

#define PTR_PAYLOAD_MAIN        0x23F00000
#define PTR_PAYLOAD_MAIN_DATA   0x20000000
#define PTR_PAYLOAD_SIZE_MAX    0x00100000
#define PTR_PAYLOAD_STAGE2      0x08006000

int main() 
{
	memcpy((void*)PTR_PAYLOAD_MAIN, (void*)PTR_PAYLOAD_MAIN_DATA, PTR_PAYLOAD_SIZE_MAX);

	//gateway
	*(volatile uint32_t*)0x80FFFC0 = 0x18300000;    // framebuffer 1 top left
	*(volatile uint32_t*)0x80FFFC4 = 0x18300000;    // framebuffer 2 top left
	*(volatile uint32_t*)0x80FFFC8 = 0x18300000;    // framebuffer 1 top right
	*(volatile uint32_t*)0x80FFFCC = 0x18300000;    // framebuffer 2 top right
	*(volatile uint32_t*)0x80FFFD0 = 0x18346500;    // framebuffer 1 bottom
	*(volatile uint32_t*)0x80FFFD4 = 0x18346500;    // framebuffer 2 bottom
	*(volatile uint32_t*)0x80FFFD8 = 1;    // framebuffer select top
	*(volatile uint32_t*)0x80FFFDC = 1;    // framebuffer select bottom

	//cakehax
	*(u32*)0x23FFFE00 = PTR_TOP_SCREEN;
	*(u32*)0x23FFFE04 = PTR_TOP_SCREEN;
	*(u32*)0x23FFFE08 = PTR_BOT_SCREEN;

	((void (*)())PTR_PAYLOAD_MAIN)();

	return 0;
}
