// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Charge/whiff weapon-anim dispatcher: toggles state byte 0x2F1 (1<->2),
// branches on aim-mode 0x25C (==3 hip vs other) and sub-mode 0x23F (1/2/other),
// fires anim id via func_001749A0(e, id, 0, 1.0f), and writes a float-bits
// recoil/scale word at 0x26C (0.7f=0x3F333333 for sub-mode 1, else 1.0f).
// Each leaf assigns *(int*)(e+0x26C) directly so mwcc tail-merges the store
// into the branch delay slots to the shared epilogue (target's b 0x22c shape);
// a single trailing temp store instead branches past the store and misses.
extern void func_001749A0(unsigned char *e, int id, int z, float f);

void func_001811F0(unsigned char *e) {
    if (e[0x2F1] == 1) {
        e[0x2F1] = 2;
        if (e[0x25C] == 3) {
            if (e[0x23F] == 1) {
                func_001749A0(e, 0xC5, 0, 1.0f);
                *(int *)(e + 0x26C) = 0x3F333333;
            } else if (e[0x23F] == 2) {
                func_001749A0(e, 0xC5, 0, 1.0f);
                *(int *)(e + 0x26C) = 0x3F800000;
            } else {
                func_001749A0(e, 0xC7, 0, 1.0f);
                *(int *)(e + 0x26C) = 0x3F800000;
            }
        } else {
            if (e[0x23F] == 1) {
                func_001749A0(e, 0xC4, 0, 1.0f);
                *(int *)(e + 0x26C) = 0x3F333333;
            } else if (e[0x23F] == 2) {
                func_001749A0(e, 0xC4, 0, 1.0f);
                *(int *)(e + 0x26C) = 0x3F800000;
            } else {
                func_001749A0(e, 0xC6, 0, 1.0f);
                *(int *)(e + 0x26C) = 0x3F800000;
            }
        }
    } else {
        e[0x2F1] = 1;
        if (e[0x25C] == 3) {
            if (e[0x23F] == 1) {
                func_001749A0(e, 0xBD, 0, 1.0f);
                *(int *)(e + 0x26C) = 0x3F333333;
            } else if (e[0x23F] == 2) {
                func_001749A0(e, 0xBD, 0, 1.0f);
                *(int *)(e + 0x26C) = 0x3F800000;
            } else {
                func_001749A0(e, 0xBF, 0, 1.0f);
                *(int *)(e + 0x26C) = 0x3F800000;
            }
        } else {
            if (e[0x23F] == 1) {
                func_001749A0(e, 0xBE, 0, 1.0f);
                *(int *)(e + 0x26C) = 0x3F333333;
            } else if (e[0x23F] == 2) {
                func_001749A0(e, 0xBE, 0, 1.0f);
                *(int *)(e + 0x26C) = 0x3F800000;
            } else {
                func_001749A0(e, 0xC0, 0, 1.0f);
                *(int *)(e + 0x26C) = 0x3F800000;
            }
        }
    }
}
