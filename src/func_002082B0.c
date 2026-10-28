// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 90.79% NEARMISS).
// Fixes read from the original instructions:
//  - the +0x40 tag doubleword is n | 0x4400000000008000 (0x8000 sits in the low
//    word); the old C put 0x8000 in the high word (wrong constant)
//  - the NLOOP count is a plain int (no 16-bit sign extension); the +0x10
//    quadword is cleared with one 128-bit store, as in func_001D6580
//  - |count| is a ternary on a temp, the fraction is computed in place in span
//    (span = span - ...; span *= 16), and the rect width reads +0xC before +8
//  - loop guard `0 < n`; the two packed-colour words are each one OR expression
//  - local declaration order fixes the saved-register assignment
//
// Builds a per-actor GS sprite-tile display-list record for a health/status
// bar-style strip. arg1 is a float rect {x0,y0,x1,y1} at +0/+4/+8/+0xC; the
// strip is subdivided into ceil(|(x1-y0)/16|) columns (count), the leftover
// fractional remainder is kept in span for the last column's step. Two
// color-ramp accumulators (D_70003600/D_70003610) are seeded from
// arg1+0x40/+0x20 and +0x50/+0x30 (func_001028D0), stepped by
// func_00102850(rate=count+2), and read back once into the packed-byte
// color scratch D_70003620/D_70003630 (func_00102948) before the loop.
//
// The context object for actor arg0 is D_00275670[arg0]; its +0x10 field is
// a bump-allocated GS display-list cursor. A 0x60-byte GIFtag/prim record
// is written at the cursor (a cleared quadword at +0x10, NLOOP=(count+2)*2+4-1
// packed with flags at +0x1C, PRIM/RGBAQ-ish qwords at +0x20/+0x28/+0x30/+0x38, a second tag
// word at +0x40/+0x48), the cursor is advanced by ((count+2)*2+4+1)*0x10,
// and the vertex loop (count+2 iterations) writes a packed color qword pair
// (float_to_int() of the ramp scratch) plus a UV/XYZ pair (float_to_int()
// of scaled trig terms) into two 16-byte vertex slots at v, advancing
// the column position by either 16.0 units or the fractional remainder on
// the final column.
typedef unsigned __int128 u128;
extern int float_to_int(float);
extern void func_00102850(void *dst, void *src, float rate);
extern void func_001028B8(void *dst, void *a, void *b);
extern void func_001028D0(void *dst, void *a, void *b);
extern void func_00102948(void *dst, void *src);
extern float func_0011DE90(float a); /* cos */
extern float func_0011E2A8(float a); /* sin */

extern char D_70003600[];
extern char D_70003610[];
extern char D_70003620[];
extern char D_70003630[];
extern char *D_00275670;

void func_002082B0(int arg0, char *arg1) {
    float ang;
    float span;
    float y0;
    int a;
    int count;
    int i;
    int b, c, d;
    char *v;
    char *elem;
    int n;
    int nloop;
    char *rec;
    char *ret;
    int t;

    span = (*(float *)(arg1 + 0xC) - *(float *)(arg1 + 8)) / 16.0f;
    y0 = *(float *)(arg1 + 8);
    count = float_to_int(span);
    t = count;
    count = (t < 0) ? -t : t;
    span = span - (float)float_to_int(span);
    span *= 16.0f;

    func_001028D0(D_70003600, arg1 + 0x40, arg1 + 0x20);
    func_001028D0(D_70003610, arg1 + 0x50, arg1 + 0x30);
    func_00102850(D_70003600, D_70003600, (float)(count + 2));
    func_00102850(D_70003610, D_70003610, (float)(count + 2));
    func_00102948(D_70003620, arg1 + 0x20);
    func_00102948(D_70003630, arg1 + 0x30);

    n = count + 2;
    nloop = n * 2 + 4;
    elem = D_00275670 + (arg0 << 2);
    (*(char **)(elem + 0x10))[3] = (char)0x10;
    *(int *)(*(char **)(elem + 0x10) + 4) = 0;
    *(short *)(*(char **)(elem + 0x10) + 0) = nloop;
    i = 0;
    rec = *(char **)(elem + 0x10);
    *(char **)(elem + 0x10) = rec + ((nloop + 1) * 0x10);
    ret = rec + 0x10;
    *(u128 *)ret = 0;
    *(int *)(rec + 0x1C) = (nloop - 1) | 0x50000000;
    *(long long *)(rec + 0x20) = 0x8001 | ((long long)0x10000000 << 32);
    *(long long *)(rec + 0x28) = 0xE;
    v = ret + 0x40;
    *(long long *)(rec + 0x30) = 0x14C;
    *(long long *)(rec + 0x38) = 0;
    *(long long *)(rec + 0x40) = (long long)n | (0x8000 | ((long long)0x44000000 << 32));
    *(long long *)(rec + 0x48) = 0x4141;

    if (0 < n) {
        do {
            ang = (3.1415927f * y0) / 180.0f;
            a = float_to_int(*(float *)(arg1 + 0) + (0.8f * (16.0f * *(float *)(arg1 + 0x10) * func_0011E2A8(ang))));
            b = float_to_int(*(float *)(arg1 + 4) + (0.5f * (16.0f * *(float *)(arg1 + 0x14) * func_0011DE90(ang))));
            c = float_to_int(*(float *)(arg1 + 0) + (0.8f * (16.0f * *(float *)(arg1 + 0x18) * func_0011E2A8(ang))));
            d = float_to_int(*(float *)(arg1 + 4) + (0.5f * (16.0f * *(float *)(arg1 + 0x1C) * func_0011DE90(ang))));

            *(long long *)(v + 0) = (long long)(float_to_int(*(float *)0x70003620) | (float_to_int(*(float *)0x70003624) << 8) | (float_to_int(*(float *)0x70003628) << 0x10) | (float_to_int(*(float *)0x7000362C) << 0x18));

            *(long long *)(v + 0x10) = (long long)(float_to_int(*(float *)0x70003630) | (float_to_int(*(float *)0x70003634) << 8) | (float_to_int(*(float *)0x70003638) << 0x10) | (float_to_int(*(float *)0x7000363C) << 0x18));

            *(long long *)(v + 8) = (long long)(a | (b << 0x10)) | ((long long)0xFFFFFF << 32);
            *(long long *)(v + 0x18) = (long long)(c | (d << 0x10)) | ((long long)0xFFFFFF << 32);

            func_001028B8(D_70003620, D_70003620, D_70003600);
            func_001028B8(D_70003630, D_70003630, D_70003610);

            if (i == count) {
                y0 += span;
            } else {
                y0 += 16.0f;
            }
            i += 1;
            v += 0x20;
        } while (i < count + 2);
    }
}
