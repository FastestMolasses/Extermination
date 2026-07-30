// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void anim_clip_init(unsigned char *, int, float, float);
extern int func_00122BB8();
extern void func_00132490(unsigned char *, unsigned char *);
extern void func_001333F0(unsigned char *, unsigned char *);
extern void func_00133640(unsigned char *, unsigned char *);
extern int func_001B13F0(unsigned char *, unsigned char *, float);
extern void func_001FBD50(unsigned char *, int, int, float);

extern unsigned short D_00244E90[][16];
extern unsigned char D_00244F10[][16];
extern unsigned char D_00810350[];

void func_0012EB60(unsigned char *arg0, unsigned char *arg1) {
    int t;
    int n;
    unsigned char v;
    float f;

    *(int *)(arg1 + 0x3C) = 0;
    func_001333F0(arg0, arg1);
    switch (*(unsigned char *)(arg0 + 6)) {
    case 0:
        *(unsigned short *)(arg1 + 0x58) = 0;
        if (*(unsigned short *)(arg1 + 0x56) != 0) {
            *(unsigned char *)(arg0 + 6) = 6;
            if (*(unsigned char *)(arg1 + 0x61) != 0) {
                *(float *)(arg1 + 0x34) = 2.0f;
                anim_clip_init(arg0, 4, 0.0f, 0.0f);
            }
        } else if ((*(unsigned char *)(arg0 + 0xD) & 3) == 3 && *(unsigned char *)(arg1 + 0x61) == 0) {
            *(unsigned char *)(arg0 + 5) = 9;
            *(unsigned char *)(arg0 + 6) = 0;
        } else {
            t = (func_00122BB8() >> 8) & 0xF;
            v = D_00244F10[*(unsigned char *)(arg1 + 0x68)][t];
            *(unsigned char *)(arg0 + 6) = v;
            if (v == 0) {
                *(unsigned char *)(arg0 + 6) = 6;
                if (*(unsigned char *)(arg1 + 0x61) != 0) {
                    anim_clip_init(arg0, 4, 10.0f, 0.0f);
                }
            } else {
                *(unsigned short *)(arg1 + 0x50) =
                    D_00244E90[*(unsigned char *)(arg0 + 6)][(func_00122BB8() >> 9) & 0xF];
                if (*(unsigned char *)(arg0 + 6) == 3) {
                    if (func_001B13F0(D_00810350, arg0 + 0xB0, 70.0f) != 0 ||
                        (*(unsigned char *)(arg1 + 0x69) & 1)) {
                        *(unsigned char *)(arg0 + 6) = 1;
                        if (*(unsigned char *)(arg1 + 0x61) != 0) {
                            anim_clip_init(arg0, 4, 10.0f, 0.0f);
                        }
                    } else if (*(unsigned char *)(arg1 + 0x61) == 0) {
                        anim_clip_init(arg0, 3, 10.0f, 0.0f);
                    }
                } else if (*(unsigned char *)(arg1 + 0x61) != 0) {
                    anim_clip_init(arg0, 4, 10.0f, 0.0f);
                }
            }
        }
        break;
    case 1:
        if (*(unsigned char *)(arg1 + 0x61) != 0) {
            if ((*(unsigned short *)(arg1 + 0x58) & 0x5000) == 0) {
                break;
            }
            *(unsigned char *)(arg1 + 0x61) = 0;
        }
        *(unsigned char *)(arg0 + 6) = 4;
        anim_clip_init(arg0, 5, 10.0f, 0.0f);
        break;
    case 2:
        if (*(unsigned char *)(arg1 + 0x61) != 0) {
            if ((*(unsigned short *)(arg1 + 0x58) & 0x5000) == 0) {
                break;
            }
            *(unsigned char *)(arg1 + 0x61) = 0;
        }
        *(unsigned char *)(arg0 + 6) = 4;
        anim_clip_init(arg0, 6, 10.0f, 0.0f);
        break;
    case 3:
        if (*(unsigned char *)(arg1 + 0x61) == 0) {
            f = *(float *)(arg0 + 0x3C);
            if (f == 46.0f || f == 22.0f) {
                func_001FBD50(arg0, 0x7DD, 0, 300.0f);
            }
            if ((*(unsigned short *)(arg1 + 0x58) & 0x5000) == 0) {
                break;
            }
            *(unsigned char *)(arg1 + 0x61) = 1;
        }
        if ((func_00122BB8() >> 8) & 1) {
            *(unsigned char *)(arg0 + 6) = 5;
            if (func_00122BB8() & 1) {
                anim_clip_init(arg0, 0, 10.0f, 0.0f);
            } else {
                anim_clip_init(arg0, 1, 10.0f, 0.0f);
            }
        } else {
            *(unsigned char *)(arg0 + 5) = 1;
            *(unsigned char *)(arg0 + 6) = 0;
        }
        break;
    case 4:
        if ((*(unsigned char *)(arg0 + 0xD) & 2) && *(unsigned char *)0x70003B8D != 0) {
            break;
        }
        n = *(unsigned short *)(arg1 + 0x50) - 1;
        *(unsigned short *)(arg1 + 0x50) = n;
        if ((n & 0xFFFF) == 0 || *(unsigned short *)(arg1 + 0x56) != 0) {
            *(unsigned char *)(arg0 + 6) = 0;
        }
        break;
    case 5:
        if ((*(short *)(arg0 + 0x2C) & 0xFFFF7FFF) == 1 && *(float *)(arg0 + 0x3C) == 95.0f) {
            func_001FBD50(arg0, 0x7DF, 0, 300.0f);
        }
        n = *(unsigned short *)(arg1 + 0x50) - 1;
        *(unsigned short *)(arg1 + 0x50) = n;
        if ((n & 0xFFFF) == 0 || *(unsigned short *)(arg1 + 0x56) != 0) {
            *(unsigned char *)(arg0 + 6) = 0;
        } else if (*(unsigned short *)(arg1 + 0x58) & 0x3000) {
            if (func_00122BB8() & 1) {
                anim_clip_init(arg0, 0, 10.0f, 0.0f);
            } else {
                anim_clip_init(arg0, 1, 10.0f, 0.0f);
            }
        }
        break;
    case 6:
        if (*(unsigned char *)(arg1 + 0x61) != 0) {
            if ((*(unsigned short *)(arg1 + 0x58) & 0x5000) == 0) {
                break;
            }
            *(float *)(arg1 + 0x34) = 1.0f;
        }
        *(unsigned char *)(arg0 + 5) = 1;
        *(unsigned char *)(arg0 + 6) = 0;
        *(unsigned char *)(arg1 + 0x61) = 0;
        break;
    case 7:
        if (*(float *)(arg1 + 0x44) <= (float)*(short *)(arg0 + 0x56) ||
            (*(unsigned char *)(arg0 + 0xA) & 1)) {
            *(unsigned short *)(arg0 + 0x56) = 0;
            *(unsigned char *)(arg0 + 5) = 1;
            *(unsigned char *)(arg0 + 6) = 0;
        }
        break;
    }
    func_00132490(arg0, arg1);
    if (*(unsigned char *)0x70003B8D == 0) {
        func_00133640(arg0, arg1);
    }
}
