// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// State handler: state 0 runs func_001C6380(self) unless
// func_001B1020(self, self+0xD, -1, 0) returns nonzero; state 1 runs
// func_001B17A0(self) and the callback self+0x4C; state 3 and any other state
// run func_001AFC10(self).
extern int func_001B1020(unsigned char *e, int t, int a, int b);
extern void func_001B17A0(unsigned char *p);
extern void func_001AFC10(void *self);
extern void func_001C6380(unsigned char *self);

void func_001C4960(unsigned char *self) {
    switch (self[4]) {
    case 0:
        if (func_001B1020(self, self[0xD], -1, 0) == 0) {
            func_001C6380(self);
        }
        break;
    case 1:
        func_001B17A0(self);
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        break;
    case 3:
    default:
        func_001AFC10(self);
        break;
    }
}
