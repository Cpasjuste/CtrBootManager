#include "common.h"
#include "i2c.h"
#include "screen_init.h"

#define PAYLOAD_ADDRESS		0x23F00000
#define PAYLOAD_DATA		0x22000000
#define PAYLOAD_STAGE2		0x08006000
#define PAYLOAD_SIZE		0x00100000

#define A11_PAYLOAD_LOC 	0x1FFF4C80
#define SCREEN_SIZE 		400 * 240 * 3 / 4

extern u8 screen_init_bin[];
extern u32 screen_init_bin_size;

void ownArm11()
{
	memcpy((void*)A11_PAYLOAD_LOC, screen_init_bin, screen_init_bin_size);
	*((u32*)0x1FFAED80) = 0xE51FF004;
	*((u32*)0x1FFAED84) = A11_PAYLOAD_LOC;
	for(int i = 0; i < 0x80000; i++)
	{
		*((u8*)0x1FFFFFF0) = 2;
	}
	for(volatile unsigned int i = 0; i < 0xF; ++i);
	while(*(volatile uint32_t *)0x1FFFFFF8 != 0);
}

//fixes the snow issue
void clearScreen()
{
	for(int i = 0; i < (SCREEN_SIZE); i++)
	{
		*((unsigned int*)0x18300000 + i) = 0;
		*((unsigned int*)0x18346500 + i) = 0;
	}
}

int main() 
{
	//gateway
	*(volatile uint32_t*)0x80FFFC0 = 0x18300000;    // framebuffer 1 top left
	*(volatile uint32_t*)0x80FFFC4 = 0x18300000;    // framebuffer 2 top left
	*(volatile uint32_t*)0x80FFFC8 = 0x18300000;    // framebuffer 1 top right
	*(volatile uint32_t*)0x80FFFCC = 0x18300000;    // framebuffer 2 top right
	*(volatile uint32_t*)0x80FFFD0 = 0x18346500;    // framebuffer 1 bottom
	*(volatile uint32_t*)0x80FFFD4 = 0x18346500;    // framebuffer 2 bottom
	*(volatile uint32_t*)0x80FFFD8 = 1;		// framebuffer select top
	*(volatile uint32_t*)0x80FFFDC = 1;		// framebuffer select bottom

	//cakehax
	*(u32*)0x23FFFE00 = 0x18300000;
	*(u32*)0x23FFFE04 = 0x18300000;
	*(u32*)0x23FFFE08 = 0x18346500;

	ownArm11();
	screenInit();
	clearScreen();

	memcpy((void*)PAYLOAD_ADDRESS, (void*)PAYLOAD_DATA, PAYLOAD_SIZE);
	((void (*)())PAYLOAD_ADDRESS)();

	return 0;
}

