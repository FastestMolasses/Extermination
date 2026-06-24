// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Two-channel level-trigger driver. Builds a 16-bit area key from the two
// adjacent level bytes (D_00810700<<8 | D_00810701) and resets the global event
// counter D_00275C3C. For the two "scripted" areas (0x1600 / 0x800) it
// unconditionally fires both channels at the fixed (1.0, 10.0) envelope and bumps
// the counter once. Otherwise it fires channel 2 only when arg0 is past the
// threshold at D_00275670+0xB8 and channel 3 only when arg1 is past +0xBC,
// bumping the counter for each that fires.
//
// Matched 100.0 with mwcc 2.3.3; the pinned 991202 build reaches 95.0%. Keys:
// (1) the 0x1600/0x800 area test is written as a `switch` with the two values as
// fall-through case labels (NOT `if (key==A || key==B)`), which makes mwcc emit
// the `beq 0x1600` + `beql 0x800` dispatch with the block's `lui 0x3f80` (1.0f)
// speculated into the branch-likely delay slot -- the if/|| form lowers to a plain
// `bne` instead. (2) case labels are written 0x800 then 0x1600 because mwcc
// reverses them into the target's 0x1600-first compare order. (3) D_00810700/01
// are over-declared as [64] so they exceed the sdata threshold and stay %hi/%lo
// absolute, while D_00275670/D_00275C3C remain gp-relative. The only objdiff
// deltas are it naming the $f13 odd-half register `fa0f` vs `fa1`; .text bytes are
// byte-identical (verified with cmp).
extern void func_0021B9A0(int idx, float a, float b);
extern char *D_00275670;
extern int D_00275C3C;
extern unsigned char D_00810700[64];
extern unsigned char D_00810701[64];

void func_001F0190(float arg0, float arg1) {
    int key;

    key = (D_00810700[0] << 8) + D_00810701[0];
    D_00275C3C = 0;
    switch (key) {
    case 0x800:
    case 0x1600:
        func_0021B9A0(2, 1.0f, 10.0f);
        func_0021B9A0(3, 1.0f, 10.0f);
        D_00275C3C = D_00275C3C + 1;
        break;
    default:
        if (!(*(float *)(D_00275670 + 0xB8) <= arg0)) {
            func_0021B9A0(2, 0.0f, arg0);
            D_00275C3C = D_00275C3C + 1;
        }
        if (!(*(float *)(D_00275670 + 0xBC) <= arg1)) {
            func_0021B9A0(3, 0.0f, arg1);
            D_00275C3C = D_00275C3C + 1;
        }
        break;
    }
}
