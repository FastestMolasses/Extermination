// NEARMISS func_002082B0  (vram 0x002082B0, 0x49C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 90.79% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body/structure/frame fully recovered (all 9 saved regs match after applying the D_00275670-array reload idiom from func_001D3AD0.c; every GIFtag/vertex write and the mula.s/madd.s-equivalent dot-product loop matches instruction-for-instruction). Sole residual is a saved-register COLORING permutat...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// Builds a per-actor GS sprite-tile display-list record for a health/status
// bar-style strip. arg1 is a float rect {x0,y0,x1,y1} at +0/+4/+8/+0xC; the
// strip is subdivided into ceil(|(x1-y0)/16|) columns (count), the leftover
// fractional remainder is kept as frac for the last column's step. Two
// color-ramp accumulators (D_70003600/D_70003610) are seeded from
// arg1+0x40/+0x20 and +0x50/+0x30 (func_001028D0), stepped by
// func_00102850(rate=count+2), and read back once into the packed-byte
// color scratch D_70003620/D_70003630 (func_00102948) before the loop.
//
// The context object for actor arg0 is D_00275670[arg0]; its +0x10 field is
// a bump-allocated GS display-list cursor. A single 0x60-byte GIFtag/prim
// record is written at the cursor (NLOOP=(count+2)*2+4-1 packed with flags
// at +0x1C, PRIM/RGBAQ-ish qwords at +0x20/+0x28/+0x30/+0x38, a second tag
// word at +0x40/+0x48), the cursor is advanced by ((count+2)*2+4+1)*0x10,
// and the vertex loop (count+2 iterations) writes a packed color qword pair
// (float_to_int() of the ramp scratch) plus a UV/XYZ pair (float_to_int()
// of scaled trig terms) into two 16-byte vertex slots at var_s4, advancing
// the column position by either 16.0 units or the fractional remainder on
// the final column.
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
    float y0;
    float span;
    float frac;
    int count;
    int i;
    short nloop;
    int n;
    char *rec;
    char *v;
    int a, b, c, d;
    int s7a, s7b;
    char *elem;

    y0 = *(float *)(arg1 + 8);
    span = (*(float *)(arg1 + 0xC) - y0) / 16.0f;
    count = float_to_int(span);
    if (count < 0) {
        count = -count;
    }
    frac = (span - (float)float_to_int(span)) * 16.0f;

    func_001028D0(D_70003600, arg1 + 0x40, arg1 + 0x20);
    func_001028D0(D_70003610, arg1 + 0x50, arg1 + 0x30);
    func_00102850(D_70003600, D_70003600, (float)(count + 2));
    func_00102850(D_70003610, D_70003610, (float)(count + 2));
    func_00102948(D_70003620, arg1 + 0x20);
    func_00102948(D_70003630, arg1 + 0x30);

    n = count + 2;
    nloop = (short)(n * 2 + 4);
    elem = D_00275670 + (arg0 << 2);
    (*(char **)(elem + 0x10))[3] = (char)0x10;
    *(int *)(*(char **)(elem + 0x10) + 4) = 0;
    *(short *)(*(char **)(elem + 0x10) + 0) = nloop;
    i = 0;
    rec = *(char **)(elem + 0x10);
    *(char **)(elem + 0x10) = rec + ((nloop + 1) * 0x10);
    *(long long *)(rec + 0x10) = 0;
    *(long long *)(rec + 0x18) = 0;
    *(int *)(rec + 0x1C) = (int)(nloop - 1) | 0x50000000;
    *(long long *)(rec + 0x20) = 0x8001 | ((long long)0x10000000 << 32);
    *(long long *)(rec + 0x28) = 0xE;
    v = rec + 0x10 + 0x40;
    *(long long *)(rec + 0x30) = 0x14C;
    *(long long *)(rec + 0x38) = 0;
    *(long long *)(rec + 0x40) = (long long)n | ((long long)(0x8000 | (0x44000000 << 0)) << 32);
    *(long long *)(rec + 0x48) = 0x4141;

    if (n > 0) {
        do {
            float ang = (3.1415927f * y0) / 180.0f;
            a = float_to_int(*(float *)(arg1 + 0) + (0.8f * (16.0f * *(float *)(arg1 + 0x10) * func_0011E2A8(ang))));
            b = float_to_int(*(float *)(arg1 + 4) + (0.5f * (16.0f * *(float *)(arg1 + 0x14) * func_0011DE90(ang))));
            c = float_to_int(*(float *)(arg1 + 0) + (0.8f * (16.0f * *(float *)(arg1 + 0x18) * func_0011E2A8(ang))));
            d = float_to_int(*(float *)(arg1 + 4) + (0.5f * (16.0f * *(float *)(arg1 + 0x1C) * func_0011DE90(ang))));

            s7a = float_to_int(*(float *)0x70003620);
            s7a = s7a | (float_to_int(*(float *)0x70003624) << 8);
            s7a = (float_to_int(*(float *)0x70003628) << 0x10) | s7a;
            *(long long *)(v + 0) = (long long)((float_to_int(*(float *)0x7000362C) << 0x18) | s7a);

            s7b = float_to_int(*(float *)0x70003630);
            s7b = s7b | (float_to_int(*(float *)0x70003634) << 8);
            s7b = (float_to_int(*(float *)0x70003638) << 0x10) | s7b;
            *(long long *)(v + 0x10) = (long long)((float_to_int(*(float *)0x7000363C) << 0x18) | s7b);

            *(long long *)(v + 8) = (long long)(a | (b << 0x10)) | ((long long)0xFFFFFF << 32);
            *(long long *)(v + 0x18) = (long long)(c | (d << 0x10)) | ((long long)0xFFFFFF << 32);

            func_001028B8(D_70003620, D_70003620, D_70003600);
            func_001028B8(D_70003630, D_70003630, D_70003610);

            if (i == count) {
                y0 += frac;
            } else {
                y0 += 16.0f;
            }
            i += 1;
            v += 0x20;
        } while (i < count + 2);
    }
}
