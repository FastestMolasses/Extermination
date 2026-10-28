// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 95.33% NEARMISS).
// Fixes read from the original instructions (twin of func_00217090):
//  - The shared game-state words at 0x002821B0/B4/B8 and 0x00282240 and the
//    menu bytes at 0x00810CA6/CA7 are written through their literal addresses
//    (mwcc keeps those accesses in source order: B4 before 2240, CA7 stored
//    before CA6 is re-read); CA7 is an unsigned byte.
//  - The state-0 flag scan reads D_00810700[i + 0x564] with the slot value
//    computed into a byte local first; one shared int loop counter serves
//    both state-0 loops, and the search compares the list byte first.
//  - The state-1 candidate logic was wrong: the candidate is list byte + 0xA,
//    tested as (cand - 0xC) < 3, the flag mask is compared against 7 only,
//    and the out-of-range path keeps the candidate itself; the first flag
//    is an if/else assignment.
//  - The list byte is read as arg0[index + 0x50] and the draw mode is
//    spelled (byte >= 2) ? 4 : 0.
//
// Weapon/reload-style state machine (same family as func_00217090 /
// func_00217FA0), dispatched on arg0+5 (the state byte) via a
// branch-likely chain (case 4,3,2,1,0,default). Case 0 resets the
// per-weapon record array (arg0+0x18 count, entries at arg0+0x50 each 1
// byte "slot id"), scanning the five flag bytes D_00810700[0x564 + 0xA..0xE]
// and appending a slot per nonzero entry; if a pending swap
// request D_008106B0 is set, scans those slots for one matching
// D_008106B1-0xA, latches its index (row/col at +0x17/+0x19), force-sets
// the FSM to state 3 with a 0xF0 countdown at +6, clears the swap request,
// and returns (through the shared tail). Otherwise it falls through into
// case 1 (advancing the state byte first): if the "fire" input bit (0x20)
// is held, jump straight to a "quick reload" path (func_0020CD60's
// tail-thunk, state->0). Otherwise it starts the normal reload animation
// (func_0020AE40/func_0020BF20/func_0020B210), and either aborts back one
// state step if that failed, or (once +0x18 has entries and input bit 0x40
// is held) computes a candidate (list byte of the active slot + 0xA); for
// candidates 0xC..0xE it latches D_0081070B and, when all three flags
// D_00810C70..C72 are set, picks 0xC instead of +0x12; a candidate that
// differs from +0x12 is stored at +0x13 and starts state 4.
// Case 2 runs the mid-reload animation and, on completion signal, backs
// the state up by one. Case 3 draws, counts the +6 timer down and returns
// to state 1 when it expires or a button in 0x5060 is pressed. Case 4
// draws, resets some global mode trackers, ticks a duck-in/out counter at +6 based on input bits
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
extern unsigned char D_008106B0;
extern unsigned char D_008106B1;
extern unsigned char D_00810700[];
extern unsigned char D_0081070B;
extern unsigned char D_00810C70;
extern unsigned char D_00810C71;
extern unsigned char D_00810C72;
extern unsigned char D_00810CA4;
extern unsigned char D_00810CA5;
extern unsigned short D_00810E74;
extern int D_008106E0;
extern int D_008106E4;
extern int D_008106E8;

