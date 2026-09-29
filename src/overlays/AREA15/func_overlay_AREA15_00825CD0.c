// NEARMISS func_overlay_AREA15_00825CD0 (96.45%, mwcc 2.3.3; linked from its splat .s)
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// AREA15 overlay, runtime 0x00825D10 (splat/link name 00825CD0; overlay code is
// linked 0x40 below where it runs), 0x8E4 bytes.
// Role (read from the instructions): the mirrored twin of 0x825430: offsets
//  -(x - 869) and 966.5 - z, the pinned angles and the two amplitude branches
//  swapped; the rest (free oscillation, tail, init) is the same.
// Divergence: the same store-then-reload difference as its twin 0x825430 (two
//  extra reloads of 0x70003A2C in the original); nothing else in the multiset
//  audit.
extern unsigned char **D_00275B40;
extern float D_00810350[];
typedef struct { float f0; float f4; float f8; float fC; float f10; float f14; float f18; float f1C; } Spad;
extern Spad D_70003A20;
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern float func_0011DF78(float x);
extern float func_0011E2A8(float x);
extern void func_001FBD50(unsigned char *self, int id, int a2, float f12);
extern void func_001A2370(unsigned char *self, void *p);
extern unsigned char func_001B1630(float x, float y, float z);
extern void func_001B1B70(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA15_00825CD0(unsigned char *self) {
    float a;
    float b;
    float *p1;
    float *p3;
    float *p5;
    switch (self[4]) {
    case 0:
        func_001B0FD0(self);
        func_001C6380(self);
        *(int *)(self + 0x1F0) = 0;
        *(float *)(self + 0x1FC) = 0.0f;
        *(float *)(self + 0x200) = 0.08f;
        *(float *)(self + 0x204) = 0.0f;
        break;
    case 1:
        D_70003A20.f0 = -(D_00810350[0] - 869.0f);
        D_70003A20.f4 = 966.5f - D_00810350[2];
        if (func_0011DF78(D_70003A20.f0) < 7.7f && D_70003A20.f4 > -2.0f && D_70003A20.f4 < 9.5f) {
            if (D_70003A20.f4 < 0.0f) {
                if (*(int *)(self + 0x1F0) > 0 && D_70003A20.f0 < 2.0f) {
                    *(float *)(D_00275B40[0] + 0x74) = -1.3788102f;
                } else if (*(int *)(self + 0x1F0) < 0 && D_70003A20.f0 > -2.0f) {
                    *(float *)(D_00275B40[0] + 0x74) = 1.3788102f;
                }
                *(float *)(self + 0x1FC) = 1.5707964f;
                *(float *)(self + 0x1F4) = *(float *)(D_00275B40[0] + 0x74);
            } else {
                if (*(int *)(self + 0x1F0) == 0) {
                    *(float *)(self + 0x1F4) = -*(float *)(self + 0x1F4);
                    if (D_00810350[9] > 0.0f) {
                        *(int *)(self + 0x1F0) = 1;
                    } else {
                        *(int *)(self + 0x1F0) = -1;
                    }
                }
                p5 = (float *)(self + 0x1F0) + 5;
                if (!*(float *)(self + 0x204) &&
                    func_0011DF78(*(float *)(D_00275B40[0] + 0x74)) > 0.5235988f) {
                    func_001FBD50(self, 0x404, 0, 300.0f);
                    *p5 = 1.0f;
                }
                *(float *)(self + 0x1FC) += *(float *)(self + 0x200);
                p3 = (float *)(self + 0x1F0) + 3;
                if (*(float *)(self + 0x1FC) > 3.1415927f) {
                    *p3 = -3.1415927f;
                } else if (*(float *)(self + 0x1FC) < -3.1415927f) {
                    *p3 = 3.1415927f;
                }
                if (*(int *)(self + 0x1F0) >= 0) {
                    D_70003A20.fC = D_70003A20.f0 - 4.0f;
                    if (D_70003A20.fC > 0.0f) {
                        D_70003A20.fC = 0.0f;
                    }
                    D_70003A20.fC = 2.0799727f * ((9.5f - D_70003A20.f4) * D_70003A20.fC);
                    if (D_70003A20.fC < -79.0f) {
                        D_70003A20.fC = -79.0f;
                    }
                    if (D_70003A20.fC > 0.0f) {
                        D_70003A20.fC = 0.0f;
                    }
                    D_70003A20.fC = (3.1415927f * D_70003A20.fC) / 180.0f;
                    p1 = (float *)(self + 0x1F0) + 1;
                    if (D_70003A20.fC > *p1) {
                        a = *p1 * func_0011E2A8(*p3);
                        b = D_70003A20.fC;
                        D_70003A20.f4 = a;
                        if (b > a) {
                            D_70003A20.fC = a;
                            *p1 = a;
                        } else {
                            *p1 = *p1 + 0.01f * (b - *p1);
                        }
                    } else {
                        *p1 = D_70003A20.fC;
                        *p3 = 1.5707964f;
                    }
                } else {
                    D_70003A20.fC = 4.0f + D_70003A20.f0;
                    if (D_70003A20.fC < 0.0f) {
                        D_70003A20.fC = 0.0f;
                    }
                    D_70003A20.fC = 2.0799727f * ((9.5f - D_70003A20.f4) * D_70003A20.fC);
                    if (D_70003A20.fC > 79.0f) {
                        D_70003A20.fC = 79.0f;
                    }
                    if (D_70003A20.fC < 0.0f) {
                        D_70003A20.fC = 0.0f;
                    }
                    D_70003A20.fC = (3.1415927f * D_70003A20.fC) / 180.0f;
                    p1 = (float *)(self + 0x1F0) + 1;
                    if (D_70003A20.fC < *p1) {
                        a = *p1 * func_0011E2A8(*p3);
                        b = D_70003A20.fC;
                        D_70003A20.f4 = a;
                        if (b < a) {
                            D_70003A20.fC = a;
                            *p1 = a;
                        } else {
                            *p1 = *p1 - 0.01f * (*p1 - b);
                        }
                    } else {
                        *p1 = D_70003A20.fC;
                        *p3 = 1.5707964f;
                    }
                }
                *(float *)(D_00275B40[0] + 0x74) = D_70003A20.fC;
            }
        } else {
            *(int *)(self + 0x1F0) = 0;
            p1 = (float *)(self + 0x1F0) + 1;
            if (func_0011DF78(*(float *)(self + 0x1F4)) > 0.02f) {
                D_70003A20.f1C = *(float *)(self + 0x1FC);
                p3 = (float *)(self + 0x1F0) + 3;
                *(float *)(self + 0x1FC) += *(float *)(self + 0x200);
                if (*(float *)(self + 0x1FC) < 0.0f && D_70003A20.f1C > 0.0f) {
                    func_001FBD50(self, 0x403, 0, 300.0f * *p1);
                }
                if (*p3 > 0.0f && D_70003A20.f1C < 0.0f) {
                    func_001FBD50(self, 0x403, 0, 300.0f * *p1);
                }
                if (*p3 > 3.1415927f) {
                    *p3 = -3.1415927f;
                    func_001FBD50(self, 0x403, 0, 300.0f * *p1);
                } else if (*p3 < -3.1415927f) {
                    *p3 = 3.1415927f;
                    func_001FBD50(self, 0x403, 0, 300.0f * *p1);
                }
                a = *p1 * func_0011E2A8(*p3);
                D_70003A20.f4 = a;
                *(float *)(D_00275B40[0] + 0x74) = a;
                *p1 *= 0.985f;
            } else {
                *(float *)(D_00275B40[0] + 0x74) = 0.0f;
                *p1 = 0.0f;
                *(int *)(self + 0x1F0) = 0;
            }
            *(float *)(self + 0x204) = 0.0f;
        }
        func_001C6380(self);
        func_001A2370(self, D_00275B40[0] + 0x90);
        self[1] = func_001B1630(*(float *)(self + 0xB0), *(float *)(self + 0xB4),
                                *(float *)(self + 0xB8));
        if (self[1] != 0) {
            a = *(float *)(D_00275B40[0] + 0x74);
            if (a < -1.3613569f || a > 1.3613569f) {
                func_001B1B70(self);
            }
        }
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
