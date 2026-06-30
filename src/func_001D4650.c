// NEARMISS func_001D4650  (vram 0x001D4650, 0xE8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 88.34% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-coloring + instruction-scheduling permutation: 88.34% on 2.3.3, body/values fully correct after recovering (a) CSE of arg0*4 shared across both tag blocks while D_00275670 is reloaded per block, (b) reusing one D_00275670 pointer for both the array base and the ->0x9C field in block 1, (...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// Emits two VIF unpack tags into the DMA chain buffer for context arg0
// (D_00275670[arg0]): the first points at a matrix/data block in D_00816840
// indexed by D_00275670->0x9C, the second at arg1+0x40 with count arg1->4.
// Wraps the two writes between func_001D1F80 begin/end and a func_001D7080 call.
extern int vif_append_ref_tag();
extern int func_001D1F80();
extern int func_001D7080();
extern char D_0023C750[];
extern char *D_00275670;
extern char D_00816840[];

void func_001D4650(int arg0, char *arg1) {
    int off;
    char *e;
    char *q;

    vif_append_ref_tag(arg0, D_0023C750);
    func_001D1F80(arg0, 0, 0);
    func_001D7080(arg0, -0x80, 0.0f);

    off = arg0 * 4;
    {
        char *base = D_00275670;
        char *vptr = &D_00816840[*(int *)(base + 0x9C) << 7];
        e = base + off;
        *(char *)(*(char **)(e + 0x10) + 3) = 0x30;
        *(char **)(*(char **)(e + 0x10) + 4) = vptr;
        *(short *)(*(char **)(e + 0x10) + 0) = 8;
        *(char **)(e + 0x10) = *(char **)(e + 0x10) + 0x10;
    }

    q = D_00275670 + off;
    *(char *)(*(char **)(q + 0x10) + 3) = 0x30;
    *(char **)(*(char **)(q + 0x10) + 4) = arg1 + 0x40;
    *(short *)(*(char **)(q + 0x10) + 0) = *(short *)(arg1 + 4);
    *(char **)(q + 0x10) = *(char **)(q + 0x10) + 0x10;

    func_001D1F80(arg0, 1, 0, q);
}
