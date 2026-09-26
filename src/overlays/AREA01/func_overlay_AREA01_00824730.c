// NEARMISS func_overlay_AREA01_00824730  (runtime 0x00824770, 0x5DC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.08% via mwccps2 2.3.3 (tools/overlay/overlay_match.py check AREA01 <this file>).
// Not compiled by tools/overlay/compile_overlay_src.py; the overlay links this
// function from its splat pieces. Splat names overlay code 0x40 below its runtime
// address (the MWo3 header is loaded first).
// O3 owner family (called from 0x823CD0): three-state move along the spot table
// 0x829DE0 (0x18-byte records: position, rotation) indexed by the event byte +0xE2,
// turning toward the player (D_008106C0) and rebuilding the actor matrix each frame.
// DIVERGENCE: state 1 opens with the turn rate constant (0.0698 rad) materialized
// into the third float argument before the goal/current loads; mwcc 2.3.3 builds it
// last, which also turns the dispatch branch-likely into a plain branch. Rate as a
// local, block temp, operand swap and double cast (6 variants) all stay at 99.08%.
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
typedef struct { float px, py, pz, rx, ry, rz; } Spot;
#define SPF(a) (*(float *)(a))
extern unsigned char *D_008106C0;
extern unsigned char D_008107DF;
extern Spot D_overlay_AREA01_00829DE0[];
extern int D_overlay_AREA01_00829DC0[];
extern float D_70003000[12];
extern float D_70003030[4];
extern float D_700036A0[12];
extern float D_700036D0[4];
extern float D_700038A0[4];
extern void func_001029C0(void *m);
extern void func_00102B08(void *dst, void *src, float a);
extern void func_00102BB0(void *dst, void *src, float a);
extern void func_00102A60(void *dst, void *src, float a);
extern void func_001031E0(void *dst, void *src);
extern void func_001026D0(void *dst, void *a, void *b);
extern void func_001026A0(void *dst, void *a, void *b);
extern void func_00102958(void *dst, void *src);
extern void func_00103230(void *a, void *b, float angle);
extern float func_001B12B0(float goal, float cur, float rate);
extern float func_001B1470(float a);
extern void func_001287F0(void *a0, void *a1, short a2, float f12);

