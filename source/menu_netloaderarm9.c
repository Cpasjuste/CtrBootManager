//
// Created by cpasjuste on 23/02/16.
//

#include <3ds.h>
#include <string.h>
#include <arpa/inet.h>
#include <CakeBrah/include/brahma.h>
#include <sys/unistd.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <malloc.h>
#include "utility.h"
#include "menu.h"
#include "gfx.h"

int menu_netloaderarm9(void) {

    s32 sockfd;
    struct sockaddr_in sa;
    s32 clientfd;
    struct sockaddr_in client_addr;
    s32 addrlen = sizeof(client_addr);
    s32 sflags = 0;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        debug("Err: socket");
        return -1;
    }

    bzero(&sa, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(BRAHMA_NETWORK_PORT);
    sa.sin_addr.s_addr = gethostid();

    if (bind(sockfd, (struct sockaddr *) &sa, sizeof(sa)) != 0) {
        debug("Err: bind");
        close(sockfd);
        return -1;
    }

    if (listen(sockfd, 1) != 0) {
        debug("Err: listen()");
        close(sockfd);
        return -1;
    }

    char msg[256];
    sprintf(msg,
            "NetLoader Active - waiting for arm9\n\n"
                    "IP: %s, Port: %d\n\nPress B to cancel\n",
            inet_ntoa(sa.sin_addr), BRAHMA_NETWORK_PORT);
    drawBg();
    gfxDrawTextf(GFX_TOP, GFX_LEFT, &fontDefault, MENU_MIN_X + 16, MENU_MIN_Y + 16, msg);
    gfxSwap();

    sflags = fcntl(sockfd, F_GETFL);
    if (sflags == -1) {
        debug("Err: fcntl() (1)\n");
        close(sockfd);
    }
    fcntl(sockfd, F_SETFL, sflags | O_NONBLOCK);

    while (aptMainLoop()) {

        hidScanInput();
        if (hidKeysDown() & KEY_B) {
            close(sockfd);
            return -1;
        }

        clientfd = accept(sockfd, (struct sockaddr *) &client_addr, &addrlen);
        svcSleepThread(100000000);
        if (clientfd > 0)
            break;
    }

    s32 recvd;
    u32 total = 0;
    u8 *arm9_buf = memalign(0x1000, ARM9_PAYLOAD_MAX_SIZE);

    while ((recvd = recv(clientfd, arm9_buf + total,
                         ARM9_PAYLOAD_MAX_SIZE - total, 0)) != 0) {
        if (recvd != -1) {
            total += recvd;
            drawBg();
            gfxDrawTextf(GFX_TOP, GFX_LEFT, &fontDefault, MENU_MIN_X + 16, MENU_MIN_Y + 16, "%i", total);
            gfxSwap();
        }
        if (total >= ARM9_PAYLOAD_MAX_SIZE) {
            debug("Err: invalid payload size\n");
            close(clientfd);
            close(sockfd);
            free(arm9_buf);
            return -1;
        }
    }

    fcntl(sockfd, F_SETFL, sflags & ~O_NONBLOCK);
    close(clientfd);
    close(sockfd);

    gfxExit();

    brahma_init();
    if (load_arm9_payload_from_mem(arm9_buf, total) != 1) {
        debug("Err: Couldn't load arm9 payload...\n");
        return -1;
    }
    firm_reboot();
    brahma_exit();
}
