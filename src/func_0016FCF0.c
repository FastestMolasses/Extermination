// NEARMISS func_0016FCF0  (vram 0x0016FCF0, 0x6EC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.95% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 4 instructions (2 identical pairs), pure compare-result register allocation. At 0x270 and 0x634 the target emits `slti $at,$v0,K; bnez $at` (K=3 for p[0x2F0]>=3, K=2 for p[0x23F]>=2); mwcc 2.3.3 emits `slti $v0,$v0,K; bnez $v0`. This is a COMPILER-VERSION split, not a C-shape issue: mwcc 2.3 (991...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

extern void func_0017B300(unsigned char *, int);
extern void func_001749A0(unsigned char *, int, int, float);
extern void func_0016F530(unsigned char *, int);
extern void anim_eval_skeleton(unsigned char *);
extern void copy_qw4(unsigned char *, unsigned char *);
extern void func_0017ABA0(unsigned char *);
extern unsigned char *func_00185A10(unsigned char *, unsigned char *);
extern unsigned char *func_00185E30(unsigned char *, unsigned char *);
extern void func_00170A60(unsigned char *, int);
extern void func_00171320(unsigned char *);
extern void func_00171670(unsigned char *);
extern void func_00171B00(unsigned char *);
extern void func_00171E90(unsigned char *);
extern void func_001723D0(unsigned char *);
extern void func_0016F600(unsigned char *);
extern float func_0011E620(float, float);
extern float func_001B1470(float);
extern void anim_matrix_dispatch(unsigned char *);
extern void func_001FBD50(unsigned char *, int, int, float);
extern float func_001B12B0(float, float, float);
extern void func_00174AC0(unsigned char *, int);
extern void func_0017C440(unsigned char *, int);
extern void func_0017C540(unsigned char *);
extern void func_00178B90(unsigned char *, int);
extern void func_001764E0(unsigned char *);
extern void func_00175900(unsigned char *, int);
extern void func_001796C0(unsigned char *);

extern short D_00248B88[];
extern short D_00248C68[];
extern unsigned char *D_00275B40;
extern unsigned char *D_008106E0[2];
extern unsigned char D_00810CA4[8];

void func_0016FCF0(unsigned char *p) {
    unsigned char state;
    unsigned char sub;
    short t;
    int flag;

    state = p[6];
    switch (state) {
    case 0:
        func_0017B300(p, 0);
        if (p[0x317] == 0) {
            p[6] = p[6] + 1;
            *(float *)(p + 0x278) = 0.5f;
            p[0x2F2] = 0;
            *(short *)(p + 0x2E) = 0;
            p[0x275] = 0;
            func_001749A0(p, 0x110, 0, 1.0f);
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
        break;
    case 1:
        if (*(int *)(p + 0x200) & 0x1000) {
            p[6] = state + 1;
            func_0016F530(p, 0);
            sub = p[5];
            if (sub == 0x1D || sub == 0x1E) {
                func_001749A0(p, D_00248B88[p[0x275]], 0, 0.0f);
            } else {
                func_001749A0(p, D_00248C68[p[0x275]], 0, 0.0f);
            }
            anim_eval_skeleton(p);
            copy_qw4(p + 0x2A0, *(unsigned char **)(D_00275B40 + 0x10) + 0x90);
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
        if (D_00810CA4[0] == 0 && p[0x274] != 0) {
            p[0x2F0] = p[0x2F0] + 1;
            if (p[0x2F0] >= 3) {
                p[0x2F0] = 0;
            }
        }
        switch (p[0x275]) {
        case 0:
            func_00170A60(p, 0);
            break;
        case 1:
            func_00171320(p);
            break;
        case 2:
            func_00171670(p);
            break;
        case 3:
            func_00171B00(p);
            break;
        case 4:
            func_00171E90(p);
            break;
        case 5:
            func_001723D0(p);
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
        p[6] = state + 1;
        *(short *)(p + 0x28) = 4;
        *(float *)(p + 0x26C) = (0.5f - *(float *)(p + 0x27C)) / 4.0f;
        *(float *)(p + 0x270) = (0.5f - *(float *)(p + 0x278)) / 4.0f;
        *(volatile float *)0x70003A20 = func_0011E620(-*(float *)(*(unsigned char **)(p + 0x20) + 0xC8),
                                                      *(float *)(*(unsigned char **)(p + 0x20) + 0xC0));
        *(float *)(p + 0x218) = func_001B1470(1.5707964f + *(volatile float *)0x70003A20);
        /* fallthrough */
    case 0x64:
        t = *(short *)(p + 0x28);
        *(short *)(p + 0x28) = t - 1;
        if (t == 0) {
            p[6] = p[6] + 1;
            *(float *)(p + 0x27C) = 0.5f;
            *(float *)(p + 0x278) = 0.5f;
            sub = p[5];
            if (sub == 0x1D || sub == 0x1E) {
                func_001749A0(p, D_00248B88[p[0x275]], 0, 0.0f);
            } else {
                func_001749A0(p, D_00248C68[p[0x275]], 0, 0.0f);
            }
        } else {
            *(float *)(p + 0x27C) += *(float *)(p + 0x26C);
            *(float *)(p + 0x278) += *(float *)(p + 0x270);
            flag = 0;
            if (p[0x1F0] != 0x33) {
                anim_matrix_dispatch(p);
                sub = p[0x1F0];
                if (sub == 0x32 || sub == 0x35) {
                    copy_qw4(p + 0x2A0, *(unsigned char **)(D_00275B40 + 0x10) + 0x90);
                    flag = 1;
                } else if (p[0x275] == 4) {
                    copy_qw4(p + 0x2A0, *(unsigned char **)(D_00275B40 + 0x10) + 0x90);
                    flag = 1;
                } else if (p[0x2F2] != 0) {
                    copy_qw4(p + 0x2A0, *(unsigned char **)(D_00275B40 + 0x10) + 0x90);
                    flag = 1;
                }
                if (flag == 0) {
                    *(float *)(p + 0x2D0) = *(float *)(*(unsigned char **)(D_00275B40 + 0x10) + 0xC0);
                    *(float *)(p + 0x2D4) = *(float *)(*(unsigned char **)(D_00275B40 + 0x10) + 0xC4);
                    *(float *)(p + 0x2D8) = *(float *)(*(unsigned char **)(D_00275B40 + 0x10) + 0xC8);
                }
            } else {
                anim_matrix_dispatch(p);
            }
        }
        break;
    case 0x65:
        p[6] = state + 1;
        *(short *)(p + 0x276) = 0;
        func_001749A0(p, 0x111, 0, 1.0f);
        func_001FBD50(p, 0x163, 0, 300.0f);
        p[0x317] = 0;
        break;
    case 0x66:
        if (*(float *)(p + 0x3C) <= 4.0f) {
            p[6] = 0x6E;
        } else {
            *(float *)(p + 0xC4) = func_001B12B0(*(float *)(p + 0x218), *(float *)(p + 0xC4), 0.0872664675116539f);
        }
        break;
    case 0x6E:
        func_00174AC0(p, 1);
        if (p[0x23F] >= 2) {
            p[6] = p[6] + 1;
            func_0017C440(p, 0);
        } else {
            p[0x25C] = 0;
            func_0017C540(p);
        }
        break;
    case 0x6F:
        func_00174AC0(p, 1);
        func_00178B90(p, 0);
        if (!(*(int *)(p + 0x200) & 0x8000)) {
            func_0017C540(p);
        }
        break;
    }
    func_001764E0(p);
    *(float *)(p + 0xB4) += -0.2f;
    func_00175900(p, 1);
    func_001796C0(p);
}
