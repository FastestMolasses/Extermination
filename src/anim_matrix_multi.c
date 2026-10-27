// NEARMISS anim_matrix_multi  (vram 0x00148B40, 0x1010 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.88% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). Object similarity does not prove semantic equivalence.
// Remaining differences in this candidate:
// Two loop-bottom branches only (same size, same registers): the first blend loop in each arm (into D_00287140, weight wy) ends in a branch-likely in the original and a plain branch here, i.e. mwcc saw the first argument register live on that loop's exit in the original. Fixed this wave (was 96.60%): every scratchpad word is a relocated extern (idiom-32: 0x70003A20/24 plain, the 0x700038A0..BC words volatile, opted in with // SPAD:), func_001B1560 takes the 0.7853982f range as a third (float) argument, the D_008104E4 == 1 test shares the e[5] = 8 tail (the not-found case leaves with break), float tests use if (turn) / if (!x), and the vec4 copy temporaries are declared last-to-first (a second block-local set for the second copy).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x700038A0 0x700038A4 0x700038A8 0x700038AC 0x700038B0 0x700038B4 0x700038B8 0x700038BC 0x700036D0 0x700036D4 0x700036D8 0x700036DC

//
// Enemy "circling flyer" AI update (entity `e`, per-entity AI state `st`).
// Sub-state machine on e[6]:
//   0 = spawn: reset spin timer, kick off approach SFX/anim (variant-dependent).
//   1 = wait for the "ready" flag (st+0x20 & 0x1000), then randomise the two
//       effect countdowns (st+0x28 / st+0x2C) and arm the 300-frame attack timer.
//   2 = main circling/strafing behaviour: steer yaw toward the player, animate
//       three interpolated pose buffers (D_00287140/D_00286340 -> D_00288D40),
//       spawn trail effects on the two countdowns, and either dive-attack when
//       close (st+0x68 <= 20) or fall through to the next sub-state.
//   3 = death/reset: wait for the flag, then rewind to e[5]=0.
// Tail: whenever `turn` is non-zero, blend the entity yaw (e+0xC4) toward the
// player bearing by `turn` radians/frame.
//
// D_00810360/364/368 = player world position.
// D_00245B40[] = 2x9 int table of animation ids, row = "large variant" bit
//                (e[0xD] & 0x80), stride 0x24.
// 0x700036A0 / 0x700038A0 / 0x700038B0 = scratchpad matrix + two vec4 temps.

extern float D_00810360;
extern float D_00810364;
extern float D_00810368;
extern unsigned char D_008104E4;
extern unsigned char D_0081070A;

extern int D_00245B40[];
extern int D_00245B44[];
extern int D_00245B48[];
extern int D_00245B4C[];
extern int D_00245B50[];
extern int D_00245B54[];
extern int D_00245B58[];
extern int D_00245B5C[];
extern int D_00245B60[];

extern char D_00286340[];
extern char D_00287140[];
extern char D_00287F40[];
extern char D_00288D40[];
extern float D_70003A20[4];
extern float D_70003A24[4];
extern char D_700036A0[];
extern volatile float D_700038A0[4];
extern volatile float D_700038A4[4];
extern volatile float D_700038A8[4];
extern volatile float D_700038AC[4];
extern volatile float D_700038B0[4];
extern volatile float D_700038B4[4];
extern volatile float D_700038B8[4];
extern volatile float D_700038BC[4];
extern float D_700036D0[4];
extern float D_700036D4[4];
extern float D_700036D8[4];
extern float D_700036DC[4];

extern int func_00122BB8(void);
extern int func_001B1560(void *e, void *target, float range);
extern void copy_qw4(void *dst, void *src);
extern void func_001026A0(void *dst, void *a, void *b);
extern void func_001028B8(void *dst, void *a, void *b);
extern float func_0011DF78(float x);
extern float func_0011E620(float x, float z);
extern float func_0011E748(float x);
extern int float_to_int(float t);
extern int func_0019A570(void *a, void *b, int c, int d);
extern float func_001B1240(void *pos, float px, float pz);
extern float func_001B12B0(float target, float cur, float step);
extern float func_001B1470(float a);
extern void func_001B2B10(void *e, void *a, void *b);
extern void func_001B3C30(void *e, void *m, void *c, float a, float b);
extern void anim_clip_init(void *e, int id, float a, float b);
extern void func_001C81C0(int a, int clip, int frame, void *dst);
extern void func_001C82A0(void *e, void *pose);
extern void func_001C9D50(void *dst, void *a, void *b, float t);
extern void func_001FBD50(void *e, int id, int flag, float a);

