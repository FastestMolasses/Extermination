// NEARMISS func_00218640  (vram 0x00218640, 0x744 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.03% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation-order / instruction-scheduling permutation, same class as the sibling FSM func_00217FA0 (99.15% NEARMISS) and func_001936E0 (94.99% NEARMISS). Body/structure/control-flow fully recovered and verified 1:1 against the .s (weapon-reload FSM, same shape as func_00217FA0 but with a...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Weapon/reload-style state machine, dispatched on arg0+5 (the state byte)
// via a branch-likely chain (case 4,3,2,1,0,default). Case 0 resets the
// per-weapon record array (arg0+0x18 count, entries at arg0+0x50 each 1
// byte "slot id"), always appends slot 0 then slot 1, plus slot 2 if
// D_00810C6D is set; if a pending swap request D_008106B0 is set, scans
// those slots for one matching D_008106B1-7, latches its index at +0x17,
// force-sets the FSM to state 3 with a 0xF0 countdown at +6, clears the
// swap request, and returns (through the shared tail). Otherwise it falls
// through into case 1 (advancing the state byte first): if the "fire" input
// bit (0x20) is held, jump straight to a "quick reload" path (matches
// func_0020CD60's tail-thunk, state->0). Otherwise it starts the normal
// reload animation (func_0020AE40/func_0020BF20/func_0020B210), and either
// advances the state (+1) on success, or (once +0x18 has entries and input
// bit 0x40 is held) may kick off a different animation depending on whether
// the currently-latched color (+0x50 of the latched slot, offset by +7) vs.
// D_00810CA7 differs, or bump the busy counter at +7 if unchanged.
// Case 2 runs the mid-reload animation and, on completion signal, backs the
// state down by one. Case 3 runs the tail animation and, once its frame
// countdown (+6) hits 0 (or input bits 0x5060 are held), snaps back to
// state 1. Case 4 (the ==4 fast path, checked first) runs the finishing
// animation, resets some global mode trackers, ticks a duck-in/out counter
// at +6 based on input bits 0x8000/0x2000, and then (bit 0x40 held) either
// requests a re-reload if +6 isn't 0, or (else) commits the slot swap
// (computes a fresh D_00810CA7 color id from the latched slot + a table
// offset at +0x1E, updates the D_00810CA4/CA5/CA6 mode-tracker trio, resets
// to state 0), or (bit 0x20 held) requests the quick-reload tail. Every
// path falls through to the shared tail which decrements a busy-counter at
// +7 if set.
extern void func_001FB9F0(int id, int a1, int a2, int a3);
extern void func_001FCF10(void);
extern void func_00207D00(int slot, int mode);
extern void func_0020A7A0(long long tag);
extern void func_0020AE40(void *arg0, void *p, int flags);
extern void func_0020B0D0(int arg0, char *arg1);
extern int func_0020B210(void *arg0, void *p, long long tag, int flags);
extern void func_0020BBE0(char *arg0, int arg1);
extern int func_0020BC50(void *arg0, void *p, long long tag, int flags);
extern void func_0020BF20(void *p, int a1, int a2);
extern void func_0020CCB0(unsigned char *p);
extern void func_0020CD40(void);
extern void func_0020CD60(void);
extern void func_0020CD80(void);
extern void func_0020CDA0(void);

extern char D_002663F0[];
extern char D_00266460[];
extern char D_002664A0[];
extern int D_002821B0;
extern int D_00282240;
extern int D_002821B4;
extern int D_002821B8;
extern unsigned char D_008106B0;
extern unsigned char D_008106B1;
extern unsigned char D_00810C6B;
extern unsigned char D_00810C6C;
extern unsigned char D_00810C6D;
extern unsigned char D_00810CA4;
extern unsigned char D_00810CA5;
extern unsigned char D_00810CA6;
extern unsigned char D_00810CA7;
extern unsigned short D_00810E74;

