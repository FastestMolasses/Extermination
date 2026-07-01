// NEARMISS func_00199770  (vram 0x00199770, 0x250 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.39% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FPU register-coloring permutation in the distance sqrt inputs and the two-coordinate scale/offset block (f-register assignment differs, e.g. ft0f vs ft2) + one arg-move order at the first func_00102948. Body/structure correct; 97.39% on mwcc 2.3.3. Permuter (FP-coloring) territory.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

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
    float dx;
    float dy;
    float dz;
    float dist;
    float t;

    func_00102948(D_700038A0, (void *)(*(int *)(arg0 + 0x20) + 0xA0));
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
                        *(float *)0x700038D0 = q0 / 16.0f - 2048.0f;
                        *(float *)0x700038D4 = q4 / 16.0f - 2048.0f;
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
