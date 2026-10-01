// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA21 overlay, runtime 0x00829310 (splat/link name 008292D0; overlay code
//  is linked 0x40 below where it runs), 0x1B0 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA21; lane A03C).
// Covers the splat pieces 008292D0, 00829310 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: called from 0x828700 / 0x8297C0 with (value, key). n =
//  func_001B5DC0(key): nonzero moves *value by 0.001 * n towards +-table
//  0x82DB10[n] (sign by key & 0xFF >= 0x80) without passing it; zero decays
//  |*value| by 0.01 to 0.
extern float D_overlay_AREA21_0082DB10[];
extern int func_001B5DC0(int key);
extern float func_0011DF78(float a);

void func_overlay_AREA21_008292D0(float *v, int key) {
    int n = func_001B5DC0(key);
    float step;
    float lim;
    float *t;
    float d;
    if (n != 0) {
        step = 0.001f * (float)n;
        if ((key & 0xFF) >= 0x80) {
            t = &D_overlay_AREA21_0082DB10[n];
            lim = *t;
            if (*v > lim) {
                *v -= step;
                if (*v < lim) {
                    *v = *t;
                }
            } else {
                *v += step;
                if (*v > lim) {
                    *v = *t;
                }
            }
        } else {
            t = &D_overlay_AREA21_0082DB10[n];
            lim = -*t;
            if (*v > lim) {
                *v -= step;
                if (*v < lim) {
                    *v = -*t;
                }
            } else {
                *v += step;
                if (*v > lim) {
                    *v = -*t;
                }
            }
        }
    } else if (*v) {
        d = func_0011DF78(*v);
        d -= 0.01f;
        if (d <= 0.0f) {
            *v = 0.0f;
        } else if (*v < 0.0f) {
            *v = -d;
        } else {
            *v = d;
        }
    }
}