void func_00218640(char *arg0) {
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
        *(short *)(arg0 + 0x1E) = 7;
        if (D_00810CA7 != 0xFF) {
            *(unsigned char *)(arg0 + 0x12) = D_00810CA7;
        } else {
            *(unsigned char *)(arg0 + 0x12) = 0xFF;
        }

        if (D_00810C6B != 0) {
            idx = *(unsigned char *)(arg0 + 0x18);
            *(unsigned char *)(arg0 + 0x18) = idx + 1;
            arg0[idx + 0x50] = 0;
        }
        if (D_00810C6C != 0) {
            unsigned char val = 1;
            idx = *(unsigned char *)(arg0 + 0x18);
            *(unsigned char *)(arg0 + 0x18) = idx + 1;
            arg0[idx + 0x50] = val;
        }
        if (D_00810C6D != 0) {
            unsigned char val = 2;
            idx = *(unsigned char *)(arg0 + 0x18);
            *(unsigned char *)(arg0 + 0x18) = idx + 1;
            arg0[idx + 0x50] = val;
        }

        if (D_008106B0 != 0) {
            unsigned char want;
            int i;
            want = (unsigned char) (D_008106B1 - 7);
            *(unsigned char *)(arg0 + 0x1B) = want;
            for (i = 0; i < *(unsigned char *)(arg0 + 0x18); i++) {
                if ((unsigned char) arg0[i + 0x50] == want) {
                    *(char *)(arg0 + 0x17) = (char) i;
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
            func_0020A7A0(0x200424C5LL << 32 | 0x9D4220D0u);
            func_0020AE40(arg0, D_002663F0, 0x80);
            func_0020BF20(D_00266460, 1, 0);
            if (func_0020B210(arg0, D_002664A0, 0x20041D05LL << 32 | 0xA1321F80u, 0x80) != 0) {
                *(short *)(arg0 + 0x1C) = 0;
                *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
                func_0020BBE0(arg0, *(unsigned char *)(arg0 + 0x1A));
            } else if (*(unsigned char *)(arg0 + 0x18) != 0 && (D_00810E74 & 0x40)) {
                if (D_00810CA7 != (unsigned char) (*(unsigned char *)(arg0 + *(unsigned char *)(arg0 + 0x17) + 0x50) + 7)) {
                    func_0020CD40();
                    *(unsigned char *)(arg0 + 5) = 4;
                    *(unsigned char *)(arg0 + 6) = 1;
                } else if (*(unsigned char *)(arg0 + 7) == 0) {
                    func_0020CD80();
                    *(unsigned char *)(arg0 + 7) = 0x10;
                }
            }
            func_0020B0D0((int) arg0, D_002663F0);
        }
        break;

    case 2:
        func_0020A7A0(0x200424C5LL << 32 | 0x9D4220D0u);
        func_0020AE40(arg0, D_002663F0, 0x80);
        func_0020BF20(D_00266460, 1, 0);
        if (func_0020BC50(arg0, D_002664A0, 0x20041D05LL << 32 | 0xA1321F80u, 0x80) != 0) {
            *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) - 1;
        }
        func_0020B0D0((int) arg0, D_002663F0);
        break;

    case 3:
        func_0020A7A0(0x200424C5LL << 32 | 0x9D4220D0u);
        func_0020AE40(arg0, D_002663F0, 0x80);
        func_0020BF20(D_00266460, 1, 0);
        func_0020B210(arg0, D_002664A0, 0x20041D05LL << 32 | 0xA1321F80u, 0x480);
        func_0020B0D0((int) arg0, D_002663F0);
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
        func_0020A7A0(0x200424C5LL << 32 | 0x9D4220D0u);
        func_0020AE40(arg0, D_002663F0, 0x80);
        func_0020BF20(D_00266460, 1, 0);
        func_0020B210(arg0, D_002664A0, 0x20041D05LL << 32 | 0xA1321F80u, 0x480);
        func_0020B0D0((int) arg0, D_002663F0);
        D_002821B4 = 1;
        D_00282240 = 5;
        D_002821B8 = 4;
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
                D_00810CA7 = (unsigned char) (*(unsigned char *)(arg0 + *(unsigned char *)(arg0 + 0x17) + *(unsigned char *)(arg0 + 0x19) + 0x50) + *(short *)(arg0 + 0x1E));
                *(unsigned char *)(arg0 + 0x12) = D_00810CA7;
                if (D_00810CA4 != 0xFF) {
                    if (D_00810CA4 == 2) {
                        D_00810CA6 = 0;
                        D_00810CA5 = 5;
                    } else {
                        D_00810CA5 = 5;
                        if (D_00810CA6 == 0xFF) {
                            D_00810CA6 = 0;
                        }
                    }
                    D_00810CA4 = 0xFF;
                }
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
