// CFLAGS: -O4,p -sdatathreshold 0
extern void CreateSema(int);
extern void SignalSema(int);

int func_00204B30(int *s) {
    int result;
    SignalSema(s[0x10]);                 // 0x40
    result = s[5] + (s[4] << 11);        // s[0x14] + (s[0x10] << 11)
    CreateSema(s[0x10]);                 // 0x40
    return result;
}
