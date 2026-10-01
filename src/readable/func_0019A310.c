// NEARMISS func_0019A310  (vram 0x0019A310, 0x130 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 88.71% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// Scheduling of the scratchpad vector (same artifact as func_00177460): the original reads x back
// before storing z and squares z from its register; the remaining differences follow from that.
//
// The function links from the asm body in src/func_0019A310.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Slope of the current stage record's surface normal (record 0x700031D0,
// normal at +0x24/+0x28/+0x2C, copied to the scratchpad 0x70003680..88).
// The horizontal length h = sqrtf(nx*nx + nz*nz) (func_0011E748) goes to
// 0x70003680; the ratio |ny| / h (func_0011DF78 fabsf; 3.4e38 when h is below
// 0.0001) to 0x70003684. *angle = pi/2 - atanf(ratio) (func_0011DBB8), negated
// for a negative ratio. Returns 1, or 0 when there is no stage record.
extern float func_0011E748(float x);
extern float func_0011DF78(float x);
extern float func_0011DBB8(float x);

int func_0019A310(float *angle) {
    char *rec = *(char **)0x700031D0;
    float h;
    float t;

    if (rec != 0) {
        *(volatile float *)0x70003680 = *(float *)(rec + 0x24);
        *(volatile float *)0x70003684 = *(float *)(rec + 0x28);
        *(volatile float *)0x70003688 = *(float *)(rec + 0x2C);
        h = func_0011E748(*(volatile float *)0x70003680 * *(volatile float *)0x70003680
                          + *(volatile float *)0x70003688 * *(volatile float *)0x70003688);
        *(volatile float *)0x70003680 = h;
        if (h < 0.0001f) {
            *(volatile float *)0x70003684 = 3.4e38f;
        } else {
            *(volatile float *)0x70003684 = func_0011DF78(*(volatile float *)0x70003684) / *(volatile float *)0x70003680;
        }
        t = *(volatile float *)0x70003684;
        if (t < 0.0f) {
            *angle = -(1.5707964f - func_0011DBB8(t));
        } else {
            *angle = 1.5707964f - func_0011DBB8(t);
        }
        return 1;
    }
    return 0;
}
