// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// SEMANTICS: Actor/NPC scripted-sequence state machine, dispatched on the state
// byte e[6] through jtbl_002737B0 (14 entries; 7/8/9 are empty and fall straight
// to the shared tail, index >= 14 skips the switch entirely).
//   0  - announce: func_001FBD50(e,0x152) + func_001FBD50(e,0x148) at 300.0f,
//        advance state, clear e[7]. func_0021D1A0(e) picks the variant: nonzero
//        sets e[0xD]=1 and plays anim 0x5A/0x26 (by e[0x236]); zero sets e[0xD]=0
//        and plays 0x58/0x22. Then clears the three float accumulators e[0x21C],
//        e[0x2E4], e[0x2EC] and calls func_001764E0(e).
//   1  - on input bit 0x1000 in e[0x200]: advance by 2 when e[0x236] else by 1,
//        then func_001764E0(e). Otherwise integrate: e[0x38] = clock[2]-e[0x21C]
//        (clock = *D_00275B40), latch e[0x21C], scale e[0x38] by 0.45f, drive
//        func_00178B90(e,1), then e[0x2EC] = clock[1]-e[0x2E4], latch e[0x2E4]
//        and accumulate e[0xB4] += e[0x2EC]. Both arms end with func_00175900(e,1).
//   2  - func_00178B90(e,1) + func_00179880(e,&e[0x2EC]); when func_00175900(e,1)
//        reports done, advance, play 0x23/0x27 (by e[0xD]) at 0.0f, clear e[0x21C],
//        func_001FBD50(e,0x12F,300.0f) and func_001B61C0(0,0xD0,0xA,1).
//   3  - on input 0x1000: if e[0x220] <= 0.0f jump to state 10 and play 0x25/0x29
//        at 4.0f; otherwise advance and play 0x59/0x24/0x5B/0x28 selected by
//        e[0xD] then e[0x236] (16.0f for the e[0x236] variants, else 4.0f);
//        either way clear e[0x21C]. Without the input, integrate e[0x38] and run
//        func_00178B90(e,0). Shared tail: func_001764E0, func_00179880, func_00175900.
//   4  - advance once input bit 0x8000 clears; same shared tail.
//   5  - while e[0x3C] <= 10.0f advance and func_001FBD50(e,0x151,300.0f), else
//        integrate + func_00178B90(e,0); same shared tail.
//   6  - on input 0x1000: clear e[0xF], arm the 0x3C-tick timer e[0x20E], clear
//        e[0x25C] and func_0017C540(e); else integrate; same shared tail.
//   10 - advance and func_001FBD50(e,0x156,300.0f) once input 0x8000 clears.
//   11 - while e[0x3C] <= 18.0f advance and func_0021D490(e).
//   12 - on input 0x1000: advance, clear e[7] and e[0x2EC], func_001B61C0(1,0xEE,0x3C,1).
//   13 - func_0021D2E0(e, 0x78, 0).
// Tail (all states): while e[6] < 10 and e[0x23A] == 0x5D, func_0021D250(e, 1).

extern int func_001749A0(unsigned char *e, int id, int b, float f);
extern int func_001FBD50(unsigned char *e, int id, int b, float f);
extern int func_001764E0();
extern int func_00175900();
extern int func_00178B90();
extern int func_00179880();
extern int func_0017C540();
extern int func_001B61C0();
extern int func_0021D1A0();
extern int func_0021D250();
extern int func_0021D2E0();
extern int func_0021D490();

extern float **D_00275B40;

