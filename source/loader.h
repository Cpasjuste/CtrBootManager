#ifndef _loader_h_
#define _loader_h_

int load(const char *path, long offset);

int load_3dsx(const char *path);

int load_bin(const char *path, long offset);

#endif // _loader_h_
