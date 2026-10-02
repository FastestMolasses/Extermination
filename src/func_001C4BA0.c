// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// State handler of an attachment that follows a leader (self+0x18).
// State 0 as func_001C4AF0. State 1: with self+3 == 4 it copies the leader's
// parent's matrix (leader+0x18 -> +0x110 -> +0x90, four rows by copy_qw4) into
// its own (self+0x110 -> +0x90) and its position self+0xB0 into the matrix's
// +0xC0 (func_001031E0); otherwise it copies the leader's +0x110 -> +0x7C float
// and runs func_001C6380(self). Then func_001B17A0(self) and the callback
// self+0x4C. States 2 and 3: func_001AFC10(self).
extern int func_001B0FD0(unsigned char *self);
extern void func_001B17A0(unsigned char *p);
extern void func_001AFC10(void *self);
extern void func_001C6380(unsigned char *self);
extern void func_001CA5E0(unsigned char *self, int model, int mode);
extern void copy_qw4(void *dst, void *src);
extern void func_001031E0(void *dst, void *src);

void func_001C4BA0(unsigned char *self) {
    unsigned char *leader = *(unsigned char **)(self + 0x18);
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) == 0) {
            func_001CA5E0(self, *(int *)(self + 0x44), 1);
            func_001C6380(self);
        }
        break;
    case 1:
        if (self[3] == 4) {
            copy_qw4(*(char **)(self + 0x110) + 0x90,
                     *(char **)(*(char **)(leader + 0x18) + 0x110) + 0x90);
            func_001031E0(*(char **)(self + 0x110) + 0xC0, self + 0xB0);
        } else {
            *(float *)(*(char **)(self + 0x110) + 0x7C) = *(float *)(*(char **)(leader + 0x110) + 0x7C);
            func_001C6380(self);
        }
        func_001B17A0(self);
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
