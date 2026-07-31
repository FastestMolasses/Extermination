// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Double-buffer initializer: copies 14 fixed 0x80-byte source blocks
// (D_002514D0 .. D_00251B50, contiguous at 0x80 stride) into 14 destination
// buffers (D_00816440 .. D_00817140) via block_copy(dst, src, 0x80), then
// runs the whole batch a second time with every destination advanced by 0x80
// (so each of the 14 buffers is 0x100 bytes, filled in two halves). 9 of the
// destination cursors live in callee-saved regs; the 5 stored on the stack
// occupy 0xB0..0xF0 with the loop counter spilled to 0xA0 -- declaring the
// counter first makes mwcc assign it the low 0xA0 slot, which is the key to
// the match. mwcc 2.3.3 byte-identical; pinned 991202 build reaches 75.9%.
extern void block_copy(char *dst, char *src, int n);

extern char D_002514D0;
extern char D_00251550;
extern char D_002515D0;
extern char D_00251650;
extern char D_002516D0;
extern char D_00251750;
extern char D_002517D0;
extern char D_00251850;
extern char D_002518D0;
extern char D_00251950;
extern char D_002519D0;
extern char D_00251A50;
extern char D_00251AD0;
extern char D_00251B50;

extern char D_00816440;
extern char D_00816540;
extern char D_00816640;
extern char D_00816740;
extern char D_00816840;
extern char D_00816940;
extern char D_00816A40;
extern char D_00816B40;
extern char D_00816C40;
extern char D_00816D40;
extern char D_00816E40;
extern char D_00816F40;
extern char D_00817040;
extern char D_00817140;

void skin_arena_init(void) {
    int i;
    char *pB0;
    char *pC0;
    char *pD0;
    char *pE0;
    char *pF0;
    char *pfp;
    char *ps7;
    char *ps0;
    char *ps1;
    char *ps2;
    char *ps3;
    char *ps4;
    char *ps5;
    char *ps6;

    pB0 = &D_00816440;
    pC0 = &D_00816640;
    pD0 = &D_00816740;
    pE0 = &D_00816540;
    pF0 = &D_00816840;
    pfp = &D_00816940;
    ps7 = &D_00816A40;
    ps0 = &D_00816B40;
    ps1 = &D_00816C40;
    ps2 = &D_00816D40;
    ps3 = &D_00816E40;
    ps4 = &D_00816F40;
    ps5 = &D_00817040;
    ps6 = &D_00817140;

    for (i = 0; i < 2; i++) {
        block_copy(pB0, &D_002514D0, 0x80);
        block_copy(pC0, &D_00251550, 0x80);
        block_copy(pD0, &D_002515D0, 0x80);
        block_copy(pE0, &D_00251650, 0x80);
        block_copy(pF0, &D_002516D0, 0x80);
        block_copy(pfp, &D_00251750, 0x80);
        block_copy(ps7, &D_002517D0, 0x80);
        block_copy(ps0, &D_00251850, 0x80);
        block_copy(ps1, &D_002518D0, 0x80);
        block_copy(ps2, &D_00251950, 0x80);
        block_copy(ps3, &D_002519D0, 0x80);
        block_copy(ps4, &D_00251A50, 0x80);
        block_copy(ps5, &D_00251AD0, 0x80);
        block_copy(ps6, &D_00251B50, 0x80);
        pfp += 0x80;
        ps7 += 0x80;
        ps0 += 0x80;
        pB0 += 0x80;
        ps1 += 0x80;
        ps2 += 0x80;
        ps3 += 0x80;
        pC0 += 0x80;
        ps4 += 0x80;
        ps5 += 0x80;
        pD0 += 0x80;
        pE0 += 0x80;
        pF0 += 0x80;
        ps6 += 0x80;
    }
}
