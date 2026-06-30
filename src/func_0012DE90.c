// NEARMISS func_0012DE90  (vram 0x0012DE90, 0x1E0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.25% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Permuter-class on a fully-recovered body. Residual 1: commutative add.s FP-coloring/operand-order on `*(float*)(arg0+0xD4)+0.02f` (target emits add.s f1,f2,f1 = field+const; mwcc colors field->ft0/const->fv1 and emits const+field; not fixed by temp or assign-in-arg). Residual 2: scheduling placem...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

extern void func_001029C0(void *p);
extern void func_001026D0(void *dst, void *m, void *src);
extern int D_00275B40;
extern char D_700036A0[];

void func_0012DE90(char *arg0) {
    int p;
    int q;
    float t;

    p = D_00275B40;
    *(float *)0x700038A0 = *(float *)(*(int *)(p + 0xC) + 0xC0);
    *(float *)0x700038A4 = *(float *)(*(int *)(p + 0xC) + 0xC4);
    *(float *)0x700038A8 = *(float *)(*(int *)(p + 0xC) + 0xC8);
    *(float *)0x700038B0 = *(float *)(*(int *)(p + 0x14) + 0xC0);
    *(float *)0x700038B4 = *(float *)(*(int *)(p + 0x14) + 0xC4);
    *(float *)0x700038B8 = *(float *)(*(int *)(p + 0x14) + 0xC8);
    func_001029C0(D_700036A0);
    t = *(float *)(arg0 + 0xD4) + 0.02f;
    *(float *)(arg0 + 0xD4) = t;
    if (!(t <= 2.0f)) {
        *(float *)(arg0 + 0xD4) = 2.0f;
    }
    q = D_00275B40;
    *(float *)0x700036A0 = *(float *)(arg0 + 0xD4);
    *(float *)0x700036B4 = *(float *)(arg0 + 0xD4);
    *(float *)0x700036C8 = *(float *)(arg0 + 0xD4);
    *(int *)(*(int *)(q + 0xC) + 0xC0) = 0;
    *(int *)(*(int *)(D_00275B40 + 0xC) + 0xC4) = 0;
    *(int *)(*(int *)(D_00275B40 + 0xC) + 0xC8) = 0;
    *(int *)(*(int *)(D_00275B40 + 0x14) + 0xC0) = 0;
    *(int *)(*(int *)(D_00275B40 + 0x14) + 0xC4) = 0;
    *(int *)(*(int *)(D_00275B40 + 0x14) + 0xC8) = 0;
    func_001026D0((void *)(*(int *)(D_00275B40 + 0xC) + 0x90), D_700036A0, (void *)(*(int *)(D_00275B40 + 0xC) + 0x90));
    func_001026D0((void *)(*(int *)(D_00275B40 + 0x14) + 0x90), D_700036A0, (void *)(*(int *)(D_00275B40 + 0x14) + 0x90));
    *(float *)(*(int *)(D_00275B40 + 0xC) + 0xC0) = *(float *)0x700038A0;
    *(float *)(*(int *)(D_00275B40 + 0xC) + 0xC4) = *(float *)0x700038A4;
    *(float *)(*(int *)(D_00275B40 + 0xC) + 0xC8) = *(float *)0x700038A8;
    *(float *)(*(int *)(D_00275B40 + 0x14) + 0xC0) = *(float *)0x700038B0;
    *(float *)(*(int *)(D_00275B40 + 0x14) + 0xC4) = *(float *)0x700038B4;
    *(float *)(*(int *)(D_00275B40 + 0x14) + 0xC8) = *(float *)0x700038B8;
}
