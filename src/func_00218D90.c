// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 94.05% NEARMISS).
// Fixes read from the original instructions:
//  - The shared game-state words at 0x002821B0/B4/B8 and 0x00282240 are written
//    through their literal addresses, and the mode-tracker bytes
//    D_00810CA4..CA7 are one array (as in the twin func_00218640), which keeps
//    the original load/store order.
//  - The colour clamp is a switch with ascending cases (1, 2, 3, 4, default),
//    not a nested if chain; mwcc 2.3.3 emits its compares in reverse source
//    order and keeps the cclr==3 arm a separate branch, as the original does.
//    This also removed the stray delay-slot-fill leftovers elsewhere.
//  - Case 1's colour compare reads the slot index into a local and indexes
//    the list as (k + arg0)[0x50] with an unsigned char cast, the tracker
//    byte on the left of the compare.
//  - Case 4's new colour is the list byte (index + arg0)[0x50] plus the signed
//    short at +0x1E, added in an int and stored into the tracker byte.
// Weapon reload/animation FSM dispatched on arg0+5 (the state byte), sibling
// of func_00217090/func_00217FA0 (same field offsets: +5 state, +6 countdown,
// +7 busy-counter, +0x12 mode index, +0x17..0x1E slot-search/heading fields,
// +0x18/+0x50 slot table). Case 0 resets the record, seeds +0x12 from
// D_00810CA6, then scans a fixed 5-entry global flag table (D_00810700[i+0x564]
// for i in 0..4) appending each SET index into the +0x50 slot table (instead
// of appending fixed literal ids like the func_00217FA0 sibling). If a pending
// swap request (D_008106B0) is set, it searches those slots for one matching
// D_008106B1, latches the found index (+0x17/+0x19), force-jumps the FSM to
// state 3 with a 0xF0 countdown, and returns via the shared tail. Otherwise it
// advances to state 1 (falling through): case 1, if the "quick" input bit
// 0x20 is held, jumps straight to a quick-cancel path (func_0020CD60,
// state->2); else it starts the reload animation (func_0020AE40/
// func_0020BF20 with a color-clamp derived from the active slot's color
// byte/func_0020B210), and on success advances state / defers to
// func_0020BBE0, or (once slots exist and input bit 0x40 is held) may replay
// a different animation depending on the active slot's color vs D_00810CA6.
// Case 2 runs the mid-reload animation and backs the state up on completion.
// Case 3 runs the tail animation and, once its frame countdown (+6) hits 0
// (or input bits 0x5060 are held), snaps back to state 1. Case 4 runs the
// finishing animation, resets global mode trackers, ticks a duck-in/out
// counter at +6 based on input bits 0x8000/0x2000, and then (bit 0x40 held)
// either requests a re-reload if +6 isn't 0 or commits the color swap
// (updates D_00810CA6/D_00810CA5/D_00810CA7/D_00810CA4, resets to state 0),
// or (bit 0x20 held) requests the quick-cancel tail. Every path falls through
// to the shared tail which decrements the busy-counter at +7 if set.
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
extern int func_0020BEF0(unsigned char *a0);
extern void func_0020CCB0(unsigned char *p);
extern void func_0020CD40(void);
extern void func_0020CD60(void);
extern void func_0020CD80(void);
extern void func_0020CDA0(void);

extern char D_002664F0[];
extern char D_00266560[];
extern char D_002665A0[];
extern unsigned char D_008106B0;
extern unsigned char D_008106B1;
extern unsigned char D_00810700[];
extern unsigned char D_00810CA4[4];
extern unsigned short D_00810E74;

