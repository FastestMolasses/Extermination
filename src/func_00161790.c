// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Player/entity behaviour state machine, dispatched on the state byte at e+6
// (cases 0,10,11,12,13,14,20,21,22,23,24,30,31,32,33; mwcc reverses the
// ascending case labels into a descending beq chain). Sibling of
// func_00162190/func_00162A40. Case 0 classifies the drop height at e+0x254
// against the descending thresholds D_002488B0..A0, picking the next state
// (30 / 20 / 10) and the sub-index e[0x2F1], then kicks clip 0x70. Cases
// 10-14 and 20-24 are two parallel land/recover sequences (the 20 run plays
// the voice line D_002754A0[e[0x2F1]] and a 300-radius sound via
// func_001FBD50); 30-33 is the finish/exit run (33 commits e+0xB4 to e+0x2F4
// and resets to state 0 / mode 0xD).
extern void func_00161690(unsigned char *e);
extern void anim_eval_skeleton(unsigned char *e);
extern void func_001749A0(unsigned char *e, int id, int z, float f);
extern void func_00174AC0(unsigned char *e, int n);
extern int func_00175900(unsigned char *e, int f);
extern int func_001760C0(unsigned char *e, void *pos, int flag, float thr);
extern void func_001764E0(unsigned char *e);
extern void func_00178B90(unsigned char *e, int a);
extern void func_001796C0(unsigned char *e);
extern void func_0017C440(unsigned char *e, int f);
extern void func_0017C540(unsigned char *e);
extern void func_0017D800(unsigned char *e);
extern int func_0017D8D0(unsigned char *e);
extern void func_0017DEB0(unsigned char *e);
extern int func_0017F320(unsigned char *e);
extern void func_00182870(unsigned char *e, int f);
extern int func_00188550(unsigned char *e);
extern void func_001FBD50(unsigned char *e, int a, int b, float f);
extern float D_00248540[];
extern float D_00248550[];
extern float D_002488A0[];
extern float D_002488A4[];
extern float D_002488A8[];
extern float D_002488AC[];
extern float D_002488B0[];
extern short D_002754A0[4];
extern int D_00275B40;
extern unsigned char D_00810700[];

