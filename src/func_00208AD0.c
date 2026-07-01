// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// HUD circular gauge draw (fuel/O2-style 0-100 meter, value in D_00810858).
// Bumps the frame counter at +0x20, draws the centered label string
// (D_00267298, width via func_001CC170) with the white glyph set D_00265510,
// underlines it (func_00207F80, color 0x80CE6000), formats the value as a
// 3-digit number (func_001C5FB0 -> scratch string D_002862C0) and blits it
// red (glyphs D_00265528) when the warning latch D_008104E4 is set or the
// value is <= 60, else white; then the '%'/unit glyphs (D_00273558/560/568).
// Submits five gauge primitives via func_002082B0: the dial ring
// (D_00265390 block: sweep 180, radius word 540), the two gradient arc color
// quads (D_00265410..D_0026544C: bright 192/224/0/128 + 224/128/24/128 when
// value > 35, dim 160/0/0/128 + 192/0/0/128 otherwise), two counter-rotating
// tick sprites (D_00265450/D_002654B0 blocks, 12 deg per 2 frames around 180),
// and the needle at -180 + 360*(value/100) deg (blend mode toggled around the
// ticks via func_00207D00(1,1)/(1,0)).
//
// MATCHING-CRITICAL: the volatile qualifiers on the prim/color globals pin
// mwcc's store order to the original GS-packet build order, and the
// "(label = D_00267298)" assignment-in-arg keeps the pointer homed in its
// callee-saved register before the width call. Byte-verified 100.0 objdiff.

extern int float_to_int(float);
extern void func_00123168(char *dst, char *src);
extern char *func_001C5FB0(int val, int digits, int pad);
extern void func_001CBA50(int a, int x, int y, int w, int h, void *str, void *glyphs);
extern int func_001CC170(char *s);
extern void func_00207D00(int a, int b);
extern void func_00207F80(int a, int x0, int y0, int x1, int y1, int color);
extern void func_002082B0(int a, void *prim);

extern char *D_00267298;
extern char D_00265510[];
extern char D_00265528[];
extern char D_00273558[];
extern char D_00273560[];
extern char D_00273568[];
extern char D_002862C0;
extern unsigned char D_008104E4;
extern float D_00810858;

extern volatile float D_00265390;
extern volatile float D_00265394;
extern volatile float D_00265398;
extern volatile float D_0026539C;
extern float D_002653F0;
extern float D_002653F4;
extern volatile float D_00265410;
extern volatile float D_00265414;
extern volatile float D_00265418;
extern volatile float D_0026541C;
extern volatile float D_00265420;
extern volatile float D_00265424;
extern volatile float D_00265428;
extern volatile float D_0026542C;
extern volatile float D_00265430;
extern volatile float D_00265434;
extern volatile float D_00265438;
extern volatile float D_0026543C;
extern volatile float D_00265440;
extern volatile float D_00265444;
extern volatile float D_00265448;
extern volatile float D_0026544C;
extern float D_00265450;
extern float D_00265454;
extern volatile float D_00265458;
extern volatile float D_0026545C;
extern float D_002654B0;
extern float D_002654B4;
extern volatile float D_002654B8;
extern volatile float D_002654BC;

