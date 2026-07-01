// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Player/entity behaviour state machine, dispatched on the state byte at e+6
// (cases 0,10,11,12,20,21,22,23,24,30,31,32,33,34; mwcc reverses the ascending
// case labels into a descending beq chain, beql for case 24). Sibling of
// func_00162A40. Case 0 splits on e[0x1F1] (drop height at e+0x254 vs the
// threshold D_002488A4) and re-triggers clip 0x69/0x6A; cases 10/20/30 wait on
// contact flag 0x1000 of e+0x200, stamp the anim rate at e+0x38 (6.3f when
// e[0x25C]==3 else 4.1f) and restart clip 0x6B/0x6C; 11/21/22 integrate the
// position at e+0xB0/B4/B8 by the velocity at e+0x2E0..2E8 (+ the gravity
// accumulator e+0x26C/0x270); 23 snaps to the camera target via func_00102948
// and branches on the hit counter e[0x23F]; 34 commits e+0xB4 to e+0x2F4 and
// resets to state 0/mode 0xD.
//
// MATCH NOTES: case bodies must end with break (NOT return) - return-shaped
// exits make mwcc fill the dispatch-chain beq delay slots with the next case
// li while the target leaves them nop (the func_00162A40 94.38% wall; this is
// the lever that cracks it). Far globals (D_002488A4/D_00810700) declared as
// unknown-size arrays to keep lui/%lo addressing under -sdatathreshold 8
// (D_00275B40 stays a scalar for the gp-rel access). Case 0/22 need the float
// temp CSE shape; case 22 the store-then-+= forward; case 23 "> 1" for the
// slti $at form. mwcc 2.3.3 (mwcps2-2.3.3-000906) byte-matches at 100.0%; the
// pinned 991202 build caps at 85.0% (it also fills the break-shaped dispatch
// slots). Verified objdiff 100.0% vs build/expected/func_00162190.o.

extern void func_00162080(unsigned char *e);
extern void func_0017D940(void);
extern void func_0017DAF0(void);
extern void func_0017DC80(void);
extern void func_0017DEB0(unsigned char *e);
extern int func_001C61D0(int, int);
extern int anim_clip_arbiter(unsigned char *e, int v, float a, float b);
extern void func_001749A0(unsigned char *e, int id, int z, float f);
extern void func_00178B90(unsigned char *e, int a);
extern int func_00175900(unsigned char *e, int f);
extern void func_0017C580(unsigned char *e);
extern void func_001764E0(unsigned char *e);
extern void func_00182870(unsigned char *e, int f);
extern void func_00102948(void *dst, void *src);
extern int func_001760C0(unsigned char *e, void *pos, int flag, float thr);
extern void func_00174AC0(unsigned char *e, int n);
extern void func_0017C440(unsigned char *e, int f);
extern void func_0017C540(unsigned char *e);
extern void func_001796C0(unsigned char *e);
extern int func_0017DE20(unsigned char *e);
extern int func_0017F320(unsigned char *e);
extern void func_001FBD50(unsigned char *e, int a, int b, float f);
extern int func_00188550(unsigned char *e);
extern float D_002488A4[];
extern int D_00275B40;
extern unsigned char D_00810700[];

