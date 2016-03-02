#ifndef _picker_h_
#define _picker_h_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    char name[512];
    char path[512];
    bool isDir;
    u64 size;
} file_s;

typedef struct {
    char now_path[512];
    file_s files[512];
    int file_count;
    int file_index;
} picker_s;

void pick_file(file_s *picked, const char *path);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _picker_h_
