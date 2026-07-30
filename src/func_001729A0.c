// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

extern void *D_00275B40;
extern void *D_008106E0[2];
extern short D_00248B88[];
extern short D_00248C68[];

extern void func_0017B300(unsigned char *, int);
extern void func_001749A0(unsigned char *, int, int, float);
extern void func_0016F530(unsigned char *, int);
extern void func_001031E0(unsigned char *, unsigned char *);
extern void anim_eval_skeleton(unsigned char *);
extern void copy_qw4(unsigned char *, unsigned char *);
extern void *func_00185A10(unsigned char *, void *);
extern void *func_00185E30(unsigned char *, void *);
extern void func_0017ABA0(unsigned char *);
extern void func_00170A60(unsigned char *, int);
extern void func_00171320(unsigned char *);
extern void func_00171670(unsigned char *);
extern void func_00171B00(unsigned char *);
extern void func_00171E90(unsigned char *);
extern void func_001723D0(unsigned char *);
extern void func_00172860(unsigned char *, float);
extern void func_0016F600(unsigned char *);
extern float func_0011E620(float, float);
extern float func_001B1470(float);
extern void anim_matrix_dispatch(unsigned char *);
extern void func_001FBD50(unsigned char *, int, int, float);
extern float func_001B12B0(float, float, float);

void func_001729A0(unsigned char *p) {
    int done;
    short cnt;
    unsigned char *q;

    switch (p[6]) {
    case 0:
        func_0017B300(p, 0);
        if (p[0x317] == 0) {
            p[6] = p[6] + 1;
            *(float *)(p + 0x278) = 0.5f;
            p[0x2F2] = 0;
            *(short *)(p + 0x2E) = 0;
            p[0x275] = 0;
            func_001749A0(p, 0x188, 0, 8.0f);
        } else {
            p[6] = p[6] + 2;
            func_0016F530(p, 0);
        }
        *(float *)(p + 0x27C) = 0.5f;
        p[7] = 0;
        D_008106E0[0] = 0;
        p[0x302] = 0;
        *(short *)(p + 0x276) = 0;
        p[0x274] = 0;
        func_001031E0(p + 0x290, p + 0xB0);
        *(float *)(p + 0x294) += 20.5f;
        *(int *)(p + 0x38) = 0;
        break;
    case 1:
        if (*(int *)(p + 0x200) & 0x1000) {
            p[6] = p[6] + 1;
            func_0016F530(p, 0);
            if (p[5] == 0x1D || p[5] == 0x1E) {
                func_001749A0(p, D_00248B88[p[0x275]], 0, 0.0f);
            } else {
                func_001749A0(p, D_00248C68[p[0x275]], 0, 0.0f);
            }
            anim_eval_skeleton(p);
            copy_qw4(p + 0x2A0, (unsigned char *)*(int *)((unsigned char *)D_00275B40 + 0x10) + 0x90);
        }
        break;
    case 2:
        p[0x302] = 0;
        func_0017ABA0(p);
        if (p[0x302] != 0 || p[0x275] != 0) {
            D_008106E0[0] = 0;
        } else if (D_008106E0[0] == 0) {
            D_008106E0[0] = func_00185A10(p, D_008106E0[0]);
        } else {
            D_008106E0[0] = func_00185E30(p, D_008106E0[0]);
        }
        switch (p[0x275]) {
        case 0:
            func_00170A60(p, 0);
            break;
        case 1:
            func_00171320(p);
            func_00172860(p, 0.014999999664723873f);
            break;
        case 2:
            func_00171670(p);
            func_00172860(p, 0.014999999664723873f);
            break;
        case 3:
            func_00171B00(p);
            func_00172860(p, 0.009999999776482582f);
            break;
        case 4:
            func_00171E90(p);
            break;
        case 5:
            func_001723D0(p);
            func_00172860(p, 0.02500000037252903f);
            break;
        }
        break;
    case 3:
        if (p[0x275] != 0) {
            D_008106E0[0] = 0;
        } else if (D_008106E0[0] == 0) {
            D_008106E0[0] = func_00185A10(p, D_008106E0[0]);
        } else {
            D_008106E0[0] = func_00185E30(p, D_008106E0[0]);
        }
        func_0016F600(p);
        break;
    case 0x63:
        p[6] = p[6] + 1;
        *(short *)(p + 0x28) = 8;
        *(float *)(p + 0x26C) = (0.5f - *(float *)(p + 0x27C)) / 8.0f;
        *(float *)(p + 0x270) = (0.5f - *(float *)(p + 0x278)) / 8.0f;
        q = (unsigned char *)*(int *)(p + 0x20);
        *(volatile float *)0x70003A20 = func_0011E620(-*(float *)(q + 0xC8), *(float *)(q + 0xC0));
        *(float *)(p + 0x218) = func_001B1470(1.5707963705062866f + *(volatile float *)0x70003A20);
        /* fallthrough */
    case 0x64:
        cnt = *(short *)(p + 0x28);
        *(short *)(p + 0x28) = cnt - 1;
        if (cnt == 0) {
            p[6] = p[6] + 1;
            *(float *)(p + 0x27C) = 0.5f;
            *(float *)(p + 0x278) = 0.5f;
            if (p[5] == 0x1D || p[5] == 0x1E) {
                func_001749A0(p, D_00248B88[p[0x275]], 0, 0.0f);
            } else {
                func_001749A0(p, D_00248C68[p[0x275]], 0, 0.0f);
            }
            break;
        }
        *(float *)(p + 0x27C) += *(float *)(p + 0x26C);
        *(float *)(p + 0x278) += *(float *)(p + 0x270);
        done = 0;
        if (p[0x1F0] != 0x33) {
            anim_matrix_dispatch(p);
            if (p[0x1F0] == 0x32 || p[0x1F0] == 0x35) {
                copy_qw4(p + 0x2A0, (unsigned char *)*(int *)((unsigned char *)D_00275B40 + 0x10) + 0x90);
                done = 1;
            } else if (p[0x275] == 4) {
                copy_qw4(p + 0x2A0, (unsigned char *)*(int *)((unsigned char *)D_00275B40 + 0x10) + 0x90);
                done = 1;
            } else if (p[0x2F2] != 0) {
                copy_qw4(p + 0x2A0, (unsigned char *)*(int *)((unsigned char *)D_00275B40 + 0x10) + 0x90);
                done = 1;
            }
            if (done == 0) {
                *(float *)(p + 0x2D0) = *(float *)((unsigned char *)*(int *)((unsigned char *)D_00275B40 + 0x10) + 0xC0);
                *(float *)(p + 0x2D4) = *(float *)((unsigned char *)*(int *)((unsigned char *)D_00275B40 + 0x10) + 0xC4);
                *(float *)(p + 0x2D8) = *(float *)((unsigned char *)*(int *)((unsigned char *)D_00275B40 + 0x10) + 0xC8);
            }
        } else {
            anim_matrix_dispatch(p);
        }
        break;
    case 0x65:
        p[6] = p[6] + 1;
        *(short *)(p + 0x276) = 0;
        func_001749A0(p, 0x189, 0, 1.0f);
        func_001FBD50(p, 0x163, 0, 300.0f);
        p[0x317] = 0;
        break;
    case 0x66:
        if (*(int *)(p + 0x200) & 0x1000) {
            p[5] = 0x14;
            p[6] = 0;
            p[0x1F0] = 0x26;
        } else {
            *(float *)(p + 0xC4) = func_001B12B0(*(float *)(p + 0x218), *(float *)(p + 0xC4), 0.04363323375582695f);
        }
        break;
    }
}
