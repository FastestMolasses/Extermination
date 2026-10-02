// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// State handler: state 0, unless func_001B0FD0(self) returns nonzero,
// binds the model self+0x44 (func_001CA5E0(self, model, 1)) and runs
// func_001C6380(self); state 1 runs func_001B17A0(self) and the callback
// self+0x4C; states 2 and 3 run func_001AFC10(self).
extern int func_001B0FD0(unsigned char *self);
extern void func_001B17A0(unsigned char *p);
extern void func_001AFC10(void *self);
extern void func_001C6380(unsigned char *self);
extern void func_001CA5E0(unsigned char *self, int model, int mode);

void func_001C4AF0(unsigned char *self) {
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) == 0) {
            func_001CA5E0(self, *(int *)(self + 0x44), 1);
            func_001C6380(self);
        }
        break;
    case 1:
        func_001B17A0(self);
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
