// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SEMANTICS: Sound-test / audio-menu screen task. jr-table dispatch (jtbl_00273750,
// 6 entries) on the state byte at p[5]; out-of-range returns immediately.
//   0  build the track list: clear cursor state (p[0x17] col, p[0x19] row base,
//      p[0x18] count, p[0x1A]), set D_002821B0=4 / D_002821B4=0 / D_00282240=3 and
//      the u16 at p[0x1E] to 0x23. Append id 1 if D_00810C88, else id 0 if
//      D_00810C87; then scan the 0x25..0x3B unlock-flag table (D_00810725[0x564+k])
//      appending (i - 0x23) for each set flag into p[0x50 + count++]. If a resume
//      request is pending (D_008106B0) set p[0x1B] = D_008106B1 - 0x23, find that id
//      in the list and set row/col from its index (p[0x19] = (i>>2)*4,
//      p[0x17] = i%4, D_00282240=4), clear the request and jump to state 3 with
//      p[6] = 0xF0; otherwise p[5]++ and FALL THROUGH into state 1.
//   1  idle/list state: cancel (pad 0x20) -> func_0020CD60, D_002821B4=2, reset
//      p[1..5]. Otherwise draw (func_0020A7A0 GS packet, func_0020AE40/func_0020B0D0
//      list open/close). If func_0020B210 (cursor move) fired, reset p[0x1C] and go
//      to state 2 with func_0020BBE0; else on confirm (pad 0x40) with a non-empty
//      list, resolve the selected track (func_0020BEF0 index -> p[0x50+i]+0x23 ->
//      func_00185420) into p[0x30]: on success and func_00182B30 idle go to state 4
//      (p[6]=1), otherwise func_0020CD80 and state 5 (p[6]=0xF0).
//   2  same draw with func_0020BC50; when it fires, step back to state 1.
//   5  error/no-track banner (D_002821B8=0x19): draw, then on pad 0x60 or the p[6]
//      countdown expiring return to state 1.
//   3  same as 5 but flags 0x5060 and countdown first (D_00282240=3).
//   4  playing state (D_002821B8=0x18): draw, func_001FCF10, func_00207D00(1,3),
//      volume up/down via pad 0x8000/0x2000 (func_0020CDA0), func_0020CCB0 refresh;
//      pad 0x40 either stops (p[6]!=0 -> back to state 1) or commits the track
//      (func_0020CD40, stream cmd 5 at p[0x30]+0xB, scratchpad 0x70003B8D = 3,
//      D_008106C5 = 0xFF); pad 0x20 returns to state 1.
// Matched 100.0 with mwcc 2.3.3 (mwcps2-2.3.3-000906) -O4,p -sdatathreshold 0.
// D_002821B0/D_002821B4 must be `volatile` so the pair keeps source store order
// (mwcc otherwise hoists the zero-store past the materialised-constant store).
extern int func_00182B30(void *);
extern void *func_00185420(int);
extern void func_001FCF10(void);
extern void func_00207D00(int, int);
extern void func_0020A7A0(long long);
extern void func_0020AE40(unsigned char *, void *, int);
extern void func_0020B0D0(unsigned char *, void *);
extern int func_0020B210(unsigned char *, void *, long long, int);
extern void func_0020BBE0(unsigned char *, int);
extern int func_0020BC50(unsigned char *, void *, long long, int);
extern int func_0020BEF0(unsigned char *);
extern void func_0020CCB0(unsigned char *);
extern void func_0020CD40(void);
extern void func_0020CD60(void);
extern void func_0020CD80(void);
extern void func_0020CDA0(void);

extern unsigned char D_00265D20[];
extern unsigned char D_00265D90[];
extern volatile int D_002821B0;
extern volatile int D_002821B4;
extern int D_002821B8;
extern int D_00282240;
extern unsigned char D_008102B0[];
extern unsigned char D_008106B0;
extern unsigned char D_008106B1;
extern unsigned char D_008106C5;
extern unsigned char D_00810725[];
extern unsigned char D_00810C87;
extern unsigned char D_00810C88;
extern unsigned short D_00810E74;

