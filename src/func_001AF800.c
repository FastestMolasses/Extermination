// NEARMISS func_001AF800  (vram 0x001AF800, 0x84 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 59.06% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Two compounding 2.3.1 artifacts neither 991 nor 233 fixes (991=73.42% best; 233 regresses to 59.06%, so 2.3.3 makes it WORSE -- not a clean-store-nop case). (1) Inner-loop instruction-scheduling padding: the CW 2.3.1 target inserts a nop before the `sq zero,0(v1)` and TWO nops before the loop `bn...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

extern int *D_00275BD0;
extern short D_00275BCC;

void func_001AF800(char *arg0) {
    int i;
    int *p;
    int j;
    int *cur;

    i = 0;
    while (i < *(unsigned char *)(arg0 + 9)) {
        p = *(int **)(arg0 + 0x110 + i * 4);
        for (j = 0xD; j != 0; j--) {
            *(int *)((char *)p + 0) = 0;
            *(int *)((char *)p + 4) = 0;
            *(int *)((char *)p + 8) = 0;
            *(int *)((char *)p + 0xC) = 0;
            p = (int *)((char *)p + 0x10);
        }
        cur = *(int **)(arg0 + 0x110 + i * 4);
        D_00275BD0 = D_00275BD0 - 1;
        *D_00275BD0 = (int)cur;
        *(int *)(arg0 + 0x110 + i * 4) = 0;
        i++;
    }
    D_00275BCC = D_00275BCC + *(unsigned char *)(arg0 + 9);
    *(unsigned char *)(arg0 + 9) = 0;
    *(unsigned char *)(arg0 + 0xC) = 0;
}
