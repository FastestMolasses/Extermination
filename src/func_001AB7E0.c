// NEARMISS func_001AB7E0  (vram 0x001AB7E0, 0x1F0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.92% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// jr-table external-dispatch wall (proven s84) — the only residual is the 2-instruction lui/addiu jump-table reloc pair (local @26 vs the original's external jtbl_0026DC50). Everything else is instruction-identical. Required mwcc233 (991202 walls at 92.94) and -sdatathreshold 4 for the gp_rel D_002...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// SEMANTICS: global boot/sequence state machine, driven by the shared
// controller record at *(void **)0x70003B6C.  Byte +0x08 = major step,
// byte +0x09 = sub-step, byte +0x0A = a per-step scratch flag.  `sub`
// aliases &rec[8] (the original keeps rec+8 live in $a3).
//   step 0  sub 0: func_001B0BA0(), sub++
//           sub 1: r = func_001FB370(D_0028A4A4); on success publish r to
//                  D_0028A734/D_0028A738, func_00225A00(), step++, sub=0
//           (any other sub: nothing)
//   step 1  step++ (via `sub`), sub=0, +0x0A=0, then FALLS THROUGH to step 2
//   step 2  func_001AB9D0(); done -> step++, sub=0
//   step 3  func_001ABC60(); done -> func_001FF1E0(0x1B), step++, sub=0
//   step 4  func_001ABE10(); done -> func_001FF1E0(0x1C), step++  (sub NOT cleared)
//   step 5  step++, func_00119978(0,0x3FFF,0x3FFF), func_00119978(1,0x3FFF,0x3FFF)
//   step 6  D_00275BDC = 0, func_001AB790(func_001AC070) and RETURN (no tail call)
// Every path except step 6 ends in func_001D2830(3, 1); so does the
// out-of-range default (step >= 7).
// Callees func_001B0BA0 / func_001AB9D0 / func_001ABC60 / func_001ABE10 are
// invoked with no argument set-up (the record pointer happens to still be
// live in $a0), so they are declared and called argument-less here.
// NEARMISS: 99.92% - the only residual is the jump-table reloc (local @NN
// vs. the original's external jtbl_0026DC50); see docs on the jr-table
// external-dispatch wall.

extern void func_001B0BA0();
extern int func_001FB370(int);
extern void func_00225A00();
extern int func_001AB9D0();
extern int func_001ABC60();
extern int func_001ABE10();
extern void func_001FF1E0(int);
extern void func_00119978(int, int, int);
extern void func_001AB790(void *);
extern void func_001D2830(int, int);
extern void func_001AC070();

extern int D_0028A4A4[2];
extern int D_0028A734[2];
extern int D_0028A738[2];
extern char D_00275BDC;

void func_001AB7E0(void) {
    unsigned char *st;
    unsigned char *sub;
    int r;

    st = (unsigned char *)*(volatile unsigned char **)0x70003B6C;
    sub = st + 8;
    switch (st[8]) {
    case 0:
        switch (st[9]) {
        case 0:
            func_001B0BA0();
            (*(volatile unsigned char **)0x70003B6C)[9] =
                (*(volatile unsigned char **)0x70003B6C)[9] + 1;
            break;
        case 1:
            r = func_001FB370(D_0028A4A4[0]);
            if (r != 0) {
                D_0028A734[0] = r;
                D_0028A738[0] = r;
                func_00225A00();
                (*(volatile unsigned char **)0x70003B6C)[8] =
                    (*(volatile unsigned char **)0x70003B6C)[8] + 1;
                (*(volatile unsigned char **)0x70003B6C)[9] = 0;
            }
            break;
        }
        break;
    case 1:
        sub[0] = sub[0] + 1;
        (*(volatile unsigned char **)0x70003B6C)[9] = 0;
        (*(volatile unsigned char **)0x70003B6C)[0xA] = 0;
        /* fallthrough */
    case 2:
        if (func_001AB9D0() != 0) {
            (*(volatile unsigned char **)0x70003B6C)[8] =
                (*(volatile unsigned char **)0x70003B6C)[8] + 1;
            (*(volatile unsigned char **)0x70003B6C)[9] = 0;
        }
        break;
    case 3:
        if (func_001ABC60() != 0) {
            func_001FF1E0(0x1B);
            (*(volatile unsigned char **)0x70003B6C)[8] =
                (*(volatile unsigned char **)0x70003B6C)[8] + 1;
            (*(volatile unsigned char **)0x70003B6C)[9] = 0;
        }
        break;
    case 4:
        if (func_001ABE10() != 0) {
            func_001FF1E0(0x1C);
            (*(volatile unsigned char **)0x70003B6C)[8] =
                (*(volatile unsigned char **)0x70003B6C)[8] + 1;
        }
        break;
    case 5:
        sub[0] = sub[0] + 1;
        func_00119978(0, 0x3FFF, 0x3FFF);
        func_00119978(1, 0x3FFF, 0x3FFF);
        break;
    case 6:
        D_00275BDC = 0;
        func_001AB790(func_001AC070);
        return;
    }
    func_001D2830(3, 1);
}
