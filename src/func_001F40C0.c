// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// Sweep over 0x80 entities (stride 0x90 from D_007709C0). For each live entity
// (the short at +0x80 == 0): run func_001F3620(entity, entity->+0x82); if it is
// still live afterward, look up a 0x60-stride record in table D_0025A350 indexed
// by the +0x82 short, and call func_001F3E30 with the shared scratch D_700036A0,
// the entity's +0x40 sub-block, and three ints from the record: +0x50, +0x54
// (sub-indexed by the loop counter's parity, i.e. (i%2)*4), and +0x5C. After the
// loop, decrement the gp-rel counter D_00275C44.
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906); the pinned 991202 build reaches
// 93.36%. Keys: (1) declaring the loop counter `i` BEFORE the pointer `e` colors
// the pointer to $s1 and the counter to $s0 (target's allocation); (2) the table
// pointer must stay un-offset and be read as int[] (base[0x14]/base[0x17] for
// +0x50/+0x5C) so 0x50/0x5C load directly off the base register while only the
// 0x54 access carries the (i%2)*4 byte adjustment -- folding +0x50 into the
// pointer (char*+0x50) emits a spurious extra addiu; (3) func_001F3E30 takes 5
// args (the +0x5C int in $t0 is the 5th); (4) sdatathreshold 8 puts the 4-byte
// D_00275C44 gp-relative. objdiff 100.0 vs expected.
extern char D_007709C0[];
extern char D_0025A350[];
extern char D_700036A0[];
extern int D_00275C44;
extern void func_001F3620(char *p, short idx);
extern void func_001F3E30(char *dst, char *src, int a, int b, int c);

void func_001F40C0(void) {
    int i;
    char *e = D_007709C0;

    for (i = 0; i < 0x80; i++) {
        if (*(short *)(e + 0x80) == 0) {
            short idx = *(short *)(e + 0x82);
            func_001F3620(e, idx);
            if (*(short *)(e + 0x80) == 0) {
                int *base = (int *)(D_0025A350 + *(short *)(e + 0x82) * 0x60);
                func_001F3E30(D_700036A0, e + 0x40,
                              base[0x14],
                              *(int *)((char *)base + (i % 2) * 4 + 0x54),
                              base[0x17]);
            }
        }
        e += 0x90;
    }
    D_00275C44 -= 1;
}
