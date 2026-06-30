// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// State-byte dispatcher on self[3] (switch 0/1; default = no-op return).
// State 0: allocates an object via func_001AFF10(), wires its update callback
//   func_00207350 into p+0x10 and stores the owner (self) into p+0x20, bumps
//   self[3], then calls func_001AED80(0).
// State 1: func_00207D00(1, 3) then func_001B0000(). The constant (1,3) reuses
//   a0=1 (left over from the case-1 compare) and a1=3 (hoisted into the
//   dispatch branch delay slot).
extern char *func_001AFF10(void);
extern void func_001AED80(int);
extern void func_00207D00(int, int);
extern void func_001B0000(void);
extern void func_00207350(void);

void func_002072C0(unsigned char *a0) {
    switch (a0[3]) {
    case 0:
        {
            char *p = func_001AFF10();
            *(void (**)(void))(p + 0x10) = func_00207350;
            *(unsigned char **)(p + 0x20) = a0;
            a0[3] = (unsigned char)(a0[3] + 1);
            func_001AED80(0);
        }
        break;
    case 1:
        func_00207D00(1, 3);
        func_001B0000();
        break;
    }
}