void func_00161790(unsigned char *e)
{
    unsigned char st;

    st = e[6];
    switch (st) {
    case 0:
        func_00161690(e);
        {
            float f = *(float *)(e + 0x254);
            if (f > D_002488B0[0]) {
                e[6] = 0x1E;
                *(float *)(e + 0x258) = *(float *)(e + 0x254) - 20.5f;
            } else if (f > D_002488AC[0]) {
                e[6] = 0x14;
                *(float *)(e + 0x258) = *(float *)(e + 0x254) - D_002488AC[0];
                e[0x2F1] = 2;
            } else if (f > D_002488A8[0]) {
                e[6] = 0x14;
                *(float *)(e + 0x258) = *(float *)(e + 0x254) - D_002488A8[0];
                e[0x2F1] = 1;
            } else if (f > D_002488A4[0]) {
                e[6] = 0x14;
                *(float *)(e + 0x258) = *(float *)(e + 0x254) - D_002488A4[0];
                e[0x2F1] = 0;
            } else {
                e[6] = 0xA;
                *(float *)(e + 0x258) = 1.0f + (*(float *)(e + 0x254) - D_002488A0[0]);
            }
        }
        func_0017D800(e);
        func_001749A0(e, 0x70, 0, 0.0f);
        break;
    case 10:
        if (*(float *)(e + 0x3C) <= 14.0f) {
            e[6] = st + 1;
            if (e[0x25C] == 3) {
                func_001749A0(e, 0x8B, 0, 1.0f);
            } else {
                func_001749A0(e, 0x8A, 0, 1.0f);
            }
            func_0017DEB0(e);
        }
        break;
    case 11:
        if (func_0017D8D0(e) != 0) {
            e[6] = e[6] + 1;
        }
        break;
    case 12:
        if (*(int *)(e + 0x200) & 0x1000) {
            e[0x25F] = 0;
            anim_eval_skeleton(e);
            *(float *)(e + 0xB4) = *(float *)((char *)*(int *)(D_00275B40 + 4) + 0xC4) - 10.0f;
            *(float *)(e + 0x38) = 1.0f + *(float *)((char *)*(int *)(D_00275B40 + 4) + 8);
            func_00178B90(e, 1);
            func_001749A0(e, 0x8D, 0, 0.0f);
            *(float *)(e + 0xB4) += -0.4f;
            if (func_00175900(e, 1) != 0) {
                e[6] = e[6] + 1;
                break;
            }
            *(float *)(e + 0x2F4) = *(float *)(e + 0xB4);
            e[5] = 7;
            e[6] = 0;
            e[0x1F0] = 0xD;
        }
        break;
    case 13:
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
        func_00182870(e, 1);
        break;
    case 14:
        func_00178B90(e, 1);
        if (!(*(int *)(e + 0x200) & 0x8000)) {
            func_0017C540(e);
        }
        *(float *)(e + 0xB4) += -0.2f;
        func_00175900(e, 1);
        func_001796C0(e);
        break;
    case 20:
        if (*(float *)(e + 0x3C) <= D_00248540[e[0x2F1]]) {
            e[6] = st + 1;
            func_001749A0(e, D_002754A0[e[0x2F1]], 0, 8.0f);
            func_0017DEB0(e);
            if (e[0x2F1] < 2) {
                func_001FBD50(e, 0x12B, 0, 300.0f);
            } else {
                func_001FBD50(e, 0x12C, 0, 300.0f);
            }
        }
        if (D_00810700[0] == 2) {
            func_001764E0(e);
        }
        break;
    case 21:
        if (func_0017D8D0(e) != 0) {
            e[6] = e[6] + 1;
        }
        if (D_00810700[0] == 2) {
            func_001764E0(e);
        }
        break;
    case 22:
        if (*(int *)(e + 0x200) & 0x1000) {
            e[0x25F] = 0;
            anim_eval_skeleton(e);
            *(float *)(e + 0xB4) = *(float *)((char *)*(int *)(D_00275B40 + 4) + 0xC4) - 11.0f;
            *(float *)(e + 0x38) = *(float *)((char *)*(int *)(D_00275B40 + 4) + 8);
            func_00178B90(e, 1);
            func_001749A0(e, 0x8C, 0, 0.0f);
            *(float *)(e + 0xB4) += -0.4f;
            if (func_00175900(e, 1) != 0) {
                e[6] = e[6] + 1;
            } else {
                *(float *)(e + 0x2F4) = *(float *)(e + 0xB4);
                e[5] = 7;
                e[6] = 0;
                e[0x1F0] = 0xD;
            }
            if (func_001760C0(e, e + 0xB0, 1, 18.0f) != 0) {
                e[0x236] = 1;
                e[0x235] = e[0x235] | 2;
            }
            break;
        }
        if (*(float *)(e + 0x3C) <= D_00248550[e[0x2F1]]) {
            *(float *)(e + 0x204) = 0.75f;
        }
        if (D_00810700[0] == 2) {
            func_001764E0(e);
        }
        break;
    case 23:
        func_00174AC0(e, 0);
        if (e[0x23F] > 1) {
            e[6] = e[6] + 1;
            func_0017C440(e, 1);
        } else {
            func_001764E0(e);
            e[0x25C] = 0;
            func_0017C540(e);
        }
        *(float *)(e + 0xB4) += -0.2f;
        func_00175900(e, 1);
        func_00182870(e, 1);
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
            func_001749A0(e, 0x71, 0, 1.0f);
            func_0017DEB0(e);
        }
        break;
    case 31:
        if (func_0017D8D0(e) != 0) {
            if (func_0017F320(e) != 0) {
                e[6] = 0x21;
                break;
            }
            e[6] = e[6] + 1;
            func_001FBD50(e, 0xFE, 0, 300.0f);
            func_001749A0(e, 0x7A, 0, 1.0f);
        }
        break;
    case 32:
        if (func_0017F320(e) != 0) {
            e[6] = 0x21;
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
    case 33:
        *(float *)(e + 0x2F4) = *(float *)(e + 0xB4);
        e[5] = 7;
        e[6] = 0;
        e[0x1F0] = 0xD;
        *(float *)(e + 0x2EC) = -0.2f;
        break;
    }
}
