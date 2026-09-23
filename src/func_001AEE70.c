// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Byte-matched (objdiff 100%) from the former NEARMISS body. The display-list append goes through a local write-pointer w = (char **)(q + 0x14).

//
// SEMANTICS: per-frame update of the screen-fade / overlay-quad state block
// D_0028A8E0, driven by the mode byte D_0028A9A3 through a 7-entry jump table
// (jtbl_0026DD50).  Fields used inside the block:
//   +0xC0 short  fade sub-state      +0xC3 signed char  effect/flag counter
//   +0xC2 signed char  colour select +0xC4 short        current alpha (0..0xFF)
//   +0xC6 short  alpha step per frame
// Mode 0: if the sub-state is nonzero, dispatch it: 2 -> func_001AEDB0(0),
//         3 -> func_001AEDE0(4, 0), anything else -> clear sub-state and alpha.
// Mode 1: sub-state 0 -> func_001AED80(0); 1 -> func_001AEE10(4, 0);
//         otherwise force sub-state 2 and alpha 0xFF.
// Mode 2: fade OUT - sub-state 2 just sets the flag byte to 3, otherwise the
//         sub-state becomes 1 and the alpha drops by the step, clamping at 0
//         (which also clears the flag byte and the sub-state).
// Mode 3: fade IN  - sub-state 0 just sets the flag byte to 2, otherwise the
//         sub-state becomes 3 and the alpha rises by the step, clamping at 0xFF
//         (flag byte 1, sub-state 2).
// Modes 4/5: bump the flag byte.   Mode 6: set the flag byte to 2.
// Then, for the per-slot record at D_0028A8E0 + D_00810E80 * 0x60 (0x60-byte
// stride, slot index in the global short D_00810E80): write the alpha into the
// three RGB words at +0x30/+0x34/+0x38 and pack a 64-bit GS-style value
// (0xA1 or 0x68 in the low word, 0x80 in the high word) into +0x20.
// Finally, when the mode is nonzero, append a 0x10-byte display-list entry
// through the write pointer at D_00275670+0x14 (tag 6, byte 0x30 at +3, the
// slot-record address at +4) and bump that pointer.
// NOTE: the `(short)` casts in mode 0 are semantic no-ops (the value already
// comes from a short load); they only reproduce the original's dsll32/dsra32
// re-narrowing of the compared value.

extern char *D_00275670;
extern char D_0028A8E0[];
extern signed char D_0028A9A3[8];
extern short D_00810E80[8];

extern void func_001AED80(int);
extern void func_001AEDB0(int);
extern void func_001AEDE0(int, int);
extern void func_001AEE10(int, int);

void func_001AEE70(void) {
    char *p;
    int st;
    int v;
    char *dst;
    char *q;
    char **w;

    p = D_0028A8E0;
    st = D_0028A9A3[0];
    switch (st) {
    case 0:
        v = *(short *)(p + 0xC0);
        if (v != 0) {
            if ((short)v == 2) {
                func_001AEDB0(0);
            } else if ((short)v == 3) {
                func_001AEDE0(4, 0);
            } else {
                *(short *)(p + 0xC0) = 0;
                *(short *)(p + 0xC4) = 0;
            }
        }
        break;
    case 1:
        if (*(short *)(p + 0xC0) == 0) {
            func_001AED80(0);
        } else if (*(short *)(p + 0xC0) == 1) {
            func_001AEE10(4, 0);
        } else {
            *(short *)(p + 0xC0) = 2;
            *(short *)(p + 0xC4) = 0xFF;
        }
        break;
    case 2:
        if (*(short *)(p + 0xC0) == 2) {
            *(signed char *)(p + 0xC3) = 3;
        } else {
            *(short *)(p + 0xC0) = 1;
            *(short *)(p + 0xC4) -= *(short *)(p + 0xC6);
            if (*(short *)(p + 0xC4) < 0) {
                *(short *)(p + 0xC4) = 0;
                *(signed char *)(p + 0xC3) = 0;
                *(short *)(p + 0xC0) = 0;
            }
        }
        break;
    case 3:
        if (*(short *)(p + 0xC0) == 0) {
            *(signed char *)(p + 0xC3) = 2;
        } else {
            *(short *)(p + 0xC0) = 3;
            *(short *)(p + 0xC4) = *(short *)(p + 0xC4) + *(short *)(p + 0xC6);
            if (*(short *)(p + 0xC4) >= 0xFF) {
                *(short *)(p + 0xC4) = 0xFF;
                *(signed char *)(p + 0xC3) = 1;
                *(short *)(p + 0xC0) = 2;
            }
        }
        break;
    case 4:
    case 5:
        *(signed char *)(p + 0xC3) = *(signed char *)(p + 0xC3) + 1;
        break;
    case 6:
        *(signed char *)(p + 0xC3) = 2;
        break;
    }
    *(int *)((char *)(D_00810E80[0] * 0x60) + (int)p + 0x30) = *(short *)(p + 0xC4);
    *(int *)((char *)(D_00810E80[0] * 0x60) + (int)p + 0x34) = *(short *)(p + 0xC4);
    *(int *)((char *)(D_00810E80[0] * 0x60) + (int)p + 0x38) = *(short *)(p + 0xC4);
    if (*(signed char *)(p + 0xC2) == 0) {
        *(long long *)((char *)(D_00810E80[0] * 0x60) + (int)p + 0x20) = 0xA1 | ((long long)(int)0x80 << 32);
    } else {
        *(long long *)((char *)(D_00810E80[0] * 0x60) + (int)p + 0x20) = 0x68 | ((long long)(int)0x80 << 32);
    }
    if (st != 0) {
        dst = p + D_00810E80[0] * 0x60;
        q = D_00275670;
        w = (char **)(q + 0x14);
        *(char *)(*w + 3) = 0x30;
        *(char **)(*w + 4) = dst;
        *(short *)(*w + 0) = 6;
        *w = *w + 0x10;
    }
}
