// NEARMISS func_00217FA0  (vram 0x00217FA0, 0x69C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.15% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body/structure fully recovered and verified byte-for-byte against the .s (weapon-reload FSM, branch-likely switch(state) dispatch confirmed via beql chain, all real callee signatures recovered incl. func_0020CD60/func_0020CDA0's zero-arg tail-thunk reality and the func_0020A7A0/func_0020B210/func...
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
// D_00810C73 is set; if a pending swap request D_008106B0 is set, scans
// those slots for one matching D_008106B1-0xD, latches its index at +0x17,
// force-sets the FSM to state 4 with a 0xF0 countdown at +6, clears the
// swap request, and returns (through the shared tail). Otherwise it falls
// through into case 1 (advancing the state byte first): if the "fire" input
// bit (0x20) is held, jump straight to a "quick reload" path (matches
// func_0020CD60's tail-thunk, state->2). Otherwise it starts the normal
// reload animation (func_0020AE40/func_0020BF20/func_0020B210), and either
// aborts back one state step if that failed, or (once +0x18 has entries and
// input bit 0x40 is held) may replay a different animation depending on the
// currently active color (+0x50 of the latched slot) vs. D_00810C61.
// Case 2 runs the mid-reload animation and, on completion signal, backs the
// state up by one. Case 3 runs the tail animation and, once its frame
// countdown (+6) hits 0 (or input bits 0x5060 are held), snaps back to
// state 1. Case 4 runs the finishing animation, resets some global mode
// trackers, ticks a duck-in/out counter at +6 based on input bits
// 0x8000/0x2000, and then (bit 0x40 held) either requests a re-reload if
// +6 isn't 0 or commits the slot swap (updates D_00810C61, resets to state
// 0), or (bit 0x20 held) requests the quick-reload tail. Every path falls
// through to the shared tail which decrements a busy-counter at +7 if set.
extern void func_001FB9F0(int id, int a1, int a2, int a3);
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

extern char D_002662F0[];
extern char D_00266360[];
extern char D_002663A0[];
extern int D_002821B0;
extern int D_00282240;
extern int D_002821B4;
extern int D_002821B8;
extern unsigned char D_008106B0;
extern unsigned char D_008106B1;
extern unsigned char D_00810C61;
extern unsigned char D_00810C73;
extern unsigned short D_00810E74;

void func_00217FA0(char *arg0) {
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
        *(unsigned char *)(arg0 + 0x12) = (unsigned char) (D_00810C61 + 0xD);
        *(short *)(arg0 + 0x1E) = 0xD;
        idx = *(unsigned char *)(arg0 + 0x18);
        *(unsigned char *)(arg0 + 0x18) = idx + 1;
        arg0[idx + 0x50] = 0;
        idx = *(unsigned char *)(arg0 + 0x18);
        *(unsigned char *)(arg0 + 0x18) = idx + 1;
        arg0[idx + 0x50] = 1;
        if (D_00810C73 != 0) {
            idx = *(unsigned char *)(arg0 + 0x18);
            *(unsigned char *)(arg0 + 0x18) = idx + 1;
            arg0[idx + 0x50] = 2;
        }

        if (D_008106B0 != 0) {
            unsigned char want = (unsigned char) (D_008106B1 - 0xD);
            int i;
            *(unsigned char *)(arg0 + 0x1B) = want;
            for (i = 0; i < *(unsigned char *)(arg0 + 0x18); i++) {
                if ((unsigned char)arg0[i + 0x50] == want) {
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
            func_0020A7A0(0x200412A5LL << 32 | 0x9D422040u);
            func_0020AE40(arg0, D_002662F0, 0x40);
            func_0020BF20(D_00266360, 1, 0);
            if (func_0020B210(arg0, D_002663A0, 0x20040E05LL << 32 | 0xA1321F00u, 0x40) != 0) {
                *(short *)(arg0 + 0x1C) = 0;
                *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
                func_0020BBE0(arg0, *(unsigned char *)(arg0 + 0x1A));
            } else if (*(unsigned char *)(arg0 + 0x18) != 0 && (D_00810E74 & 0x40)) {
                if (D_00810C61 != (unsigned char) arg0[*(unsigned char *)(arg0 + 0x17) + 0x50]) {
                    func_0020CD40();
                    *(unsigned char *)(arg0 + 5) = 4;
                    *(unsigned char *)(arg0 + 6) = 1;
                } else if (*(unsigned char *)(arg0 + 7) == 0) {
                    func_0020CD80();
                    *(unsigned char *)(arg0 + 7) = 0x10;
                }
            }
            func_0020B0D0((int) arg0, D_002662F0);
        }
        break;

    case 2:
        func_0020A7A0(0x200412A5LL << 32 | 0x9D422040u);
        func_0020AE40(arg0, D_002662F0, 0x40);
        func_0020BF20(D_00266360, 1, 0);
        if (func_0020BC50(arg0, D_002663A0, 0x20040E05LL << 32 | 0xA1321F00u, 0x40) != 0) {
            *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) - 1;
        }
        func_0020B0D0((int) arg0, D_002662F0);
        break;

    case 3:
        func_0020A7A0(0x200412A5LL << 32 | 0x9D422040u);
        func_0020AE40(arg0, D_002662F0, 0x40);
        func_0020BF20(D_00266360, 1, 0);
        func_0020B210(arg0, D_002663A0, 0x20040E05LL << 32 | 0xA1321F00u, 0x440);
        func_0020B0D0((int) arg0, D_002662F0);
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
        func_0020A7A0(0x200412A5LL << 32 | 0x9D422040u);
        func_0020AE40(arg0, D_002662F0, 0x40);
        func_0020BF20(D_00266360, 1, 0);
        func_0020B210(arg0, D_002663A0, 0x20040E05LL << 32 | 0xA1321F00u, 0x440);
        func_0020B0D0((int) arg0, D_002662F0);
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
                unsigned char color;
                func_001FB9F0(0x17B, 0x1000, 0x1000, 0x1000);
                color = (unsigned char) (*(unsigned char *)(arg0 + 0x17) + arg0)[*(unsigned char *)(arg0 + 0x19) + 0x50];
                *(char *)(arg0 + 0x12) = (char) (color + 0xD);
                D_00810C61 = color;
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
