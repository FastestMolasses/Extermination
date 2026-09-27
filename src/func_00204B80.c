// CFLAGS: -O4,p -sdatathreshold 0
extern void SignalSema(int);
extern void WaitSema(int);

void func_00204B80(int *s) {
    WaitSema(s[0x10]);                         // 0x40
    s[5] = ((s[5] + 0x7FF) >> 11) << 11;         // round s[0x14] up to 2048
    SignalSema(s[0x10]);                         // 0x40
}
