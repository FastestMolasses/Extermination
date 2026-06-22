// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Object/actor setup: writes a 0x3C timer/count byte at +0x31F of the object,
// initializes a sub-system via func_001EFE00(0x80000040, obj), sets the global
// flag D_008106F0 = 1, then runs two further setup calls:
//   func_001FBD50(obj, 0x14D, 0, 300.0f)   (0x43960000 == 300.0f)
//   func_001B61C0(1, 0xEE, 0x3C, 1)
extern void func_001EFE00(unsigned int arg0, char *arg1);
extern void func_001FBD50(char *arg0, int arg1, int arg2, float arg3);
extern void func_001B61C0(int arg0, int arg1, int arg2, int arg3);
extern char D_008106F0;

void func_0021C120(char *arg0) {
    *(char *)(arg0 + 0x31F) = 0x3C;
    func_001EFE00(0x80000040, arg0);
    D_008106F0 = 1;
    func_001FBD50(arg0, 0x14D, 0, 300.0f);
    func_001B61C0(1, 0xEE, 0x3C, 1);
}
