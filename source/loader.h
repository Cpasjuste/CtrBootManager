#ifndef _loader_h_
#define _loader_h_

int load(char *path, long offset);

int load_3dsx(char *path);

int load_bin(char *path, long offset);

extern void __appExit();

#endif // _loader_h_