void anim_matrix_multi(unsigned char *e, unsigned char *st) {
    float turn;
    float wy;
    float wp;
    float r;
    float q3;
    float q2;
    float q1;
    float q0;
    int i;

    turn = 0.0f;
    switch (e[6]) {
    case 0:
        e[6] = e[6] + 1;
        *(float *)(e + 0x3C) = 0.0f;
        st[0x83] = 0;
        st[0x88] = 1;
        *(int *)(st + 0x50) = 0;
        *(int *)(st + 0x4C) = 0;
        if (e[0xD] & 0x80) {
            anim_clip_init(e, 0xB, 5.0f, turn);
        } else {
            anim_clip_init(e, 9, 5.0f, turn);
        }
        break;
    case 1:
        turn = 0.034906585f;
        if (*(int *)(st + 0x20) & 0x1000) {
            e[6] = e[6] + 1;
            *(float *)(st + 0x54) = 0.0f;
            *(float *)(st + 0x58) = 0.0f;
            *(float *)(st + 0x64) = 0.0f;
            *(int *)(st + 0x28) = (func_00122BB8() >> 5) & 7;
            *(int *)(st + 0x2C) = (func_00122BB8() >> 4) & 7;
            *(int *)(st + 0x30) = 300;
            st[0x83] = 1;
        }
        break;
    case 2:
        turn = 0.0034906587f;
        if (!*(float *)(st + 0x64)) {
            D_70003A20[0] = D_00810360 - *(float *)(e + 0xB0);
            D_70003A24[0] = D_00810368 - *(float *)(e + 0xB8);
                        *(float *)(st + 0x60) = func_001B1470(
                func_0011E620(D_70003A20[0], D_70003A24[0]) - *(float *)(e + 0xC4));
            if (*(float *)(st + 0x58) > *(float *)(st + 0x60)) {
                *(float *)(st + 0x64) = -0.011344640f;
            } else {
                *(float *)(st + 0x64) = 0.011344640f;
            }
        }
        *(float *)(st + 0x58) += *(float *)(st + 0x64);
        if (!(func_0011DF78(*(float *)(st + 0x58)) < 0.7853982f)) {
            *(float *)(st + 0x64) = 0.0f;
            if (func_001B1560(e, &D_00810360, 0.7853982f) == 0) {
                *(int *)(st + 0x30) = 0;
            }
        }
        D_70003A20[0] = D_00810360 - *(float *)(e + 0xB0);
        D_70003A24[0] = D_00810368 - *(float *)(e + 0xB8);
                D_70003A20[0] =
            func_0011E748(D_70003A20[0] * D_70003A20[0] + D_70003A24[0] * D_70003A24[0]);
        D_70003A24[0] = *(float *)(e + 0xB4);
        if (e[0xD] & 0x80) {
            D_70003A24[0] += 10.0f;
        } else {
            D_70003A24[0] += 30.0f;
        }
        D_70003A24[0] -= D_00810364;
        r = func_001B1470(func_0011E620(D_70003A24[0], D_70003A20[0]));
        *(float *)(st + 0x5C) = r;
        if (r < -0.7853982f) {
            *(float *)(st + 0x5C) = -0.7853982f;
        } else if (!(r <= 0.7853982f)) {
            *(float *)(st + 0x5C) = 0.7853982f;
        }
        *(float *)(st + 0x54) =
            func_001B12B0(*(float *)(st + 0x5C), *(float *)(st + 0x54), 0.017453292f);
        r = func_0011DF78(*(float *)(st + 0x54));
        wy = r;
        if (!(r <= 1.0f)) {
            wy = 1.0f;
        }
        r = func_0011DF78(*(float *)(st + 0x58));
        wp = r;
        if (!(r <= 1.0f)) {
            wp = 1.0f;
        }
        if (*(float *)(st + 0x54) < 0.0f) {
            func_001C81C0(*(int *)(e + 0x40), D_00245B50[((int)(e[0xD] & 0x80) >> 7) * 9],
                          float_to_int(*(float *)(e + 0x3C)), D_00288D40);
            func_001C81C0(*(int *)(e + 0x40), D_00245B5C[((int)(e[0xD] & 0x80) >> 7) * 9],
                          float_to_int(*(float *)(e + 0x3C)), D_00287F40);
            for (i = 0; i < e[0xC]; i++) {
                func_001C9D50(&D_00287140[i * 0x40], &D_00288D40[i * 0x40],
                              &D_00287F40[i * 0x40], wy);
            }
            if (*(float *)(st + 0x58) < 0.0f) {
                func_001C81C0(*(int *)(e + 0x40), D_00245B4C[((int)(e[0xD] & 0x80) >> 7) * 9],
                              float_to_int(*(float *)(e + 0x3C)), D_00288D40);
                func_001C81C0(*(int *)(e + 0x40), D_00245B58[((int)(e[0xD] & 0x80) >> 7) * 9],
                              float_to_int(*(float *)(e + 0x3C)), D_00287F40);
                for (i = 0; i < e[0xC]; i++) {
                    func_001C9D50(&D_00286340[i * 0x40], &D_00288D40[i * 0x40],
                                  &D_00287F40[i * 0x40], wy);
                }
            } else {
                func_001C81C0(*(int *)(e + 0x40), D_00245B54[((int)(e[0xD] & 0x80) >> 7) * 9],
                              float_to_int(*(float *)(e + 0x3C)), D_00288D40);
                func_001C81C0(*(int *)(e + 0x40), D_00245B60[((int)(e[0xD] & 0x80) >> 7) * 9],
                              float_to_int(*(float *)(e + 0x3C)), D_00287F40);
                for (i = 0; i < e[0xC]; i++) {
                    func_001C9D50(&D_00286340[i * 0x40], &D_00288D40[i * 0x40],
                                  &D_00287F40[i * 0x40], wy);
                }
            }
            for (i = 0; i < e[0xC]; i++) {
                func_001C9D50(&D_00288D40[i * 0x40], &D_00287140[i * 0x40],
                              &D_00286340[i * 0x40], wp);
            }
        } else {
            func_001C81C0(*(int *)(e + 0x40), D_00245B50[((int)(e[0xD] & 0x80) >> 7) * 9],
                          float_to_int(*(float *)(e + 0x3C)), D_00288D40);
            func_001C81C0(*(int *)(e + 0x40), D_00245B44[((int)(e[0xD] & 0x80) >> 7) * 9],
                          float_to_int(*(float *)(e + 0x3C)), D_00287F40);
            for (i = 0; i < e[0xC]; i++) {
                func_001C9D50(&D_00287140[i * 0x40], &D_00288D40[i * 0x40],
                              &D_00287F40[i * 0x40], wy);
            }
            if (*(float *)(st + 0x58) < 0.0f) {
                func_001C81C0(*(int *)(e + 0x40), D_00245B4C[((int)(e[0xD] & 0x80) >> 7) * 9],
                              float_to_int(*(float *)(e + 0x3C)), D_00288D40);
                func_001C81C0(*(int *)(e + 0x40), D_00245B40[((int)(e[0xD] & 0x80) >> 7) * 9],
                              float_to_int(*(float *)(e + 0x3C)), D_00287F40);
                for (i = 0; i < e[0xC]; i++) {
                    func_001C9D50(&D_00286340[i * 0x40], &D_00288D40[i * 0x40],
                                  &D_00287F40[i * 0x40], wy);
                }
            } else {
                func_001C81C0(*(int *)(e + 0x40), D_00245B54[((int)(e[0xD] & 0x80) >> 7) * 9],
                              float_to_int(*(float *)(e + 0x3C)), D_00288D40);
                func_001C81C0(*(int *)(e + 0x40), D_00245B48[((int)(e[0xD] & 0x80) >> 7) * 9],
                              float_to_int(*(float *)(e + 0x3C)), D_00287F40);
                for (i = 0; i < e[0xC]; i++) {
                    func_001C9D50(&D_00286340[i * 0x40], &D_00288D40[i * 0x40],
                                  &D_00287F40[i * 0x40], wy);
                }
            }
            for (i = 0; i < e[0xC]; i++) {
                func_001C9D50(&D_00288D40[i * 0x40], &D_00287140[i * 0x40],
                              &D_00286340[i * 0x40], wp);
            }
        }
        *(float *)(e + 0x3C) += 1.0f;
        if (!(*(float *)(e + 0x3C) < 25.0f)) {
            *(float *)(e + 0x3C) = 0.0f;
        }
        func_001C82A0(e, D_00288D40);
        if (*(int *)(st + 0x28) != 0) {
            *(int *)(st + 0x28) = *(int *)(st + 0x28) - 1;
        } else {
            if (e[0xD] & 0x80) {
                *(int *)(st + 0x28) = 10;
            } else {
                *(int *)(st + 0x28) = 7;
            }
            copy_qw4(D_700036A0, (void *)(*(int *)(e + 0x17C) + 0x90));
            D_700038A0[0] = 11.0f;
            D_700038A4[0] = 0.0f;
            D_700038A8[0] = 0.0f;
            D_700038AC[0] = 1.0f;
            func_001026A0(D_700038A0, D_700036A0, D_700038A0);
            q0 = D_700038A0[0];
            q1 = D_700038A4[0];
            q2 = D_700038A8[0];
            q3 = D_700038AC[0];
            D_700036D0[0] = q0;
            D_700036D4[0] = q1;
            D_700036D8[0] = q2;
            D_700036DC[0] = q3;
            D_700038B0[0] = 0.8f;
            D_700038B4[0] = 0.8f;
            D_700038B8[0] = 0.8f;
            D_700038BC[0] = 1.0f;
            if (e[0xD] & 0x80) {
                if (D_0081070A != 0) {
                    func_001B3C30(e, D_700036A0, D_700038B0, 200.0f, 20.0f);
                } else {
                    func_001B3C30(e, D_700036A0, D_700038B0, 200.0f, 15.0f);
                }
            } else if (D_0081070A != 0) {
                func_001B3C30(e, D_700036A0, D_700038B0, 200.0f, 15.0f);
            } else {
                func_001B3C30(e, D_700036A0, D_700038B0, 200.0f, 10.0f);
            }
            func_001FBD50(e, 0x7E5, 0, 300.0f);
        }
        if (e[0xD] & 0x80) {
            if (*(int *)(st + 0x2C) != 0) {
                *(int *)(st + 0x2C) = *(int *)(st + 0x2C) - 1;
            } else {
                float w3;
                float w2;
                float w1;
                float w0;

                *(int *)(st + 0x2C) = 10;
                copy_qw4(D_700036A0, (void *)(*(int *)(e + 0x180) + 0x90));
                D_700038A0[0] = 8.0f;
                D_700038A4[0] = 0.0f;
                D_700038A8[0] = 0.0f;
                D_700038AC[0] = 1.0f;
                func_001026A0(D_700038A0, D_700036A0, D_700038A0);
                w0 = D_700038A0[0];
                w1 = D_700038A4[0];
                w2 = D_700038A8[0];
                w3 = D_700038AC[0];
                D_700036D0[0] = w0;
                D_700036D4[0] = w1;
                D_700036D8[0] = w2;
                D_700036DC[0] = w3;
                D_700038B0[0] = 0.8f;
                D_700038B4[0] = 0.8f;
                D_700038B8[0] = 0.8f;
                D_700038BC[0] = 1.0f;
                if (D_0081070A != 0) {
                    func_001B3C30(e, D_700036A0, D_700038B0, 200.0f, 20.0f);
                } else {
                    func_001B3C30(e, D_700036A0, D_700038B0, 200.0f, 15.0f);
                }
                func_001FBD50(e, 0x7E5, 0, 300.0f);
            }
        }
        if (*(float *)(st + 0x68) <= 20.0f) {
            *(int *)(st + 0x30) = 0;
            st[0x83] = 0;
            e[0x5D] = 1;
            if (D_008104E4 != 1) {
                D_700038A0[0] = 0.0f;
                D_700038A4[0] = 10.0f;
                D_700038A8[0] = 21.0f;
                D_700038AC[0] = 1.0f;
                func_001B2B10(e, D_700038A0, D_700038A0);
                D_700038B0[0] = *(float *)(e + 0xB0);
                D_700038B4[0] = *(float *)(e + 0xB4);
                D_700038B8[0] = *(float *)(e + 0xB8);
                D_700038BC[0] = *(float *)(e + 0xBC);
                D_700038B4[0] += 10.0f;
                func_001028B8(D_700038A0, D_700038B0, D_700038A0);
                if (func_0019A570(D_700038B0, D_700038A0, 6, 0) == 0) {
                    e[5] = 7;
                    e[6] = 0;
                    break;
                }
            }
            e[5] = 8;
            e[6] = 0;
        } else if (*(int *)(st + 0x30) != 0) {
            *(int *)(st + 0x30) = *(int *)(st + 0x30) - 1;
        } else {
            e[6] = e[6] + 1;
            st[0x83] = 0;
            if (e[0xD] & 0x80) {
                anim_clip_init(e, 0xC, 5.0f, 0.0f);
            } else {
                anim_clip_init(e, 0xA, 5.0f, 0.0f);
            }
        }
        break;
    case 3:
        if (*(int *)(st + 0x20) & 0x1000) {
            e[5] = 0;
            e[6] = 0;
            *(int *)(st + 0x3C) = 0;
            *(int *)(st + 0x38) = 0;
            st[0x88] = 0;
        }
        break;
    }
    if (turn) {
        *(float *)(st + 0x44) = func_001B1240(e + 0xB0, D_00810360, D_00810368);
        *(float *)(e + 0xC4) =
            func_001B12B0(*(float *)(st + 0x44), *(float *)(e + 0xC4), turn);
    }
}
