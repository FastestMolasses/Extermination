// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Entity state machine (byte state at +6), 12 states, compare-chain dispatch.
// State 0: init — func_001B61C0(0,0xC0,5,1) (timer/track setup), voice/SFX cue 0x146
// via func_001FBD50(e,id,0,300.0f), state++, clears +7; optional cues 0x152/0x153 +
// func_0021C350/func_0021C270 when floats +0x224/+0x22C are set; anim func_001749A0(e,0x27,0,0.0f);
// clears flag byte +0xF if bit1 set; zeroes +0x38 (delta) and +0x21C (last-time).
// State 1: waits on input/event mask +0x200: on bit 0x1000, branches on +0x220<=0:
//   either func_0021C200(e) + state=0x1E + anim 0x1C4, or state=0xA + anim 0x29(4.0f);
//   else on +0x228>=100 && D_008106F1: state=0x14 + anim 0x28(16.0f); else state++ +
//   anim 0x28(8.0f); then cue 0x12F. Otherwise accumulates frame delta from
//   (*D_00275B40)->time (+8) into +0x38/+0x21C and idles via func_00178B90(e,1).
// States 2/10/20: wait for bit 0x8000 clear -> state++ (10 also cues 0x156, 20 calls
// func_0021C120). State 11: waits +0x3C<=18.0 -> state++ + func_0021D490. State 12:
// bit 0x1000 -> state++, +7=0, func_001B61C0(1,0xEE,0x3C,1). State 13: func_0021D2E0(e,0x78,0).
// State 21: func_0021C190() ? state++ : (rate +0x204=0.2f, delta accum + idle).
// State 22/3: bit 0x1000 -> +0x20E=60, +0x25C=0, func_0017C540(e); else (22: rate=0.5f)
// delta accum + idle. State 30: bit 0x8000 clear -> +4=2,+5=3,state=0,+0x1F0=0x3F (recycle);
// else delta accum + idle.

extern void func_001749A0(char *, int, int, float);
extern void func_00178B90(char *, int);
extern void func_0017C540(char *);
extern void func_001B61C0(int, int, int, int);
extern void func_001FBD50(char *, int, int, float);
extern void func_0021C120(char *);
extern int func_0021C190(void);
extern void func_0021C200(char *);
extern void func_0021C270(char *);
extern void func_0021C350(char *);
extern void func_0021D2E0(char *, int, int);
extern void func_0021D490(char *);

extern char **D_00275B40;
extern unsigned char D_008106F1[];

