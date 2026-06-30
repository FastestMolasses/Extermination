// NEARMISS func_001D3F60  (vram 0x001D3F60, 0x164 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 88.09% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation coloring + entry callee-save order. Body/structure fully faithful (op-for-op): inverse-CSE hoist of addr=(D_00816F40+(base[0x9C]<<7)) before the first store aligned the Tag-1 block, and vif_append_ref_tag(arg0, D_0023CC50) is the correct 2-arg form. Sole residual is mwcc color...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// NEARMISS 88.1% (mwcc 2.3.3; 71.8% on 991202) -- body/logic op-for-op faithful,
// sole residual is callee-save register coloring (entry save order s2=arg0 / s1=arg1)
// plus v0/v1/a0/a1 churn across the three identical DMA ref-tag store blocks.
// Permuter territory; -sdatathreshold 4 keeps D_00275670 gp-relative.
//
// Emits DMA "ref" tags into channel arg0's command buffer. base = D_00275670; the
// channel descriptor is base+(arg0<<2), its write cursor is *(desc+0x10). After a
// 2-arg vif_append_ref_tag(arg0, D_0023CC50), three setup helpers run
// (func_001D1F80/6B10/6BA0/1FF0). Tag 1: +3=0x30, +4=D_00816F40+(base[0x9C]<<7)
// (0x80-stride table), +0=8; cursor+=0x10. If func_001D2910(0)==0, Tag 2: +3=0x30,
// +4=D_002514B0, +0=2; cursor+=0x10. Tag 3 (always): +3=0x30, +4=arg1+0x40,
// +0=(short)*(int*)(arg1+4); cursor+=0x10. Final flush via func_001D1F20.
extern void vif_append_ref_tag();
extern void func_001D1F80();
extern void func_001D6B10();
extern void func_001D6BA0();
extern void func_001D1FF0();
extern int func_001D2910();
extern void func_001D1F20();

extern char D_0023CC50[];
extern char D_002514B0[];
extern char D_00816F40[];
extern char *D_00275670;
extern int D_00275678;

void func_001D3F60(int arg0, char *arg1)
{
    int idx;
    int *p;
    char *e;
    int v;
    int addr;

    vif_append_ref_tag(arg0, D_0023CC50);
    func_001D1F80(arg0, 1, 0);
    func_001D6B10(arg0, D_00275678, 8, 8);
    func_001D6BA0(arg0, D_00275678, 8, 8, 2, 0);
    func_001D1FF0(arg0, 3);

    idx = arg0 << 2;
    e = D_00275670 + idx;
    addr = (int)(D_00816F40 + (*(int *)(D_00275670 + 0x9C) << 7));
    p = *(int **)(e + 0x10);
    *(char *)((char *)p + 3) = 0x30;
    p = *(int **)(e + 0x10);
    *(int *)((char *)p + 4) = addr;
    p = *(int **)(e + 0x10);
    *(short *)p = 8;
    p = *(int **)(e + 0x10);
    *(int **)(e + 0x10) = (int *)((char *)p + 0x10);

    if (func_001D2910(0) == 0) {
        e = D_00275670 + idx;
        p = *(int **)(e + 0x10);
        *(char *)((char *)p + 3) = 0x30;
        p = *(int **)(e + 0x10);
        *(int *)((char *)p + 4) = (int)D_002514B0;
        p = *(int **)(e + 0x10);
        *(short *)p = 2;
        p = *(int **)(e + 0x10);
        *(int **)(e + 0x10) = (int *)((char *)p + 0x10);
    }

    v = *(int *)(arg1 + 4);
    e = D_00275670 + idx;
    p = *(int **)(e + 0x10);
    *(char *)((char *)p + 3) = 0x30;
    p = *(int **)(e + 0x10);
    *(int *)((char *)p + 4) = (int)(arg1 + 0x40);
    p = *(int **)(e + 0x10);
    *(short *)p = v;
    p = *(int **)(e + 0x10);
    *(int **)(e + 0x10) = (int *)((char *)p + 0x10);
    func_001D1F20(arg0, 0x30, e, v);
}