void func_00215870(unsigned char *p) {
    int i;
    unsigned char *q;
    unsigned char *e;
    int n;

    switch (p[5]) {
    case 0:
        p[0x17] = 0;
        p[0x19] = 0;
        p[0x18] = 0;
        p[0x1A] = 0;
        D_002821B0 = 4;
        D_002821B4 = 0;
        D_00282240 = 3;
        *(short *)(p + 0x1E) = 0x23;
        if (D_00810C88 != 0) {
            p[0x50 + p[0x18]++] = 1;
        } else if (D_00810C87 != 0) {
            p[0x50 + p[0x18]++] = 0;
        }
        for (i = 0x25, q = D_00810725; i < 0x3C; i++, q++) {
            if (q[0x564] != 0) {
                p[0x50 + p[0x18]++] = i - 0x23;
            }
        }
        if (D_008106B0 != 0) {
            p[0x1B] = D_008106B1 - 0x23;
            for (i = 0; i < p[0x18]; i++) {
                e = p + i;
                if (e[0x50] == p[0x1B]) {
                    p[0x19] = (i >> 2) * 4;
                    p[0x17] = i % 4;
                    D_00282240 = 4;
                    break;
                }
            }
            D_008106B0 = 0;
            p[5] = 3;
            p[6] = 0xF0;
            return;
        }
        p[5] = p[5] + 1;
        /* fallthrough */
    case 1:
        if (D_00810E74 & 0x20) {
            func_0020CD60();
            D_002821B4 = 2;
            p[1] = 3;
            p[2] = 0;
            p[3] = 0;
            p[4] = 0;
            p[5] = 0;
            return;
        }
        func_0020A7A0(0x9D422250LL | (0x200453A5LL << 32));
        func_0020AE40(p, D_00265D20, 4);
        if (func_0020B210(p, D_00265D90, 0xA1321F80LL | (0x20044D05LL << 32), 4) != 0) {
            *(short *)(p + 0x1C) = 0;
            p[5] = p[5] + 1;
            func_0020BBE0(p, p[0x1A]);
        } else if (p[0x18] != 0 && (D_00810E74 & 0x40)) {
            *(void **)(p + 0x30) = func_00185420(p[0x50 + func_0020BEF0(p)] + 0x23);
            if (*(void **)(p + 0x30) != 0 && func_00182B30(D_008102B0) == 0) {
                func_0020CD40();
                p[5] = 4;
                p[6] = 1;
            } else {
                func_0020CD80();
                D_002821B4 = 0;
                p[5] = 5;
                p[6] = 0xF0;
            }
        }
        func_0020B0D0(p, D_00265D20);
        return;
    case 2:
        func_0020A7A0(0x9D422250LL | (0x200453A5LL << 32));
        func_0020AE40(p, D_00265D20, 4);
        if (func_0020BC50(p, D_00265D90, 0xA1321F80LL | (0x20044D05LL << 32), 4) != 0) {
            p[5] = p[5] - 1;
        }
        func_0020B0D0(p, D_00265D20);
        break;
    case 5:
        D_002821B4 = 1;
        D_00282240 = 5;
        D_002821B8 = 0x19;
        func_0020A7A0(0x9D422250LL | (0x200453A5LL << 32));
        func_0020AE40(p, D_00265D20, 4);
        func_0020B210(p, D_00265D90, 0xA1321F80LL | (0x20044D05LL << 32), 0x604);
        func_0020B0D0(p, D_00265D20);
        if ((D_00810E74 & 0x60) || --p[6] == 0) {
            if (D_00810E74 & 0x60) {
                func_0020CD60();
            }
            D_002821B4 = 0;
            D_008106B0 = 0;
            D_00282240 = 3;
            p[5] = 1;
        }
        break;
    case 3:
        func_0020A7A0(0x9D422250LL | (0x200453A5LL << 32));
        func_0020AE40(p, D_00265D20, 4);
        func_0020B210(p, D_00265D90, 0xA1321F80LL | (0x20044D05LL << 32), 0x404);
        func_0020B0D0(p, D_00265D20);
        if (--p[6] == 0 || (D_00810E74 & 0x5060)) {
            if (D_00810E74 & 0x5060) {
                func_0020CD60();
            }
            D_008106B0 = 0;
            p[5] = 1;
            D_00282240 = 3;
        }
        break;
    case 4:
        func_0020A7A0(0x9D422250LL | (0x200453A5LL << 32));
        func_0020AE40(p, D_00265D20, 4);
        func_0020B210(p, D_00265D90, 0xA1321F80LL | (0x20044D05LL << 32), 0x404);
        func_0020B0D0(p, D_00265D20);
        D_002821B4 = 1;
        D_00282240 = 5;
        D_002821B8 = 0x18;
        func_001FCF10();
        func_00207D00(1, 3);
        if (D_00810E74 & 0x8000) {
            if (p[6] != 0) {
                p[6] = p[6] - 1;
                func_0020CDA0();
            }
        } else if (D_00810E74 & 0x2000) {
            if (p[6] == 0) {
                p[6] = p[6] + 1;
                func_0020CDA0();
            }
        }
        func_0020CCB0(p);
        if (D_00810E74 & 0x40) {
            if (p[6] != 0) {
                D_002821B4 = 0;
                D_00282240 = 3;
                p[5] = 1;
                func_0020CD60();
                return;
            }
            func_0020CD40();
            *(char *)(*(int *)(p + 0x30) + 0xB) = 5;
            *(char *)0x70003B8D = 3;
            D_008106C5 = 0xFF;
            return;
        }
        if (D_00810E74 & 0x20) {
            D_002821B4 = 0;
            D_00282240 = 3;
            p[5] = 1;
            func_0020CD60();
        }
        break;
    }
}
