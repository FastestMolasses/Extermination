// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001F0060(int, int);

void func_0021D4E0(unsigned char *a0) {
    if (a0[0x234] == 0) {
        func_001F0060(0x80000061, 0);
    } else {
        func_001F0060(0x80000062, 0);
    }
}
