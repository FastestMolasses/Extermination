// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

extern void anim_eval_skeleton(void *p);
extern void anim_matrix_dispatch(void *p);
extern void copy_qw4(void *dst, void *src);
extern float func_0011E620(void *p, float a, float b);
extern void func_0016F530(void *p, int a);
extern void func_0016F600(void *p);
extern void func_00170A60(void *p, int a);
extern void func_00171320(void *p);
extern void func_00171670(void *p);
extern void func_00171B00(void *p);
extern void func_00171E90(void *p);
extern void func_001723D0(void *p);
extern void func_001749A0(void *self, short code, int a2, float a3);
extern void func_0017ABA0(void *p);
extern void func_0017B300(void *p, int a);
extern void func_00199220(void *p);
extern float func_001B12B0(void *p, float a, float b, float c);
extern float func_001B1470(float a);
extern void func_001FBD50(void *self, short code, int a2, float a3);

extern short D_00248B88[];
extern short D_00248C68[];
extern int D_00275B40;
extern int D_008106E0[];
extern unsigned char D_00810CA4[];

void func_00173000(unsigned char *arg0)
{
    unsigned char st;

    D_008106E0[0] = 0;
    st = arg0[6];
    switch (st) {
    case 0:
        func_0017B300(arg0, 0);
        if (arg0[0x317] == 0) {
            arg0[6] = arg0[6] + 1;
            *(float *)(arg0 + 0x278) = 0.5f;
            arg0[0x2F2] = 0;
            *(short *)(arg0 + 0x2E) = 0;
            arg0[0x275] = 0;
            func_001749A0(arg0, 0x188, 0, 1.0f);
        } else {
            arg0[6] = arg0[6] + 2;
            func_0016F530(arg0, 0);
        }
        *(float *)(arg0 + 0x27C) = 0.5f;
        arg0[7] = 0;
        arg0[0x302] = 0;
        *(short *)(arg0 + 0x276) = 0;
        arg0[0x2F0] = 0;
        arg0[0x274] = 0;
        break;
    case 1:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            unsigned char st5;

            arg0[6] = arg0[6] + 1;
            func_0016F530(arg0, 0);
            st5 = arg0[5];
            if (st5 == 0x1D || st5 == 0x1E) {
                func_001749A0(arg0, D_00248B88[arg0[0x275]], 0, 0.0f);
            } else {
                func_001749A0(arg0, D_00248C68[arg0[0x275]], 0, 0.0f);
            }
        }
        anim_eval_skeleton(arg0);
        copy_qw4(arg0 + 0x2A0, (char *)(*(int *)(D_00275B40 + 0x10)) + 0x90);
        break;
    case 2:
        *(short *)(arg0 + 0x94) = 7;
        arg0[0x302] = 0;
        func_0017ABA0(arg0);
        if (D_00810CA4[0] == 0) {
            if (arg0[0x274] != 0) {
                arg0[0x2F0] = arg0[0x2F0] + 1;
                if (arg0[0x2F0] > 2) {
                    arg0[0x2F0] = 0;
                }
            }
            func_00199220(arg0);
        } else if (D_00810CA4[0] == 1) {
            func_00199220(arg0);
        }
        switch (arg0[0x275]) {
        case 0:
            func_00170A60(arg0, 1);
            break;
        case 1:
            func_00171320(arg0);
            break;
        case 2:
            func_00171670(arg0);
            break;
        case 3:
            func_00171B00(arg0);
            break;
        case 4:
            func_00171E90(arg0);
            break;
        case 5:
            func_001723D0(arg0);
            break;
        }
        break;
    case 3:
        func_0016F600(arg0);
        if (arg0[0x1F0] == 0x33) {
            arg0[1] = 0;
        }
        break;
    case 0x63:
    {
        void *link;

        arg0[6] = st + 1;
        *(short *)(arg0 + 0x28) = 8;
        *(float *)(arg0 + 0x26C) = (0.5f - *(float *)(arg0 + 0x27C)) / 8.0f;
        *(float *)(arg0 + 0x270) = (0.5f - *(float *)(arg0 + 0x278)) / 8.0f;
        link = *(void **)(arg0 + 0x20);
        *(float *)0x70003A20 = func_0011E620(arg0, -(*(float *)((char *)link + 0xC8)), *(float *)((char *)link + 0xC0));
        *(float *)(arg0 + 0x218) = func_001B1470(1.5707964f + *(float *)0x70003A20);
        /* fallthrough */
    }
    case 0x64:
    {
        short cnt;

        cnt = *(short *)(arg0 + 0x28);
        *(short *)(arg0 + 0x28) = cnt - 1;
        if (cnt == 0) {
            unsigned char st5;

            arg0[6] = arg0[6] + 1;
            *(float *)(arg0 + 0x27C) = 0.5f;
            *(float *)(arg0 + 0x278) = 0.5f;
            st5 = arg0[5];
            if (st5 == 0x1D || st5 == 0x1E) {
                func_001749A0(arg0, D_00248B88[arg0[0x275]], 0, 0.0f);
            } else {
                func_001749A0(arg0, D_00248C68[arg0[0x275]], 0, 0.0f);
            }
        } else {
            int hit;

            *(float *)(arg0 + 0x27C) += *(float *)(arg0 + 0x26C);
            *(float *)(arg0 + 0x278) += *(float *)(arg0 + 0x270);
            hit = 0;
            if (arg0[0x1F0] != 0x33) {
                anim_matrix_dispatch(arg0);
                if (arg0[0x1F0] == 0x32 || arg0[0x1F0] == 0x35) {
                    copy_qw4(arg0 + 0x2A0, (char *)(*(int *)(D_00275B40 + 0x10)) + 0x90);
                    hit = 1;
                } else if (arg0[0x275] == 4) {
                    copy_qw4(arg0 + 0x2A0, (char *)(*(int *)(D_00275B40 + 0x10)) + 0x90);
                    hit = 1;
                } else if (arg0[0x2F2] != 0) {
                    copy_qw4(arg0 + 0x2A0, (char *)(*(int *)(D_00275B40 + 0x10)) + 0x90);
                    hit = 1;
                }
                if (hit == 0) {
                    *(float *)(arg0 + 0x2D0) = *(float *)((char *)(*(int *)(D_00275B40 + 0x10)) + 0xC0);
                    *(float *)(arg0 + 0x2D4) = *(float *)((char *)(*(int *)(D_00275B40 + 0x10)) + 0xC4);
                    *(float *)(arg0 + 0x2D8) = *(float *)((char *)(*(int *)(D_00275B40 + 0x10)) + 0xC8);
                }
            } else {
                anim_matrix_dispatch(arg0);
            }
        }
        break;
    }
    case 0x65:
        arg0[6] = st + 1;
        *(short *)(arg0 + 0x276) = 0;
        func_001749A0(arg0, 0x189, 0, 1.0f);
        func_001FBD50(arg0, 0x163, 0, 300.0f);
        arg0[0x317] = 0;
        break;
    case 0x66:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            arg0[5] = 0x14;
            arg0[6] = 0;
            arg0[0x1F0] = 0x26;
        } else {
            *(float *)(arg0 + 0xC4) = func_001B12B0(arg0, *(float *)(arg0 + 0x218), *(float *)(arg0 + 0xC4), 0.043633234f);
        }
        break;
    }
}
