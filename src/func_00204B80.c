// CFLAGS: -O4,p -sdatathreshold 0
extern void CreateSema(int);
extern void SignalSema(int);

void func_00204B80(int *s) {
    SignalSema(s[0x10]);                         // 0x40
    s[5] = ((s[5] + 0x7FF) >> 11) << 11;         // round s[0x14] up to 2048
    CreateSema(s[0x10]);                         // 0x40
}
