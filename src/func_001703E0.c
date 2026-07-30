// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// SEMANTICS: main per-frame state machine for the player/actor arg0, dispatched on
// the state byte at arg0+6 via a sparse compare chain (states 0-3 and 0x63-0x6F).
// The inner switch on the sub-state byte at arg0+0x275 (0-5) uses jump table
// jtbl_0026D680 and picks the per-stance handler.
// arg0+5 is the character id, arg0+0x28 a countdown, arg0+0x94 a pose id,
// arg0+0x1F0 a mode byte, arg0+0x200 a button/event mask, arg0+0x23F a counter,
// arg0+0x26C/0x270 blend deltas, arg0+0x274/0x275 stance selectors,
// arg0+0x278/0x27C the two anim blend weights, arg0+0x2A0 a quaternion,
// arg0+0x2D0..0x2D8 a cached camera position, arg0+0x317 a resume flag.
// D_00248B88/D_00248C68 are per-stance clip-id tables indexed by arg0+0x275.
// State 0 initialises the blends and enters state 1 (or state 2 when +0x317 is set).
// State 1 waits for button 0x1000 then starts the stance clip and evaluates the
// skeleton. State 2 is the active stance: it ticks the stance handler and cycles
// arg0+0x2F0 through 0..2. State 3 idles. States 0x63/0x64 are the blend-in
// transition (8 frames, aiming yaw seeded from atan2 of the target at arg0+0x20).
// State 0x65 plays clip 0x111 and effect 0x163. State 0x66 turns toward the aim yaw
// until button 0x1000 jumps to state 0x6E. States 0x6E/0x6F are the exit sequence.
// Every frame ends with func_001764E0, a -0.2 drop on arg0+0xB4, func_00175900 and
// func_001796C0.

extern void anim_eval_skeleton(char *e);
extern void anim_matrix_dispatch(char *e);
extern void copy_qw4(char *dst, char *src);
extern float func_0011E620(float y, float x);
extern void func_0016F530(char *e, int a);
extern void func_0016F600(char *e);
extern void func_00170A60(char *e, int a);
extern void func_00171320(char *e);
extern void func_00171670(char *e);
extern void func_00171B00(char *e);
extern void func_00171E90(char *e);
extern void func_001723D0(char *e);
extern int func_001749A0(char *e, int clip, int flags, float blend);
extern int func_00174AC0(char *e, int a);
extern int func_00175900(char *e, int a);
extern void func_001764E0(char *e);
extern int func_00178B90(char *e, int a);
extern void func_001796C0(char *e);
extern void func_00199220(char *e);
extern void func_0017ABA0(char *e);
extern void func_0017B300(char *e, int a);
extern void func_0017C440(char *e, int a);
extern void func_0017C540(char *e);
extern float func_001B12B0(float cur, float target, float rate);
extern float func_001B1470(float a);
extern int func_001FBD50(char *e, int a, int b, float f);
extern short D_00248B88[];
extern short D_00248C68[];
extern char *D_00275B40;
extern int D_008106E0[];
extern unsigned char D_00810CA4[];

