// CFLAGS: -O4,p -sdatathreshold 0
extern void SignalSema(int);
extern void WaitSema(int);

void func_00204490(int *s, int a1) {
    WaitSema(s[0x10]);                                  // 0x40
    s[5] = s[5] + a1;                                     // 0x14
    *(long long *)((char *)s + 0x48) = *(long long *)((char *)s + 0x48) + (long long)a1;
    SignalSema(s[0x10]);                                  // 0x40
}
