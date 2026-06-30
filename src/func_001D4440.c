// NEARMISS func_001D4440  (vram 0x001D4440, 0x1F8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.55% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body-correct near-miss at 96.5% (mwcc 2.3.3; 991202=75.7%). Exact twin of func_001D40E0 (same VIF/DMA list-builder structure); differs only in globals (gp-rel D_00275680 vs D_0027567C; .data D_0026E4C0/D0, D_00251C10/20/24/28) and the func_001D1F80 third arg (7 vs 1). Same sdatathreshold-4 split ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Twin of func_001D40E0 (VIF/DMA display-list builder): identical structure,
// different list bank (D_00275680, D_0026E4C0/D0, D_00251C10/20/24/28) and the
// func_001D1F80 third arg = 7. Appends three 0x10-byte command records into the
// per-slot list at D_00275670[a0] as in the twin.
extern char D_0023D0C0[16];
extern char D_002514B0[];
extern char D_00251C10[16];
extern int D_00251C20[2];
extern int D_00251C24[2];
extern int D_00251C28[2];
extern char D_0026E4C0[16];
extern char D_0026E4D0[16];
extern char *D_00275670;
extern int D_00275680;
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

void func_001D4440(int a0, char *a1, char *a2)
{
    char *v1;
    char *t0;
    int v0;
    int t1;
    char **a2p;
    char *ptr;

    vif_append_ref_tag(D_0023D0C0);
    func_001D6930(a0, D_00275680, 8, 8, D_0026E4C0);
    func_001D1F80(a0, 0, 2);
    D_00251C20[0] = float_to_int(128.0f * *(float *)(a2 + 0));
    D_00251C24[0] = float_to_int(128.0f * *(float *)(a2 + 4));
    D_00251C28[0] = float_to_int(128.0f * *(float *)(a2 + 8));
    func_001D6580(a0, D_0026E4D0, D_00251C10, 0x48);
    func_001D1F20(a0);
    func_001D1F80(a0, 1, 7);
    func_001D6BA0(a0, D_00275680, 8, 8, 0, 0);
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
