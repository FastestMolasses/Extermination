// CFLAGS: -O4,p -sdatathreshold 0
extern int func_001D2710(int);
extern int func_001E0C60(int);

int func_001D2910(int a0) {
    if (a0 < 0x20) {
        return func_001D2710(a0);
    }
    if (a0 < 0x40) {
        return func_001E0C60(a0);
    }
    return 0;
}
