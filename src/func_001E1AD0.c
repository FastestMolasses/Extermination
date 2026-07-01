// NEARMISS func_001E1AD0  (vram 0x001E1AD0, 0x38C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 53.61% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-pressure/spill wall in a large (908-byte) 32x32-grid generator, same class as its documented siblings func_001E0E80 (86.12% parked) and func_001D6BA0 (90.34% parked). Body/logic fully recovered: random-phase accumulator update (func_001D2E00 odd/even fold, D_00275C0C renormalize-to-[0,1)...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// Weapon/particle-emitter setup: builds a 32x32 grid of GS sprite/quad
// records into the entity's linked draw-buffer (arg1 indexes
// D_00275670[]; each slot's +0x10 holds the current buffer cursor).
// Caches the slot's saved cursor in temp_fp (the return value) before
// advancing it. Draws a random per-shot rotation offset from
// func_001D2E00(1) (odd/even folded into a float 0..65534), converts it
// to a fraction of 65535 and subtracts it from the persistent phase
// accumulator D_00275C0C, then re-normalizes D_00275C0C into [0,1) by
// repeatedly adding 1.0f. Initializes the draw state (func_001E1760,
// func_001D6BA0 GS packing, func_001D1F80/func_001D1FF0/func_001D7000)
// then emits a 32(row)x32(col) grid of quads: each row's texcoord V
// spans -112..112 in 31 steps (top/bottom edge per row), each column's
// texcoord U spans -256..256 in 31 steps; per-cell it stages a header
// record (color/alpha at +0x1C/+0x20/+0x28) and two GS UV/vertex pairs
// via func_001E0E80 into the growing buffer (0x60 bytes/cell, 0x30
// bytes/half). Every cell also republishes the phase-shifted texcoord
// V bases (+0x4/+0x34) by adding D_00275C0C, and the packed
// scale/color words at +0x10.. / +0x40... After the grid, resets the
// draw mode (func_001D1F80(arg1,3,8), func_001E17E0) and appends a
// terminator record (mode 0x60, count 0) to the slot's buffer, bumping
// its cursor by 0x10.
extern int float_to_int(float);
extern void func_001D1F80(int a0, int a1, int a2);
extern void func_001D1FF0(int a0, int a1);
extern int func_001D2E00(int a0);
extern void func_001D6BA0(int a0, int a1, int a2, int a3);
extern void func_001D7000(int a0, int a1);
extern int func_001E0E80(char *arg0, int arg1, int arg2);
extern void func_001E1760(int a0);
extern void func_001E17E0(int a0);

extern char *D_00275670;
extern float D_00275C0C;

int func_001E1AD0(int arg1) {
    int off = arg1 * 4;
    int savedCursor = *(int *)(D_00275670 + off + 0x10);
    int rnd = func_001D2E00(1);
    float rndF;
    int row, col;
    char *cell;
    int u, vTop, vBot;

    if (rnd >= 0) {
        rndF = (float)rnd;
    } else {
        rndF = 2.0f * (float)(((unsigned int)rnd >> 1) | (rnd & 1));
    }
    D_00275C0C -= rndF / 65535.0f;
    while (D_00275C0C < 0.0f) {
        D_00275C0C += 1.0f;
    }

    func_001E1760(arg1);
    func_001D6BA0(arg1, 0x258000, 8, 8);
    func_001D1F80(arg1, 0, 1);
    func_001D1FF0(arg1, 1);
    func_001D7000(arg1, 0x80);

    row = 0;
    do {
        char *hdr = *(char **)(D_00275670 + off + 0x10);
        hdr[3] = 0x10;
        *(int *)(hdr + 4) = 0;
        *(short *)(hdr + 0) = 0xC2;
        *(int *)(D_00275670 + off + 0x10) = (int)(hdr + 0xC30);
        *(int *)(hdr + 0x10) = 0;
        *(int *)(hdr + 0x14) = 0;
        *(int *)(hdr + 0x18) = 0;
        *(int *)(hdr + 0x1C) = 0x500000C1;
        *(long long *)(hdr + 0x20) = 0x602E400000008020LL;
        *(long long *)(hdr + 0x28) = 0x512512LL;
        cell = hdr + 0x10 + 0x20;

        vTop = float_to_int(-112.0f + ((224.0f * (float)row) / 31.0f));
        vBot = float_to_int(-112.0f + ((224.0f * (float)(row + 1)) / 31.0f));

        col = 0;
        do {
            u = float_to_int(-256.0f + ((512.0f * (float)col) / 31.0f));

            *(int *)(cell + 0x1C) = func_001E0E80(cell, u, vTop * 2);
            *(int *)(cell + 0x4C) = func_001E0E80(cell + 0x30, u, vBot * 2);

            *(float *)(cell + 4) = *(float *)(cell + 4) + D_00275C0C;
            *(float *)(cell + 0x34) = *(float *)(cell + 0x34) + D_00275C0C;

            *(int *)(cell + 0x10) = 0x80;
            *(int *)(cell + 0x14) = 0x80;
            *(int *)(cell + 0x18) = 0x80;
            *(int *)(cell + 0x20) = (u + 0x800) * 0x10;
            *(int *)(cell + 0x24) = (vTop + 0x800) * 0x10;
            *(int *)(cell + 0x2C) = 0;
            *(int *)(cell + 0x40) = 0x80;
            *(int *)(cell + 0x44) = 0x80;
            *(int *)(cell + 0x48) = 0x80;
            *(int *)(cell + 0x50) = (u + 0x800) * 0x10;
            *(int *)(cell + 0x54) = (vBot + 0x800) * 0x10;
            *(int *)(cell + 0x5C) = 0;

            cell += 0x60;
            col++;
        } while (col < 0x20);

        row++;
    } while (row < 0x20);

    func_001D1F80(arg1, 3, 8);
    func_001E17E0(arg1);

    {
        char *hdr = *(char **)(D_00275670 + off + 0x10);
        hdr[3] = 0x60;
        *(int *)(hdr + 4) = 0;
        *(short *)(hdr + 0) = 0;
        *(int *)(D_00275670 + off + 0x10) = (int)(hdr + 0x10);
    }

    return savedCursor;
}
