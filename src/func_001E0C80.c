// NEARMISS func_001E0C80  (vram 0x001E0C80, 0x3C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 89.93% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). Object similarity does not prove semantic equivalence.
// Remaining differences in this candidate:
// The target computes the flag word address (D_00275670 + 0x174) into its own register for the store and keeps the bit-test result in a0; mwcc 2.3.3 folds both accesses into base + 0x174 and keeps the result in v1.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// Sets (set != 0) or clears bit (bit - 32) of the flag word D_00275670 + 0x174
// and returns whether it was set before.

extern char *D_00275670;

int func_001E0C80(int bit, int set) {
    int n = bit - 0x20;
    char *base = D_00275670;
    int mask = 1 << n;
    int v = *(int *)(base + 0x174);
    int was = v & mask;
    if (set) {
        v |= mask;
    } else {
        v &= ~mask;
    }
    *(int *)(base + 0x174) = v;
    return was != 0;
}
