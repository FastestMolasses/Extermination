// NEARMISS func_001373B0  (vram 0x001373B0, 0x47C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.06% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). Body now
// faithful (matching wave 2026-10-27, was 93.64%): the state dispatch is a switch with break
// cases; state 3 passes the object itself to func_001AFC10; the near-check (<= 15) tests the
// FIRST distance while the second distance is only stored to 0x70003A20 and re-read for the
// limit test; position integration is compound `+=`. Per-frame recon/pursuit-check state
// machine (states 0-3) with float easing (func_001B12B0), atan2/sin/cos steering and a
// los/probe test (func_0019A570) followed by build_trs_matrix.
// Remaining differences: state 2's constant 2 (shared with the dispatch compare) is coloured
// into the third argument register instead of the fourth, so its byte store lands before the
// third-argument setup rather than in the call's delay slot. Pure register colouring; source
// variants (types, decl order, temps, prototypes) and 540 s of the 2.3.3 permuter (stuck at
// its base score) did not move it.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void build_trs_matrix(void *mtx, void *pos, void *rot, void *scale);
extern void func_00102948(void *dst, void *src, ...);
extern int func_001031E0(void *a, void *b);
extern void func_00103230(void *a, void *b, float v);
extern float func_0011DE90(float a);          /* cos */
extern float func_0011E2A8(float a);          /* sin */
extern float func_0011E620(float a, float b); /* atan2 */
extern int func_00122BB8(void);
extern int func_0019A570(void *a, void *b, int c, int d);
extern void func_001AFC10(unsigned char *handle);
extern float func_001B1240(char *origin, float x, float z);
extern float func_001B12B0(float goal, float cur, float rate);
extern float func_001B1470(float a);
extern float func_001B15D0(void *a, void *b);
extern void func_001EFD90(int id, void *a, void *b);
extern int func_001EFE00(int a, void *p);
extern int func_001FBD50(void *self, int a, int b, float f12);

extern char D_700031B0[];
extern char D_700038A0[];
extern char D_700038B0[];

void func_001373B0(char *arg0) {
    char *s0;
    int st;
    float f20;
    float f21;

    st = *(unsigned char *)(arg0 + 4);
    s0 = arg0 + 0x1F0;

    switch (st) {
    case 0:
    *(unsigned char *)(arg0 + 4) = st + 1;
    *(int *)(s0 + 0x10) = 0;
    func_00103230(arg0 + 0xC0, arg0 + 0xC0, 4.0f);
    *(int *)(arg0 + 0x20) = func_001EFE00(0x80000037, arg0);
    *(float *)(arg0 + 0xA0) = *(float *)(arg0 + 0xB0);
    *(float *)(arg0 + 0xA4) = *(float *)(arg0 + 0xB4);
    *(float *)(arg0 + 0xA8) = *(float *)(arg0 + 0xB8);
    *(float *)(arg0 + 0xAC) = *(float *)(arg0 + 0xBC);
    *(float *)(s0 + 4) = *(float *)(s0 + 4) +
        15.0f * ((float) ((func_00122BB8() >> 0x10) & 0xFF) / 255.0f);
        break;
    case 1:
    *(int *)(s0 + 0x10) = *(int *)(s0 + 0x10) + 1;
    func_00102948(D_700038A0, s0, 2);
    func_00102948(D_700038B0, arg0 + 0xB0);
    f21 = *(float *)0x700038A4 - *(float *)0x700038B4;
    *(float *)0x700038A4 = 0.0f;
    *(float *)0x700038B4 = 0.0f;
    f20 = func_001B15D0(D_700038A0, D_700038B0);

    if ((*(int *)(s0 + 0x10) >= 0xA) && (*(int *)(s0 + 0x10) < 0x12D) &&
        !(*(unsigned char *)(arg0 + 0xD) & 1)) {
        *(float *)0x70003A20 = func_001B1240(arg0 + 0xB0, *(float *)(s0 + 0), *(float *)(s0 + 8));
        *(float *)(s0 + 0x24) = func_001B12B0(*(float *)0x70003A20, *(float *)(s0 + 0x24), 0.012217305f);
        *(float *)0x70003A20 = func_001B1470(func_0011E620(f21, f20));
        *(float *)(s0 + 0x28) = func_001B12B0(*(float *)0x70003A20, *(float *)(s0 + 0x28), 0.012217305f);
    }

    *(float *)(arg0 + 0xC0) = 2.2f * func_0011E2A8(*(float *)(s0 + 0x24));
    *(float *)(arg0 + 0xC4) = 2.2f * func_0011E2A8(*(float *)(s0 + 0x28));
    *(float *)(arg0 + 0xC8) = 2.2f * func_0011DE90(*(float *)(s0 + 0x24));
    *(float *)(arg0 + 0xB0) += *(float *)(arg0 + 0xC0);
    *(float *)(arg0 + 0xB4) += *(float *)(arg0 + 0xC4);
    *(float *)(arg0 + 0xB8) += *(float *)(arg0 + 0xC8);

    func_00102948(D_700038A0, arg0 + 0xB0);
    *(float *)0x700038A4 = 0.0f;
    *(float *)0x700038B0 = 340.0f;
    *(float *)0x700038B4 = 0.0f;
    *(float *)0x700038B8 = 270.0f;
    *(int *)0x700038BC = 0x3F800000;
    *(float *)0x70003A20 = func_001B15D0(D_700038A0, D_700038B0);

    if (*(unsigned char *)(arg0 + 0xD) & 2) {
        *(float *)0x70003A24 = 269.0f;
    } else if ((*(int *)(s0 + 0x10) >= 0x14) && (f20 <= 15.0f)) {
        *(unsigned char *)(arg0 + 4) = 2;
        return;
    } else {
        *(float *)0x70003A24 = 190.0f;
    }

    if (*(float *)0x70003A20 >= *(float *)0x70003A24) {
        *(unsigned char *)(arg0 + 4) = 2;
        return;
    }

    if (func_0019A570(arg0 + 0xA0, arg0 + 0xB0, 7, 0x20) != 0) {
        func_001031E0(arg0 + 0xB0, D_700031B0);
        *(unsigned char *)(arg0 + 4) = 2;
    }

    *(float *)(arg0 + 0xA0) = *(float *)(arg0 + 0xB0);
    *(float *)(arg0 + 0xA4) = *(float *)(arg0 + 0xB4);
    *(float *)(arg0 + 0xA8) = *(float *)(arg0 + 0xB8);
    *(float *)(arg0 + 0xAC) = *(float *)(arg0 + 0xBC);
    *(int *)0x700038A0 = 0;
    *(float *)0x700038A4 = 0.0f;
    *(int *)0x700038A8 = 0;
    *(int *)0x700038AC = 0x3F800000;
    build_trs_matrix(arg0 + 0xD0, arg0 + 0xB0, D_700038A0, D_700038A0);
        break;
    case 2:
    *(unsigned char *)(arg0 + 4) = st + 1;
    *(char *)(*(int *)(arg0 + 0x20) + 4) = 2;
    func_001EFD90(0x8000006B, arg0 + 0xB0, arg0 + 0xC0);
    func_001FBD50(arg0, ((func_00122BB8() >> 0x12) & 1) + 0x449, 0, 450.0f);
        break;
    case 3:
        func_001AFC10((unsigned char *)arg0);
        break;
    }
}