void func_00208AD0(int *counter, int px, int py) {
    int c0;
    int labelW;
    char *label;
    int c1;
    int boxR, boxT, boxL, boxB;
    int lx;
    float fx, fy;
    float ang;

    counter[8] = counter[8] + 1;
    func_00207D00(1, 0);

    labelW = px - (func_001CC170(label = D_00267298) >> 1);
    c0 = float_to_int(16.0f * (float) (labelW + 0x700)) >> 4;
    func_001CBA50(1, c0, float_to_int(16.0f * (float) (((py - 0x4E) >> 1) + 0x790)) >> 4, 0xC, 0xC,
                  label, &D_00265510);

    boxL = float_to_int(16.0f * (float) (labelW + 0x6F4));
    boxT = float_to_int(16.0f * (float) (((py - 0x4C) >> 1) + 0x790));
    boxR = float_to_int(16.0f * (float) (labelW + 0x6FC));
    boxB = float_to_int(16.0f * (float) (((py - 0x44) >> 1) + 0x790));
    func_00207F80(1, boxL, boxT, boxR, boxB, 0x80CE6000);

    func_00123168(&D_002862C0, func_001C5FB0(float_to_int(D_00810858), 3, 1));

    lx = px - 0x2A;
    if (D_008104E4 != 0) {
        c1 = float_to_int(16.0f * (float) (lx + 0x700)) >> 4;
        c0 = float_to_int(16.0f * (float) (((py + 0x42) >> 1) + 0x790)) >> 4;
        func_001CBA50(1, c1, c0, 0xC, 0xC, &D_002862C0, &D_00265528);
        func_001CBA50(1, float_to_int(16.0f * (float) (px + 0x706)) >> 4, c0, 0xC, 0xC,
                      &D_00273558, &D_00265528);
    } else {
        if (D_00810858 <= 60.0f) {
            func_001CBA50(1, float_to_int(16.0f * (float) (lx + 0x700)) >> 4,
                          float_to_int(16.0f * (float) (((py + 0x42) >> 1) + 0x790)) >> 4,
                          0xC, 0xC, &D_002862C0, &D_00265528);
        } else {
            func_001CBA50(1, float_to_int(16.0f * (float) (lx + 0x700)) >> 4,
                          float_to_int(16.0f * (float) (((py + 0x42) >> 1) + 0x790)) >> 4,
                          0xC, 0xC, &D_002862C0, &D_00265510);
        }
        func_001CBA50(1, float_to_int(16.0f * (float) (px + 0x706)) >> 4,
                      float_to_int(16.0f * (float) (((py + 0x42) >> 1) + 0x790)) >> 4,
                      0xC, 0xC, &D_00273560, &D_00265510);
    }
    func_001CBA50(1, float_to_int(16.0f * (float) (px + 0x6FA)) >> 4,
                  float_to_int(16.0f * (float) (((py + 0x42) >> 1) + 0x790)) >> 4,
                  0xC, 0xC, &D_00273568, &D_00265510);

    D_00265398 = 180.0f;
    D_0026539C = 540.0f;
    fx = 16.0f * (float) (px + 0x700);
    D_00265390 = fx;
    fy = 16.0f * (float) ((py >> 1) + 0x790);
    D_00265394 = fy;
    func_002082B0(1, (void *) &D_00265390);

    if (D_00810858 > 35.0f) {
        D_00265410 = 192.0f;
        D_00265414 = 224.0f;
        D_00265418 = 0.0f;
        D_0026541C = 128.0f;
        D_00265420 = 224.0f;
        D_00265424 = 128.0f;
        D_00265428 = 24.0f;
        D_0026542C = 128.0f;
        D_00265430 = 192.0f;
        D_00265434 = 224.0f;
        D_00265438 = 0.0f;
        D_0026543C = 128.0f;
        D_00265440 = 224.0f;
        D_00265444 = 128.0f;
        D_00265448 = 24.0f;
        D_0026544C = 128.0f;
    } else {
        D_00265410 = 160.0f;
        D_00265414 = 0.0f;
        D_00265418 = 0.0f;
        D_0026541C = 128.0f;
        D_00265420 = 192.0f;
        D_00265424 = 0.0f;
        D_00265428 = 0.0f;
        D_0026542C = 128.0f;
        D_00265430 = 160.0f;
        D_00265434 = 0.0f;
        D_00265438 = 0.0f;
        D_0026543C = 128.0f;
        D_00265440 = 192.0f;
        D_00265444 = 0.0f;
        D_00265448 = 0.0f;
        D_0026544C = 128.0f;
    }

    D_002653F0 = fx;
    D_002653F4 = fy;
    func_002082B0(1, &D_002653F0);

    func_00207D00(1, 1);

    ang = 180.0f + 12.0f * (float) ((counter[8] >> 1) % 30);
    D_00265450 = fx;
    D_00265454 = fy;
    D_002654B0 = fx;
    D_002654B4 = fy;
    D_00265458 = ang - 60.0f;
    D_0026545C = ang;
    D_002654B8 = ang;
    D_002654BC = 60.0f + ang;
    func_002082B0(1, &D_00265450);
    func_002082B0(1, &D_002654B0);

    func_00207D00(1, 0);

    D_00265398 = -180.0f + 360.0f * (D_00810858 / 100.0f);
    D_0026539C = 180.0f;
    D_00265390 = fx;
    D_00265394 = fy;
    func_002082B0(1, (void *) &D_00265390);
}
