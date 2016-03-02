#include "common.h"

#define PAYLOAD_ADDRESS		0x23F00000
#define PAYLOAD_DATA		0x20000000
#define PAYLOAD_STAGE2		0x08006000
#define PAYLOAD_SIZE		0x00100000

int main() 
{
	memcpy((void*)PAYLOAD_ADDRESS, (void*)PAYLOAD_DATA, PAYLOAD_SIZE);

	//cakehax
	*(u32*)0x23FFFE00 = 0x18300000;
	*(u32*)0x23FFFE04 = 0x18300000;
	*(u32*)0x23FFFE08 = 0x18346500;

	((void (*)())PAYLOAD_ADDRESS)();

	return 0;
}
