// NEARMISS func_001C4A00  (vram 0x001C4A00, 0xE8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 86.78% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// NEARMISS 86.78% (mwcc233). Logic/structure fully recovered (4-state switch dispatcher, all call sites and CFG byte-shape correct). Three residuals, ALL agreed on by BOTH 991202 and 2.3.3 (ruling out a build-version fix between them): (1) shared-case likely-branch assignment swap — target emits be...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern int func_001B0FD0();
extern void func_001B1B70();
extern void func_001C6380(char *arg0);
extern void func_001AFC10(char *arg0);
extern unsigned char D_008107D8[];

void func_001C4A00(char *arg0) {
    unsigned char st;
    st = *(unsigned char *)(arg0 + 4);
    switch (st) {
    case 0:
        if (func_001B0FD0() == 0) {
            func_001C6380(arg0);
        }
        break;
    case 1:
        if (*(unsigned char *)0x70003B92 == 0) {
            func_001B1B70();
            (*(void (**)(char *))(arg0 + 0x4C))(arg0);
        } else if (D_008107D8[*(unsigned char *)(arg0 + 3)] != *(short *)(arg0 + 0x56)) {
            func_001B1B70();
            (*(void (**)(char *))(arg0 + 0x4C))(arg0);
        }
        break;
    case 2:
    case 3:
        func_001AFC10(arg0);
        break;
    }
}
