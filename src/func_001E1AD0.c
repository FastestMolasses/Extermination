// NEARMISS func_001E1AD0  (vram 0x001E1AD0, 0x38C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.08% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful.
// Fixes read from the original (matching wave 2026-10-28, was 53.61%): the channel index is the
// SECOND parameter (arg0 unused); func_001D6BA0 takes six arguments (..., 0, 1); the random
// value is converted as an unsigned int (no hand-written odd/even fold); both grid loops are
// for-loops (a do-while let the compiler hoist the whole row-V expression, the original only
// hoists the 224*row products); the row header goes through q = p + 0x10 with a quadword zero
// store, set before the cursor bump; local declaration order; the prologue sum puts the offset first.
// Remaining differences: scratch-register colouring and ordering in the per-row header stores
// (the original keeps the channel pointer in the register of its gp load; same size 0x38C).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// Grid packet builder: builds a 32x32 grid of GS vertex-pair records into the
// channel's linked draw buffer (arg1 indexes D_00275670[]; each slot's +0x10
// holds the current buffer cursor) and returns the cursor as it was on entry.
// Draws a random value from func_001D2E00(1), converts it (unsigned) to a
// fraction of 65535 and subtracts it from the persistent phase accumulator
// D_00275C0C, then re-normalizes D_00275C0C into [0,1) by repeatedly adding
// 1.0f. Initializes the draw state (func_001E1760, func_001D6BA0,
// func_001D1F80/func_001D1FF0/func_001D7000), then emits 32 rows: each row
// starts with a DMA tag (byte +3 = 0x10, word +4 = 0, half +0 = qwc 0xC2;
// cursor advanced by 0xC30), a zeroed quadword, VIF DIRECT 0xC1 at +0x1C and a
// GIFtag (0x602E4000_00008020 / 0x512512), followed by 32 cells of 0x60 bytes.
// Per cell, U spans -256..256 in 31 steps over the column and V spans
// -112..112 in 31 steps over the row (top edge row, bottom edge row + 1);
// func_001E0E80 fills each half (top at +0, bottom at +0x30) from U and 2*V
// and its result goes to +0x1C/+0x4C; the halves' +4 floats get D_00275C0C
// added, the colour words are 0x80 and the screen XY words are
// (coord + 0x800) * 16. After the grid, resets the draw mode
// (func_001D1F80(arg1, 3, 8), func_001E17E0) and appends a terminator tag
// (byte +3 = 0x60, count 0) to the slot's buffer, bumping its cursor by 0x10.
typedef unsigned int u128 __attribute__((mode(TI)));
extern int float_to_int(float);
extern void func_001D1F80(int a0, int a1, int a2);
extern void func_001D1FF0(int a0, int a1);
extern int func_001D2E00(int a0);
extern char *func_001D6BA0(int arg0, int arg1, int arg2, int arg3, int t0p, int t1p);
extern void func_001D7000(int a0, int a1);
extern int func_001E0E80(char *arg0, int arg1, int arg2);
extern void func_001E1760(int a0);
extern void func_001E17E0(int a0);

extern char *D_00275670;
extern float D_00275C0C;

int func_001E1AD0(int arg0, int arg1) {
    int col;
    int row;
    int u;
    int vTop;
    int vBot;
    char *q;
    int off;
    char *ch;
    char *p;
    int savedCursor;

    off = arg1 * 4;
    savedCursor = *(int *)(off + (int)D_00275670 + 0x10);
    D_00275C0C -= (float)(unsigned int)func_001D2E00(1) / 65535.0f;
    while (D_00275C0C < 0.0f) {
        D_00275C0C += 1.0f;
    }

    func_001E1760(arg1);
    func_001D6BA0(arg1, 0x258000, 8, 8, 0, 1);
    func_001D1F80(arg1, 0, 1);
    func_001D1FF0(arg1, 1);
    func_001D7000(arg1, 0x80);

    for (row = 0; row < 0x20; row++) {
        ch = D_00275670 + off;
        *(char *)(*(char **)(ch + 0x10) + 3) = 0x10;
        *(int *)(*(char **)(ch + 0x10) + 4) = 0;
        *(short *)(*(char **)(ch + 0x10) + 0) = 0xC2;
        p = *(char **)(ch + 0x10);
        q = p + 0x10;
        *(char **)(ch + 0x10) = p + 0xC30;
        *(u128 *)q = 0;
        *(int *)(q + 0xC) = 0x500000C1;
        *(long long *)(q + 0x10) = (long long)0x8020 | ((long long)0x602E4000 << 32);
        *(long long *)(q + 0x18) = 0x512512;
        q += 0x20;

        for (col = 0; col < 0x20; col++) {
            u = float_to_int(-256.0f + ((512.0f * (float)col) / 31.0f));
            vTop = float_to_int(-112.0f + ((224.0f * (float)row) / 31.0f));
            vBot = float_to_int(-112.0f + ((224.0f * (float)(row + 1)) / 31.0f));

            *(int *)(q + 0x1C) = func_001E0E80(q, u, vTop * 2);
            *(int *)(q + 0x4C) = func_001E0E80(q + 0x30, u, vBot * 2);

            *(float *)(q + 4) = *(float *)(q + 4) + D_00275C0C;
            *(float *)(q + 0x34) = *(float *)(q + 0x34) + D_00275C0C;

            *(int *)(q + 0x10) = 0x80;
            *(int *)(q + 0x14) = 0x80;
            *(int *)(q + 0x18) = 0x80;
            *(int *)(q + 0x20) = (u + 0x800) * 0x10;
            *(int *)(q + 0x24) = (vTop + 0x800) * 0x10;
            *(int *)(q + 0x2C) = 0;
            *(int *)(q + 0x40) = 0x80;
            *(int *)(q + 0x44) = 0x80;
            *(int *)(q + 0x48) = 0x80;
            *(int *)(q + 0x50) = (u + 0x800) * 0x10;
            *(int *)(q + 0x54) = (vBot + 0x800) * 0x10;
            *(int *)(q + 0x5C) = 0;

            q += 0x60;
        }

    }

    func_001D1F80(arg1, 3, 8);
    func_001E17E0(arg1);

    ch = D_00275670 + off;
    *(char *)(*(char **)(ch + 0x10) + 3) = 0x60;
    *(int *)(*(char **)(ch + 0x10) + 4) = 0;
    *(short *)(*(char **)(ch + 0x10) + 0) = 0;
    *(char **)(ch + 0x10) = *(char **)(ch + 0x10) + 0x10;

    return savedCursor;
}
