// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// SEMANTICS: per-frame camera/view update for the actor block `act`.
//   mode = D_00810525  -- global camera/view mode byte (0..5).
//   First refreshes the 4-quadword transform at (*D_00275B40)+0x90 from
//   D_008103D0+0x90 (copy_qw4).
//   sel = variant index used to pick a 16-byte parameter row: when mode==0 it
//   is 7 if D_00810CA4==0, 6 if D_00810CA4==2, else 0; for any other mode it
//   is mode itself.
//   Stages a vec4 (-3.0f, D_0024A224[sel], 0.0f, 1.0f) in EE scratchpad
//   0x70003600, then builds two oriented vectors into act+0xA0 (from
//   D_00810550 + the scratchpad vec4) and act+0xB0 (from D_00810550 +
//   D_0024A220[sel]); func_001028D0 combines them into act+0xC0 and
//   func_00102760 normalises that in place.
//   act+0x1F0 is the derived view block: rebuilt from (*D_00275B40)+0x90 with
//   D_0024A2A0[mode], and its +0x20 field cleared.
//   D_008104A0 (0x31/0x32/0x34/0x35) selects one of two region/variant hooks
//   (func_001854E0 / func_00185760), enabled by D_008104A1==1 (and, for the
//   0x31/0x34 group, D_008105A2 != 0); D_008105C8 swaps which of the two runs.
//   Finally a 6-entry jump table on D_00810525 runs the per-mode one-shot
//   transition, each guarded by the "transition pending" halfword at act+0x2E
//   (cleared as the transition fires):
//     0: func_001861C0 + func_00187CC0, snapshot the view into 0x700036A0,
//        derive 0x700036D0 with D_0024A300[mode], notify func_001F4010(3,..)
//     1,2: func_001869A0 + func_001B61C0(0,0xE8,0xF,1)
//     3: func_00186A60 + func_00187CC0 + func_001B61C0(0,0xD8,0xC,1)
//     4: only when act+0x2E == 1 -- snapshot the view, build a quarter-turn
//        (pi/2) rotation in 0x700036E0, rotate the snapshot by it, extract
//        0x700036D0 from act+0xB0, notify func_001EFEB0(0x80000039,..) and
//        func_001B61C0(0,0x65,5,1); act+0x2E is cleared unconditionally.
//     5: func_001872C0 + func_001B61C0(1,0xF8,0x12,1)

extern char **D_00275B40;
extern char *D_008103D0[16];
extern unsigned char D_00810525[8];
extern unsigned char D_00810CA4[8];
extern unsigned char D_008104A0[8];
extern unsigned char D_008104A1[8];
extern unsigned char D_008105A2[8];
extern unsigned char D_008105C8[8];
extern char D_00810550[16];
extern char D_0024A220[];
extern char D_0024A224[];
extern char D_0024A2A0[];
extern char D_0024A300[];
extern char D_70003600[16];
extern char D_700036A0[16];
extern char D_700036D0[16];
extern char D_700036E0[16];

extern void copy_qw4(void *dst, void *src);
extern void func_001026A0(void *dst, void *src, void *m);
extern void func_001026D0(void *dst, void *a, void *b);
extern void func_00102760(void *dst, void *src);
extern void func_001028D0(void *dst, void *a, void *b);
extern void func_001029C0(void *q);
extern void func_00102BB0(void *dst, void *src, float ang);
extern void func_001031E0(void *dst, void *src);
extern void func_001854E0(char *act);
extern void func_00185760(char *act);
extern void func_001861C0(char *act);
extern void func_001869A0(char *act);
extern void func_00186A60(char *act);
extern void func_001872C0(char *act);
extern void func_00187CC0(char *act);
extern void func_001B61C0(int a, int b, int c, int d);
extern void func_001EFEB0(int model, void *pos);
extern void func_001F4010(int a, void *pos);

void func_00188630(char *act)
{
    char *ent;
    int mode;
    int sel;
    int off;
    int moff;
    int k;

    mode = D_00810525[0];
    ent = act + 0x1F0;
    copy_qw4(*D_00275B40 + 0x90, D_008103D0[0] + 0x90);

    if (mode == 0 && D_00810CA4[0] == 0) {
        sel = 7;
    } else if (mode == 0 && D_00810CA4[0] == 2) {
        sel = 6;
    } else {
        sel = mode;
    }
    off = sel * 16;

    *(volatile int *)0x70003600 = 0xC0400000;
    *(volatile float *)0x70003604 = *(float *)(D_0024A224 + off);
    *(volatile int *)0x70003608 = 0;
    *(volatile int *)0x7000360C = 0x3F800000;
    func_001026A0(act + 0xA0, D_00810550, D_70003600);
    func_001026A0(act + 0xB0, D_00810550, D_0024A220 + off);
    func_001028D0(act + 0xC0, act + 0xB0, act + 0xA0);
    func_00102760(act + 0xC0, act + 0xC0);

    moff = mode * 16;
    func_001026A0(ent, *D_00275B40 + 0x90, D_0024A2A0 + moff);
    *(int *)(ent + 0x20) = 0;

    k = D_008104A0[0];
    if (k == 0x32 || k == 0x35) {
        if (D_008104A1[0] == 1) {
            if (D_008105C8[0] == 0) {
                func_001854E0(act);
            } else {
                func_00185760(act);
            }
        }
    } else if (k == 0x31 || k == 0x34) {
        if (D_008104A1[0] == 1) {
            if (D_008105A2[0] != 0) {
                if (D_008105C8[0] == 0) {
                    func_00185760(act);
                } else {
                    func_001854E0(act);
                }
            }
        }
    }

    switch (D_00810525[0]) {
    case 0:
        if (*(unsigned short *)(act + 0x2E) != 0) {
            *(short *)(act + 0x2E) = 0;
            func_001861C0(act);
            func_00187CC0(act);
            copy_qw4(D_700036A0, *D_00275B40 + 0x90);
            func_001026A0(D_700036D0, *D_00275B40 + 0x90, D_0024A300 + moff);
            func_001F4010(3, D_700036A0);
        }
        break;
    case 1:
    case 2:
        if (*(unsigned short *)(act + 0x2E) != 0) {
            *(short *)(act + 0x2E) = 0;
            func_001869A0(act);
            func_001B61C0(0, 0xE8, 0xF, 1);
        }
        break;
    case 3:
        if (*(unsigned short *)(act + 0x2E) != 0) {
            *(short *)(act + 0x2E) = 0;
            func_00186A60(act);
            func_00187CC0(act);
            func_001B61C0(0, 0xD8, 0xC, 1);
        }
        break;
    case 4:
        if (*(unsigned short *)(act + 0x2E) == 1) {
            copy_qw4(D_700036A0, *D_00275B40 + 0x90);
            func_001029C0(D_700036E0);
            func_00102BB0(D_700036E0, D_700036E0, 1.57079637f);
            func_001026D0(D_700036A0, D_700036A0, D_700036E0);
            func_001031E0(D_700036D0, act + 0xB0);
            func_001EFEB0(0x80000039, D_700036A0);
            func_001B61C0(0, 0x65, 5, 1);
        }
        *(short *)(act + 0x2E) = 0;
        break;
    case 5:
        if (*(unsigned short *)(act + 0x2E) != 0) {
            *(short *)(act + 0x2E) = 0;
            func_001872C0(act);
            func_001B61C0(1, 0xF8, 0x12, 1);
        }
        break;
    }
}