void func_00224FE0(char *e) {
    unsigned char st;

    st = *(unsigned char *)(e + 6);
    switch (st) {
    case 0:
        func_001B61C0(0, 0xC0, 5, 1);
        func_001FBD50(e, 0x146, 0, 300.0f);
        *(unsigned char *)(e + 6) += 1;
        *(char *)(e + 7) = 0;
        if (*(float *)(e + 0x224)) {
            func_001FBD50(e, 0x152, 0, 300.0f);
            func_0021C350(e);
        }
        if (*(float *)(e + 0x22C)) {
            func_001FBD50(e, 0x153, 0, 300.0f);
            func_0021C270(e);
        }
        func_001749A0(e, 0x27, 0, 0.0f);
        if (*(unsigned char *)(e + 0xF) & 2) {
            *(unsigned char *)(e + 0xF) = 0;
        }
        *(float *)(e + 0x38) = 0.0f;
        *(float *)(e + 0x21C) = 0.0f;
        break;
    case 1:
        if (*(int *)(e + 0x200) & 0x1000) {
            if (*(float *)(e + 0x220) <= 0.0f) {
                if (*(unsigned char *)(e + 0xF) == 0x63 || *(unsigned char *)(e + 0x234) == 1) {
                    func_0021C200(e);
                    *(unsigned char *)(e + 6) = 0x1E;
                    func_001749A0(e, 0x1C4, 0, 16.0f);
                } else {
                    *(unsigned char *)(e + 6) = 0xA;
                    func_001749A0(e, 0x29, 0, 4.0f);
                }
            } else if (!(*(float *)(e + 0x228) < 100.0f) && D_008106F1[0] != 0) {
                *(unsigned char *)(e + 6) = 0x14;
                func_001749A0(e, 0x28, 0, 16.0f);
            } else {
                *(unsigned char *)(e + 6) += 1;
                func_001749A0(e, 0x28, 0, 8.0f);
            }
            func_001FBD50(e, 0x12F, 0, 300.0f);
            *(float *)(e + 0x38) = 0.0f;
            *(float *)(e + 0x21C) = 0.0f;
        } else {
            *(float *)(e + 0x38) = *(float *)(*D_00275B40 + 8) - *(float *)(e + 0x21C);
            *(float *)(e + 0x21C) = *(float *)(*D_00275B40 + 8);
            func_00178B90(e, 1);
        }
        break;
    case 2:
        if (!(*(int *)(e + 0x200) & 0x8000)) {
            *(unsigned char *)(e + 6) = st + 1;
        }
        break;
    case 3:
        if (*(int *)(e + 0x200) & 0x1000) {
            *(short *)(e + 0x20E) = 0x3C;
            *(char *)(e + 0x25C) = 0;
            func_0017C540(e);
        } else {
            *(float *)(e + 0x38) = *(float *)(*D_00275B40 + 8) - *(float *)(e + 0x21C);
            *(float *)(e + 0x21C) = *(float *)(*D_00275B40 + 8);
            func_00178B90(e, 1);
        }
        break;
    case 10:
        if (!(*(int *)(e + 0x200) & 0x8000)) {
            *(unsigned char *)(e + 6) = st + 1;
            func_001FBD50(e, 0x156, 0, 300.0f);
        }
        break;
    case 11:
        if (*(float *)(e + 0x3C) <= 18.0f) {
            *(unsigned char *)(e + 6) = st + 1;
            func_0021D490(e);
        }
        break;
    case 12:
        if (*(int *)(e + 0x200) & 0x1000) {
            *(unsigned char *)(e + 6) = st + 1;
            *(char *)(e + 7) = 0;
            func_001B61C0(1, 0xEE, 0x3C, 1);
        }
        break;
    case 13:
        func_0021D2E0(e, 0x78, 0);
        break;
    case 20:
        if (!(*(int *)(e + 0x200) & 0x8000)) {
            *(unsigned char *)(e + 6) = st + 1;
            func_0021C120(e);
        }
        break;
    case 21:
        if (func_0021C190()) {
            *(unsigned char *)(e + 6) += 1;
        } else {
            *(float *)(e + 0x204) = 0.2f;
            *(float *)(e + 0x38) = *(float *)(*D_00275B40 + 8) - *(float *)(e + 0x21C);
            *(float *)(e + 0x21C) = *(float *)(*D_00275B40 + 8);
            func_00178B90(e, 1);
        }
        break;
    case 22:
        if (*(int *)(e + 0x200) & 0x1000) {
            *(short *)(e + 0x20E) = 0x3C;
            *(char *)(e + 0x25C) = 0;
            func_0017C540(e);
        } else {
            *(float *)(e + 0x204) = 0.5f;
            *(float *)(e + 0x38) = *(float *)(*D_00275B40 + 8) - *(float *)(e + 0x21C);
            *(float *)(e + 0x21C) = *(float *)(*D_00275B40 + 8);
            func_00178B90(e, 1);
        }
        break;
    case 30:
        if (!(*(int *)(e + 0x200) & 0x8000)) {
            *(char *)(e + 4) = 2;
            *(char *)(e + 5) = 3;
            *(unsigned char *)(e + 6) = 0;
            *(char *)(e + 0x1F0) = 0x3F;
        } else {
            *(float *)(e + 0x38) = *(float *)(*D_00275B40 + 8) - *(float *)(e + 0x21C);
            *(float *)(e + 0x21C) = *(float *)(*D_00275B40 + 8);
            func_00178B90(e, 1);
        }
        break;
    }
}
