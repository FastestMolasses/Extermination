// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Byte-matched (lane DMATCH 2026-10-02, was companion C at 97.10%): obj+0x1C is read
// through a volatile int (it is loaded before the table base, as in the original), and the
// three floats are read as fields of a 12-byte struct indexed by the halfword index.
// Reads vertex i of obj into out: returns 0 when i >= obj+0x18 (byte count);
// otherwise the halfword index at (0x70003204 base + obj+0x1C) + 2i selects a 12-byte
// entry of the table at 0x700031FC whose three floats are copied to out; returns the
// count.
typedef struct { float x; float y; float z; } V3;
int func_0019F680(float *out, unsigned char *obj, int i) {
    char *base = *(char **)0x70003204 + *(volatile int *)(obj + 0x1C);
    short *idx;
    if (i >= obj[0x18]) {
        return 0;
    }
    idx = (short *)base + i;
    out[0] = (*(V3 **)0x700031FC)[*idx].x;
    out[1] = (*(V3 **)0x700031FC)[*idx].y;
    out[2] = (*(V3 **)0x700031FC)[*idx].z;
    return obj[0x18];
}
