// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Camera height easing (CAMERA_AREA11_SPECIALS.md "fixed eye"). With
// d = (target + cam+0x98) - cam+0x14 and a = |d| (func_0011DF78 fabsf): when
// d > 0 and a exceeds up (and bit 7 of cam+7 is clear) the height rises; when
// d <= 0 and a exceeds down (and bit 6 of cam+7 is clear) it falls. Beyond 1.0
// it moves by min(a / 10, 3.0); within 1.0 it takes a fifth of d.
extern float func_0011DF78(float x);

void func_00192010(unsigned char *cam, float target, float down, float up) {
    float d;
    float a;
    float step;

    target += *(float *)(cam + 0x98);
    d = target - *(float *)(cam + 0x14);
    a = func_0011DF78(d);

    if (!(d <= 0.0f)) {
        if (!(a <= up) && !(cam[7] & 0x80)) {
            if (!(a <= 1.0f)) {
                step = a / 10.0f;
                step = (3.0f > step) ? step : 3.0f;
                *(float *)(cam + 0x14) += step;
            } else {
                *(float *)(cam + 0x14) = *(float *)(cam + 0x14) + d / 5.0f;
            }
        }
    } else if (!(a <= down) && !(cam[7] & 0x40)) {
        if (!(a <= 1.0f)) {
            step = a / 10.0f;
            step = (3.0f > step) ? step : 3.0f;
            *(float *)(cam + 0x14) -= step;
        } else {
            *(float *)(cam + 0x14) = *(float *)(cam + 0x14) + d / 5.0f;
        }
    }
}
