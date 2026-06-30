// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// Builds two VIF ref-tag command-buffer entries indexed by a0 in the
// per-slot ring D_00275670[a0<<2] (command buffer at field +0x10): a
// conditional first entry (qcount=0x30, ptr=D_002514B0, size=2) emitted
// only when func_001D2910(0)==0, then an unconditional second entry
// (ptr=a1+0x40, size=*(int*)(a1+0x4)). Match lever: the second entry's
// size word *(int*)(a1+0x4) must be hoisted to a temp BEFORE the header
// stores so mwcc 2.3.3 schedules the lw a3,0x4(s0) early (the pinned
// 991202 build loses this regalloc/schedule tie-break).
extern void vif_append_ref_tag(int a0, void *a1);
extern int func_001D2910(int a0);
extern char *D_00275670;
extern char D_002354A0[];
extern char D_002514B0[];

void func_001D3AD0(int a0, char *a1) {
    char *e;
    char *p;
    vif_append_ref_tag(a0, D_002354A0);
    if (func_001D2910(0) == 0) {
        e = D_00275670 + (a0 << 2);
        *(char *)(*(int *)(e + 0x10) + 0x3) = 0x30;
        *(int *)(*(int *)(e + 0x10) + 0x4) = (int)D_002514B0;
        *(short *)(*(int *)(e + 0x10) + 0x0) = 2;
        *(int *)(e + 0x10) = *(int *)(e + 0x10) + 0x10;
    }
    p = D_00275670 + (a0 << 2);
    {
        int h = *(int *)(a1 + 0x4);
        *(char *)(*(int *)(p + 0x10) + 0x3) = 0x30;
        *(int *)(*(int *)(p + 0x10) + 0x4) = (int)(a1 + 0x40);
        *(short *)(*(int *)(p + 0x10) + 0x0) = h;
    }
    *(int *)(p + 0x10) = *(int *)(p + 0x10) + 0x10;
}
