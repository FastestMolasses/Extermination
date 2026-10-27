// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 97.39% NEARMISS).
// Fixes read from the original instructions:
// - The first call's source pointer is spelled as a char pointer read from
//   arg0+0x20 plus 0xA0 (not an int sum cast to void *); that restores the
//   original argument setup order (destination first).
// - The distance deltas are declared in the order dz, dy, dx, which gives the
//   original FPU register assignment for the squared-length sum.
// - The screen-space scale block works through locals: both coordinates are
//   multiplied by t first (Y read before X), then each is divided by 16 into its
//   local and stored minus 2048; Y is then scaled by 1.5 from the stored value.
//
// Walks the D_00275B90 entry list (D_00275B98 entries). For each entry whose
// first byte is set, func_00183B80 accepts and field 0x34 is non-zero, it fetches
// the entry position, and if it lies within 360 units of arg0's position
// (offset 0xA0) it transforms it by the matrix at D_70003AC0, divides by w,
// recentres on 2048 (Y scaled by 1.5) and, when both magnitudes are inside
// 180 / 90, calls func_001DD170 with the integer distance.

extern void func_00102948(void *dst, void *src);
extern void func_001026A0(void *a, void *b, void *c);
extern int func_00183B80(unsigned char *p);
extern void func_00183C40(unsigned char *p, void *out);
extern float func_0011E748(float x);
extern float func_0011DF78(float x);
extern int float_to_int(float x);
extern int func_001DD170(int a, void *b, int c, int d, int e);
extern unsigned char **D_00275B90;
extern short D_00275B98;
extern char D_700038A0[];
extern char D_700038B0[];
extern char D_700038D0[];
extern char D_70003AC0[];

void func_00199770(char *arg0) {
    int n;
    unsigned char **pp;
    unsigned char *e;
    float dz;
    float dy;
    float dx;
    float dist;
    float t;

    func_00102948(D_700038A0, *(char **)(arg0 + 0x20) + 0xA0);
    n = D_00275B98;
    pp = D_00275B90;
    if (n != 0) {
        do {
            e = *pp;
            n -= 1;
            pp += 1;
            if (*(unsigned char *)e != 0 && func_00183B80(e) != 0 && *(short *)(e + 0x34) != 0) {
                func_00183C40(e, D_700038B0);
                dx = *(float *)(arg0 + 0xA0) - *(float *)0x700038B0;
                dy = *(float *)(arg0 + 0xA4) - *(float *)0x700038B4;
                dz = *(float *)(arg0 + 0xA8) - *(float *)0x700038B8;
                dist = func_0011E748(dx * dx + dy * dy + dz * dz);
                if (dist < 360.0f) {
                    func_00102948(D_700038D0, D_700038B0);
                    *(float *)0x700038DC = 1.0f;
                    func_001026A0(D_700038D0, D_70003AC0, D_700038D0);
                    t = 16.0f / *(float *)0x700038DC;
                    if (!(t < 0.0f)) {
                        float q4 = *(float *)0x700038D4 * t;
                        float q0 = *(float *)0x700038D0 * t;
                        q0 = q0 / 16.0f;
                        *(float *)0x700038D0 = q0 - 2048.0f;
                        q4 = q4 / 16.0f;
                        *(float *)0x700038D4 = q4 - 2048.0f;
                        *(float *)0x700038D4 = 1.5f * *(float *)0x700038D4;
                        if (func_0011DF78(*(float *)0x700038D0) < 180.0f && func_0011DF78(*(float *)0x700038D4) < 90.0f) {
                            func_001DD170(1, D_700038B0, 1, 0x80808080, float_to_int(dist));
                        }
                    }
                }
            }
        } while (n != 0);
    }
}
