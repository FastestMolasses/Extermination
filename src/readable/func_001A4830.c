// NEARMISS func_001A4830  (vram 0x001A4830, 0x4E0 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 99.87% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// one FPU register pair in each publication block (the original keeps the c[0] difference in
// $f2 and the reloaded hit z in $f1; this C swaps them). Matching wave 2026-10-28 fixed the rest:
// the range tests read the segment bounds and v[2] / v[3] directly (no x0/x/z0/z temporaries,
// which fixed the range-test register pair), and the hit x / z (0x700031B0 / 0x700031B8) are
// volatile // SPAD symbols, stored and then re-read into temporaries before the normal is
// written (fixes the lui hoisted into the first hit branch slot and the reload order).
// Decl orders, per-block temporaries, difference-first spellings, mwcc24 and a 500 s permuter
// run (5.8k iterations, no improvement) did not move the last pair.
//
// The function links from the asm body in src/func_001A4830.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// SPAD: 0x70003190 0x70003198 0x700031A0 0x700031A4 0x700031A8 0x700031B0 0x700031B8
extern float D_70003190;
extern float D_70003198;
extern float D_700031A0;
extern float D_700031A4;
extern float D_700031A8;
extern volatile float D_700031B0;
extern volatile float D_700031B8;
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
    if ((D_70003190 < v[2] && !(D_700031A0 <= v[2])) || (!(D_70003190 <= v[2]) && D_700031A0 < v[2])
        || (D_70003198 < v[3] && !(D_700031A8 <= v[3]))
        || (!(D_70003198 <= v[3]) && D_700031A8 < v[3])) {
        D_700031B0 = v[2];
        *(float *)0x700031B4 = D_700031A4;
        D_700031B8 = v[3];
        {
            float hz;
            float hx = D_700031B0;   /* re-read the published hit point */
            hz = D_700031B8;
            *(float *)0x700030D4 = (hx - c[0]) / c[3];
            *(int *)0x700030D8 = 0;
            *(float *)0x700030DC = (hz - c[2]) / c[3];
        }
        *(short *)0x700030CA = 0x2000;
        return 1;
    }
    v[2] = ox + c[0] + v[0] * h;
    v[3] = oz + c[2] + v[1] * h;
    if ((D_70003190 < v[2] && !(D_700031A0 <= v[2])) || (!(D_70003190 <= v[2]) && D_700031A0 < v[2])
        || (D_70003198 < v[3] && !(D_700031A8 <= v[3]))
        || (!(D_70003198 <= v[3]) && D_700031A8 < v[3])) {
        D_700031B0 = v[2];
        *(float *)0x700031B4 = D_700031A4;
        D_700031B8 = v[3];
        {
            float hz;
            float hx = D_700031B0;   /* re-read the published hit point */
            hz = D_700031B8;
            *(float *)0x700030D4 = (hx - c[0]) / c[3];
            *(int *)0x700030D8 = 0;
            *(float *)0x700030DC = (hz - c[2]) / c[3];
        }
        *(short *)0x700030CA = 0x2000;
        return 1;
    }
    return 0;
}
