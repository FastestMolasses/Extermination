// NEARMISS func_001AB430  (vram 0x001AB430, 0xB0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 82.16% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Instruction-scheduling near-miss: target hoists shared lui %hi(D_00810118) and keeps source order of three independent flag-byte stores (0x118/0x119/0x11C); mwcc 2.3.3 groups the two zero-stores. Loop, calls, and table clear all match. Not the clean-store-nop wall. Best=2.3.3 82.16%.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_0010C648(void);
extern void func_00121A28(unsigned char *, int, int);
extern int func_001AEA50(int *);
extern void func_001FC9B0(void);
extern unsigned char D_00275C78;
extern int D_0028A490[];
extern int D_0028A734;
extern unsigned char D_00810118;
extern unsigned char D_00810119;
extern unsigned char D_0081011C;
extern short D_00810E80;
extern unsigned char D_00821058;

void func_001AB430(void) {
    int *p;
    int i;

    func_0010C648();
    *(unsigned char *)0x70003B90 = 0;
    *(int *)0x70003B64 = 0;
    D_00810E80 = 0;
    D_00821058 = 0;
    D_00275C78 = 0;
    func_00121A28(&D_00810118, 0, 0xC);
    D_00810118 = 0;
    D_00810119 = 1;
    D_0081011C = 0;
    p = D_0028A490;
    i = 0;
    do {
        i += 1;
        *p = 0;
        p += 1;
    } while (i < 0xAF);
    D_0028A734 = 0xB00000;
    func_001AEA50(p);
    func_001FC9B0();
}
