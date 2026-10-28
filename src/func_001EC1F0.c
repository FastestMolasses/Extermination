// NEARMISS func_001EC1F0  (vram 0x001EC1F0, 0x7C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 93.55% via mwcc 991202 (-O4,p -sdatathreshold 4); 80.42% via mwcc 2.3.3 (the
// byte-matched sibling func_001EAF80 spelling). The LOGIC and STRUCTURE are faithful.
// Remaining differences: under 991202 only the second call's arg1 copy into the first
// argument register is scheduled one slot early. Under 2.3.3 only the order in which
// the three float constants (1.0, 5.0, 1e-6) are set up differs, and that order
// depends on what earlier functions in the same translation unit did (wave 2026-10-27:
// a preceding function in the same file that stores 1e-6 moves 1e-6 to the end and
// reaches 99.61%), so a one-function file cannot reproduce it.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc
// CFLAGS: -O4,p -sdatathreshold 4
//
// Animation/effect setup wrapper, sibling of the byte-matched func_001EAF80 and the
// single-call near-misses func_001EACF0/func_001EBBB0/func_001EBE10 (same template;
// this one uses constant 5.0f, table D_00256700, flag 1). Pulls two floats from the
// gp-relative object *D_00275C34 (fields +0x54 and +0x5C) plus the constants 1.0f,
// 1e-6f and 5.0f into func_001CFB50(D_0081F8F0, 0, arg0, ...), then one
// func_001CFBE0(arg1, 1, D_00256700, D_0081F8F0, 0). arg1 is kept in a saved
// register across the first call.
extern char *D_00275C34;
extern int D_00256700[8];
extern int D_0081F8F0[8];
extern void func_001CFB50(int *, int, int, float, float, float, float, float);
extern void func_001CFBE0(int, int, int *, int *, int);

void func_001EC1F0(int arg0, int arg1) {
    float one = 1.0f;
    float five = 5.0f;
    float f54 = *(float *)(D_00275C34 + 0x54);
    float tiny = 1e-6f;
    float f5c = *(float *)(D_00275C34 + 0x5C);
    func_001CFB50(D_0081F8F0, 0, arg0, f54, f5c, one, tiny, five);
    func_001CFBE0(arg1, 1, D_00256700, D_0081F8F0, 0);
}
