// NEARMISS func_001D37D0  (vram 0x001D37D0, 0xC4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.41% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body fully recovered (reload idiom #10 applied: *slot is reloaded before each of the 4 field writes per block; the short field is written from a word load via an int temp so mwcc emits `lw .. ; sh ..` not `lh`; far globals over-declared as arrays for absolute addressing). Sole residual is the emi...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// NEARMISS (95.4% mwcc 2.3.3; 87.0% 991202) -- body/logic correct, sole residual
// is a callee-saved arg-copy scheduling/delay-slot permutation (see wall note).
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
extern void vif_append_ref_tag(void *p);
extern int func_001D2910(int a);
extern unsigned char D_0023C750[16];
extern unsigned char D_002514B0[16];
extern int D_00275670;

void func_001D37D0(int arg0, char *arg1) {
    char **slot;
    int v;

    vif_append_ref_tag(D_0023C750);
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