void func_overlay_AREA01_00824730(unsigned char *self, unsigned char *ev) {
    unsigned char *pl = D_008106C0;
    int idx = ev[0xE2];
    int st = self[6];
    float yaw;
    switch (st) {
    case 0:
        self[6] = st + 1;
        *(float *)(ev + 0x30) = D_overlay_AREA01_00829DE0[idx].px;
        *(float *)(ev + 0x34) = D_overlay_AREA01_00829DE0[idx].py;
        *(float *)(ev + 0x38) = D_overlay_AREA01_00829DE0[idx].pz;
        *(float *)(ev + 0x40) = D_overlay_AREA01_00829DE0[idx].rx;
        *(float *)(ev + 0x44) = D_overlay_AREA01_00829DE0[idx].ry;
        *(float *)(ev + 0x48) = D_overlay_AREA01_00829DE0[idx].rz;
        func_001029C0(D_700036A0);
        func_00102B08(D_700036A0, D_700036A0, *(float *)(ev + 0x40));
        func_00102BB0(D_700036A0, D_700036A0, *(float *)(ev + 0x44));
        func_00102A60(D_700036A0, D_700036A0, *(float *)(ev + 0x48));
        func_001031E0(D_700036D0, ev + 0x30);
        func_001026D0(D_700036A0, *(char **)((char *)(D_overlay_AREA01_00829DC0[idx] << 2) + (int)pl + 0x110) + 0x90, D_700036A0);
        *(float *)(ev + 0x10) = SPF(0x700036D0) - *(float *)(self + 0xB0);
        *(float *)(ev + 0x14) = SPF(0x700036D4) - *(float *)(self + 0xB4);
        *(float *)(ev + 0x18) = SPF(0x700036D8) - *(float *)(self + 0xB8);
        *(float *)(ev + 0x1C) = 1.0f;
        *(float *)(ev + 0xE8) = *(float *)(pl + 0xC4);
        *(float *)(ev + 0xD4) = 1.0f;
    case 1:
        *(float *)(self + 0xC4) = func_001B12B0(3.14159274f + *(float *)(pl + 0xC4), *(float *)(self + 0xC4), 0.0698131695f);
        *(float *)(self + 0xC0) = func_001B12B0(-1.57079637f, *(float *)(self + 0xC0), 0.0698131695f);
        *(float *)(ev + 0xD4) -= 0.02f;
        func_00103230(ev + 0x10, ev + 0x10, *(float *)(ev + 0xD4));
        yaw = func_001B1470(*(float *)(pl + 0xC4) - *(float *)(ev + 0xE8));
        func_001029C0(D_700036A0);
        func_00102BB0(D_700036A0, D_700036A0, func_001B1470(3.14159274f + yaw));
        func_001026A0(D_700038A0, D_700036A0, ev + 0x10);
        func_001029C0(D_700036A0);
        func_00102B08(D_700036A0, D_700036A0, *(float *)(ev + 0x40));
        func_00102BB0(D_700036A0, D_700036A0, *(float *)(ev + 0x44));
        func_00102A60(D_700036A0, D_700036A0, *(float *)(ev + 0x48));
        func_001031E0(D_700036D0, ev + 0x30);
        func_001026D0(D_700036A0, *(char **)((char *)(D_overlay_AREA01_00829DC0[idx] << 2) + (int)pl + 0x110) + 0x90, D_700036A0);
        *(float *)(self + 0xB0) = SPF(0x700036D0) + SPF(0x700038A0);
        *(float *)(self + 0xB4) = SPF(0x700036D4) - SPF(0x700038A4);
        *(float *)(self + 0xB8) = SPF(0x700036D8) + SPF(0x700038A8);
        func_001029C0(D_70003000);
        func_00102B08(D_70003000, D_70003000, *(float *)(self + 0xC0));
        func_00102BB0(D_70003000, D_70003000, *(float *)(self + 0xC4));
        func_00102A60(D_70003000, D_70003000, *(float *)(self + 0xC8));
        func_00102958(self + 0xD0, D_70003000);
        func_001031E0(D_70003030, self + 0xB0);
        if (*(float *)(ev + 0xD4) <= 0.2f) {
            *(float *)(ev + 0x30) = D_overlay_AREA01_00829DE0[idx].px;
            *(float *)(ev + 0x34) = D_overlay_AREA01_00829DE0[idx].py;
            *(float *)(ev + 0x38) = D_overlay_AREA01_00829DE0[idx].pz;
            *(float *)(self + 0xC0) = *(float *)(ev + 0x40);
            *(float *)(self + 0xC4) = *(float *)(ev + 0x44);
            *(float *)(self + 0xC8) = *(float *)(ev + 0x48);
            func_001287F0(self, ev, 0x16, 0.0f);
            *(short *)(ev + 0xD0) = 0x1C2;
            if (ev[0xE2] == 0) {
                D_008107DF = 0x10;
                self[4] = 4;
                self[5] = 0;
                self[6] = 0;
            } else {
                self[6]++;
                self[7] = 0;
            }
        }
        break;
    case 2:
        func_001029C0(D_70003000);
        func_00102B08(D_70003000, D_70003000, *(float *)(self + 0xC0));
        func_00102BB0(D_70003000, D_70003000, *(float *)(self + 0xC4));
        func_00102A60(D_70003000, D_70003000, *(float *)(self + 0xC8));
        func_001031E0(D_70003030, ev + 0x30);
        func_001026D0(D_70003000, *(char **)((char *)(D_overlay_AREA01_00829DC0[idx] << 2) + (int)pl + 0x110) + 0x90, D_70003000);
        func_001031E0(self + 0xB0, D_70003030);
        if (--*(short *)(ev + 0xD0) == 0) {
            self[5] = 7;
            self[6] = 0;
            self[7] = 0;
        }
        break;
    }
}
