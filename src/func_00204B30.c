// CFLAGS: -O4,p -sdatathreshold 0
extern void SignalSema(int);
extern void WaitSema(int);

int func_00204B30(int *s) {
    int result;
    WaitSema(s[0x10]);                 // 0x40
    result = s[5] + (s[4] << 11);        // s[0x14] + (s[0x10] << 11)
    SignalSema(s[0x10]);                 // 0x40
    return result;
}
