#ifndef _loader_h_
#define _loader_h_

int load(char *path, long offset);

int load_bin(char *path, long offset);

#ifndef ARM9
int load_3dsx(char *path);
#endif

#endif // _loader_h_
