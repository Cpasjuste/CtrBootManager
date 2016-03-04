#include <string.h>

#define PTR_PAYLOAD_MAIN        0x23F00000
#define PTR_PAYLOAD_MAIN_DATA   0x26000000
#define PTR_PAYLOAD_SIZE_MAX    0x000FFD00
#define PTR_PAYLOAD_STAGE2      0x08006000

int main() 
{
	memcpy((void*)PTR_PAYLOAD_MAIN, (void*)PTR_PAYLOAD_MAIN_DATA, PTR_PAYLOAD_SIZE_MAX);
	((void (*)())PTR_PAYLOAD_MAIN)();

	return 0;
}
