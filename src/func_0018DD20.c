// NEARMISS func_0018DD20  (vram 0x0018DD20, 0x1B48 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.18% via mwcc233 (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// is a genuine compiler artifact that no source change fixes here:
// vec-scale block x6: target interleaves the [0]/[2] chains across f0-f3, hoisting each next load into the mul->add latency gap and above an intervening stack store; no temp arrangement reproduces it (8 forms tried, all worse). Plus an s2/s3 swap on two zero-inited locals.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern float func_00102738(float *, float *);                          
extern void func_00102760(float *, float *);           
extern void func_001028B8(float *, float *, float *);  
extern void func_001028D0(float *, float *, float *);  
extern void func_00102900(float *, float *, float);    
extern void func_00102948(float *, float *);           
extern void func_001031E0(float *, float *);           
extern void func_00103230(float *, float *, float);    
extern float func_0011DE90(float);                     
extern float func_0011DF78(float);                     
extern float func_0011E2A8(float);                     
extern float func_0011E620(float, float);              
extern int func_0019A910(float *, float *, int);       
extern float func_001B1240(float *, float, float);                          
extern float func_001B1470(float);                     

extern float D_00810690;
extern unsigned char D_00810700;
extern float D_700031B0;
extern float D_700038A0;
extern float D_700038B0;
extern float D_700038C0;
extern float D_700038D0;
extern float D_700038E0;
extern float D_700038F0;
extern float D_70003900;
extern float D_70003910;
extern float D_70003920;
extern float D_70003930;
extern float D_70003940;
extern float D_70003950;
extern float D_70003960;

struct A {
    char pad00[0xC];
    float f0C;
    float pos[4];
    float prev[4];
    char pad30[0x20];
    float f50;
    float f54;
    unsigned short h58;
    unsigned short h5A;
    float f5C;
    char pad60[0x30];
    float f90;
};

struct B {
    char pad00[0xA0];
    float eye[4];
    float look[4];
};

