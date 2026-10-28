// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x70003680 0x70003684 0x70003688

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an 88.71% NEARMISS
// companion). Fixes read from the original instructions:
//  - The three scratchpad words are relocated externs (idiom-32, opted in with
//    // SPAD:), one extern per address, as in func_00177460. With the named
//    externs mwcc reloads x after the stores and squares z from its register,
//    matching the original's scheduling; every other difference followed from
//    that, so the body is otherwise unchanged.

// Slope of the current stage record's surface normal (record 0x700031D0,
// normal at +0x24/+0x28/+0x2C, copied to the scratchpad 0x70003680..88).
// The horizontal length h = sqrtf(nx*nx + nz*nz) (func_0011E748) goes to
// 0x70003680; the ratio |ny| / h (func_0011DF78 fabsf; 3.4e38 when h is below
// 0.0001) to 0x70003684. *angle = pi/2 - atanf(ratio) (func_0011DBB8), negated
// for a negative ratio. Returns 1, or 0 when there is no stage record.
extern float D_70003680;
extern float D_70003684;
extern float D_70003688;
extern float func_0011E748(float x);
extern float func_0011DF78(float x);
extern float func_0011DBB8(float x);

int func_0019A310(float *angle) {
    char *rec = *(char **)0x700031D0;
    float h;
    float t;

    if (rec != 0) {
        D_70003680 = *(float *)(rec + 0x24);
        D_70003684 = *(float *)(rec + 0x28);
        D_70003688 = *(float *)(rec + 0x2C);
        h = func_0011E748(D_70003680 * D_70003680 + D_70003688 * D_70003688);
        D_70003680 = h;
        if (h < 0.0001f) {
            D_70003684 = 3.4e38f;
        } else {
            D_70003684 = func_0011DF78(D_70003684) / D_70003680;
        }
        t = D_70003684;
        if (t < 0.0f) {
            *angle = -(1.5707964f - func_0011DBB8(t));
        } else {
            *angle = 1.5707964f - func_0011DBB8(t);
        }
        return 1;
    }
    return 0;
}
