// NEARMISS func_0018C0D0  (vram 0x0018C0D0, 0x3D8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 82.52% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// NEARMISS 82.52% (233) / 76.11% (991202). Body/logic fully recovered: two mula.s/madd.s squared-2D-length checks (x*x+z*z via func_0011E748, floor-clamped to 0.001), a camera-target scratch build with an arg1/state-byte dispatch (four arms: copy-through via func_001031E0, or D_008105D0 + k*offset ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_00102760(void *a0, void *a1);
extern void func_00102798(void *a0, void *a1);
extern void func_001028D0(void *a0, void *a1, void *a2);
extern void func_00102948(void *a0, void *a1);
extern void func_00102CD0(void *a0, void *a1, void *a2, void *a3);
extern void func_001031E0(float *a0, float *a1);
extern float func_0011DF78(float a);
extern float func_0011E620(float a, float b);
extern float func_0011E748(float a);
extern float func_001B1240(void *origin, float x, float z);

extern float D_00810354;
extern float D_008105D0;
extern float D_008105D4;
extern float D_008105D8;
extern int D_008105DC;
extern float D_008105E0;
extern float D_008105E8;
extern int D_008105EC;
extern char D_008105F0[];
extern char D_00810600[];
extern char D_00810610[];
extern char D_00810650[];
extern float D_00810690;
extern float D_00810694;
extern float D_00810698;
extern float D_0081069C;
extern float D_008106A0;
extern float D_700038A0;
extern float D_700038B0;
extern float D_700038C0;

void func_0018C0D0(unsigned char *arg0, int arg1) {
    float d;

    D_008105EC = 0x3F800000;
    D_008105DC = 0x3F800000;

    func_001028D0(&D_700038A0, &D_008105E0, &D_008105D0);
    *(int *)0x700038AC = 0x3F800000;
    {
        float x = *(float *)0x700038A0;
        float z = *(float *)0x700038A8;
        d = func_0011E748(z * z + x * x);
    }
    D_0081069C = d;
    if (d < 0.001f) {
        D_0081069C = 0.001f;
        *(float *)0x700038A8 = 0.001f;
        D_008105D8 = D_008105D8 + 0.001f;
    }

    D_00810698 = D_008105D4 - D_00810354;

    func_001028D0(&D_700038B0, arg0 + 0x20, arg0 + 0x10);
    {
        float x = *(float *)0x700038B0;
        float z = *(float *)0x700038B8;
        d = func_0011E748(z * z + x * x);
    }
    D_00810690 = d;
    if (d < 0.001f) {
        D_00810690 = 0.001f;
        *(float *)0x700038B8 = 0.001f;
        *(float *)(arg0 + 0x18) = *(float *)(arg0 + 0x18) + 0.001f;
    }

    D_00810694 = func_0011DF78(*(float *)0x700038B4);

    func_00102760(&D_700038A0, &D_700038A0);

    if (arg1 == 0) {
        if (*(unsigned char *)(arg0 + 4) == 3) {
            func_001031E0(&D_700038C0, &D_008105D0);
        } else if (*(unsigned char *)(arg0 + 6) == 2 || *(unsigned char *)(arg0 + 6) == 1) {
            float k = 4.0f;
            *(float *)0x700038C0 = D_008105D0 + k * *(float *)0x700038A0;
            *(float *)0x700038C4 = D_008105D4 + k * *(float *)0x700038A4;
            *(float *)0x700038C8 = D_008105D8 + k * *(float *)0x700038A8;
        } else {
            func_001031E0(&D_700038C0, &D_008105D0);
        }
    } else {
        if (*(unsigned char *)(arg0 + 6) == 0xA) {
            float k = -1.0f;
            *(float *)0x700038C0 = D_008105D0 + k * *(float *)0x700038A0;
            *(float *)0x700038C4 = D_008105D4 + k * *(float *)0x700038A4;
            *(float *)0x700038C8 = D_008105D8 + k * *(float *)0x700038A8;
        } else {
            float k = 4.0f;
            *(float *)0x700038C0 = D_008105D0 + k * *(float *)0x700038A0;
            *(float *)0x700038C4 = D_008105D4 + k * *(float *)0x700038A4;
            *(float *)0x700038C8 = D_008105D8 + k * *(float *)0x700038A8;
        }
    }

    func_00102CD0(&D_00810610, &D_700038C0, &D_700038A0, &D_008105F0);
    func_00102948(arg0 + 0xB0, &D_700038A0);
    func_00102798(&D_00810650, &D_00810610);
    D_008106A0 = func_0011E620(-*(float *)0x700038A8, *(float *)0x700038A0);
    func_00102948(&D_00810600, &D_700038A0);
    *(float *)(arg0 + 0x9C) = func_001B1240(&D_008105D0, D_008105E0, D_008105E8);
}