void func_00218D90(char *arg0) {
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
        *(short *)(arg0 + 0x1E) = 0;

        if (D_00810CA4[2] != 0xFF) {
            *(unsigned char *)(arg0 + 0x12) = D_00810CA4[2];
        } else {
            *(unsigned char *)(arg0 + 0x12) = 0xFF;
        }

        i = 0;
        do {
            if (D_00810700[i + 0x564] != 0) {
                idx = *(unsigned char *)(arg0 + 0x18);
                *(unsigned char *)(arg0 + 0x18) = idx + 1;
                arg0[idx + 0x50] = (char) i;
            }
            i += 1;
        } while (i < 5);

        if (D_008106B0 != 0) {
            *(unsigned char *)(arg0 + 0x1B) = D_008106B1;
            for (i = 0; i < (int) *(unsigned char *)(arg0 + 0x18); i++) {
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
            unsigned char cclr;
            int clamp;
            func_0020A7A0(0x20043885LL << 32 | 0x9D422130u);
            func_0020AE40(arg0, D_002664F0, 0x100);
            cclr = (unsigned char) arg0[func_0020BEF0((unsigned char *) arg0) + 0x50];
            switch (cclr) {
        case 1: clamp = 1; break;
        case 2: clamp = 2; break;
        case 3: clamp = 2; break;
        case 4: clamp = 3; break;
        default: clamp = 0; break;
        }
            func_0020BF20(D_00266560, 1, clamp);
            if (func_0020B210(arg0, D_002665A0, 0x20042C05LL << 32 | 0xA1321F80u, 0x100) != 0) {
                *(short *)(arg0 + 0x1C) = 0;
                *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
                func_0020BBE0(arg0, *(unsigned char *)(arg0 + 0x1A));
            } else if (*(unsigned char *)(arg0 + 0x18) != 0 && (D_00810E74 & 0x40)) {
                int k = func_0020BEF0((unsigned char *) arg0);
                if (D_00810CA4[2] != (unsigned char) (k + arg0)[0x50]) {
                    func_0020CD40();
                    *(unsigned char *)(arg0 + 5) = 4;
                    *(unsigned char *)(arg0 + 6) = 1;
                } else if (*(unsigned char *)(arg0 + 7) == 0) {
                    func_0020CD80();
                    *(unsigned char *)(arg0 + 7) = 0x10;
                }
            }
            func_0020B0D0((int) arg0, D_002664F0);
        }
        break;

    case 2:
        func_0020A7A0(0x20043885LL << 32 | 0x9D422130u);
        func_0020AE40(arg0, D_002664F0, 0x100);
        func_0020BF20(D_00266560, 1, 0);
        if (func_0020BC50(arg0, D_002665A0, 0x20042C05LL << 32 | 0xA1321F80u, 0x100) != 0) {
            *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) - 1;
        }
        func_0020B0D0((int) arg0, D_002664F0);
        break;

    case 3: {
        unsigned char cclr;
        int clamp;
        func_0020A7A0(0x20043885LL << 32 | 0x9D422130u);
        func_0020AE40(arg0, D_002664F0, 0x100);
        cclr = (unsigned char) arg0[func_0020BEF0((unsigned char *) arg0) + 0x50];
        switch (cclr) {
        case 1: clamp = 1; break;
        case 2: clamp = 2; break;
        case 3: clamp = 2; break;
        case 4: clamp = 3; break;
        default: clamp = 0; break;
        }
        func_0020BF20(D_00266560, 1, clamp);
        func_0020B210(arg0, D_002665A0, 0x20042C05LL << 32 | 0xA1321F80u, 0x500);
        func_0020B0D0((int) arg0, D_002664F0);
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
    }

    case 4: {
        unsigned char cclr;
        int clamp;
        func_0020A7A0(0x20043885LL << 32 | 0x9D422130u);
        func_0020AE40(arg0, D_002664F0, 0x100);
        cclr = (unsigned char) arg0[func_0020BEF0((unsigned char *) arg0) + 0x50];
        switch (cclr) {
        case 1: clamp = 1; break;
        case 2: clamp = 2; break;
        case 3: clamp = 2; break;
        case 4: clamp = 3; break;
        default: clamp = 0; break;
        }
        func_0020BF20(D_00266560, 1, clamp);
        func_0020B210(arg0, D_002665A0, 0x20042C05LL << 32 | 0xA1321F80u, 0x500);
        func_0020B0D0((int) arg0, D_002664F0);
        *(int *)0x002821B4 = 1;
        *(int *)0x00282240 = 5;
        *(int *)0x002821B8 = 4;
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
                {
                    unsigned char e = (func_0020BEF0((unsigned char *) arg0) + arg0)[0x50];
                    int c = e + *(short *)(arg0 + 0x1E);
                    D_00810CA4[2] = c;
                }
                *(unsigned char *)(arg0 + 0x12) = D_00810CA4[2];
                if (D_00810CA4[0] != 0xFF) {
                    if (D_00810CA4[0] == 2) {
                        D_00810CA4[3] = 7;
                        D_00810CA4[1] = 5;
                        D_00810CA4[0] = 0xFF;
                    }
                }
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
    }

    cnt = *(unsigned char *)(arg0 + 7);
    if (cnt != 0) {
        *(unsigned char *)(arg0 + 7) = cnt - 1;
    }
}
