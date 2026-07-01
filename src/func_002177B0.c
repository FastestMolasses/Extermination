// NEARMISS func_002177B0  (vram 0x002177B0, 0x7F0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.33% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// compiler artifact: register-allocation/coloring permutation (at-vs-v0/v1/a0 naming) throughout the function body. Body and control-flow are fully recovered and structurally identical to the target disassembly (dispatch chain, D_0081070A scan loop, the D_0081070B/D_00810C70-72 'full house' flag te...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Weapon/reload-style state machine (same family as func_00217090 /
// func_00217FA0), dispatched on arg0+5 (the state byte) via a
// branch-likely chain (case 4,3,2,1,0,default). Case 0 resets the
// per-weapon record array (arg0+0x18 count, entries at arg0+0x50 each 1
// byte "slot id"), scanning D_0081070A[0xA..0xE] (a fixed 5-entry table at
// +0x564 stride) and appending a slot per nonzero entry; if a pending swap
// request D_008106B0 is set, scans those slots for one matching
// D_008106B1-0xA, latches its index (row/col at +0x17/+0x19), force-sets
// the FSM to state 3 with a 0xF0 countdown at +6, clears the swap request,
// and returns (through the shared tail). Otherwise it falls through into
// case 1 (advancing the state byte first): if the "fire" input bit (0x20)
// is held, jump straight to a "quick reload" path (func_0020CD60's
// tail-thunk, state->0). Otherwise it starts the normal reload animation
// (func_0020AE40/func_0020BF20/func_0020B210), and either aborts back one
// state step if that failed, or (once +0x18 has entries and input bit 0x40
// is held) may replay a different animation depending on the currently
// active color (+0x50 of the latched slot) vs computed candidate +0x12.
// Case 2 runs the mid-reload animation and, on completion signal, backs
// the state up by one. Case 3 (dispatch label .L00217CD8, entered when
// state==4) runs the finishing animation, resets some global mode
// trackers, ticks a duck-in/out counter at +6 based on input bits
// 0x8000/0x2000, and then (bit 0x40 held) either requests a re-reload if
// +6 isn't 0 or commits the slot swap (updates D_00810CA4..CA7, resets to
// state 1), or (bit 0x20 held) requests the quick-reload tail. Every path
// falls through to the shared tail which decrements a busy-counter at +7
// if set.
extern void func_001FB9F0(int id, int a1, int a2, int a3);
extern void func_001FCF10(void);
extern void func_00207D00(int slot, int mode);
extern void func_0020A7A0(long long tag);
extern void func_0020AE40(void *arg0, void *p, int flags);
extern void func_0020B0D0(int arg0, char *arg1);
extern int func_0020B210(void *arg0, void *p, long long tag, int flags);
extern int func_0020BC50(void *arg0, void *p, long long tag, int flags);
extern int func_0020BEF0(void *arg0);
extern void func_0020BBE0(char *arg0, int arg1);
extern void func_0020BF20(void *p, int a1, int a2);
extern void func_0020CCB0(unsigned char *p);
extern void func_0020CD40(void);
extern void func_0020CD60(void);
extern void func_0020CD80(void);
extern void func_0020CDA0(void);

extern char D_002661C0[];
extern char D_00266230[];
extern char D_00266270[];
extern int D_002821B0;
extern int D_00282240;
extern int D_002821B4;
extern int D_002821B8;
extern unsigned char D_008106B0;
extern unsigned char D_008106B1;
extern unsigned char D_0081070A[];
extern unsigned char D_0081070B;
extern unsigned char D_00810C70;
extern unsigned char D_00810C71;
extern unsigned char D_00810C72;
extern unsigned char D_00810CA4;
extern unsigned char D_00810CA5;
extern unsigned char D_00810CA6;
extern signed char D_00810CA7;
extern unsigned short D_00810E74;
extern int D_008106E0;
extern int D_008106E4;
extern int D_008106E8;

