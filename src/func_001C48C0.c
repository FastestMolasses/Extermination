// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// State handler: state 0 runs func_001C6380(self) unless
// func_001B0FD0(self) returns nonzero; state 1 runs func_001B1B70(self) and the
// callback self+0x4C; states 2 and 3 run func_001AFC10(self).
extern int func_001B0FD0(unsigned char *self);
extern void func_001B1B70(void *self);
extern void func_001AFC10(void *self);
extern void func_001C6380(unsigned char *self);

void func_001C48C0(unsigned char *self) {
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) == 0) {
            func_001C6380(self);
        }
        break;
    case 1:
        func_001B1B70(self);
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
