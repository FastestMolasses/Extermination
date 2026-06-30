// NEARMISS func_001115D0  (vram 0x001115D0, 0xB0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 74.52% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// ee-gcc address-base register-allocation + reload-scheduling permutation. Logic/structure correct (store *a0->D_00241D48, D_00241D4C=D_00241D48 reload, on D_00241D18 flag set && D_00241D48==0xB clear D_00241D48+D_00241D14 & return; else _iSignalSema(D_00241D08), then if D_00241D04 && D_00241D44 _iSignalSema(D_00241D1...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int D_00241D04;
extern int D_00241D08;
extern int D_00241D10;
extern int D_00241D14;
extern int D_00241D18;
extern int D_00241D44;
extern int D_00241D48;
extern int D_00241D4C;

extern int _iSignalSema(int);

void func_001115D0(int *p) {
    D_00241D48 = *p;
    D_00241D4C = D_00241D48;
    if (D_00241D18 != 0) {
        if (D_00241D48 == 0xB) {
            D_00241D48 = 0;
            D_00241D14 = 0;
            return;
        }
    }
    _iSignalSema(D_00241D08);
    if (D_00241D04 != 0 && D_00241D44 != 0) {
        _iSignalSema(D_00241D10);
    } else {
        D_00241D14 = 0;
    }
    D_00241D48 = 0;
}