void func_002177B0(char *arg0) {
    unsigned char st;
    unsigned char cnt;
    unsigned char idx;

    st = *(unsigned char *)(arg0 + 5);

    switch (st) {
    case 0:
        *(char *)(arg0 + 0x17) = 0;
        *(unsigned char *)(arg0 + 0x19) = 0;
        *(unsigned char *)(arg0 + 0x18) = 0;
        *(unsigned char *)(arg0 + 0x1A) = 0;
        D_002821B0 = 4;
        D_002821B4 = 0;
        D_00282240 = 3;
        *(unsigned char *)(arg0 + 7) = 0;
        *(short *)(arg0 + 0x1E) = 0xA;

        if (D_00810CA4 != 0xFF) {
            *(unsigned char *)(arg0 + 0x12) = (unsigned char) (D_00810CA4 + (unsigned char) *(short *)(arg0 + 0x1E));
        } else {
            *(unsigned char *)(arg0 + 0x12) = 0xFF;
        }

        {
            int i;
            char *p = (char *)D_0081070A;
            for (i = 0xA; i < 0xF; i++) {
                if (*(unsigned char *)(p + 0x564) != 0) {
                    idx = *(unsigned char *)(arg0 + 0x18);
                    *(unsigned char *)(arg0 + 0x18) = idx + 1;
                    arg0[idx + 0x50] = (char) (i - 0xA);
                }
                p += 1;
            }
        }

        if (D_008106B0 != 0) {
            int i;
            *(unsigned char *)(arg0 + 0x1B) = (unsigned char) (D_008106B1 - 0xA);
            for (i = 0; i < *(unsigned char *)(arg0 + 0x18); i++) {
                if (*(unsigned char *)(arg0 + i + 0x50) == *(unsigned char *)(arg0 + 0x1B)) {
                    *(char *)(arg0 + 0x19) = (char) ((i >> 2) * 4);
                    *(char *)(arg0 + 0x17) = (char) (i % 4);
                    D_00282240 = 4;
                    break;
                }
            }
            D_008106B0 = 0;
            *(unsigned char *)(arg0 + 5) = 3;
            *(unsigned char *)(arg0 + 6) = 0xF0;
            break;
        }

        *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
        /* fall through */
    case 1:
        if (D_00810E74 & 0x20) {
            func_0020CD60();
            D_002821B4 = 2;
            *(char *)(arg0 + 1) = 3;
            *(char *)(arg0 + 2) = 0;
            *(char *)(arg0 + 3) = 0;
            *(char *)(arg0 + 4) = 0;
            *(unsigned char *)(arg0 + 5) = 0;
        } else {
            func_0020A7A0(0x200427C5LL << 32 | 0x9D422110u);
            func_0020AE40(arg0, D_002661C0, 0x20);
            { unsigned char slotColor = *(unsigned char *)(arg0 + func_0020BEF0(arg0) + 0x50); func_0020BF20(D_00266230, 1, (slotColor < 2) ? 0 : 4); }
            if (func_0020B210(arg0, D_00266270, 0x20042805LL << 32 | 0xA1321F80u, 0x20) != 0) {
                *(short *)(arg0 + 0x1C) = 0;
                *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
                func_0020BBE0(arg0, *(unsigned char *)(arg0 + 0x1A));
            } else if (*(unsigned char *)(arg0 + 0x18) != 0 && (D_00810E74 & 0x40)) {
                unsigned char slotColor = *(unsigned char *)(arg0 + func_0020BEF0(arg0) + 0x50);
                int cand = slotColor + 0xA - 0xC;
                int want;
                int cur;
                if ((unsigned int) cand < 3) {
                    int full;
                    int cmp;
                    D_0081070B = cand;
                    full = (D_00810C70 != 0) ? 1 : 0;
                    if (D_00810C71 != 0) {
                        full |= 2;
                    }
                    cmp = 0;
                    if (D_00810C72 != 0) {
                        full |= 4;
                        cmp = 7;
                    }
                    if (full == cmp) {
                        want = 0xC;
                    } else {
                        want = *(unsigned char *)(arg0 + 0x12);
                    }
                } else {
                    want = *(unsigned char *)(arg0 + 0x12);
                }
                cur = *(unsigned char *)(arg0 + 0x12);
                if (cur != want) {
                    *(unsigned char *)(arg0 + 0x13) = want;
                    func_0020CD40();
                    *(unsigned char *)(arg0 + 5) = 4;
                    *(unsigned char *)(arg0 + 6) = 1;
                } else if (*(unsigned char *)(arg0 + 7) == 0) {
                    func_0020CD80();
                    *(unsigned char *)(arg0 + 7) = 0x10;
                }
            }
            func_0020B0D0((int) arg0, D_002661C0);
        }
        break;

    case 2:
        func_0020A7A0(0x200427C5LL << 32 | 0x9D422110u);
        func_0020AE40(arg0, D_002661C0, 0x20);
        func_0020BF20(D_00266230, 1, 0);
        if (func_0020BC50(arg0, D_00266270, 0x20042805LL << 32 | 0xA1321F80u, 0x20) != 0) {
            *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) - 1;
        }
        func_0020B0D0((int) arg0, D_002661C0);
        break;

    case 3:
        func_0020A7A0(0x200427C5LL << 32 | 0x9D422110u);
        func_0020AE40(arg0, D_002661C0, 0x20);
        { unsigned char slotColor = *(unsigned char *)(arg0 + func_0020BEF0(arg0) + 0x50); func_0020BF20(D_00266230, 1, (slotColor < 2) ? 0 : 4); }
        func_0020B210(arg0, D_00266270, 0x20042805LL << 32 | 0xA1321F80u, 0x420);
        func_0020B0D0((int) arg0, D_002661C0);
        cnt = *(unsigned char *)(arg0 + 6) - 1;
        *(unsigned char *)(arg0 + 6) = cnt;
        if (cnt == 0 || (D_00810E74 & 0x5060)) {
            if (D_00810E74 & 0x5060) {
                func_0020CD60();
            }
            D_008106B0 = 0;
            *(unsigned char *)(arg0 + 5) = 1;
            D_00282240 = 3;
        }
        break;

    case 4:
        func_0020A7A0(0x200427C5LL << 32 | 0x9D422110u);
        func_0020AE40(arg0, D_002661C0, 0x20);
        { unsigned char slotColor = *(unsigned char *)(arg0 + func_0020BEF0(arg0) + 0x50); func_0020BF20(D_00266230, 1, (slotColor < 2) ? 0 : 4); }
        func_0020B210(arg0, D_00266270, 0x20042805LL << 32 | 0xA1321F80u, 0x420);
        func_0020B0D0((int) arg0, D_002661C0);
        D_002821B4 = 1;
        D_00282240 = 5;
        if (*(unsigned char *)(arg0 + 0x13) == 0xC) {
            D_002821B8 = 3;
        } else {
            D_002821B8 = 2;
        }
        func_001FCF10();
        func_00207D00(1, 3);

        if (D_00810E74 & 0x8000) {
            unsigned char v = *(unsigned char *)(arg0 + 6);
            if (v != 0) {
                *(unsigned char *)(arg0 + 6) = v - 1;
                func_0020CDA0();
            }
        } else if (D_00810E74 & 0x2000) {
            unsigned char v = *(unsigned char *)(arg0 + 6);
            if (v == 0) {
                *(unsigned char *)(arg0 + 6) = v + 1;
                func_0020CDA0();
            }
        }
        func_0020CCB0((unsigned char *) arg0);

        if (D_00810E74 & 0x40) {
            if (*(unsigned char *)(arg0 + 6) != 0) {
                D_002821B4 = 0;
                D_00282240 = 3;
                *(unsigned char *)(arg0 + 5) = 1;
                func_0020CD60();
            } else {
                func_001FB9F0(0x17B, 0x1000, 0x1000, 0x1000);
                if (*(unsigned char *)(arg0 + 0x13) == 0xC) {
                    D_00810CA4 = 2;
                    D_00810CA5 = 0xFF;
                    D_00810CA6 = 0xFF;
                    D_00810CA7 = 0xFF;
                } else {
                    D_008106E8 = 0;
                    D_008106E4 = 0;
                    D_008106E0 = 0;
                    D_00810CA4 = (unsigned char) (*(unsigned char *)(arg0 + 0x13) - (unsigned char) *(short *)(arg0 + 0x1E));
                    D_00810CA5 = 0xFF;
                    D_00810CA7 = 0xFF;
                    if (D_00810CA6 == 0xFF) {
                        D_00810CA6 = 0;
                    }
                }
                *(unsigned char *)(arg0 + 0x12) = (unsigned char) (D_00810CA4 + (unsigned char) *(short *)(arg0 + 0x1E));
                D_002821B4 = 2;
                *(char *)(arg0 + 1) = 3;
                *(char *)(arg0 + 2) = 0;
                *(char *)(arg0 + 3) = 0;
                *(char *)(arg0 + 4) = 0;
                *(unsigned char *)(arg0 + 5) = 0;
            }
        } else if (D_00810E74 & 0x20) {
            func_0020CD60();
            D_002821B4 = 0;
            D_00282240 = 3;
            *(unsigned char *)(arg0 + 5) = 1;
        }
        break;
    }

    cnt = *(unsigned char *)(arg0 + 7);
    if (cnt != 0) {
        *(unsigned char *)(arg0 + 7) = cnt - 1;
    }
}
