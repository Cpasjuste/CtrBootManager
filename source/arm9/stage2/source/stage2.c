#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "common.h"

#define PAYLOAD_ADDRESS		0x23F00000
#define PAYLOAD_DATA		0x24F00000
#define PAYLOAD_STAGE2		0x24000000
#define PAYLOAD_SIZE		0x00100000

int main() {
	
	memcpy((void*)PAYLOAD_ADDRESS, (void*)PAYLOAD_DATA, PAYLOAD_SIZE);
	((void (*)())PAYLOAD_ADDRESS)();
	
	return 0;
}

