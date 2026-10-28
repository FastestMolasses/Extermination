// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Sub-state machine on self+3 (returns a status to func_00226E90):
//   0 -> step 1, return 0
//   1 -> func_00227DE0(self): 0/1/2 are returned as-is, 3 moves to step 2 and
//        returns 0, any other result returns 1
//   2 -> draws via func_001FCBD0(0x3C, 0x9B, 7, 0x1B, 0x70808080), then
//        func_00228070(self): 0 returns 0; 1 -> step 3, func_00225CF0(self, 4, 7),
//        return 3; anything else -> step 4, func_00225CF0(self, 4, 7), return 0
//   3 -> return 0
//   4 -> return func_00227270()
//   other -> return 1
// Switch cases are listed in ascending order (2.3.3 emits the compares in
// reverse source order); the inner result-0 case is an empty `break`.
extern void func_001FCBD0(int arg0, int arg1, int arg2, int arg3, unsigned int flags);
extern void func_00225CF0(unsigned char *a0, unsigned int a1, unsigned int a2);
extern int func_00227270(void);
extern int func_00227DE0(char *arg0);
extern int func_00228070(char *arg0);

int func_00227C60(char *self) {
    switch (*(unsigned char *)(self + 3)) {
    case 0:
        *(unsigned char *)(self + 3) = 1;
        return 0;
    case 1:
        switch (func_00227DE0(self)) {
        case 0:
            return 0;
        case 1:
            return 1;
        case 2:
            return 2;
        case 3:
            *(unsigned char *)(self + 3) = 2;
            return 0;
        }
        break;
    case 2:
        func_001FCBD0(0x3C, 0x9B, 7, 0x1B, 0x70808080);
        switch (func_00228070(self)) {
        case 0:
            break;
        case 1:
            *(unsigned char *)(self + 3) = 3;
            func_00225CF0((unsigned char *)self, 4, 7);
            return 3;
        default:
            *(unsigned char *)(self + 3) = 4;
            func_00225CF0((unsigned char *)self, 4, 7);
            break;
        }
        return 0;
    case 3:
        return 0;
    case 4:
        return func_00227270();
    }
    return 1;
}
