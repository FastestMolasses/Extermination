// NEARMISS func_001D3E40  (vram 0x001D3E40, 0x110 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 71.07% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// register-allocation/scheduling artifact. Body logically complete & faithful (verified op-for-op; all diffs are register renames / reordering, no missing or extra operations). -sdatathreshold 4 makes D_00275670 gp-relative (matches target). The func is: vif_append_ref_tag(arg1+0x40, D_0023C480); t...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// NEARMISS (72.3% on pinned mwcc 991202; 233 = 71.1%): body logically complete &
// faithful (op-for-op verified; all deltas are register renames/reordering, no
// missing/extra ops). Sole residual is callee-saved register coloring + entry
// save order (target arg1->s0, idx->s2 [reuses arg0's reg], tag->s1; mwcc differs)
// plus an lh-vs-lw narrowing on the last half-store. Permuter territory.
// -sdatathreshold 4 required so D_00275670 is gp-relative.
//
// Emits up to three DMA "ref" tags into the per-channel command buffer for channel
// arg0. base = D_00275670; the channel descriptor is at base + (arg0<<2); its
// current write pointer is *(desc+0x10). vif_append_ref_tag(arg1+0x40, D_0023C480)
// runs first. Tag 1: qword count byte at +3 = 0x30, address word at +4 =
// D_00816440 + (base[0x9C] << 7) (a 0x80-stride table indexed by base[0x9C]),
// half at +0 = 8; pointer += 0x10. If func_001D2910(0) returns 0, Tag 2: +3 = 0x30,
// +4 = D_002514B0, +0 = 2; pointer += 0x10. Tag 3 (always): +3 = 0x30, +4 =
// arg1+0x40, +0 = *(int*)(arg1+4); pointer += 0x10.

extern void vif_append_ref_tag(void *a0, void *a1);
extern int func_001D2910(int a0);

extern char *D_00275670;
extern char D_0023C480[];
extern char D_00816440[];
extern char D_002514B0[];

void func_001D3E40(int arg0, char *arg1)
{
    int idx;
    char *tag;
    char *e;
    int *p;

    tag = arg1 + 0x40;
    vif_append_ref_tag(tag, D_0023C480);

    idx = arg0 << 2;
    e = D_00275670 + idx;
    p = *(int **)(e + 0x10);
    *(char *)((char *)p + 3) = 0x30;
    p = *(int **)(e + 0x10);
    *(int *)((char *)p + 4) = (int)(D_00816440 + (*(int *)(D_00275670 + 0x9C) << 7));
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

    e = D_00275670 + idx;
    p = *(int **)(e + 0x10);
    *(char *)((char *)p + 3) = 0x30;
    p = *(int **)(e + 0x10);
    *(int *)((char *)p + 4) = (int)tag;
    p = *(int **)(e + 0x10);
    *(short *)p = *(int *)(arg1 + 4);
    p = *(int **)(e + 0x10);
    *(int **)(e + 0x10) = (int *)((char *)p + 0x10);
}
