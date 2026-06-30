// NEARMISS func_001D40E0  (vram 0x001D40E0, 0x1F8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.55% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body-correct near-miss at 96.5% (mwcc 2.3.3; 991202=76.2%). Full logic recovered: VIF/DMA list builder -- vif_append_ref_tag, func_001D6930 (5-arg), func_001D1F80, three float_to_int(128.0*pos) stores to D_00251BE0/4/8 (.data), func_001D6580, sibling-call func_001D1F20, func_001D6BA0 (6-arg), fun...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// VIF/DMA display-list builder: appends a ref tag, sets up two unpacks
// (func_001D6930/func_001D1F80), writes three float_to_int(128.0*pos) values
// to D_00251BE0/4/8, runs func_001D6580/func_001D1F20/func_001D6BA0/func_001D1FF0,
// then appends three 0x10-byte command records into the per-slot list at
// D_00275670[a0]: a bank record (size 8, base D_00817040 + (slot<<7)),
// a fallback record (D_002514B0, size 2) only when func_001D2910 returns 0,
// and a final record from arg1 (size *(arg1+4), base arg1+0x40).
extern char D_0023D0C0[16];
extern char D_002514B0[];
extern char D_00251BD0[16];
extern int D_00251BE0[2];
extern int D_00251BE4[2];
extern int D_00251BE8[2];
extern char D_0026E470[16];
extern char D_0026E480[16];
extern char *D_00275670;
extern int D_0027567C;
extern char D_00817040[16];

extern int vif_append_ref_tag(void *p);
extern void func_001D6930(int a0, int a1, int a2, int a3, char *t0);
extern void func_001D1F80(int a0, int a1, int a2);
extern int float_to_int(float f);
extern void func_001D6580(int a0, void *a1, void *a2, int a3);
extern void func_001D1F20();
extern void func_001D6BA0(int a0, int a1, int a2, int a3, int t0, int t1);
extern void func_001D1FF0(int a0, int a1);
extern int func_001D2910(int a0, char *a1, char *a2, int a3);

void func_001D40E0(int a0, char *a1, char *a2)
{
    char *v1;
    char *t0;
    int v0;
    int t1;
    char **a2p;
    char *ptr;

    vif_append_ref_tag(D_0023D0C0);
    func_001D6930(a0, D_0027567C, 8, 8, D_0026E470);
    func_001D1F80(a0, 0, 2);
    D_00251BE0[0] = float_to_int(128.0f * *(float *)(a2 + 0));
    D_00251BE4[0] = float_to_int(128.0f * *(float *)(a2 + 4));
    D_00251BE8[0] = float_to_int(128.0f * *(float *)(a2 + 8));
    func_001D6580(a0, D_0026E480, D_00251BD0, 0x48);
    func_001D1F20(a0);
    func_001D1F80(a0, 1, 1);
    func_001D6BA0(a0, D_0027567C, 8, 8, 0, 0);
    func_001D1FF0(a0, 3);

    v1 = D_00275670;
    t1 = *(int *)(v1 + 0x9c);
    v0 = a0 << 2;
    a2p = (char **)(v1 + v0);
    t1 = t1 << 7;
    t0 = D_00817040 + t1;
    a2p[4][3] = 0x30;
    *(char **)(a2p[4] + 4) = t0;
    *(short *)a2p[4] = 8;
    a2p[4] = a2p[4] + 0x10;
    if (func_001D2910(0, (char *)a2p, t0, t1) == 0) {
        a2p = (char **)(D_00275670 + v0);
        a2p[4][3] = 0x30;
        *(char **)(a2p[4] + 4) = D_002514B0;
        *(short *)a2p[4] = 2;
        a2p[4] = a2p[4] + 0x10;
    }
    t1 = *(int *)(a1 + 4);
    a2p = (char **)(D_00275670 + v0);
    a2p[4][3] = 0x30;
    *(char **)(a2p[4] + 4) = a1 + 0x40;
    *(short *)a2p[4] = t1;
    a2p[4] = a2p[4] + 0x10;
    func_001D1F20(a0, 0x30, (char *)a2p, t1);
}
