// NEARMISS func_0014D7C0  (vram 0x0014D7C0, 0x46C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 91.23% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Full logic/structure recovered; notably decoded that the shipped binary reads a stale FPU register (the inner range-switch's low-bound constant) at a later comparison instead of a fresh literal -- modeled as a carried-out `lo` local (took match ~84%->~91%). Residual: (1) two anim_clip_init call-s...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Per-state animation/clip driver for entity arg0 (state byte at arg0+6), companion record
// arg1. State 0: advance the state, zero arg1+4/0xC, seed arg1+8=0x5A and arg1+0x20=0.525f,
// clear arg1+0x5D, clear the linked object's byte 5 (arg0+0x20) if present, start clip 3
// and fire SFX 0x862 (vol 300). State 1: once arg1+0 bit 0x1000 is set, advance the state
// and start clip D_00275418[arg1+4] (a 3-entry per-phase clip table); if phase!=0 also
// re-arm the countdown at arg1+8 (0x2D). State 2: a 3-phase (arg1+4 in [0,2]) proximity
// gate on arg0+0x3C against per-phase [lo,hi] bands (12-26 / 14-24 / 22-30); in-range sets
// the linked object's byte 5 to 3, out-of-range to 0. If arg1+0xC is still 0 and arg0+0x3C
// has dropped to/below that same phase's lo bound, latch arg1+0xC=1 and fire SFX 0x865 (vol
// 300). If in-range and func_0021BE40(&D_008102B0,arg0)==0 and func_001A7B80(arg0)!=0,
// trigger the "player spotted" reaction once: OR 2 into D_008102B0, latch D_008102BF=2,
// seed a scratch aim-delta D_00810320/0x328 from D_00810360/0x368 minus arg0+0xB0/0xB8
// (zeroing D_00810324), normalize via func_00102760, pick D_008104D4 (25.0f if phase==2,
// else 20.0f/25.0f by D_0081070A), and call func_001B55E0(arg0,1). Once arg1+0 bit 0x1000
// is set, advance arg1+4; at phase>=3 advance the state, mark arg1+0x5D=1, reset
// arg1+0x20=0.2f, start clip 7, and clear the linked object's byte 5; otherwise reset
// arg0+6 to 1, clear arg1+0xC, and ease arg1+0x20 up by 0.15f. State 3: once arg1+0 bit
// 0x1000 is set, reset arg0+5/+6, arg1+0x20=1.0f, arg1+0x5D=0, and RNG-pick a facing byte
// into arg1+0x50. Tail (all states): while arg1+8 is nonzero, count it down and ease
// arg1+0x28 toward arg1+0x38 via func_001B12B0 at rate pi/120 (~1.5 deg/tick).
extern void anim_clip_init(char *self, int clip, float a, float b);
extern void func_00102760(void *a, void *b);
extern int func_00122BB8(void);
extern int func_001A7B80(char *p);
extern float func_001B12B0(float goal, float cur, float rate);
extern void func_001B55E0(char *p, int a);
extern void func_001FBD50(char *p, int a, int b, float f);
extern int func_0021BE40(void *a, char *p);

extern short D_00275418[3];
extern unsigned char D_008102B0;
extern signed char D_008102BF;
extern float D_00810320;
extern int D_00810324;
extern float D_00810328;
extern float D_00810360;
extern float D_00810368;
extern int D_008104D4;
extern unsigned char D_0081070A;

