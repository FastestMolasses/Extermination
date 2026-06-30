// NEARMISS func_001D42E0  (vram 0x001D42E0, 0x14C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 94.82% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// param-save emit order in prologue: target emits paddub s1,a1 before sq s0 and fills the first jal delay slot with paddub s0,a0 (arg0); mwcc emits arg0's copy first and defers arg1's to the slot. Documented idiom-17 'before-jal copy order locked ascending-source-param' residual. Body byte-identica...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// NEARMISS 94.8% (mwcc233). VIF/DMA chain builder for object row D_00275670[arg0].
// Appends a ref tag (D_0023D4D0), configures the channel via func_001D1F80 /
// func_001D6F60 (packed 64-bit GS-reg constant 0x2004818599422020) / func_001D1FF0,
// then writes up to three DMAtag entries into the row's current packet pointer
// (off 0x10): a DMACnt to the matrix table D_00817140 + (row->f9C<<7) (qwc 8),
// optionally a DMACnt to D_002514B0 (qwc 2) when func_001D2910 returns 0, and a
// final DMACnt to arg1+0x40 (qwc = *(int*)(arg1+4)). Each tag: id byte 0x30 at +3,
// addr at +4, qwc halfword at +0; pointer advanced by 0x10.
// Body byte-identical to target; sole residual is the prologue param-save emit
// order (paddub s1,a1 vs paddub s0,a0 ordering) — register/scheduling artifact.
extern int vif_append_ref_tag(void *p);
extern void func_001D1F80(int a, int b, int c);
extern void func_001D6F60(int a, long long b, int c);
extern void func_001D1FF0(int a, int b);
extern int func_001D2910(int a, void *b, void *c, int d);
extern char D_0023D4D0[];
extern char D_002514B0[];
extern char *D_00275670;
extern char D_00817140[];

void func_001D42E0(int arg0, char *arg1) {
    char *base;
    char *row;
    char *dma;
    int off7;

    vif_append_ref_tag(D_0023D4D0);
    func_001D1F80(arg0, 1, 1);
    func_001D6F60(arg0, 0x2004818599422020LL, 0x80);
    func_001D1FF0(arg0, 1);

    base = D_00275670;
    off7 = *(int *)(base + 0x9C) << 7;
    dma = D_00817140 + off7;
    row = base + arg0 * 4;

    *(char *)(*(char **)(row + 0x10) + 3) = 0x30;
    *(char **)(*(char **)(row + 0x10) + 4) = dma;
    *(short *)(*(char **)(row + 0x10) + 0) = 8;
    *(char **)(row + 0x10) = *(char **)(row + 0x10) + 0x10;

    if (func_001D2910(0, row, dma, off7) == 0) {
        row = D_00275670 + arg0 * 4;
        *(char *)(*(char **)(row + 0x10) + 3) = 0x30;
        *(char **)(*(char **)(row + 0x10) + 4) = D_002514B0;
        *(short *)(*(char **)(row + 0x10) + 0) = 2;
        *(char **)(row + 0x10) = *(char **)(row + 0x10) + 0x10;
    }

    row = D_00275670 + arg0 * 4;
    *(char *)(*(char **)(row + 0x10) + 3) = 0x30;
    *(char **)(*(char **)(row + 0x10) + 4) = arg1 + 0x40;
    *(short *)(*(char **)(row + 0x10) + 0) = *(int *)(arg1 + 4);
    *(char **)(row + 0x10) = *(char **)(row + 0x10) + 0x10;
}
