// NEARMISS func_00111950  (vram 0x00111950, 0x68 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 70.38% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// Body is logically byte-correct (same instruction count, same relocs, same control flow: if(D_00241D04){D_00241D48=-1; CreateSema(D_00241D10);} RFU063(D_00241D08); RFU063(D_00241D0C); tail-call RFU063(D_00241D10)). The ONLY difference is a single branch-delay-slot scheduling choice in the CreateSema branch: the targe...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int D_00241D04;
extern int D_00241D08;
extern int D_00241D0C;
extern int D_00241D10;
extern int D_00241D48;
extern int CreateSema(int a0);
extern int RFU063(int a0);

int func_00111950(void) {
    if (D_00241D04 != 0) {
        D_00241D48 = -1;
        CreateSema(D_00241D10);
    }
    RFU063(D_00241D08);
    RFU063(D_00241D0C);
    return RFU063(D_00241D10);
}
