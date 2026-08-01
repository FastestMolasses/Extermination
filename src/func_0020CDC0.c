// NEARMISS func_0020CDC0  (vram 0x0020CDC0, 0xB6C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 91.52% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Structure, all constants, both jr-table dispatches (jtbl_002735B0 and jtbl_00273590, relocs pairing cleanly against the gen_jtbl_rodata expected object) and every branch shape/polarity are recovered; ~35 of 747 instructions differ, in four backend classes. (1) BIGGEST (~8 instrs incl. the frame):...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// SEMANTICS: per-frame driver for the save/load + memory-card task, dispatched on
// the global state byte D_00810131 (jtbl_002735B0) over the task block at
// D_00810130 (t[1] phase, t[2] sub-state, t[3] step, t[4] flags, t[0x10] screen id,
// t[0x11] card status, t[0x15] message id, t[0x34]/t[0x38] the animated
// progress-bar pair, t[0x3C]/t[0x3E] its tick/timeout counters).
//   0  cold start: reset the block, run func_0020DFA0, then map the pending
//      request D_008106B0 (6 = autosave, 5/4/2/1 = the load/save/format variants)
//      and, for request 1, the detailed error code D_008106B1 onto a screen id
//      (t[0x10]) and message id (t[0x15]); with no request, D_008106C5 picks the
//      controller-unplugged (4) or wrong-card (5) screens.
//   1,2 sub-state 0 installs the drawing callback func_0020E6F0 and arms the HUD
//      counters; sub-state 1 pumps the card poll (func_0020D930/func_00209DF0) and
//      converts either the card status t[0x11] or the free-space percentage
//      (100 - float_to_int(D_0081085C)) into the gauge index D_002821B8; the input
//      bits 0x830 cancel, 0x40 confirms; sub-state 2 animates the two gauge values
//      toward D_00810858/D_0081085C one unit per 3 frames until both arrive or the
//      0x78-frame timeout / an input in 0x870 ends it.
//   3  message screens: sub-state 0 fades in, sub-state 1 fires the text request
//      func_001FF080(0, id) for screen id 0..5 and waits for D_00275BD8 to clear,
//      sub-state 2 dispatches to the per-screen handlers (func_0020EE50 /
//      func_0020F950 / func_00211970 / func_00214020 / func_002072C0) or leaves.
//   4  restart at phase 1.   5  run the card sub-task func_0020E0C0.
//   6  shutdown: once func_00225AC0(1) reports done, tear the task down.
// Returns the sub-task result (nonzero = finished).
//
// NEARMISS 95.085% (pinned mwcc 2.3.1.01; 91.52% on mwcc 2.3.3).  Structure,
// constants, both jump tables and every branch shape are recovered.  Residuals,
// all backend: (1) CodeWarrior keeps the literal 100 alive in $s2 across the
// float_to_int call (materialised in the jal delay slot), so its frame is 0x40 and
// the compare re-reads $s2; mwcc rematerialises the constant and uses a 0x30 frame
// -- no C spelling reaches it (a function-scope `hundred` local was measured and
// does not).  (2) both jr-table dispatches emit `sll` one slot earlier.  (3) about
// ten 2-instruction swaps where CW hoists a store's `li` above the preceding
// volatile `sw`.  (4) one dead `addiu v0,1` CW re-materialises before the
// D_008106C5 == 2 arm.
extern int float_to_int(float);
extern void func_0015C700(void *);
extern void func_001AED80(int);
extern void func_001AFE60(void);
extern void func_001AFEB0(void);
extern int *func_001AFF10(void);
extern void func_001B0000(void);
extern void func_001FB9F0(int, int, int, int);
extern int func_001FEF70();
extern void func_001FF080(int, int);
extern void func_00200970(int);
extern void func_002072C0(void *);
extern void func_00209DF0(void *);
extern int func_0020A7A0(long long);
extern void func_0020CD40(void);
extern void func_0020CD60(void);
extern void func_0020CD80(void);
extern void func_0020D930(void *, int);
extern void func_0020DFA0(void);
extern void func_0020E020(void);
extern void func_0020E080(void);
extern int func_0020E0C0(void *);
extern void func_0020E250(void);
extern void func_0020EE50(void *);
extern void func_0020F950(void *);
extern void func_00211970(void *);
extern void func_00214020(void *);
extern void func_00225A00(void);
extern int func_00225AC0(int);
extern void func_0020E6F0(void);

