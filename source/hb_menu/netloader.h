//
// Created by cpasjuste on 13/11/15.
//

#ifndef CTRBOOTMANAGER_NETLOADER_H
#define CTRBOOTMANAGER_NETLOADER_H

#define NETLOADER_PORT 17491

extern char *netloadedPath;
extern char *netloaded_commandline;
extern int netloaded_cmdlen;
extern bool netloader_boot;

int netloader_activate(void);

int netloader_deactivate(void);

int netloader_init(void);

int netloader_loop(void);

int netloader_exit(void);

int netloader_draw_error(void);


#endif //CTRBOOTMANAGER_NETLOADER_H
