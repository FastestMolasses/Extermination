// NEARMISS sub_PsIIlibpad_2000  (vram 0xsIIlibpad_2000, 0x134 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.74% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// PREREQUISITE FOR THE 100.0 (must land before integrating): the recorded 99.74% was NEVER a compiler wall - it is a defect in OUR expected object. splat invented a pseudo-symbol D_FFFF (four F's, addr 0x0000FFFF) and paired the delay loop's `lui $v0,0x1` with the loop body's `addiu $v0,$v0,-1` int...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

//
// libpad bring-up: publishes the "pad system active" flag, then opens the two
// controller ports through the SIF helper func_0010E6F8, spinning on a
// 0x10000-iteration nop delay loop until each port's ready word (D_00279700[9]
// for port 0, [19] for port 1) goes non-zero. Once both are up it queries the
// libpad module version (func_001114B8): the high byte must be 3, otherwise it
// prints the two diagnostic strings and bails with 0. On success it tail-calls
// func_001107C8 with the caller's argument.

extern int func_0010E6F8(int a0, int a1, int a2);
extern int func_001114B8(void);
extern void func_00122B58(char *fmt, ...);
extern int func_001107C8(int a0);

extern int D_00241CB0;
extern int D_00279700[];
extern char D_0026BA28[];
extern char D_0026BA50[];

int sub_PsIIlibpad_2000(int a0)
{
    int i;
    int r;

    D_00241CB0 = 1;

    for (;;) {
        func_0010E6F8((int)&D_00279700[0], 0x80000100, 0);
        if (D_00279700[9] != 0) {
            break;
        }
        i = 0x10000;
        do {
            i--;
            asm("nop");
            asm("nop");
            asm("nop");
            asm("nop");
        } while (i != -1);
    }

    for (;;) {
        func_0010E6F8((int)&D_00279700[10], 0x80000101, 0);
        if (D_00279700[19] != 0) {
            break;
        }
        i = 0x10000;
        do {
            i--;
            asm("nop");
            asm("nop");
            asm("nop");
            asm("nop");
        } while (i != -1);
    }

    r = func_001114B8();
    if ((r >> 8) != 3) {
        func_00122B58(D_0026BA28);
        func_00122B58(D_0026BA50, 3, 0, r >> 8, r & 0xFF);
        return 0;
    }
    return func_001107C8(a0);
}