void func_002177B0(char *arg0) {
    unsigned char st;
    unsigned char cnt;
    unsigned char idx;
    int i;

    st = *(unsigned char *)(arg0 + 5);

    switch (st) {
    case 0:
        *(char *)(arg0 + 0x17) = 0;
        *(unsigned char *)(arg0 + 0x19) = 0;
        *(unsigned char *)(arg0 + 0x18) = 0;
        *(unsigned char *)(arg0 + 0x1A) = 0;
        *(int *)0x002821B0 = 4;
        *(int *)0x002821B4 = 0;
        *(int *)0x00282240 = 3;
        *(unsigned char *)(arg0 + 7) = 0;
        *(short *)(arg0 + 0x1E) = 0xA;

        if (D_00810CA4 != 0xFF) {
            *(unsigned char *)(arg0 + 0x12) = (unsigned char) (D_00810CA4 + (unsigned char) *(short *)(arg0 + 0x1E));
        } else {
            *(unsigned char *)(arg0 + 0x12) = 0xFF;
        }

        for (i = 0xA; i < 0xF; i++) {
            if (D_00810700[i + 0x564] != 0) {
                unsigned char v = i - 0xA;
                idx = *(unsigned char *)(arg0 + 0x18);
                *(unsigned char *)(arg0 + 0x18) = idx + 1;
                arg0[idx + 0x50] = v;
            }
        }

        if (D_008106B0 != 0) {
            *(unsigned char *)(arg0 + 0x1B) = (unsigned char) (D_008106B1 - 0xA);
            for (i = 0; i < *(unsigned char *)(arg0 + 0x18); i++) {
                if (*(unsigned char *)(arg0 + 0x50 + i) == *(unsigned char *)(arg0 + 0x1B)) {
                    *(char *)(arg0 + 0x19) = (char) ((i >> 2) * 4);
                    *(char *)(arg0 + 0x17) = (char) (i % 4);
                    *(int *)0x00282240 = 4;
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
            *(int *)0x002821B4 = 2;
            *(char *)(arg0 + 1) = 3;
            *(char *)(arg0 + 2) = 0;
            *(char *)(arg0 + 3) = 0;
            *(char *)(arg0 + 4) = 0;
            *(unsigned char *)(arg0 + 5) = 0;
        } else {
            func_0020A7A0(0x200427C5LL << 32 | 0x9D422110u);
            func_0020AE40(arg0, D_002661C0, 0x20);
            { unsigned char slotColor = (unsigned char)arg0[func_0020BEF0(arg0) + 0x50]; func_0020BF20(D_00266230, 1, (slotColor >= 2) ? 4 : 0); }
            if (func_0020B210(arg0, D_00266270, 0x20042805LL << 32 | 0xA1321F80u, 0x20) != 0) {
                *(short *)(arg0 + 0x1C) = 0;
                *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
                func_0020BBE0(arg0, *(unsigned char *)(arg0 + 0x1A));
            } else if (*(unsigned char *)(arg0 + 0x18) != 0 && (D_00810E74 & 0x40)) {
                int want = (unsigned char)arg0[func_0020BEF0(arg0) + 0x50] + 0xA;
                int cur;
                if ((unsigned int) (want - 0xC) < 3) {
                    int full;
                    D_0081070B = want - 0xC;
                    if (D_00810C70 != 0) {
                        full = 1;
                    } else {
                        full = 0;
                    }
                    if (D_00810C71 != 0) {
                        full |= 2;
                    }
                    if (D_00810C72 != 0) {
                        full |= 4;
                    }
                    if (full == 7) {
                        want = 0xC;
                    } else {
                        want = *(unsigned char *)(arg0 + 0x12);
                    }
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
        { unsigned char slotColor = (unsigned char)arg0[func_0020BEF0(arg0) + 0x50]; func_0020BF20(D_00266230, 1, (slotColor >= 2) ? 4 : 0); }
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
            *(int *)0x00282240 = 3;
        }
        break;

    case 4:
        func_0020A7A0(0x200427C5LL << 32 | 0x9D422110u);
        func_0020AE40(arg0, D_002661C0, 0x20);
        { unsigned char slotColor = (unsigned char)arg0[func_0020BEF0(arg0) + 0x50]; func_0020BF20(D_00266230, 1, (slotColor >= 2) ? 4 : 0); }
        func_0020B210(arg0, D_00266270, 0x20042805LL << 32 | 0xA1321F80u, 0x420);
        func_0020B0D0((int) arg0, D_002661C0);
        *(int *)0x002821B4 = 1;
        *(int *)0x00282240 = 5;
        if (*(unsigned char *)(arg0 + 0x13) == 0xC) {
            *(int *)0x002821B8 = 3;
        } else {
            *(int *)0x002821B8 = 2;
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
                *(int *)0x002821B4 = 0;
                *(int *)0x00282240 = 3;
                *(unsigned char *)(arg0 + 5) = 1;
                func_0020CD60();
            } else {
                func_001FB9F0(0x17B, 0x1000, 0x1000, 0x1000);
                if (*(unsigned char *)(arg0 + 0x13) == 0xC) {
                    D_00810CA4 = 2;
                    D_00810CA5 = 0xFF;
                    *(unsigned char *)0x00810CA6 = 0xFF;
                    *(unsigned char *)0x00810CA7 = 0xFF;
                } else {
                    D_008106E8 = 0;
                    D_008106E4 = 0;
                    D_008106E0 = 0;
                    D_00810CA4 = (unsigned char) (*(unsigned char *)(arg0 + 0x13) - (unsigned char) *(short *)(arg0 + 0x1E));
                    D_00810CA5 = 0xFF;
                    *(unsigned char *)0x00810CA7 = 0xFF;
                    if (*(unsigned char *)0x00810CA6 == 0xFF) {
                        *(unsigned char *)0x00810CA6 = 0;
                    }
                }
                *(unsigned char *)(arg0 + 0x12) = (unsigned char) (D_00810CA4 + (unsigned char) *(short *)(arg0 + 0x1E));
                *(int *)0x002821B4 = 2;
                *(char *)(arg0 + 1) = 3;
                *(char *)(arg0 + 2) = 0;
                *(char *)(arg0 + 3) = 0;
                *(char *)(arg0 + 4) = 0;
                *(unsigned char *)(arg0 + 5) = 0;
            }
        } else if (D_00810E74 & 0x20) {
            func_0020CD60();
            *(int *)0x002821B4 = 0;
            *(int *)0x00282240 = 3;
            *(unsigned char *)(arg0 + 5) = 1;
        }
        break;
    }

    cnt = *(unsigned char *)(arg0 + 7);
    if (cnt != 0) {
        *(unsigned char *)(arg0 + 7) = cnt - 1;
    }
}
