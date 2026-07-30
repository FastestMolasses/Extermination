// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// SEMANTICS: per-enemy-type camera/aim setup. Dispatches on self[3] (the type
// byte, 12-entry jump table; types 0, 2, 8 and anything >= 12 fall straight
// through with `scale` and `idx` left uninitialised, exactly as the target does).
// Each case writes a 4-float vector into EE scratchpad at 0x70003600 (w = 1.0f)
// and picks a bone/slot index; several cases additionally branch on bit 0 and
// bit 7 of the flag byte self[0xD]. After the switch the vector is transformed
// by func_001026A0 against ((char **)D_00275B40)[idx] + 0x90, a 4-int box is
// written to scratchpad 0x70003610 (again keyed on flag bit 7), and
// func_001F4E20 is called with the per-type `scale`.

extern void func_00102948(void *dst, void *src);
extern void func_001026A0(float *quat, void *node, float *out);
extern void func_001F4E20(float *quat, int *box, float scale);

extern float D_70003600[4];
extern int D_70003610[4];
extern char **D_00275B40;

void func_001B4810(unsigned char *self) {
    float *q = (float *)0x70003600;
    int *box = (int *)0x70003610;
    float scale;
    int idx;

    switch (self[3]) {
    case 1:
        scale = 5.0f;
        idx = 4;
        if (self[0xD] & 1) {
            if (self[0xD] & 0x80) {
                q[0] = 2.142f;
                q[1] = 1.968f;
                q[2] = -0.059f;
                q[3] = 1.0f;
            } else {
                q[0] = 2.152f;
                q[1] = 1.954f;
                q[2] = -0.049f;
                q[3] = 1.0f;
            }
        } else if (self[0xD] & 0x80) {
            q[0] = 2.177f;
            q[1] = 1.896f;
            q[2] = 0.0f;
            q[3] = 1.0f;
        } else {
            q[0] = 2.06f;
            q[1] = 2.109f;
            q[2] = 0.0f;
            q[3] = 1.0f;
        }
        break;
    case 3:
        scale = 8.0f;
        q[0] = 1.089f;
        q[1] = 0.816f;
        q[2] = 0.0f;
        q[3] = 1.0f;
        idx = 2;
        func_00102948(D_70003610, self + 0xC0);
        break;
    case 4:
        scale = 5.0f;
        q[0] = 0.335f;
        q[1] = 0.569f;
        q[2] = 0.0f;
        idx = 2;
        q[3] = 1.0f;
        break;
    case 5:
        scale = 5.0f;
        q[0] = 2.389f;
        q[1] = -0.655f;
        q[2] = 0.0f;
        idx = 13;
        q[3] = 1.0f;
        break;
    case 6:
        scale = 5.0f;
        idx = 5;
        if (self[0xD] & 0x80) {
            q[0] = -0.318f;
            q[1] = -2.732f;
            q[2] = 0.0f;
            q[3] = 1.0f;
        } else {
            q[0] = 0.0f;
            q[1] = -2.025f;
            q[2] = 0.0f;
            q[3] = 1.0f;
        }
        break;
    case 7:
        scale = 5.0f;
        idx = 11;
        if (self[0xD] & 0x80) {
            q[0] = 3.02f;
            q[1] = 0.44f;
            q[2] = 0.0f;
            q[3] = 1.0f;
        } else {
            q[0] = 2.027f;
            q[1] = 0.0f;
            q[2] = 0.0f;
            q[3] = 1.0f;
        }
        break;
    case 9:
        scale = 5.0f;
        q[0] = 2.4293f;
        q[1] = -1.9016f;
        q[2] = -0.1725f;
        idx = 28;
        q[3] = 1.0f;
        break;
    case 10:
        scale = 20.0f;
        q[0] = 29.812f;
        q[1] = -0.935f;
        q[2] = 0.0f;
        idx = 16;
        q[3] = 1.0f;
        break;
    case 11:
        scale = 20.0f;
        q[0] = 11.549f;
        q[1] = 16.735f;
        q[2] = -0.008f;
        idx = 2;
        q[3] = 1.0f;
        break;
    }

    func_001026A0(D_70003600, D_00275B40[idx] + 0x90, D_70003600);
    if (self[0xD] & 0x80) {
        box[0] = 0x80;
        box[1] = 0x50;
        box[2] = 0x30;
        box[3] = 0x80;
    } else {
        box[0] = 0x30;
        box[1] = 0x80;
        box[2] = 0x30;
        box[3] = 0x80;
    }
    func_001F4E20(D_70003600, D_70003610, scale);
}
