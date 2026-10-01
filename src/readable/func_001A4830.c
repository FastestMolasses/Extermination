// NEARMISS func_001A4830  (vram 0x001A4830, 0x4E0 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 96.63% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// Scratchpad access spelling: with the segment inputs (0x70003190..0x700031A8) as // SPAD symbols
// and the hit outputs as literal addresses the control flow, both hit tests and the duplicated
// publication line up; what remains is one FPU register pair in the range tests, a lui hoisted
// into one branch slot, and the reload order of the published hit x / z (all-literal, all-symbol
// and mixed spellings measured).
//
// The function links from the asm body in src/func_001A4830.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// SPAD: 0x70003190 0x70003198 0x700031A0 0x700031A4 0x700031A8
extern float D_70003190;
extern float D_70003198;
extern float D_700031A0;
extern float D_700031A4;
extern float D_700031A8;
extern void func_001028E8(float *dst, float *a, float *b); /* VU0 lane multiply */
extern float func_0011E748(float);                          /* sqrtf */

int func_001A4830(short *rec) {
    float *c = (float *)(rec + 2);
    float hh;
    float r2;
    float ox;
    float oz;
    float t;
    float h;
    float x0;
    float x;
    float z0;
    float z;
    float px;
    float pz;
    float d[4];
    float v[4];

    if (rec[0] & 0x8000) {
        hh = c[3];
    } else {
        hh = c[4];
    }
    if (!(c[1] - hh <= D_700031A4) || c[1] + hh < D_700031A4) return 0;

    r2 = c[3] * c[3];
    px = D_70003190;
    pz = D_70003198;
    d[0] = d[2] = D_700031A0 - px;
    d[1] = d[3] = D_700031A8 - pz;
    ox = px - c[0];
    oz = pz - c[2];
    v[0] = d[0];
    v[1] = d[1];
    v[2] = ox;
    v[3] = oz;
    func_001028E8(v, v, d);
    t = -(v[2] + v[3]) / (v[0] + v[1]);
    ox += d[0] * t;
    oz += d[1] * t;
    if (r2 < ox * ox + oz * oz) return 0;

    v[2] = D_70003190 - c[0];
    v[3] = D_70003198 - c[2];
    v[3] = v[2] * v[2] + v[3] * v[3];
    if (v[3] < r2) return 0;

    h = func_0011E748(r2 - (ox * ox + oz * oz));
    v[2] = func_0011E748(d[0] * d[0] + d[1] * d[1]);
    v[0] = d[0] / v[2];
    v[1] = d[1] / v[2];

    v[2] = ox + c[0] + v[0] * -h;
    v[3] = oz + c[2] + v[1] * -h;
    x0 = D_70003190;
    x = v[2];
    if ((x0 < x && !(D_700031A0 <= x)) || (!(x0 <= x) && D_700031A0 < x)
        || ((z0 = D_70003198, z = v[3], z0 < z) && !(D_700031A8 <= z))
        || (!(z0 <= z) && D_700031A8 < z)) {
        *(float *)0x700031B0 = x;
        *(float *)0x700031B4 = D_700031A4;
        *(float *)0x700031B8 = v[3];
        *(float *)0x700030D4 = (*(float *)0x700031B0 - c[0]) / c[3];
        *(int *)0x700030D8 = 0;
        *(float *)0x700030DC = (*(float *)0x700031B8 - c[2]) / c[3];
        *(short *)0x700030CA = 0x2000;
        return 1;
    }
    v[2] = ox + c[0] + v[0] * h;
    v[3] = oz + c[2] + v[1] * h;
    x = v[2];
    if ((x0 < x && !(D_700031A0 <= x)) || (!(x0 <= x) && D_700031A0 < x)
        || ((z = v[3], z0 < z) && !(D_700031A8 <= z))
        || (!(z0 <= z) && D_700031A8 < z)) {
        *(float *)0x700031B0 = x;
        *(float *)0x700031B4 = D_700031A4;
        *(float *)0x700031B8 = v[3];
        *(float *)0x700030D4 = (*(float *)0x700031B0 - c[0]) / c[3];
        *(int *)0x700030D8 = 0;
        *(float *)0x700030DC = (*(float *)0x700031B8 - c[2]) / c[3];
        *(short *)0x700030CA = 0x2000;
        return 1;
    }
    return 0;
}
