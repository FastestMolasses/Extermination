// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001B1190(int);
extern void anim_clip_init(int, int, float, float);
extern void func_001FBD50(int, int, int, float);
extern void func_0021C040(char *, char *);
extern int func_001EFE00(int, char *);
extern void func_001CA5E0(char *, int, int);
extern int D_0028A490[];
extern char D_008102B0[];
extern unsigned char D_008106BD;

void func_001455A0(char *a0, char *a1) {
    char *s1 = a0;
    char *s0 = a1;
    unsigned char a2 = *(unsigned char *)(a0 + 6);

    switch (a2) {
    case 0:
        *(char *)(s1 + 0) = 2;
        *(char *)(s0 + 0x75) = 0;
        *(int *)(s0 + 0x4C) = 0;
        *(int *)(s0 + 0x50) = 0;
        *(int *)(s0 + 0x40) = 0x3F800000;
        *(float *)(s0 + 0x54) = 0.08f;
        func_001B1190(*(unsigned char *)(s1 + 0x9A));
        if (*(char *)(s0 + 0x7C) != 0) {
            *(unsigned char *)(s1 + 6) = 2;
            anim_clip_init((int)s1, 0x21, 1.0f, 0.0f);
        } else {
            *(unsigned char *)(s1 + 6) = 1;
            anim_clip_init((int)s1, 0x20, 1.0f, 0.0f);
        }
        func_001FBD50((int)s1, 0x83B, 0, 300.0f);
        if (*(char *)(s0 + 0x7B) != 0) {
            *(char *)(s0 + 0x7B) = 0;
            func_0021C040(D_008102B0, s1);
            if (D_008106BD == 1) {
                D_008106BD = 0;
            }
        }
        break;
    case 1:
        if (*(int *)(s0 + 0x30) & 0x1000) {
            *(unsigned char *)(s1 + 6) = (unsigned char)(a2 + 1);
            *(int *)(s0 + 0x4C) = 0;
            anim_clip_init((int)s1, 0x21, 1.0f, 0.0f);
        }
        break;
    case 2:
        if ((*(int *)(s0 + 0x30) & 0x1000) && *(char *)(s0 + 0x78) == 0) {
            *(unsigned char *)(s1 + 6) = (unsigned char)(a2 + 1);
            *(char *)(s0 + 0x7E) = 0;
            func_001FBD50((int)s1, 0x83A, 0, 300.0f);
            if (func_001EFE00(0x8000001E, s1) == 0) {
                *(char *)(s1 + 4) = 3;
            }
        }
        break;
    case 3:
        break;
    default:
        break;
    }

    if (*(char *)(s0 + 0x78) != 0) {
        *(float *)(s1 + 0x8C) += *(float *)(s0 + 0x54);
        if (*(float *)(s0 + 0x54) > 0.0f) {
            if (*(float *)(s1 + 0x8C) >= 4.0f) {
                *(float *)(s1 + 0x8C) = 4.0f;
                *(float *)(s0 + 0x54) = -0.08f;
                func_001CA5E0(s1, D_0028A490[(*(unsigned char *)(s1 + 0xD) & 0x80) ? 0x7E : 0x7D], 0);
                *(int *)(s1 + 0x80) = 0x3F800000;
                *(int *)(s1 + 0x84) = 0x3F800000;
                *(int *)(s1 + 0x88) = 0x3F800000;
            }
        } else if (*(float *)(s1 + 0x8C) <= 1.0f) {
            *(float *)(s1 + 0x8C) = 1.0f;
            *(float *)(s0 + 0x54) = 0.0f;
            *(char *)(s0 + 0x78) = 0;
        }
    }
}