extern unsigned char D_00275BD8;
extern volatile int D_002821B0[16];
extern volatile int D_002821B4[16];
extern volatile int D_002821B8[16];
extern volatile int D_00282240[16];
extern char D_00810130[];
extern unsigned char D_00810131[16];
extern float D_008104D0[4];
extern float D_008104D8[4];
extern char D_008102B0[16];
extern unsigned char D_008106B0[16];
extern unsigned char D_008106B1[16];
extern unsigned char D_008106C5[16];
extern float D_00810858[4];
extern float D_0081085C[4];
extern unsigned char D_00810C60[16];
extern unsigned short D_00810E74[8];

int func_0020CDC0(void) {
    unsigned char *t;
    int r;
    int lvl;
    int pct;
    int hundred;
    int *dl;
    int sel;
    float fv;

    t = (unsigned char *)D_00810130;
    r = 0;
    switch (D_00810131[0]) {
    case 0:
        func_001AED80(0);
        func_001FB9F0(0xB, 0x1000, 0x1000, 0x1000);
        t[2] = 0;
        t[3] = 0;
        t[4] = 0;
        t[5] = 0;
        func_0020DFA0();
        if (D_008106B0[0] == 6) {
            func_00225A00();
            t[0x10] = 0;
            t[1] = 3;
            t[2] = 2;
            t[4] = 2;
            t[0x15] = 3;
            goto done;
        }
        t[1] = 1;
        t[0xC] = D_00810C60[0];
        *(int *)(t + 8) = func_001FEF70();
        if (D_008106B0[0] != 0) {
            t[1] = 3;
            if (D_008106B0[0] == 5) {
                t[0x10] = 2;
            } else if (D_008106B0[0] == 4) {
                t[0x10] = 0;
                t[2] = 2;
                t[4] = 2;
                t[0x15] = 5;
            } else if (D_008106B0[0] == 1) {
                if (D_008106B1[0] & 0xC0) {
                    t[0x10] = 0;
                    t[2] = 2;
                    t[4] = 2;
                    t[0x15] = 3;
                } else if ((int)D_008106B1[0] < 0x17) {
                    t[0x10] = 2;
                    t[2] = 2;
                    t[4] = 2;
                    if ((int)D_008106B1[0] < 5) {
                        t[0x15] = 6;
                    } else if ((int)D_008106B1[0] < 7) {
                        t[0x15] = 2;
                    } else if ((int)D_008106B1[0] < 0xA) {
                        t[0x15] = 5;
                    } else if ((int)D_008106B1[0] < 0xF) {
                        t[0x15] = 3;
                    } else if ((int)D_008106B1[0] < 0x10) {
                        t[0x15] = 4;
                    } else {
                        t[2] = 0;
                        t[4] = 0;
                    }
                } else {
                    t[0x10] = 0;
                    t[2] = 2;
                    t[4] = 2;
                    if ((int)D_008106B1[0] < 0x1B) {
                        t[0x15] = 1;
                    } else if ((int)D_008106B1[0] < 0x1E) {
                        t[0x15] = 3;
                    } else if ((int)D_008106B1[0] < 0x23) {
                        t[0x15] = 5;
                    } else {
                        t[0x15] = 4;
                    }
                }
            } else if (D_008106B0[0] == 2) {
                t[0x10] = 1;
            } else {
                t[0x10] = 3;
            }
            goto done;
        }
        if (D_008106C5[0] != 0) {
            t[1] = 3;
            if (D_008106C5[0] != 2) {
                t[0x10] = 4;
            } else {
                t[0x10] = 5;
            }
        }
        goto done;
    case 1:
    case 2:
        switch (t[2]) {
        case 0:
            func_001AFEB0();
            func_001AFE60();
            func_0020E020();
            dl = func_001AFF10();
            dl[4] = (int)func_0020E6F0;
            func_0020E250();
            D_002821B0[0] = 4;
            D_002821B4[0] = 0;
            D_00282240[0] = 0;
            if (t[1] == 2) {
                t[2] = 2;
            } else {
                t[2] = t[2] + 1;
            }
            goto done;
        case 1:
            func_0020A7A0(((long long)0x20045EE5 << 0x20) | ((long long)0x9D42 << 0x10) | 0x1E40);
            func_001B0000();
            func_0020D930(t, 0);
            func_00209DF0(t);
            lvl = t[0x11];
            switch (lvl) {
            case 1:
                D_002821B4[0] = 1;
                D_002821B8[0] = 0;
                break;
            case 2:
                D_002821B4[0] = 1;
                D_002821B8[0] = 9;
                break;
            case 3:
                D_002821B4[0] = 1;
                D_002821B8[0] = 2;
                break;
            case 4:
                D_002821B4[0] = 1;
                D_002821B8[0] = 1;
                break;
            default:
                hundred = 100;
                pct = hundred - float_to_int(D_0081085C[0]);
                if (pct == hundred) {
                    D_002821B4[0] = 0;
                } else {
                    if (pct >= 0x51) {
                        D_002821B8[0] = 4;
                    } else if (pct >= 0x33) {
                        D_002821B8[0] = 5;
                    } else if (pct >= 0x1F) {
                        D_002821B8[0] = 6;
                    } else if (pct >= 0xB) {
                        D_002821B8[0] = 7;
                    } else if (pct > 0) {
                        D_002821B8[0] = 8;
                    } else {
                        D_002821B8[0] = 3;
                    }
                    D_002821B4[0] = 1;
                }
                break;
            }
            if (D_00810E74[0] & 0x830) {
                func_0020CD60();
                D_002821B4[0] = 0;
                t[1] = 5;
                t[2] = 0;
                t[3] = 0;
            } else if (D_00810E74[0] & 0x40) {
                if ((int)t[0x11] > 0 && (int)t[0x11] < 5) {
                    func_0020CD40();
                    D_002821B4[0] = 0;
                    t[1] = 3;
                    t[2] = 0;
                    t[3] = 0;
                    t[4] = 0;
                    if (t[0x11] == 3) {
                        t[0x10] = 1;
                    } else if (t[0x11] == 1) {
                        t[0x10] = 3;
                    } else if (t[0x11] == 2) {
                        t[0x10] = 2;
                    } else if (t[0x11] == 4) {
                        t[0x10] = 0;
                    }
                } else {
                    func_0020CD80();
                }
            }
            goto done;
        case 2:
            switch (t[3]) {
            case 0:
                *(short *)(t + 0x3C) = 3;
                *(short *)(t + 0x3E) = 0x78;
                t[3] = t[3] + 1;
                if (D_008106B0[0] == 2) {
                    D_002821B4[0] = 1;
                    D_00282240[0] = 5;
                    D_002821B8[0] = 0x14;
                }
                /* fallthrough */
            case 1:
                t[4] = 0;
                func_0020A7A0(((long long)0x20045EE5 << 0x20) | ((long long)0x9D42 << 0x10) | 0x1E40);
                func_001B0000();
                func_00209DF0(t);
                if (!(*(volatile int *)0x70003B64 & 0x3F)) {
                    func_001FB9F0(0xA, 0x1000, 0x1000, 0x1000);
                }
                *(short *)(t + 0x3C) = *(short *)(t + 0x3C) - 1;
                if (*(float *)(t + 0x34) != D_00810858[0]) {
                    if (*(short *)(t + 0x3C) == 0) {
                        D_00810858[0] += 1.0f;
                    }
                } else {
                    t[4] = 1;
                }
                if (*(float *)(t + 0x38) != D_0081085C[0]) {
                    if (*(short *)(t + 0x3C) == 0) {
                        D_0081085C[0] -= 1.0f;
                    }
                } else {
                    t[4] = t[4] | 2;
                }
                if (t[4] == 3 && *(short *)(t + 0x3E) != 0) {
                    *(short *)(t + 0x3E) = *(short *)(t + 0x3E) - 1;
                }
                if (*(short *)(t + 0x3E) == 0 || (D_00810E74[0] & 0x870)) {
                    fv = *(float *)(t + 0x34);
                    D_00810858[0] = fv;
                    D_008104D0[0] = fv;
                    fv = *(float *)(t + 0x38);
                    D_0081085C[0] = fv;
                    D_008104D8[0] = fv;
                    func_0015C700(D_008102B0);
                    if (D_00810E74[0] & 0x870) {
                        func_0020CD60();
                    }
                    if (D_008106B0[0] == 1) {
                        D_008106B0[0] = 0;
                        D_002821B0[0] = 4;
                        D_002821B4[0] = 0;
                        D_00282240[0] = 0;
                        t[1] = 1;
                        t[2] = 1;
                        t[3] = 0;
                    } else {
                        D_008106B0[0] = 0;
                        D_002821B4[0] = 0;
                        t[1] = 5;
                        t[2] = 0;
                        t[3] = 0;
                    }
                    goto done;
                }
                if (*(short *)(t + 0x3C) == 0) {
                    *(short *)(t + 0x3C) = 3;
                }
                goto done;
            }
            goto done;
        }
        goto done;
    case 3:
        switch (t[2]) {
        case 0:
            func_001AFEB0();
            func_001AFE60();
            t[2] = t[2] + 1;
            t[3] = 0;
            goto done;
        case 1:
            switch (t[3]) {
            case 0:
                switch (t[0x10]) {
                case 0:
                    D_00275BD8 = 1;
                    sel = 0x1F;
                    func_001FF080(0, sel);
                    t[3] = t[3] + 1;
                    break;
                case 1:
                    D_00275BD8 = 1;
                    sel = 0x1E;
                    func_001FF080(0, sel);
                    t[3] = t[3] + 1;
                    break;
                case 2:
                    D_00275BD8 = 1;
                    sel = 0x2C;
                    func_001FF080(0, sel);
                    t[3] = t[3] + 1;
                    break;
                case 3:
                    D_00275BD8 = 1;
                    sel = 0x24;
                    func_001FF080(0, sel);
                    t[3] = t[3] + 1;
                    break;
                case 4:
                    D_00275BD8 = 1;
                    sel = 0x25;
                    func_001FF080(0, sel);
                    t[3] = t[3] + 1;
                    break;
                case 5:
                    D_00275BD8 = 1;
                    sel = 0x26;
                    func_001FF080(0, sel);
                    t[3] = t[3] + 1;
                    break;
                default:
                    t[3] = *(volatile unsigned char *)(t + 3) + 1;
                    break;
                }
                goto done;
            case 1:
                if (D_00275BD8 == 0) {
                    t[2] = t[2] + 1;
                    t[3] = 0;
                    t[4] = 0;
                }
                goto done;
            }
            goto done;
        case 2:
            if (D_008106C5[0] == 0xFF) {
                D_002821B4[0] = 0;
                t[1] = 5;
                t[2] = 0;
                t[3] = 0;
                func_00200970(1);
            } else if (D_00275BD8 == 0 && D_008106B0[0] == 0 && D_008106C5[0] == 0 &&
                       (D_00810E74[0] & 0x810)) {
                func_0020CD60();
                D_002821B4[0] = 0;
                t[1] = 5;
                t[2] = 0;
                t[3] = 0;
                func_00200970(1);
            } else {
                switch (t[0x10]) {
                case 0:
                    func_0020EE50(t);
                    break;
                case 1:
                    func_0020F950(t);
                    break;
                case 2:
                    func_00211970(t);
                    break;
                case 3:
                    func_00214020(t);
                    break;
                case 4:
                case 5:
                    func_002072C0(t);
                    break;
                case 0x63:
                default:
                    D_002821B4[0] = 0;
                    t[1] = 4;
                    t[2] = 0;
                    t[3] = 0;
                    func_00200970(1);
                    break;
                case 8:
                    D_002821B4[0] = 0;
                    t[1] = 2;
                    t[2] = 0;
                    t[3] = 0;
                    func_00200970(1);
                    break;
                }
            }
            goto done;
        }
        goto done;
    case 4:
        t[1] = 1;
        t[2] = 0;
        t[3] = 0;
        goto done;
    case 5:
        r = func_0020E0C0(t);
        goto done;
    case 6:
        r = func_00225AC0(1);
        if (r != 0) {
            func_0020E080();
            func_001AFEB0();
            func_001FB9F0(0xD, 0x1000, 0x1000, 0x1000);
            t[0] = 0;
            t[1] = 0;
            t[2] = 0;
            t[3] = 0;
        }
        goto done;
    }
done:
    return r;
}
