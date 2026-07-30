// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// SEMANTICS: cutscene/camera-move command tick. Dispatches on the command kind
// *(int *)(cmd + 8) via an 11-entry jump table (0..10; out of range -> return 0).
// Kinds 0/1/9/10 are one-shot (snap position / push camera / spin / snap both
// vectors) and return 1. Kinds 2/4/6/7 and 3/5 and 8 run a multi-phase move
// driven by the sub-state byte cam[4]: phase 0 latches the start vector and
// zeroes the timer at cmd+0x10, later phases advance the timer against the
// duration at cmd+0xC and lerp with r = timer/duration. Kinds 6 and 7 ease that
// r through (1 + sin(pi*r - pi/2))/2. Kinds 3/5 first turn the camera yaw
// (func_001B1240/func_001B12B0, step 4 degrees = 0.06981317 rad) before lerping
// through the EE scratchpad at 0x70003600. Kind 8 orbits by 0.4 * cos/sin of the
// camera yaw at D_008102B0+0xC4. Returns 1 when the move has finished (or was
// one-shot), 0 while it is still running.

extern void func_00102948(void *dst, void *src);
extern void func_001028D0(void *dst, void *a, void *b);
extern float func_0011DE90(float);
extern float func_0011E2A8(float);
extern void func_00182F90(void *cam, void *pos);
extern float func_001B1240(void *cam, float x, float z);
extern float func_001B12B0(float target, float cur, float step);
extern void func_001B6F80(void *vec, float f);

extern short D_0024D8F0[];
extern unsigned char D_008102B0[];
extern unsigned char D_008105E0[];
extern float D_70003600[4];

int func_001B94F0(unsigned char *self, unsigned char *cam, unsigned char *cmd) {
    unsigned char *g = D_008102B0;
    float *spr = (float *)0x70003600;
    float t;
    float u;
    float r;

    switch (*(int *)(cmd + 8)) {
    case 0:
        func_00102948(self + 0xB0, cmd + 0x30);
        return 1;
    case 1:
        func_00182F90(g, cmd + 0x30);
        return 1;
    case 2:
    case 4:
    case 6:
    case 7:
        switch (cam[4]) {
        case 0:
            cam[4] = cam[4] + 1;
            func_00102948(cmd + 0x20, self + 0xB0);
            *(float *)(cmd + 0x10) = 0.0f;
            /* fallthrough */
        case 1:
            if (!(*(float *)(cmd + 0x10) < *(float *)(cmd + 0xC))) {
                return 1;
            }
            *(float *)(cmd + 0x10) += 1.0f;
            r = *(float *)(cmd + 0x10) / *(float *)(cmd + 0xC);
            if (*(int *)(cmd + 8) == 6 || *(int *)(cmd + 8) == 7) {
                r = (1.0f + func_0011E2A8(3.1415927f * r - 1.5707964f)) / 2.0f;
            }
            func_001028D0(self + 0xB0, cmd + 0x30, cmd + 0x20);
            *(float *)(self + 0xB0) = *(float *)(cmd + 0x20) + *(float *)(self + 0xB0) * r;
            *(float *)(self + 0xB4) = *(float *)(cmd + 0x24) + *(float *)(self + 0xB4) * r;
            *(float *)(self + 0xB8) = *(float *)(cmd + 0x28) + *(float *)(self + 0xB8) * r;
            if (*(int *)(cmd + 8) == 4 || *(int *)(cmd + 8) == 7) {
                func_00102948(D_008105E0, self + 0xB0);
            }
            break;
        }
        break;
    case 3:
    case 5:
        switch (cam[4]) {
        case 0:
            func_00102948(cmd + 0x20, g + 0xA0);
            cam[4] = cam[4] + 1;
            *(float *)(cmd + 0x10) = 0.0f;
            *(short *)(g + 0x1F2) = D_0024D8F0[*(int *)(cmd + 0x14)];
            g[0x25C] = cmd[0x14];
            *(float *)(g + 0x1F8) = 4.0f;
            break;
        case 1:
            t = func_001B1240(g + 0xA0, *(float *)(cmd + 0x30), *(float *)(cmd + 0x38));
            u = func_001B12B0(t, *(float *)(g + 0xC4), 0.06981317f);
            *(float *)(g + 0xC4) = u;
            if (u != t) {
                break;
            }
            cam[4] = cam[4] + 1;
            /* fallthrough */
        case 2:
            if (!(*(float *)(cmd + 0x10) < *(float *)(cmd + 0xC))) {
                *(short *)(g + 0x1F2) = D_0024D8F0[0];
                g[0x25C] = 0;
                *(float *)(g + 0x1F8) = 4.0f;
                return 1;
            }
            *(float *)(cmd + 0x10) += 1.0f;
            r = *(float *)(cmd + 0x10) / *(float *)(cmd + 0xC);
            func_001028D0(cam + 0x10, cmd + 0x30, cmd + 0x20);
            spr[0] = *(float *)(cmd + 0x20) + *(float *)(cam + 0x10) * r;
            spr[1] = *(float *)(cmd + 0x24) + *(float *)(cam + 0x14) * r;
            spr[2] = *(float *)(cmd + 0x28) + *(float *)(cam + 0x18) * r;
            func_00182F90(g, D_70003600);
            if (*(int *)(cmd + 8) == 5) {
                func_00102948(D_008105E0, g + 0xB0);
            }
            break;
        }
        break;
    case 8:
        switch (cam[4]) {
        case 0:
            func_00102948(cmd + 0x20, g + 0xA0);
            cam[4] = cam[4] + 1;
            *(float *)(cmd + 0x10) = 0.0f;
            *(short *)(g + 0x1F2) = D_0024D8F0[*(int *)(cmd + 0x14)];
            g[0x25C] = cmd[0x14];
            *(float *)(g + 0x1F8) = 4.0f;
            break;
        case 1:
            if (!(*(float *)(cmd + 0x10) < *(float *)(cmd + 0xC))) {
                *(short *)(g + 0x1F2) = D_0024D8F0[0];
                g[0x25C] = 0;
                *(float *)(g + 0x1F8) = 4.0f;
                return 1;
            }
            *(float *)(cmd + 0x10) += 1.0f;
            *(float *)(cmd + 0x20) =
                *(float *)(cmd + 0x20) + 0.4f * func_0011E2A8(*(float *)(g + 0xC4));
            *(float *)(cmd + 0x28) =
                *(float *)(cmd + 0x28) + 0.4f * func_0011DE90(*(float *)(g + 0xC4));
            func_00182F90(g, cmd + 0x20);
            break;
        }
        break;
    case 9:
        func_001B6F80(cmd + 0x20, *(float *)(cmd + 0x34));
        return 1;
    case 10:
        func_00102948(self + 0xB0, cmd + 0x20);
        func_00102948(self + 0xC0, cmd + 0x30);
        return 1;
    }
    return 0;
}
