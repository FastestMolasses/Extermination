// NEARMISS func_00114C30  (vram 0x00114C30, 0xE8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 92.36% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc o32-vs-eabi list-scheduler wall. Body decompiled correctly (sizes equal 232==232); diverges only in the func_00123418 argument-setup scheduling/delay-slot fill (expected fills delay slot with sw s0,0x10(s1) and orders addiu a2,0x3ff earlier; our eabi64 ee-gcc fills with addiu a2,0x3ff). objdump confirms ABI mi...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int D_00241D68;
extern unsigned char D_0027B0C0[];
extern int D_0027B170;
extern unsigned char D_0027B680[];
extern int D_0027C680;
extern int func_00114BA8();
extern int func_00123418(void *, int, int);
extern int func_0010DFD8(void *, int);
extern int func_0010E8A8(void *, int, int, void *, int, void *, int, void *, int);

int func_00114C30(int a0, int a1, int a2, int a3)
{
    int t0 = a1;
    int *s3;
    int *s1;
    int v1;

    if (D_00241D68 != 0)
        return D_00241D68;

    s3 = (int *)D_0027B0C0;
    if (s3[9] == 0)
        return -0x64;

    s1 = &D_0027B170;
    s1[0] = a0;
    s1[4] = (int)D_0027B680;
    s1[1] = t0;
    func_00123418(&s1[5], a2, 0x3FF);
    *((unsigned char *)s1 + 0x413) = 0;
    func_0010DFD8(D_0027B680, 0x400);

    v1 = func_0010E8A8(s3, 0xC, 0x1, s1, 0x414, &D_0027C680, 0x4,
                       func_00114BA8, a3);
    if (v1 == 0)
        D_00241D68 = 0xC;
    return v1;
}
