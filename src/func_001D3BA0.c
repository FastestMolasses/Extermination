// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// Calls func_001D38A0(idx,a1); then indexes the gp-rel pointer table D_00275670
// at idx*4 to reach a node whose +0x10 field is a write cursor (char**). Writes a
// 3-tag (0x30) at +0x3, a base address (&D_00816540[(node->0x9C)<<7]) at +0x4, an
// 8 (short) at +0x0, advances the cursor by 0x10, then tail-calls func_001D3AD0(idx,a1).
extern char D_00816540[];
extern char *D_00275670;
extern void func_001D38A0(int idx, int a1);
extern void func_001D3AD0(int idx, int a1);

void func_001D3BA0(int idx, int a1) {
    char *base;
    char **new_var;
    char *a3;
    char *t0;
    char *p;

    func_001D38A0(idx, a1);
    base = D_00275670;
    t0 = &D_00816540[(*((int *)(base + 0x9C))) << 7];
    a3 = base + (idx * 4);
    new_var = (char **)(a3 + 0x10);
    *((char *)((*new_var) + 0x3)) = 0x30;
    *((int *)((*new_var) + 0x4)) = (int)t0;
    *((short *)((*new_var) + 0x0)) = 8;
    p = (*new_var) + 0x10;
    *new_var = p;
    func_001D3AD0(idx, a1);
}