void func_00162190(unsigned char *e)
{
    unsigned char st;
    short t;

    st = e[6];
    switch (st) {
    case 0:
        func_00162080(e);
        if (e[0x1F1] == 0) {
            float f = *(float *)(e + 0x254);
            *(float *)(e + 0x258) = f;
            if (f < D_002488A4[0]) {
                e[6] = 0xA;
                func_0017D940();
            } else {
                e[6] = 0x14;
                e[0x25C] = 1;
                func_0017DAF0();
            }
        } else {
            e[6] = 0x1E;
            *(float *)(e + 0x258) = *(float *)(e + 0x254) - 20.5f;
            func_0017DC80();
        }
        if (e[0x25C] == 3) {
            *(float *)0x70003A20 = (float) func_001C61D0(*(int *)(e + 0x40), 0x69);
            anim_clip_arbiter(e, 0x69, 0.0f, *(float *)0x70003A20 - 4.0f);
        } else {
            func_001749A0(e, 0x6A, 0, 0.0f);
        }
        func_0017DEB0(e);
        break;
    case 10:
        if (*(int *)(e + 0x200) & 0x1000) {
            e[6] = st + 1;
            if (e[0x25C] == 3) {
                *(float *)(e + 0x38) = 6.3f;
                func_001749A0(e, 0x6B, 0, 0.0f);
            } else {
                *(float *)(e + 0x38) = 4.1f;
                func_001749A0(e, 0x6C, 0, 0.0f);
            }
            func_00178B90(e, 1);
        }
        break;
    case 11:
        t = *(short *)(e + 0x28);
        *(short *)(e + 0x28) = t - 1;
        if (t == 0) {
            e[6] = e[6] + 1;
            *(float *)(e + 0xB4) = *(float *)(e + 0x258);
            *(float *)(e + 0xB0) = *(float *)(e + 0x2F4);
            *(float *)(e + 0xB8) = *(float *)(e + 0x2F8);
            *(float *)(e + 0xC4) = *(float *)(e + 0x218);
        } else {
            *(float *)(e + 0xB4) += *(float *)(e + 0x2E4);
            *(float *)(e + 0xB0) += *(float *)(e + 0x2E0);
            *(float *)(e + 0xB8) += *(float *)(e + 0x2E8);
            *(float *)(e + 0xB4) += *(float *)(e + 0x26C);
            *(float *)(e + 0x26C) -= *(float *)(e + 0x270);
        }
        if (e[0x25C] == 3) {
            *(float *)(e + 0x204) = 2.0f;
        }
        break;
    case 12:
        *(float *)(e + 0xB4) += -0.2f;
        if (func_00175900(e, 1) != 0) {
            *(float *)(e + 0x2F4) = 1.0f + *(float *)(e + 0xB4);
            func_0017C580(e);
            break;
        }
        *(float *)(e + 0x2F4) = *(float *)(e + 0xB4);
        e[5] = 7;
        e[6] = 0;
        e[0x1F0] = 0xD;
        break;
    case 20:
        if (*(int *)(e + 0x200) & 0x1000) {
            e[6] = st + 1;
            *(float *)(e + 0x38) = 4.1f;
            func_00178B90(e, 1);
            func_001749A0(e, 0x6C, 0, 0.0f);
            break;
        }
        if (D_00810700[0] == 2) {
            func_001764E0(e);
        }
        break;
    case 21:
        *(short *)(e + 0x28) = *(short *)(e + 0x28) - 1;
        *(float *)(e + 0xB0) += *(float *)(e + 0x2E0);
        *(float *)(e + 0xB8) += *(float *)(e + 0x2E8);
        *(float *)(e + 0xB4) += *(float *)(e + 0x2E4);
        *(float *)(e + 0xB4) += *(float *)(e + 0x26C);
        *(float *)(e + 0x26C) -= *(float *)(e + 0x270);
        if (*(short *)(e + 0x28) < 9) {
            e[6] = e[6] + 1;
            func_001749A0(e, 0x7D, 0, 8.0f);
            *(float *)(e + 0xC4) = *(float *)(e + 0x218);
        } else {
            *(float *)(e + 0x204) = 2.0f;
        }
        if (D_00810700[0] == 2) {
            func_001764E0(e);
        }
        break;
    case 22:
        t = *(short *)(e + 0x28);
        *(short *)(e + 0x28) = t - 1;
        if (t == 0) {
            *(float *)(e + 0xB0) = *(float *)(e + 0x2F4);
            *(float *)(e + 0xB8) = *(float *)(e + 0x2F8);
            *(float *)(e + 0xB4) = *(float *)(e + 0x258);
            *(float *)(e + 0xB4) += -0.2f;
            if (func_00175900(e, 1) != 0) {
                e[6] = e[6] + 1;
                func_00182870(e, 1);
                break;
            }
            *(float *)(e + 0x2F4) = *(float *)(e + 0xB4);
            e[5] = 7;
            e[6] = 0;
            e[0x1F0] = 0xD;
            break;
        }
        *(float *)(e + 0xB0) += *(float *)(e + 0x2E0);
        *(float *)(e + 0xB8) += *(float *)(e + 0x2E8);
        *(float *)(e + 0xB4) += 0.5f;
        break;
    case 23:
        if (*(int *)(e + 0x200) & 0x1000) {
            func_00102948(e + 0xB0, (char *)*(int *)(D_00275B40 + 4) + 0xC0);
            *(float *)(e + 0xB4) -= 11.0f;
            func_001749A0(e, 0x8C, 0, 0.0f);
            if (func_001760C0(e, e + 0xB0, 1, 18.0f) != 0) {
                e[0x236] = 1;
                e[0x235] = e[0x235] | 2;
            }
            func_00174AC0(e, 0);
            if (e[0x23F] > 1) {
                e[6] = e[6] + 1;
                func_0017C440(e, 1);
            } else {
                e[0x25C] = 0;
                func_0017C540(e);
                func_001764E0(e);
            }
            *(float *)(e + 0xB4) += -0.2f;
            func_00175900(e, 1);
            func_001796C0(e);
            break;
        }
        if (D_00810700[0] == 2) {
            func_001764E0(e);
        }
        break;
    case 24:
        func_00178B90(e, 1);
        if (!(*(int *)(e + 0x200) & 0x8000)) {
            func_0017C540(e);
        }
        *(float *)(e + 0xB4) += -0.2f;
        func_00175900(e, 1);
        func_001796C0(e);
        break;
    case 30:
        if (*(int *)(e + 0x200) & 0x1000) {
            e[6] = st + 1;
            if (e[0x25C] == 3) {
                *(float *)(e + 0x38) = 6.3f;
                func_001749A0(e, 0x6B, 0, 0.0f);
            } else {
                *(float *)(e + 0x38) = 4.1f;
                func_001749A0(e, 0x6C, 0, 0.0f);
            }
            func_00178B90(e, 1);
        }
        break;
    case 31:
        func_0017DE20(e);
        if (*(short *)(e + 0x28) < 9) {
            e[6] = e[6] + 1;
            func_001749A0(e, 0x7A, 0, 8.0f);
            *(float *)(e + 0xC4) = *(float *)(e + 0x218);
            break;
        }
        if (e[0x25C] == 3) {
            *(float *)(e + 0x204) = 3.0f;
        }
        break;
    case 32:
        if (func_0017DE20(e) != 0) {
            if (func_0017F320(e) != 0) {
                e[6] = 0x22;
                break;
            }
            e[6] = e[6] + 1;
            func_001FBD50(e, 0xFF, 0, 300.0f);
        }
        break;
    case 33:
        if (func_0017F320(e) != 0) {
            e[6] = 0x22;
            break;
        }
        if (*(int *)(e + 0x200) & 0x1000) {
            e[5] = 9;
            e[6] = 0;
            e[0x1F0] = 0x10;
            e[0xD] = 0;
            func_001749A0(e, func_00188550(e), 0, 16.0f);
        }
        break;
    case 34:
        *(float *)(e + 0x2F4) = *(float *)(e + 0xB4);
        e[5] = 7;
        e[6] = 0;
        e[0x1F0] = 0xD;
        *(float *)(e + 0x2EC) = -0.2f;
        break;
    }
}
