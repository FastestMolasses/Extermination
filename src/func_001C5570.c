// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Allocates and initialises a 0xC-class object (an effect/particle emitter
// instance). Clears the 0x2E counter, the p+3 / p+0x9A byte flags and the
// 0x54/0x56 shorts, stores the caller's type byte at 0xD, sets the 0xE handle
// to 0xFFFF, copies three 0x10-byte vectors (arg1 -> +0xA0, arg0+0xB0 -> +0xB0,
// arg0+0xC0 -> +0xC0) and installs the per-mode update handler at +0x10:
// mode 0 -> func_001C5760, mode 1 -> func_001C5680, mode 2 -> func_001C5760
// with the +0xA "alt" byte set. Returns the new object, or NULL if the
// allocator failed.
//
// NOTE: the chained assignment through the u16 field at 0x2E is load-bearing.
// mwcc evaluates `a = b = *(unsigned short *)(p + 0x2E) = 0` by storing 0 to
// the short, then re-materialising the assignment's u16 result with
// `andi v0, zero, 0xffff` and reusing that GPR for the two byte stores --
// exactly what the target does. Writing the three stores separately emits
// `sb zero` and loses the andi.

extern char *func_001AFA90(int);
extern void func_00102948(char *, char *);
extern void func_001C5680(void);
extern void func_001C5760(void);

char *func_001C5570(char *arg0, int arg1, char arg2, int arg3) {
    char *p;

    p = func_001AFA90(0xC);
    if (p != 0) {
        *(char *)(p + 0x9A) = *(char *)(p + 3) = *(unsigned short *)(p + 0x2E) = 0;
        *(char *)(p + 0xD) = arg2;
        *(unsigned short *)(p + 0xE) = 0xFFFF;
        *(short *)(p + 0x56) = 0;
        *(short *)(p + 0x54) = 0;
        func_00102948(p + 0xA0, (char *)arg1);
        func_00102948(p + 0xB0, arg0 + 0xB0);
        func_00102948(p + 0xC0, arg0 + 0xC0);
        *(char *)(p + 0xA) = 0;
        switch (arg3) {
        case 0:
            *(void (**)(void))(p + 0x10) = func_001C5760;
            break;
        case 1:
            *(void (**)(void))(p + 0x10) = func_001C5680;
            break;
        case 2:
            *(char *)(p + 0xA) = 1;
            *(void (**)(void))(p + 0x10) = func_001C5760;
            break;
        }
    }
    return p;
}
