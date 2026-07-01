// NEARMISS func_0014DC30  (vram 0x0014DC30, 0x414 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 91.28% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body/structure fully recovered (all 4 states decoded and structurally landed: idiom-20 branch-likely dispatch, memory-reload-not-cache before array index, delay-slot short-store idiom, raw-int D_7000xxxx-less but %hi/%lo D_0081xxxx globals matched). Two residual genuine compiler artifacts: (1) ca...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// 4-state actor tick keyed on p[6] (a0=p self, a1=e companion record).
// state 0: seed e+8=0, e+0x20=0.4f, e+0x5D=0; pick e+4 = (p+0x38<0.0f)?0:1 as
// a facing/side flag; start anim clip D_00245D00[e+4] (rate 5.0) and fire SFX
// 0x862 (vol 300). state 1: if e+8==0 and p+0x3C<=33.0 latch e+8=1 and fire
// SFX 0x863 (vol 300); ease e+0x28 toward 0 via func_001B12B0(rate 0.0349);
// on settle (e+0x28==0.0f) and e+0 bit 0x1000, advance state, clear e+8, start
// clip D_00245D06[e+4]. state 2: gate D_008102B0/func_0021BE40/func_001A7B80
// to trigger a player "spotted" reaction once (sets D_008102B0 bit 2, latches
// D_008102BF=2, seeds a scratch aim-delta D_00810320/0x324/0x328 from
// D_00810360/0x368 minus p+0xB0/0xB8, normalizes via func_00102760, picks
// D_008104D4 by D_0081070A, calls func_001B55E0(p,1)); sets p+0x20's state
// byte (2 in [24,52], else 0) unless null; if e+8==0 and p+0x3C<=49.0 latch
// e+8=1 and fire SFX 0x865 (vol 300); on e+0 bit 0x1000, advance state, reset
// e+0x20=0.2f, e+0x5D=1, start clip D_00245D0C[e+4]. state 3: on e+0 bit
// 0x1000, reset p's state bytes (5/6), e+0x20=1.0f, e+0x5D=0, and RNG-pick a
// facing angle into e+0x50 (range depends on p+0x34 vs 0xF1).
extern void anim_clip_init(char *self, int clip, float a, float b);
extern void func_00102760(void *a, void *b);
extern int func_00122BB8(void);
extern int func_001A7B80(char *p);
extern float func_001B12B0(float goal, float cur, float rate);
extern void func_001B55E0(char *p, int a);
extern void func_001FBD50(char *p, int a, int b, float f);
extern int func_0021BE40(void *a, char *p);

extern short D_00245D00[2];
extern short D_00245D06[2];
extern short D_00245D0C[2];
extern unsigned char D_008102B0;
extern signed char D_008102BF;
extern float D_00810320;
extern int D_00810324;
extern float D_00810328;
extern float D_00810360;
extern float D_00810368;
extern int D_008104D4;
extern unsigned char D_0081070A;

void func_0014DC30(char *p, char *e) {
    unsigned char state = *(unsigned char *)(p + 6);

    switch (state) {
    case 0:
        *(unsigned char *)(p + 6) = state + 1;
        *(int *)(e + 8) = 0;
        *(float *)(e + 0x20) = 0.4f;
        *(char *)(e + 0x5D) = 0;
        if (*(float *)(e + 0x38) < 0.0f) {
            *(int *)(e + 4) = 0;
        } else {
            *(int *)(e + 4) = 1;
        }
        anim_clip_init(p, D_00245D00[*(int *)(e + 4)], 5.0f, 0.0f);
        func_001FBD50(p, 0x862, 0, 300.0f);
        return;
    case 1:
        if (*(int *)(e + 8) == 0 && *(float *)(p + 0x3C) <= 33.0f) {
            *(int *)(e + 8) = 1;
            func_001FBD50(p, 0x863, 0, 300.0f);
        }
        *(float *)(e + 0x28) = func_001B12B0(0.0f, *(float *)(e + 0x28), 0.0349065870f);
        if (*(float *)(e + 0x28) == 0.0f && *(int *)(e + 0) & 0x1000) {
            *(unsigned char *)(p + 6) = *(unsigned char *)(p + 6) + 1;
            *(int *)(e + 8) = 0;
            anim_clip_init(p, D_00245D06[*(int *)(e + 4)], 0.0f, 0.0f);
            return;
        }
        return;
    case 2:
        if (*(int *)(e + 4) == 2
            || (!(*(float *)(p + 0x3C) < 27.0f) && func_0021BE40(&D_008102B0, p) == 0
                && func_001A7B80(p) != 0)) {
            D_008102B0 |= 2;
            D_008102BF = 2;
            D_00810320 = D_00810360 - *(float *)(p + 0xB0);
            D_00810324 = 0;
            D_00810328 = D_00810368 - *(float *)(p + 0xB8);
            func_00102760(&D_00810320, &D_00810320);
            if (D_0081070A != 0) {
                D_008104D4 = 0x41A00000;
            } else {
                D_008104D4 = 0x41700000;
            }
            func_001B55E0(p, 1);
        }
        if (*(float *)(p + 0x3C) <= 52.0f && !(*(float *)(p + 0x3C) < 24.0f)) {
            if (*(char **)(p + 0x20) != 0) {
                (*(char **)(p + 0x20))[5] = 2;
            }
        } else {
            if (*(char **)(p + 0x20) != 0) {
                (*(char **)(p + 0x20))[5] = 0;
            }
        }
        if (*(int *)(e + 8) == 0 && *(float *)(p + 0x3C) <= 49.0f) {
            *(int *)(e + 8) = 1;
            func_001FBD50(p, 0x865, 0, 300.0f);
        }
        if (*(int *)(e + 0) & 0x1000) {
            *(unsigned char *)(p + 6) = *(unsigned char *)(p + 6) + 1;
            *(float *)(e + 0x20) = 0.2f;
            *(char *)(e + 0x5D) = 1;
            anim_clip_init(p, D_00245D0C[*(int *)(e + 4)], 0.0f, 0.0f);
            return;
        }
        break;
    case 3:
        if (*(int *)(e + 0) & 0x1000) {
            *(char *)(p + 5) = 0;
            *(unsigned char *)(p + 6) = 0;
            *(float *)(e + 0x20) = 1.0f;
            *(char *)(e + 0x5D) = 0;
            if (*(short *)(p + 0x34) < 0xF1) {
                *(short *)(e + 0x50) = (func_00122BB8() >> 12) & 0x7F;
            } else {
                *(short *)(e + 0x50) = (func_00122BB8() >> 19) & 0xFF;
            }
        }
        break;
    }
}
