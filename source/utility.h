#ifndef _utility_h_
#define _utility_h_

void openSDArchive();
void closeSDArchive();
bool fileExists(char* path);
void sleep(u32 millis);
void debug(const char *fmt, ...);
void reboot();
void shutdown();

#endif // _utility_h_
