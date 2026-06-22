// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// Reset/clear of a 0x20-entry table living at base pointer D_00275670
// (stride 0x80 per entry). Per entry: zero the eight words at +0x228,
// +0x22C(=-1), +0x230, +0x234, +0x238, +0x23C, +0x240, +0x244, +0x248,
// +0x24C; +0x22C is set to -1 (sentinel), the rest to 0. Two header words
// at base +0x210/+0x214 are zeroed first. Finally calls func_001F68B0 and
// func_001F6E40 (no live args -- leftover registers, declared arg-less).
// 991202 reorders the loop tail (sw +0x228 / addiu off) and stalls at
// 92.8%; mwcc 2.3.3 byte-matches. sdatathreshold 8 makes D_00275670 gp-rel.
extern void func_001F68B0();
extern void func_001F6E40();
extern unsigned char *D_00275670;

void func_001D7BB0(void) {
    int i;
    int off;
    unsigned char *p;

    *(int *)(D_00275670 + 0x210) = 0;
    *(int *)(D_00275670 + 0x214) = 0;
    i = 0;
    off = 0;
    do {
        i += 1;
        p = D_00275670 + off;
        *(int *)(p + 0x23C) = 0;
        *(int *)(p + 0x238) = 0;
        *(int *)(p + 0x234) = 0;
        *(int *)(p + 0x230) = 0;
        *(int *)(p + 0x24C) = 0;
        *(int *)(p + 0x248) = 0;
        *(int *)(p + 0x244) = 0;
        *(int *)(p + 0x240) = 0;
        *(int *)(p + 0x22C) = -1;
        off += 0x80;
        *(int *)(p + 0x228) = 0;
    } while (i < 0x20);
    func_001F68B0();
    func_001F6E40();
}
