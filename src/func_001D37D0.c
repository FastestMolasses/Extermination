// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// Byte-matched (mwcc 2.3.3). The 95.4% NEARMISS residual came from a wrong
// prototype: vif_append_ref_tag takes (channel, address), so arg0 is forwarded
// in a0 and &D_0023C750 goes in a1.
//
// Builds DMA/VIF ref-tag descriptor entries. Appends a ref tag for &D_0023C750,
// then if func_001D2910(0)==0 writes a 0x10-byte descriptor at the cursor
// *(base + arg0*4 + 0x10) (base = gp-rel global D_00275670): byte +3 = 0x30,
// word +4 = &D_002514B0, half +0 = 2, and bumps the cursor by 0x10. Then
// unconditionally writes a second descriptor at the same cursor slot: byte +3 =
// 0x30, word +4 = arg1+0x40, half +0 = (short)*(int*)(arg1+4), cursor += 0x10.
// The cursor pointer *(slot) is reloaded before each field store (idiom #10).
// -sdatathreshold 4 keeps D_00275670 gp-relative; D_0023C750 / D_002514B0 are
// over-declared as arrays for absolute %hi/%lo addressing (idiom #20).
extern void vif_append_ref_tag(int ch, void *p);
extern int func_001D2910(int a);
extern unsigned char D_0023C750[16];
extern unsigned char D_002514B0[16];
extern int D_00275670;

void func_001D37D0(int arg0, char *arg1) {
    char **slot;
    int v;

    vif_append_ref_tag(arg0, D_0023C750);
    if (func_001D2910(0) == 0) {
        slot = (char **)((D_00275670 + arg0 * 4) + 0x10);
        *(char *)(*slot + 3) = 0x30;
        *(char **)(*slot + 4) = (char *)D_002514B0;
        *(short *)(*slot + 0) = 2;
        *slot = *slot + 0x10;
    }
    v = *(int *)(arg1 + 4);
    slot = (char **)((D_00275670 + arg0 * 4) + 0x10);
    *(char *)(*slot + 3) = 0x30;
    *(char **)(*slot + 4) = arg1 + 0x40;
    *(short *)(*slot + 0) = v;
    *slot = *slot + 0x10;
}