int func_0018DD20(struct A *arg0, struct B *arg1, int arg2, int arg3) {
    float spA[4];
    float spB[4];
    float temp_f0;
    float temp_f0_10;
    float temp_f0_11;
    float temp_f0_12;
    float temp_f0_13;
    float temp_f0_14;
    float temp_f0_15;
    float temp_f0_2;
    float temp_f0_3;
    float temp_f0_4;
    float temp_f0_5;
    float temp_f0_6;
    float temp_f0_7;
    float temp_f0_8;
    float temp_f0_9;
    float temp_f1;
    float temp_f1_10;
    float temp_f1_11;
    float temp_f1_12;
    float temp_f1_13;
    float temp_f1_14;
    float temp_f1_2;
    float temp_f1_3;
    float temp_f1_4;
    float temp_f1_5;
    float temp_f1_6;
    float temp_f1_7;
    float temp_f1_8;
    float temp_f1_9;
    float temp_f20;
    float var_f0;
    float var_f0_2;
    float var_f0_3;
    float var_f0_4;
    float var_f0_5;
    float var_f0_6;
    float clampY;
    int temp_v0;
    int temp_v0_2;
    int temp_v1_4;
    int temp_v1_6;
    int probeHit;
    int hitA;
    int touched;
    int steep;
    int resolved;
    int hitB;
    int ret;
    int doSlide;
    unsigned short temp_s3;
    unsigned short temp_s7;
    unsigned short temp_v1;
    unsigned short temp_v1_2;
    unsigned short temp_v1_7;
    char *temp_a2;
    char *temp_a2_2;
    char *temp_a2_3;
    char *temp_a2_4;
    char *temp_a2_5;
    char *temp_v1_3;
    char *temp_v1_5;

    steep = 0;
    touched = 0;
    resolved = 0;
    func_00102948(&D_700038B0, arg0->prev);
    func_001028D0(&D_700038A0, arg0->pos, &D_700038B0);
    func_00102760(&D_700038A0, &D_700038A0);
    func_00103230(&D_700038A0, &D_700038A0, 1.5f);
    func_001028B8(&D_700038A0, &D_700038A0, arg0->pos);
    probeHit = func_0019A910(&D_700038B0, &D_700038A0, arg3);
    if (probeHit != 0) {
        touched = 1;
        arg0->h58 = (unsigned short) (*(unsigned short *)((char *)(*(char **)0x700031D0) + 0x1A));
        func_001031E0(&D_700038C0, &D_700031B0);
        func_00102948(&D_70003950, &D_700038C0);
        func_001028D0(&D_700038F0, &D_700038B0, &D_700038A0);
        *(float *)0x700038F4 = 0.0f;
        func_00102760(&D_700038F0, &D_700038F0);
        func_00102948(&D_70003960, &D_700038F0);
        temp_a2 = (*(char **)0x700031D0);
        *(float *)0x700038E0 = (*(float *)((char *)temp_a2 + 0x24));
        *(float *)0x700038E8 = (*(float *)((char *)temp_a2 + 0x2C));
        *(float *)0x700038E4 = 0.0f;
        *(int *)0x700038EC = 0x3F800000;
        temp_f0 = func_00102738(&D_700038F0, &D_700038E0);
        *(float *)0x70003A3C = temp_f0;
        if (temp_f0 < 0.707f) {
            steep = 1;
        }
        *(float *)0x700038E4 = (*(float *)((char *)(*(char **)0x700031D0) + 0x28));
        func_00102760(&D_700038E0, &D_700038E0);
        arg0->f90 = func_001B1470(func_0011E620(*(float *)0x700038E0, *(float *)0x700038E8));
        if (arg2 != 3) {
            temp_f0_2 = D_00810690 - func_0011DF78(arg0->f0C);
            *(float *)0x70003A20 = temp_f0_2;
            if (temp_f0_2 <= 0.0f) {
                if (arg0->f5C == 1.0f) {
                    float x = *(float *)0x700038B0;
                    float z = *(float *)0x700038B8;
                    *(float *)0x70003900 = x;
                    *(float *)0x70003904 = 13.0f + arg1->eye[1];
                    *(float *)0x70003908 = z;
                    *(float *)0x7000390C = 1.0f;



                } else {
                    float x = *(float *)0x700038B0;
                    float z = *(float *)0x700038B8;
                    *(float *)0x70003900 = x;
                    *(float *)0x70003904 = 17.5f + arg1->eye[1];
                    *(float *)0x70003908 = z;
                    *(float *)0x7000390C = 1.0f;



                }

                if (func_0019A910(&D_70003900, &D_700038A0, arg3) == 0) {
                    goto block_12;
                }
                if ((short) (*(unsigned short *)((char *)(*(char **)0x700031D0) + 0x1A)) & 0x8800) {
                    func_001028D0(&D_70003900, &D_700031B0, &D_700038A0);
                    temp_f0_3 = func_00102738(&D_70003900, &D_70003900);
                    *(float *)0x70003A20 = temp_f0_3;
                    if (temp_f0_3 < 1.0f) {
block_12:
                        probeHit = 0;
                    }
                }
            }
        }
        if (probeHit != 0) {
            if (arg2 == 3) {
                func_001028D0(&D_700038A0, &D_700038A0, &D_700038B0);
                func_00102760(&D_700038A0, &D_700038A0);
                temp_v1 = arg0->h58;
                if (temp_v1 & 0xD800) {
                    if (temp_v1 & 0x8800) {
                        temp_f1 = *(float *)0x700038C4;
                        touched = 8;
                        if (temp_f1 < arg0->f50) {
                            arg0->f50 = temp_f1;
                        }
                    } else {
                        temp_f1_2 = *(float *)0x700038C4;
                        touched = 0x10;
                        if (!(temp_f1_2 <= arg0->f54)) {
                            arg0->f54 = temp_f1_2;
                        }
                    }
                    func_00102948(arg0->pos, &D_700038C0);
                    arg0->pos[0] = (arg0->pos[0] + (0.5f * *(float *)0x700038A0));
                    arg0->pos[1] = (arg0->pos[1] + (0.5f * *(float *)0x700038A4));
                    arg0->pos[2] = arg0->pos[2] + (0.5f * *(float *)0x700038A8);
                } else {
                    arg0->pos[0] = *(float *)0x700038C0;
                    arg0->pos[2] = *(float *)0x700038C8;
                    arg0->pos[0] = (arg0->pos[0] + (0.5f * *(float *)0x700038A0));
                    arg0->pos[2] = arg0->pos[2] + (0.5f * *(float *)0x700038A8);
                }

            } else {
            temp_v1_2 = arg0->h58;
            if (temp_v1_2 & 0x8800) {
                touched = 8;
                func_001028D0(&D_700038A0, &D_700038A0, &D_700038B0);
                func_00102760(&D_700038A0, &D_700038A0);
                func_00102948(arg0->pos, &D_700038C0);
                arg0->pos[1] = (arg0->pos[1] - 1.0f);
                arg0->pos[0] = (arg0->pos[0] + (0.5f * *(float *)0x700038A0));
                arg0->pos[2] = (arg0->pos[2] + (0.5f * *(float *)0x700038A8));
                temp_f1_3 = arg0->pos[1];
                if (temp_f1_3 < arg0->f50) {
                    arg0->f50 = temp_f1_3;
                }
                temp_f1_4 = arg0->pos[1];
                resolved = 1;
                if (!(temp_f1_4 <= arg0->f54)) {
                    arg0->f54 = temp_f1_4;
                    resolved = 1;
                }
            } else if ((temp_v1_2 & 0x2000) && (func_0019A910(&D_700038A0, &D_700038B0, arg3) != 0)) {
                temp_a2_2 = (*(char **)0x700031D0);
                if ((short) (*(unsigned short *)((char *)temp_a2_2 + 0x1A)) & 0x2000) {
                    *(float *)0x700038D0 = (*(float *)((char *)temp_a2_2 + 0x24));
                    *(float *)0x700038D4 = (*(float *)((char *)temp_a2_2 + 0x28));
                    *(float *)0x700038D8 = (*(float *)((char *)temp_a2_2 + 0x2C));
                    *(int *)0x700038DC = 0x3F800000;
                    temp_f0_4 = func_00102738(&D_700038D0, &D_700038E0);
                    *(float *)0x70003A20 = temp_f0_4;
                    if (!(temp_f0_4 <= -0.3f)) {
                        arg0->f90 = func_001B1470(func_0011E620(*(float *)0x700038D0, *(float *)0x700038D8));
                        func_001031E0(&D_700038C0, &D_700031B0);
                        func_00103230(&D_700038D0, &D_700038D0, 4.0f);
                        func_001028B8(&D_700038C0, &D_700038C0, &D_700038D0);
                        resolved = 1;
                        arg0->pos[0] = *(float *)0x700038C0;
                        arg0->pos[2] = *(float *)0x700038C8;
                    }
                }
            }
            if (resolved == 0) {
                func_001028D0(&D_700038A0, &D_700038A0, &D_700038B0);
                func_00102760(&D_700038A0, &D_700038A0);
                arg0->pos[0] = *(float *)0x700038C0;
                arg0->pos[2] = *(float *)0x700038C8;
                arg0->pos[0] = (arg0->pos[0] + (0.5f * *(float *)0x700038A0));
                arg0->pos[2] = arg0->pos[2] + (0.5f * *(float *)0x700038A8);


            }
            }
        }
    }
    doSlide = 1;
    if (steep != 1) {
        doSlide = 0;
        if (probeHit == 0) {
            doSlide = 1;
        }
    }
    if (doSlide != 0) {
        temp_f20 = func_001B1470(func_001B1240(arg0->pos, arg0->prev[0], arg0->prev[2]) - 1.5707964f);
        if (probeHit == 0) {
            *(float *)0x700038A0 = (5.5f * func_0011E2A8(temp_f20));
            *(float *)0x700038A8 = (5.5f * func_0011DE90(temp_f20));
            *(float *)0x700038A4 = 0.0f;
            func_001028B8(&D_700038B0, arg0->pos, &D_700038A0);
            func_00102948(&D_70003910, arg0->pos);
        } else {
            *(float *)0x700038A0 = (5.5f * func_0011E2A8(temp_f20));
            *(float *)0x700038A8 = (5.5f * func_0011DE90(temp_f20));
            *(float *)0x700038A4 = 0.0f;
            func_001028D0(&D_700038B0, arg0->pos, arg0->prev);
            func_00102760(&D_700038B0, &D_700038B0);
            func_00103230(&D_700038B0, &D_700038B0, -1.5f);
            func_001028B8(&D_700038B0, &D_700038B0, arg0->pos);
            func_001028B8(&D_700038B0, &D_700038B0, &D_700038A0);
            func_00102760(&D_70003910, &D_700038A0);
            func_00103230(&D_70003910, &D_70003910, -3.0f);
            func_001028B8(&D_70003910, arg0->pos, &D_70003910);
        }
        func_00102948(spA, arg0->pos);
        func_00102948(spB, arg0->pos);
        temp_v0 = func_0019A910(&D_70003910, &D_700038B0, arg3);
        hitA = temp_v0;
        if (temp_v0 != 0) {
            func_00102948(&D_70003920, &D_700031B0);
            temp_v1_3 = (*(char **)0x700031D0);
            *(float *)0x70003900 = (*(float *)((char *)temp_v1_3 + 0x24));
            *(float *)0x70003904 = (*(float *)((char *)temp_v1_3 + 0x28));
            *(float *)0x70003908 = (*(float *)((char *)temp_v1_3 + 0x2C));
            *(int *)0x7000390C = 0x3F800000;
            if (probeHit != 0) {
                if (touched & 8) {
                    temp_f0_5 = func_00102738(&D_70003900, &D_70003960);
                    *(float *)0x70003A3C = temp_f0_5;
                    if (temp_f0_5 < -0.08f) {
                        hitA = 0;
                        *(float *)0x70003A3C = -1.0f;
                    }
                } else {
                    temp_f0_6 = func_00102738(&D_70003900, &D_700038E0);
                    *(float *)0x70003A3C = temp_f0_6;
                    if (temp_f0_6 < -0.998f) {
                        func_001028D0(&D_70003910, &D_70003920, &D_700038B0);
                        temp_f0_7 = func_00102738(&D_70003910, &D_70003910);
                        *(float *)0x70003A38 = temp_f0_7;
                        if (temp_f0_7 < 1.0f) {
                            hitA = 0;
                            *(float *)0x70003A3C = -1.0f;
                        } else {
                            {
                                float w0 = *(float *)0x70003950;
                                float w2 = *(float *)0x70003958;
                                *(float *)0x70003910 = w0;
                                *(float *)0x70003914 = arg0->pos[1];
                                *(float *)0x70003918 = w2;
                            }


                            *(int *)0x7000391C = 0x3F800000;
                            hitA = func_0019A910(&D_70003910, &D_700038B0, arg3);
                            if (hitA != 0) {
                                func_00102948(&D_70003920, &D_700031B0);
                                temp_a2_3 = (*(char **)0x700031D0);
                                *(float *)0x70003900 = (*(float *)((char *)temp_a2_3 + 0x24));
                                *(float *)0x70003904 = (*(float *)((char *)temp_a2_3 + 0x28));
                                *(float *)0x70003908 = (*(float *)((char *)temp_a2_3 + 0x2C));
                                *(int *)0x7000390C = 0x3F800000;
                                temp_f0_8 = func_00102738(&D_70003900, &D_700038E0);
                                *(float *)0x70003A3C = temp_f0_8;
                                if (temp_f0_8 < -0.998f) {
                                    hitA = 0;
                                    *(float *)0x70003A3C = -1.0f;
                                }
                            } else {
                                hitA = 0;
                                *(float *)0x70003A3C = -1.0f;
                            }
                        }
                    }
                }
            } else {
                *(float *)0x70003A3C = 0.0f;
            }
            if ((hitA != 0) && ((steep == 1) || ((temp_f1_5 = *(float *)0x70003A3C, (temp_f1_5 < 0.9f)) && !(temp_f1_5 <= -0.3f)))) {
                temp_s7 = (*(unsigned short *)((char *)(*(char **)0x700031D0) + 0x1A));
                func_001028D0(&D_700038D0, &D_700038B0, &D_70003910);
                func_001028D0(&D_700038C0, &D_700031B0, &D_700038A0);
                func_00102760(&D_700038D0, &D_700038D0);
                temp_v1_4 = temp_s7 & 0xFFFF;
                if (temp_v1_4 & 0x8800) {
                    *(float *)0x700038F0 = 0.0f;
                    *(float *)0x700038F4 = -1.0f;
                    *(float *)0x700038F8 = 0.0f;
                    *(int *)0x700038FC = 0x3F800000;
                    temp_f0_9 = func_00102738(&D_700038F0, &D_70003900);
                    *(float *)0x70003A3C = temp_f0_9;
                    if (temp_f0_9 < 0.9f) {
                        touched |= 2;
                        {
                            float t0 = *(float *)0x700038C0;
                            float t2 = *(float *)0x700038C8;
                            spA[0] = t0;
                            spA[2] = t2;
                        }
                        spA[0] += 0.1f * *(float *)0x700038D0;
                        spA[2] += 0.1f * *(float *)0x700038D8;
                    } else {
                        func_00102948(spA, &D_700038C0);
                        touched |= 0xA;
                        spA[0] += 0.1f * *(float *)0x700038D0;
                        var_f0_2 = spA[2] + (0.1f * *(float *)0x700038D8);
                        goto block_74;
                    }
                } else if (temp_v1_4 & 0x2000) {
                    touched |= 2;
                    {
                        float t0 = *(float *)0x700038C0;
                        float t2 = *(float *)0x700038C8;
                        spA[0] = t0;
                        spA[2] = t2;
                    }
                    spA[0] += 0.1f * *(float *)0x700038D0;
                    spA[2] += 0.1f * *(float *)0x700038D8;
                } else {
                    func_00102948(spA, &D_700038C0);
                    touched |= 3;
                    spA[0] += 0.1f * *(float *)0x700038D0;
                    var_f0_2 = spA[2] + (0.1f * *(float *)0x700038D8);
block_74:
                    spA[2] = var_f0_2;
                }
            }
        }
        if (probeHit == 0) {
            func_001028D0(&D_700038B0, arg0->pos, &D_700038A0);
            func_00102948(&D_70003910, arg0->pos);
        } else {
            func_001028D0(&D_700038B0, arg0->pos, arg0->prev);
            func_00102760(&D_700038B0, &D_700038B0);
            func_00103230(&D_700038B0, &D_700038B0, -1.5f);
            func_001028B8(&D_700038B0, &D_700038B0, arg0->pos);
            func_001028D0(&D_700038B0, &D_700038B0, &D_700038A0);
            func_00102760(&D_70003910, &D_700038A0);
            func_00103230(&D_70003910, &D_70003910, 3.0f);
            func_001028B8(&D_70003910, arg0->pos, &D_70003910);
        }
        temp_v0_2 = func_0019A910(&D_70003910, &D_700038B0, arg3);
        hitB = temp_v0_2;
        if (temp_v0_2 != 0) {
            func_00102948(&D_70003930, &D_700031B0);
            temp_v1_5 = (*(char **)0x700031D0);
            *(float *)0x700038F0 = (*(float *)((char *)temp_v1_5 + 0x24));
            *(float *)0x700038F4 = (*(float *)((char *)temp_v1_5 + 0x28));
            *(float *)0x700038F8 = (*(float *)((char *)temp_v1_5 + 0x2C));
            *(int *)0x700038FC = 0x3F800000;
            if (probeHit != 0) {
                if (touched & 8) {
                    temp_f0_10 = func_00102738(&D_700038F0, &D_70003960);
                    *(float *)0x70003A3C = temp_f0_10;
                    if (temp_f0_10 < -0.08f) {
                        steep = 0;
                        hitB = 0;
                        *(float *)0x70003A3C = -1.0f;
                    }
                } else {
                    temp_f0_11 = func_00102738(&D_700038F0, &D_700038E0);
                    *(float *)0x70003A3C = temp_f0_11;
                    if (temp_f0_11 < -0.998f) {
                        func_001028D0(&D_70003910, &D_70003930, &D_700038B0);
                        temp_f0_12 = func_00102738(&D_70003910, &D_70003910);
                        *(float *)0x70003A38 = temp_f0_12;
                        if (temp_f0_12 < 1.0f) {
                            steep = 0;
                            hitB = 0;
                            *(float *)0x70003A3C = -1.0f;
                        } else {
                            {
                                float w0 = *(float *)0x70003950;
                                float w2 = *(float *)0x70003958;
                                *(float *)0x70003910 = w0;
                                *(float *)0x70003914 = arg0->pos[1];
                                *(float *)0x70003918 = w2;
                            }


                            *(int *)0x7000391C = 0x3F800000;
                            hitB = func_0019A910(&D_70003910, &D_700038B0, arg3);
                            if (hitB != 0) {
                                func_00102948(&D_70003930, &D_700031B0);
                                temp_a2_4 = (*(char **)0x700031D0);
                                *(float *)0x700038F0 = (*(float *)((char *)temp_a2_4 + 0x24));
                                *(float *)0x700038F4 = (*(float *)((char *)temp_a2_4 + 0x28));
                                *(float *)0x700038F8 = (*(float *)((char *)temp_a2_4 + 0x2C));
                                *(int *)0x700038FC = 0x3F800000;
                                temp_f0_13 = func_00102738(&D_700038F0, &D_700038E0);
                                *(float *)0x70003A3C = temp_f0_13;
                                if (temp_f0_13 < -0.998f) {
                                    steep = 0;
                                    hitB = 0;
                                    *(float *)0x70003A3C = -1.0f;
                                }
                            } else {
                                steep = 0;
                                hitB = 0;
                                *(float *)0x70003A3C = -1.0f;
                            }
                        }
                    }
                }
            } else {
                *(float *)0x70003A3C = 0.0f;
            }
            if (steep != 1) {
                temp_f1_6 = *(float *)0x70003A3C;
                if (temp_f1_6 < 0.9f) {
                    if (!(temp_f1_6 <= -0.3f)) {
                        goto block_95;
                    }
                }
            } else {
block_95:
                temp_s3 = (*(unsigned short *)((char *)(*(char **)0x700031D0) + 0x1A));
                func_001028D0(&D_700038D0, &D_700038B0, &D_70003910);
                func_001028B8(&D_700038C0, &D_700031B0, &D_700038A0);
                func_00102760(&D_700038D0, &D_700038D0);
                temp_v1_6 = temp_s3 & 0xFFFF;
                if (temp_v1_6 & 0x8800) {
                    *(float *)0x700038B0 = 0.0f;
                    *(float *)0x700038B4 = -1.0f;
                    *(float *)0x700038B8 = 0.0f;
                    *(int *)0x700038BC = 0x3F800000;
                    temp_f0_14 = func_00102738(&D_700038B0, &D_700038F0);
                    *(float *)0x70003A3C = temp_f0_14;
                    if (temp_f0_14 < 0.9f) {
                        touched |= 4;
                        {
                            float t0 = *(float *)0x700038C0;
                            float t2 = *(float *)0x700038C8;
                            spB[0] = t0;
                            spB[2] = t2;
                        }
                        spB[0] += 0.1f * *(float *)0x700038D0;
                        spB[2] += 0.1f * *(float *)0x700038D8;
                    } else {
                        func_00102948(spB, &D_700038C0);
                        touched |= 8;
                        spB[0] += 0.1f * *(float *)0x700038D0;
                        var_f0_3 = spB[2] + (0.1f * *(float *)0x700038D8);
                        goto block_103;
                    }
                } else if (temp_v1_6 & 0x2000) {
                    touched |= 4;
                    {
                        float t0 = *(float *)0x700038C0;
                        float t2 = *(float *)0x700038C8;
                        spB[0] = t0;
                        spB[2] = t2;
                    }
                    spB[0] += 0.1f * *(float *)0x700038D0;
                    spB[2] += 0.1f * *(float *)0x700038D8;
                } else {
                    func_00102948(spB, &D_700038C0);
                    touched |= 1;
                    spB[0] += 0.1f * *(float *)0x700038D0;
                    var_f0_3 = spB[2] + (0.1f * *(float *)0x700038D8);
block_103:
                    spB[2] = var_f0_3;
                }
            }
        }
        if ((touched & 6) == 6) {
            func_001028B8(&D_70003940, &D_70003920, &D_70003930);
            func_00103230(&D_70003940, &D_70003940, 0.5f);
            arg0->pos[0] = *(float *)0x70003940;
            arg0->pos[2] = *(float *)0x70003948;
        } else if (touched & 2) {
            if (hitB != 0) {
                func_001028B8(&D_70003940, &D_70003920, &D_70003930);
                func_00103230(&D_70003940, &D_70003940, 0.5f);
                arg0->pos[0] = *(float *)0x70003940;
                arg0->pos[2] = *(float *)0x70003948;
            } else {
                func_00102948(arg0->pos, spA);
            }
        } else if (touched & 4) {
            if (hitA != 0) {
                func_001028B8(&D_70003940, &D_70003920, &D_70003930);
                func_00103230(&D_70003940, &D_70003940, 0.5f);
                arg0->pos[0] = *(float *)0x70003940;
                arg0->pos[2] = *(float *)0x70003948;
            } else {
                func_00102948(arg0->pos, spB);
            }
        }
    }
    if (D_00810700 == 0x12) {
        temp_f1_7 = arg0->pos[2];
        if (temp_f1_7 < 169.5f) {
            arg0->pos[2] = 169.5f;
        } else if (!(temp_f1_7 <= 230.6f)) {
            arg0->pos[2] = 230.6f;
        }
    }
    if (probeHit != 0) {
        temp_v1_7 = arg0->h58;
        if (!(temp_v1_7 & 0x2000)) {
            if (temp_v1_7 & 0x8800) {
                var_f0_4 = *(float *)0x70003954 - 1.0f;
                if (!(arg0->pos[1] <= var_f0_4)) {
                    goto block_130;
                }
            } else {
                var_f0_4 = 1.0f + *(float *)0x70003954;
                if (arg0->pos[1] < var_f0_4) {
block_130:
                    arg0->pos[1] = var_f0_4;
                }
            }
        }
    }
    if (arg0->pos[1] <= arg0->f50) {
        arg0->pos[1] = arg0->f50;
    }
    if (!(arg0->pos[1] < arg0->f54)) {
        arg0->pos[1] = arg0->f54;
    }
    func_00102948(&D_700038F0, arg1->eye);
    *(float *)0x700038F4 = (*(float *)0x700038F4 + 11.0f);
    func_001028D0(&D_700038F0, arg0->pos, &D_700038F0);
    func_00102760(&D_700038F0, &D_700038F0);
    func_00102900(&D_700038F0, &D_700038F0, 1.5f);
    func_001028D0(&D_700038F0, arg0->pos, &D_700038F0);
    {
    float q0 = *(float *)0x700038F0;
    float q2 = *(float *)0x700038F8;
    *(float *)0x700038A0 = q0;
    *(float *)0x700038A4 = (*(float *)0x700038F4 - 200.0f);
    *(float *)0x700038A8 = q2;
    }
    *(int *)0x700038AC = 0x3F800000;
    if ((func_0019A910(&D_700038F0, &D_700038A0, arg3) != 0) && ((short) (*(unsigned short *)((char *)(*(char **)0x700031D0) + 0x1A)) & 0x7000)) {
        *(float *)0x70003A38 = *(float *)0x700031B4;
        if (arg0->f5C == 1.0f) {
            var_f0_5 = *(float *)0x70003A38 + 6.0f;
        } else {
            var_f0_5 = *(float *)0x70003A38 + 17.0f;
        }
    } else {
        var_f0_5 = arg0->f50 - 200.0f;
    }
    *(float *)0x70003A38 = var_f0_5;
    {
    float q0 = *(float *)0x700038F0;
    float q2 = *(float *)0x700038F8;
    *(float *)0x700038B0 = q0;
    *(float *)0x700038B4 = (200.0f + *(float *)0x700038F4);
    *(float *)0x700038B8 = q2;
    }
    *(int *)0x700038BC = 0x3F800000;
    if ((func_0019A910(&D_700038F0, &D_700038B0, arg3) != 0) && ((short) (*(unsigned short *)((char *)(*(char **)0x700031D0) + 0x1A)) & 0x8800)) {
        temp_f1_10 = *(float *)0x700031B4;
        var_f0_6 = temp_f1_10 - 1.0f;
        *(float *)0x70003A3C = temp_f1_10;
        goto block_150;
    }
    if (D_00810700 == 0x12) {
        *(float *)0x700038B0 = arg1->look[0];
        *(float *)0x700038B4 = (200.0f + arg1->look[1]);
        *(float *)0x700038B8 = arg1->look[2];
        *(int *)0x700038BC = 0x3F800000;
        if (func_0019A910(arg1->look, &D_700038B0, arg3) != 0) {
            *(float *)0x700038B0 = 0.0f;
            *(float *)0x700038B4 = -1.0f;
            *(float *)0x700038B8 = 0.0f;
            *(int *)0x700038BC = 0x3F800000;
            temp_a2_5 = (*(char **)0x700031D0);
            *(float *)0x700038F0 = (*(float *)((char *)temp_a2_5 + 0x24));
            *(float *)0x700038F4 = (*(float *)((char *)temp_a2_5 + 0x28));
            *(float *)0x700038F8 = (*(float *)((char *)temp_a2_5 + 0x2C));
            *(int *)0x700038FC = 0x3F800000;
            temp_f0_15 = func_00102738(&D_700038B0, &D_700038F0);
            *(float *)0x70003A3C = temp_f0_15;
            if (!(temp_f0_15 <= 0.2f)) {
                temp_f1_11 = *(float *)0x700031B4;
                var_f0_6 = temp_f1_11 - 1.0f;
                *(float *)0x70003A3C = temp_f1_11;
            } else {
                var_f0_6 = 200.0f + arg0->pos[1];
            }
            goto block_150;
        }
    } else {
        var_f0_6 = 200.0f + arg0->pos[1];
block_150:
        *(float *)0x70003A3C = var_f0_6;
    }
    temp_f1_12 = *(float *)0x70003A3C;
    if (!(*(float *)0x70003A38 <= temp_f1_12)) {
        *(float *)0x70003A38 = (temp_f1_12 - 3.0f);
    }
    arg0->f50 = *(float *)0x70003A38;
    arg0->f54 = *(float *)0x70003A3C;
    if ((D_00810700 == 0x15) && !(arg0->pos[2] <= 260.0f)) {
        arg0->f50 = 70.0f;
    }
    if (arg0->pos[1] <= arg0->f50) {
        arg0->pos[1] = arg0->f50;
        touched |= 0x40;
    }
    ret = touched;
    if (!(arg0->pos[1] < arg0->f54)) {
        arg0->pos[1] = arg0->f54;
        ret = touched | 0x80;
    }
    return ret;
}
