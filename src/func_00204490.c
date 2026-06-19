// CFLAGS: -O4,p -sdatathreshold 0
extern void CreateSema(int);
extern void SignalSema(int);

void func_00204490(int *s, int a1) {
    SignalSema(s[0x10]);                                  // 0x40
    s[5] = s[5] + a1;                                     // 0x14
    *(long long *)((char *)s + 0x48) = *(long long *)((char *)s + 0x48) + (long long)a1;
    CreateSema(s[0x10]);                                  // 0x40
}