void func_001703E0(char *arg0) {
    char *p;
    char *q;
    int st;
    int sub;
    int k;
    int hit;
    short t;

    D_008106E0[0] = 0;
    p = arg0;
    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        func_0017B300(arg0, 0);
        if (*(unsigned char *)(p + 0x317) == 0) {
            *(unsigned char *)(p + 6) = *(unsigned char *)(p + 6) + 1;
            *(float *)(p + 0x278) = 0.5f;
            *(unsigned char *)(p + 0x2F2) = 0;
            *(short *)(p + 0x2E) = 0;
            *(unsigned char *)(p + 0x275) = 0;
            func_001749A0(p, 0x110, 0, 1.0f);
        } else {
            *(unsigned char *)(p + 6) = *(unsigned char *)(p + 6) + 2;
            func_0016F530(p, 0);
        }
        *(float *)(p + 0x27C) = 0.5f;
        *(char *)(p + 7) = 0;
        *(char *)(p + 0x302) = 0;
        *(short *)(p + 0x276) = 0;
        *(unsigned char *)(p + 0x2F0) = 0;
        *(unsigned char *)(p + 0x274) = 0;
        break;
    case 1:
        if (*(int *)(p + 0x200) & 0x1000) {
            *(unsigned char *)(p + 6) = st + 1;
            func_0016F530(arg0, 0);
            if (*(unsigned char *)(p + 5) == 0x1D || *(unsigned char *)(p + 5) == 0x1E) {
                func_001749A0(p, D_00248B88[*(unsigned char *)(p + 0x275)], 0, 0.0f);
            } else {
                func_001749A0(p, D_00248C68[*(unsigned char *)(p + 0x275)], 0, 0.0f);
            }
        }
        anim_eval_skeleton(p);
        copy_qw4(p + 0x2A0, *(char **)(D_00275B40 + 0x10) + 0x90);
        break;
    case 2:
        *(short *)(p + 0x94) = 7;
        *(char *)(p + 0x302) = 0;
        func_0017ABA0(arg0);
        if (D_00810CA4[0] == 0) {
            if (*(unsigned char *)(p + 0x274) != 0) {
                *(unsigned char *)(p + 0x2F0) = *(unsigned char *)(p + 0x2F0) + 1;
                if ((int)*(unsigned char *)(p + 0x2F0) > 2) {
                    *(unsigned char *)(p + 0x2F0) = 0;
                }
            }
            func_00199220(p);
        } else if (D_00810CA4[0] == 1) {
            func_00199220(p);
        }
        sub = *(unsigned char *)(p + 0x275);
        switch (sub) {
        case 0:
            func_00170A60(p, 1);
            break;
        case 1:
            func_00171320(p);
            break;
        case 2:
            func_00171670(p);
            break;
        case 3:
            func_00171B00(p);
            break;
        case 4:
            func_00171E90(p);
            break;
        case 5:
            func_001723D0(p);
            break;
        }
        break;
    case 3:
        func_0016F600(arg0);
        if (*(unsigned char *)(p + 0x1F0) == 0x33) {
            *(char *)(p + 1) = 0;
        }
        break;
    case 0x63:
        *(unsigned char *)(p + 6) = st + 1;
        *(short *)(p + 0x28) = 8;
        *(float *)(p + 0x26C) = (0.5f - *(float *)(p + 0x27C)) / 8.0f;
        *(float *)(p + 0x270) = (0.5f - *(float *)(p + 0x278)) / 8.0f;
        q = *(char **)(p + 0x20);
        *(float *)0x70003A20 = func_0011E620(-*(float *)(q + 0xC8), *(float *)(q + 0xC0));
        *(float *)(p + 0x218) = func_001B1470(1.5707964f + *(float *)0x70003A20);
        /* fallthrough */
    case 0x64:
        t = *(short *)(p + 0x28);
        *(short *)(p + 0x28) = t - 1;
        if (t == 0) {
            *(unsigned char *)(p + 6) = *(unsigned char *)(p + 6) + 1;
            *(float *)(p + 0x27C) = 0.5f;
            *(float *)(p + 0x278) = 0.5f;
            if (*(unsigned char *)(p + 5) == 0x1D || *(unsigned char *)(p + 5) == 0x1E) {
                func_001749A0(p, D_00248B88[*(unsigned char *)(p + 0x275)], 0, 0.0f);
            } else {
                func_001749A0(p, D_00248C68[*(unsigned char *)(p + 0x275)], 0, 0.0f);
            }
        } else {
            *(float *)(p + 0x27C) += *(float *)(p + 0x26C);
            *(float *)(p + 0x278) += *(float *)(p + 0x270);
            hit = 0;
            if (*(unsigned char *)(p + 0x1F0) != 0x33) {
                anim_matrix_dispatch(p);
                k = *(unsigned char *)(p + 0x1F0);
                if (k == 0x32 || k == 0x35) {
                    copy_qw4(p + 0x2A0, *(char **)(D_00275B40 + 0x10) + 0x90);
                    hit = 1;
                } else if (*(unsigned char *)(p + 0x275) == 4) {
                    copy_qw4(p + 0x2A0, *(char **)(D_00275B40 + 0x10) + 0x90);
                    hit = 1;
                } else if (*(unsigned char *)(p + 0x2F2) != 0) {
                    copy_qw4(p + 0x2A0, *(char **)(D_00275B40 + 0x10) + 0x90);
                    hit = 1;
                }
                if (hit == 0) {
                    *(float *)(p + 0x2D0) = *(float *)(*(char **)(D_00275B40 + 0x10) + 0xC0);
                    *(float *)(p + 0x2D4) = *(float *)(*(char **)(D_00275B40 + 0x10) + 0xC4);
                    *(float *)(p + 0x2D8) = *(float *)(*(char **)(D_00275B40 + 0x10) + 0xC8);
                }
            } else {
                anim_matrix_dispatch(p);
            }
        }
        break;
    case 0x65:
        *(unsigned char *)(p + 6) = st + 1;
        *(short *)(p + 0x276) = 0;
        func_001749A0(arg0, 0x111, 0, 1.0f);
        func_001FBD50(p, 0x163, 0, 300.0f);
        *(unsigned char *)(p + 0x317) = 0;
        break;
    case 0x66:
        if (*(int *)(p + 0x200) & 0x1000) {
            *(unsigned char *)(p + 6) = 0x6E;
        } else {
            *(float *)(p + 0xC4) = func_001B12B0(*(float *)(p + 0x218), *(float *)(p + 0xC4),
                                                 0.043633234f);
        }
        break;
    case 0x6E:
        func_00174AC0(arg0, 1);
        if ((int)*(unsigned char *)(p + 0x23F) > 1) {
            *(unsigned char *)(p + 6) = *(unsigned char *)(p + 6) + 1;
            func_0017C440(p, 0);
        } else {
            *(char *)(p + 0x25C) = 0;
            func_0017C540(p);
        }
        break;
    case 0x6F:
        func_00174AC0(arg0, 1);
        func_00178B90(p, 0);
        if (!(*(int *)(p + 0x200) & 0x8000)) {
            func_0017C540(p);
        }
        break;
    }
    func_001764E0(p);
    *(float *)(p + 0xB4) += -0.2f;
    func_00175900(p, 1);
    func_001796C0(p);
}
