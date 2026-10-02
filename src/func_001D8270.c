// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Actor-lighting fold gate (port ACTOR_LIGHTING.md: em_lighting_fold_gate).
// Returns 0 for the object types (byte +3) 3, 8, 9, 0x0B, 0x0D, 0x15, 0x16,
// 0x17, 0x3D and 0x3E; otherwise 1 when the float at (obj+0x44)->+0x20 is
// below 30.0, else 0.
int func_001D8270(unsigned char *obj) {
    switch (obj[3]) {
    case 0x03:
    case 0x08:
    case 0x09:
    case 0x0B:
    case 0x0D:
    case 0x15:
    case 0x16:
    case 0x17:
    case 0x3D:
    case 0x3E:
        return 0;
    default:
        /* compiled as !(d < 30.0f): a NaN distance also returns 0 */
        if (*(float *)(*(char **)(obj + 0x44) + 0x20) >= 30.0f) {
            return 0;
        }
        return 1;
    }
}