void func_0021DBB0(unsigned char *e) {
    unsigned char st = e[6];

    switch (st) {
    case 0:
        func_001FBD50(e, 0x152, 0, 300.0f);
        func_001FBD50(e, 0x148, 0, 300.0f);
        e[6] = e[6] + 1;
        e[7] = 0;
        if (func_0021D1A0(e) != 0) {
            e[0xD] = 1;
            if (e[0x236] != 0) {
                func_001749A0(e, 0x5A, 0, 1.0f);
            } else {
                func_001749A0(e, 0x26, 0, 1.0f);
            }
        } else {
            e[0xD] = 0;
            if (e[0x236] != 0) {
                func_001749A0(e, 0x58, 0, 1.0f);
            } else {
                func_001749A0(e, 0x22, 0, 1.0f);
            }
        }
        *(float *)(e + 0x21C) = 0.0f;
        *(float *)(e + 0x2E4) = 0.0f;
        *(float *)(e + 0x2EC) = 0.0f;
        func_001764E0(e);
        break;
    case 1:
        if (*(int *)(e + 0x200) & 0x1000) {
            if (e[0x236] != 0) {
                e[6] = st + 2;
            } else {
                e[6] = st + 1;
            }
            func_001764E0(e);
        } else {
            *(float *)(e + 0x38) = (*D_00275B40)[2] - *(float *)(e + 0x21C);
            *(float *)(e + 0x21C) = (*D_00275B40)[2];
            *(float *)(e + 0x38) *= 0.45f;
            func_00178B90(e, 1);
            *(float *)(e + 0x2EC) = (*D_00275B40)[1] - *(float *)(e + 0x2E4);
            *(float *)(e + 0x2E4) = (*D_00275B40)[1];
            *(float *)(e + 0xB4) += *(float *)(e + 0x2EC);
        }
        func_00175900(e, 1);
        break;
    case 2:
        func_00178B90(e, 1);
        func_00179880(e, e + 0x2EC);
        if (func_00175900(e, 1) != 0) {
            e[6] = e[6] + 1;
            if (e[0xD] == 0) {
                func_001749A0(e, 0x23, 0, 0.0f);
            } else {
                func_001749A0(e, 0x27, 0, 0.0f);
            }
            *(float *)(e + 0x21C) = 0.0f;
            func_001FBD50(e, 0x12F, 0, 300.0f);
            func_001B61C0(0, 0xD0, 0xA, 1);
        }
        break;
    case 3:
        if (*(int *)(e + 0x200) & 0x1000) {
            if (*(float *)(e + 0x220) <= 0.0f) {
                e[6] = 0xA;
                if (e[0xD] == 0) {
                    func_001749A0(e, 0x25, 0, 4.0f);
                } else {
                    func_001749A0(e, 0x29, 0, 4.0f);
                }
            } else {
                e[6] = st + 1;
                if (e[0xD] == 0) {
                    if (e[0x236] != 0) {
                        func_001749A0(e, 0x59, 0, 16.0f);
                    } else {
                        func_001749A0(e, 0x24, 0, 4.0f);
                    }
                } else {
                    if (e[0x236] != 0) {
                        func_001749A0(e, 0x5B, 0, 16.0f);
                    } else {
                        func_001749A0(e, 0x28, 0, 4.0f);
                    }
                }
            }
            *(float *)(e + 0x21C) = 0.0f;
        } else {
            *(float *)(e + 0x38) = (*D_00275B40)[2] - *(float *)(e + 0x21C);
            *(float *)(e + 0x21C) = (*D_00275B40)[2];
            func_00178B90(e, 0);
        }
        func_001764E0(e);
        func_00179880(e, e + 0x2EC);
        func_00175900(e, 1);
        break;
    case 4:
        if (!(*(int *)(e + 0x200) & 0x8000)) {
            e[6] = st + 1;
        }
        func_001764E0(e);
        func_00179880(e, e + 0x2EC);
        func_00175900(e, 1);
        break;
    case 5:
        if (*(float *)(e + 0x3C) <= 10.0f) {
            e[6] = st + 1;
            func_001FBD50(e, 0x151, 0, 300.0f);
        } else {
            *(float *)(e + 0x38) = (*D_00275B40)[2] - *(float *)(e + 0x21C);
            *(float *)(e + 0x21C) = (*D_00275B40)[2];
            func_00178B90(e, 0);
        }
        func_001764E0(e);
        func_00179880(e, e + 0x2EC);
        func_00175900(e, 1);
        break;
    case 6:
        if (*(int *)(e + 0x200) & 0x1000) {
            e[0xF] = 0;
            *(short *)(e + 0x20E) = 0x3C;
            e[0x25C] = 0;
            func_0017C540(e);
        } else {
            *(float *)(e + 0x38) = (*D_00275B40)[2] - *(float *)(e + 0x21C);
            *(float *)(e + 0x21C) = (*D_00275B40)[2];
            func_00178B90(e, 0);
        }
        func_001764E0(e);
        func_00179880(e, e + 0x2EC);
        func_00175900(e, 1);
        break;
    case 7:
    case 8:
    case 9:
        break;
    case 10:
        if (!(*(int *)(e + 0x200) & 0x8000)) {
            e[6] = st + 1;
            func_001FBD50(e, 0x156, 0, 300.0f);
        }
        break;
    case 11:
        if (*(float *)(e + 0x3C) <= 18.0f) {
            e[6] = st + 1;
            func_0021D490(e);
        }
        break;
    case 12:
        if (*(int *)(e + 0x200) & 0x1000) {
            e[6] = st + 1;
            e[7] = 0;
            *(float *)(e + 0x2EC) = 0.0f;
            func_001B61C0(1, 0xEE, 0x3C, 1);
        }
        break;
    case 13:
        func_0021D2E0(e, 0x78, 0);
        break;
    }
    if (e[6] < 0xA && e[0x23A] == 0x5D) {
        func_0021D250(e, 1);
    }
}