void func_0014D7C0(char *arg0, char *arg1) {
    unsigned char st;
    int phase;
    int inrange;
    float lo;
    float v3C;
    char *p;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = st + 1;
        *(int *)(arg1 + 4) = 0;
        *(int *)(arg1 + 8) = 0x5A;
        *(int *)(arg1 + 0xC) = 0;
        *(float *)(arg1 + 0x20) = 0.525f;
        *(char *)(arg1 + 0x5D) = 0;
        p = *(char **)(arg0 + 0x20);
        if (p != 0) {
            p[5] = 0;
        }
        anim_clip_init(arg0, 3, 5.0f, 0.0f);
        func_001FBD50(arg0, 0x862, 0, 300.0f);
        break;
    case 1:
        if (*(int *)(arg1 + 0) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            anim_clip_init(arg0, D_00275418[*(int *)(arg1 + 4)], 0.0f, 0.0f);
            if (*(int *)(arg1 + 4) != 0) {
                *(int *)(arg1 + 8) = 0x2D;
            }
        }
        break;
    case 2:
        phase = *(int *)(arg1 + 4);
        inrange = 0;
        switch (phase) {
        case 0:
            v3C = *(float *)(arg0 + 0x3C);
            lo = 12.0f;
            if (v3C <= 26.0f && !(v3C < lo)) {
                inrange = 1;
            }
            break;
        case 1:
            v3C = *(float *)(arg0 + 0x3C);
            lo = 14.0f;
            if (v3C <= 24.0f && !(v3C < lo)) {
                inrange = 1;
            }
            break;
        case 2:
            v3C = *(float *)(arg0 + 0x3C);
            lo = 22.0f;
            if (v3C <= 30.0f && !(v3C < lo)) {
                inrange = 1;
            }
            break;
        }
        if (inrange) {
            if (*(char **)(arg0 + 0x20) != 0) {
                (*(char **)(arg0 + 0x20))[5] = 3;
            }
        } else {
            if (*(char **)(arg0 + 0x20) != 0) {
                (*(char **)(arg0 + 0x20))[5] = 0;
            }
        }
        if (*(int *)(arg1 + 0xC) == 0 && *(float *)(arg0 + 0x3C) <= lo) {
            *(int *)(arg1 + 0xC) = 1;
            func_001FBD50(arg0, 0x865, 0, 300.0f);
        }
        if (inrange && func_0021BE40(&D_008102B0, arg0) == 0 && func_001A7B80(arg0) != 0) {
            D_008102B0 |= 2;
            D_008102BF = 2;
            D_00810320 = D_00810360 - *(float *)(arg0 + 0xB0);
            D_00810324 = 0;
            D_00810328 = D_00810368 - *(float *)(arg0 + 0xB8);
            func_00102760(&D_00810320, &D_00810320);
            if (*(int *)(arg1 + 4) == 2) {
                D_008104D4 = 0x41C80000;
            } else {
                D_008104D4 = 0x41A00000;
                if (D_0081070A != 0) {
                    D_008104D4 = 0x41C80000;
                }
            }
            func_001B55E0(arg0, 1);
        }
        if (*(int *)(arg1 + 0) & 0x1000) {
            phase = *(int *)(arg1 + 4) + 1;
            *(int *)(arg1 + 4) = phase;
            if (phase >= 3) {
                *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
                *(char *)(arg1 + 0x5D) = 1;
                *(float *)(arg1 + 0x20) = 0.2f;
                anim_clip_init(arg0, 7, 0.0f, 0.0f);
                if (*(char **)(arg0 + 0x20) != 0) {
                    (*(char **)(arg0 + 0x20))[5] = 0;
                }
            } else {
                *(unsigned char *)(arg0 + 6) = 1;
                *(int *)(arg1 + 0xC) = 0;
                *(float *)(arg1 + 0x20) = *(float *)(arg1 + 0x20) + 0.15f;
            }
        }
        break;
    case 3:
        if (*(int *)(arg1 + 0) & 0x1000) {
            *(char *)(arg0 + 5) = 0;
            *(unsigned char *)(arg0 + 6) = 0;
            *(float *)(arg1 + 0x20) = 1.0f;
            *(char *)(arg1 + 0x5D) = 0;
            *(short *)(arg1 + 0x50) = (func_00122BB8() >> 0xC) & 0xFF;
        }
        break;
    }

    if (*(int *)(arg1 + 8) != 0) {
        *(int *)(arg1 + 8) = *(int *)(arg1 + 8) - 1;
        *(float *)(arg1 + 0x28) = func_001B12B0(*(float *)(arg1 + 0x38), *(float *)(arg1 + 0x28), 0.0261799395f);
    }
}
