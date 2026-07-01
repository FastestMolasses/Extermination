// NEARMISS func_00187EE0  (vram 0x00187EE0, 0x298 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 90.00% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// idiom-13 delay-slot wall on address-lui speculation, 3 residual branches (case 0x5A/0x5B/0x5C): mwcc233 speculates the D_700038A0 lui-address computation into the branch delay slot but the target leaves it a nop. Tried caching the pointer in a local (no effect). All switch-dispatch order, body, a...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_00102948(void *, void *);
extern void func_001029C0(void *);
extern void func_00102B08(void *, void *, float);
extern void func_00102BB0(void *, void *, float);
extern void func_001031E0();
extern void func_001EFD90(int, void *, void *);
extern void func_001F0460(int, void *);
extern char D_700036A0[1];
extern char D_700036D0[1];
extern char D_700038A0[1];

void func_00187EE0(char *arg0) {
    struct {
        int f0;
        float f4;
        int f8;
        int fc;
    } sp20;
    unsigned char st;

    func_001031E0(&sp20);
    sp20.f4 -= 1.5f;
    st = *(unsigned char *)(arg0 + 0x23A);
    switch (st) {
    case 0:
        if (*(short *)(arg0 + 0x212) != 0) {
            func_001029C0(D_700036A0);
            func_00102BB0(D_700036A0, D_700036A0, *(float *)(arg0 + 0xC4));
            func_00102B08(D_700036A0, D_700036A0, -*(float *)(arg0 + 0x9C));
            func_00102948(D_700036D0, &sp20);
            func_001F0460(1, D_700036A0);
            return;
        }
        func_001EFD90(0x80000011, &sp20, arg0 + 0xC0);
        return;
    case 1:
    case 2:
    case 3:
    case 4:
        return;
    case 5:
        func_001EFD90(0x80000028, &sp20, arg0 + 0xC0);
        return;
    case 0x5A:
        func_001031E0(D_700038A0, arg0 + 0xB0);
        *(volatile float *)0x700038A4 = *(float *)(arg0 + 0x250);
        func_001EFD90(0x80000065, D_700038A0, arg0 + 0xC0);
        return;
    case 8:
        func_001EFD90(0x80000066, &sp20, arg0 + 0xC0);
        return;
    case 0x5C:
        func_001031E0(D_700038A0, arg0 + 0xB0);
        *(volatile float *)0x700038A4 = *(float *)(arg0 + 0x250);
        func_001EFD90(0x80000067, D_700038A0, arg0 + 0xC0);
        return;
    case 6:
        func_001EFD90(0x80000005, &sp20, arg0 + 0xC0);
        return;
    case 0x5B:
        func_001031E0(D_700038A0, arg0 + 0xB0);
        *(volatile float *)0x700038A4 = *(float *)(arg0 + 0x250);
        func_001EFD90(0x8000001D, D_700038A0, arg0 + 0xC0);
        return;
    case 0xD:
    case 0xE:
        return;
    case 7:
        func_001EFD90(0x80000068, &sp20, arg0 + 0xC0);
        return;
    }
}
