// NEARMISS func_001AD740  (vram 0x001AD740, 0x7B8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.23% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 14 instructions in three groups. (1) 2 instrs - MULTI-TABLE dispatch reloc: this function has TWO jump tables (outer jtbl_0026DD10 at the state-0xA switch, inner jtbl_0026DCF0 at the state-3 sub-switch). mwcc emits each as its own anonymous local (@83/@84) in switch-encounter order, so the OUTER ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// SEMANTICS: Game-over / continue frame task (sibling of func_001AD4E0). The task
// slot pointer lives in scratchpad 0x70003B6C; the outer state byte is slot+0xA and
// the sub-state byte is slot+0xB. Outer dispatch is jtbl_0026DD10 (7 entries); the
// state-3 sub-dispatch is jtbl_0026DCF0 (7 entries).
//   0  bring-up: func_001AEDB0(0), func_001D2880, func_0022D360,
//      func_001AEBA0(0xFF), func_001D1EF0. If the scratchpad mode byte 0x70003B93
//      is 1, bump +0xA and arm the u16 timer +0x1A = 0x30; otherwise jump to
//      state 2.
//   1  three-way sub-state (compare chain, no table):
//        0  count the timer +0x1A down; at zero func_001AEE10(6,0) and bump +0xB;
//           then func_001D1EF0 every frame.
//        1  func_001D1C50 + func_001D2830(3,1); if func_0022D380() or pad bit
//           0x800 (D_00810E74) then func_001B0C00(4) and bump +0xB; func_001D1EA0(0).
//        2  same fade calls, then once D_0028A9A0 == 2 bump +0xA and clear +0xB.
//   2  continue accepted: func_001FABB0, bump +0xA, clear +0xB, func_001AEDB0(0).
//      Then rebuild the unlock state: flag = D_00810C90 != 0; count set entries in
//      the 0x2D..0x3B unlock table D_0081072D[0x564+k]; next = D_0081070A, bumped
//      by one unless it is already 0xFF; save/restore D_00810750 across
//      func_001AF2C0; set D_00810700 = 0xB, D_00810701 = 0, D_00810702 = 0 and
//      store D_00810730[D_00810700] = D_00810701; func_001C40B0(0x10, 8) and write
//      D_0081070A = next. If flag, unlock 0x1E/0x1F/0x20/0x21/0x22 via
//      func_001C40B0. Set the score/timer quartet D_00810CA8 = 0x32,
//      D_00810CAA = 0x1E, D_00810CAC = 0xA, D_00810CB0 = 0x19, and if all 0xF
//      entries were counted also func_001C40B0(3,1), D_00810CAA = 0x63 and
//      D_00810CA6 = 3.
//   3  GIF-packet sub-state machine on +0xB (jtbl_0026DCF0):
//        0  func_001D1EF0, gp-rel D_00275BD8 = 1, func_001FF080(0, 0x37), bump.
//        1  once D_00275BD8 clears: func_001AEE10(4,0), bump, timer +0x1A = 0x1C60,
//           func_001D2830(3,1).
//        2  push the 0x21322E00/0x21323040 packet; pad bit 0x800 -> func_001AEDE0(4,0)
//           and bump, else count +0x1A down and when it was already 0 run
//           func_001AEDE0(4,0) and set +0xB = 4.
//        3,4 same packet; once D_0028A9A0 == 2, if +0xB == 4 bump it, arm
//           +0x1A = 0x1AC and func_001AEE10(4,0); otherwise bump +0xA and clear +0xB.
//        5  push the 0x21322A00/0x21322C40 packet; same 0x800 / countdown logic but
//           the expiry path bumps +0xB instead of forcing 4.
//        6  same packet; once D_0028A9A0 == 2 bump +0xA and clear +0xB.
//   4  func_001D1EF0, func_00225A00, bump +0xA.
//   5  func_001D1C50 + func_001D2830(3,1); if func_00225AC0(1) bump +0xA;
//      func_001D1EA0(0).
//   6  teardown: slot+9 = 4, slot+0xA = 0, D_00810D38 = 0.
//
// NEARMISS 99.231 with mwcc 2.3.3 (mwcps2-2.3.3-000906) -O4,p -sdatathreshold 8.
// Small globals are over-declared as arrays (idiom #20) to force absolute
// addressing while D_00275BD8 stays gp-rel at threshold 8; D_00810700/701/702 are
// additionally `volatile` so the 0xB store keeps source order.
// RESIDUALS (14 instrs, all known walls):
//  * 2 instrs - MULTI-TABLE jump-table layout: mwcc emits each switch table as its
//    own anonymous local (@83/@84) and puts the OUTER table first, whereas the
//    original .rodata has the inner table (jtbl_0026DCF0) first, so objdiff cannot
//    pair the outer dispatch reloc. This is the documented multi-table ceiling
//    recorded in tools/match/gen_jtbl_rodata.py (func_001CFBE0 / func_0022B7A0).
//  * 6 instrs - idiom-13 delay-slot fill at three branches (the 0x70003B93 test in
//    state 0 and the two `D_00810E74 & 0x800` tests in sub-states 2 and 5): mwcc
//    speculates the successor's `lui at, 0x7000` into the delay slot where CW
//    leaves a nop. Arm inversion made it worse (96.2%); volatile on the slot
//    pointer had no effect.
//  * 5 instrs - v0/v1 colouring of the D_0081070A read-modify pair; unaffected by
//    declaration order, ternary vs if/else, unsigned char vs int, or double-read.
extern void func_001ABF90(long long, long long, long long, long long);
extern void func_001AEBA0(int);
extern void func_001AEDB0(int);
extern void func_001AEDE0(short, unsigned char);
extern void func_001AEE10(short, unsigned char);
extern void func_001AF2C0(void);
extern void func_001B0C00(int);
extern void func_001C40B0(int, int);
extern void func_001D1C50(void);
extern void func_001D1EA0(int);
extern void func_001D1EF0(void);
extern void func_001D2830(int, int);
extern void func_001D2880(void);
extern void func_001FABB0(void);
extern void func_001FF080(int, int);
extern void func_00225A00(void);
extern int func_00225AC0(int);
extern void func_0022D360(void);
extern int func_0022D380(void);

extern unsigned char D_00275BD8;
extern short D_0028A9A0[8];
extern volatile unsigned char D_00810700[16];
extern volatile unsigned char D_00810701[16];
extern volatile unsigned char D_00810702[16];
extern unsigned char D_0081070A[16];
extern unsigned char D_0081072D[16];
extern unsigned char D_00810730[16];
extern int D_00810750[4];
extern unsigned char D_00810C90[16];
extern unsigned char D_00810CA6[16];
extern short D_00810CA8[8];
extern short D_00810CAA[8];
extern short D_00810CAC[8];
extern short D_00810CB0[8];
extern int D_00810D38[4];
extern unsigned short D_00810E74[8];

void func_001AD740(void) {
    unsigned char *p;
    unsigned char *q;
    unsigned char *t;
    unsigned short *r;
    int i;
    unsigned char *s;
    int flag;
    int n;
    int saved;
    int raw;
    int next;
    int nxt;

    p = *(unsigned char **)0x70003B6C;
    switch (p[0xA]) {
    case 0:
        func_001AEDB0(0);
        func_001D2880();
        func_0022D360();
        func_001AEBA0(0xFF);
        func_001D1EF0();
        if (*(unsigned char *)0x70003B93 == 1) {
            q = *(unsigned char **)0x70003B6C;
            q[0xA] = q[0xA] + 1;
            *(unsigned short *)(*(unsigned char **)0x70003B6C + 0x1A) = 0x30;
        } else {
            (*(unsigned char **)0x70003B6C)[0xA] = 2;
        }
        break;
    case 1:
        switch (p[0xB]) {
        case 0:
            r = (unsigned short *)(p + 0x1A);
            if (--*r == 0) {
                func_001AEE10(6, 0);
                q = *(unsigned char **)0x70003B6C;
                q[0xB] = q[0xB] + 1;
            }
            func_001D1EF0();
            break;
        case 1:
            func_001D1C50();
            func_001D2830(3, 1);
            if (func_0022D380() != 0 || (D_00810E74[0] & 0x800)) {
                func_001B0C00(4);
                q = *(unsigned char **)0x70003B6C;
                q[0xB] = q[0xB] + 1;
            }
            func_001D1EA0(0);
            break;
        case 2:
            func_001D1C50();
            func_001D2830(3, 1);
            func_0022D380();
            func_001D1EA0(0);
            if (D_0028A9A0[0] == 2) {
                q = *(unsigned char **)0x70003B6C;
                q[0xA] = q[0xA] + 1;
                (*(unsigned char **)0x70003B6C)[0xB] = 0;
            }
            break;
        }
        break;
    case 2:
        func_001FABB0();
        q = *(unsigned char **)0x70003B6C;
        q[0xA] = q[0xA] + 1;
        (*(unsigned char **)0x70003B6C)[0xB] = 0;
        func_001AEDB0(0);
        n = 0;
        flag = 0;
        if (D_00810C90[0] != 0) {
            flag = 1;
        }
        for (i = 0x2D, s = D_0081072D; i < 0x3C; i++, s++) {
            if (s[0x564] != 0) {
                n = n + 1;
            }
        }
        raw = D_0081070A[0];
        next = 0xFF;
        if (raw != 0xFF) {
            next = raw + 1;
        }
        saved = D_00810750[0];
        nxt = next & 0xFF;
        func_001AF2C0();
        D_00810750[0] = saved;
        D_00810700[0] = 0xB;
        D_00810701[0] = 0;
        D_00810702[0] = 0;
        D_00810730[D_00810700[0]] = D_00810701[0];
        func_001C40B0(0x10, 8);
        D_0081070A[0] = nxt;
        if (flag != 0) {
            func_001C40B0(0x1E, 0x14);
            func_001C40B0(0x1F, 5);
            func_001C40B0(0x20, 5);
            func_001C40B0(0x21, 5);
            func_001C40B0(0x22, 3);
        }
        D_00810CA8[0] = 0x32;
        D_00810CAA[0] = 0x1E;
        D_00810CAC[0] = 0xA;
        D_00810CB0[0] = 0x19;
        if (n == 0xF) {
            func_001C40B0(3, 1);
            D_00810CAA[0] = 0x63;
            D_00810CA6[0] = 3;
        }
        break;
    case 3:
        switch (p[0xB]) {
        case 0:
            func_001D1EF0();
            D_00275BD8 = 1;
            func_001FF080(0, 0x37);
            q = *(unsigned char **)0x70003B6C;
            q[0xB] = q[0xB] + 1;
            break;
        case 1:
            if (D_00275BD8 == 0) {
                func_001AEE10(4, 0);
                q = *(unsigned char **)0x70003B6C;
                q[0xB] = q[0xB] + 1;
                *(unsigned short *)(*(unsigned char **)0x70003B6C + 0x1A) = 0x1C60;
                func_001D2830(3, 1);
            }
            break;
        case 2:
            func_001ABF90(0x21322E00LL | (0x20064406LL << 32),
                          0x21322E40LL | (0x20064486LL << 32),
                          0x21323000LL | (0x20064606LL << 32),
                          0x21323040LL | (0x20064686LL << 32));
            if (D_00810E74[0] & 0x800) {
                func_001AEDE0(4, 0);
                q = *(unsigned char **)0x70003B6C;
                q[0xB] = q[0xB] + 1;
            } else {
                q = *(unsigned char **)0x70003B6C;
                r = (unsigned short *)(q + 0x1A);
                if ((*r)-- == 0) {
                    func_001AEDE0(4, 0);
                    (*(unsigned char **)0x70003B6C)[0xB] = 4;
                }
            }
            break;
        case 3:
        case 4:
            func_001ABF90(0x21322E00LL | (0x20064406LL << 32),
                          0x21322E40LL | (0x20064486LL << 32),
                          0x21323000LL | (0x20064606LL << 32),
                          0x21323040LL | (0x20064686LL << 32));
            if (D_0028A9A0[0] == 2) {
                q = *(unsigned char **)0x70003B6C;
                t = q + 0xB;
                if (q[0xB] == 4) {
                    *t = *t + 1;
                    *(unsigned short *)(*(unsigned char **)0x70003B6C + 0x1A) = 0x1AC;
                    func_001AEE10(4, 0);
                } else {
                    q[0xA] = q[0xA] + 1;
                    (*(unsigned char **)0x70003B6C)[0xB] = 0;
                }
            }
            break;
        case 5:
            func_001ABF90(0x21322A00LL | (0x20064006LL << 32),
                          0x21322A40LL | (0x20064086LL << 32),
                          0x21322C00LL | (0x20064206LL << 32),
                          0x21322C40LL | (0x20064286LL << 32));
            if (D_00810E74[0] & 0x800) {
                func_001AEDE0(4, 0);
                q = *(unsigned char **)0x70003B6C;
                q[0xB] = q[0xB] + 1;
            } else {
                q = *(unsigned char **)0x70003B6C;
                r = (unsigned short *)(q + 0x1A);
                if ((*r)-- == 0) {
                    func_001AEDE0(4, 0);
                    q = *(unsigned char **)0x70003B6C;
                    q[0xB] = q[0xB] + 1;
                }
            }
            break;
        case 6:
            func_001ABF90(0x21322A00LL | (0x20064006LL << 32),
                          0x21322A40LL | (0x20064086LL << 32),
                          0x21322C00LL | (0x20064206LL << 32),
                          0x21322C40LL | (0x20064286LL << 32));
            if (D_0028A9A0[0] == 2) {
                q = *(unsigned char **)0x70003B6C;
                q[0xA] = q[0xA] + 1;
                (*(unsigned char **)0x70003B6C)[0xB] = 0;
            }
            break;
        }
        break;
    case 4:
        func_001D1EF0();
        func_00225A00();
        q = *(unsigned char **)0x70003B6C;
        q[0xA] = q[0xA] + 1;
        break;
    case 5:
        func_001D1C50();
        func_001D2830(3, 1);
        if (func_00225AC0(1) != 0) {
            q = *(unsigned char **)0x70003B6C;
            q[0xA] = q[0xA] + 1;
        }
        func_001D1EA0(0);
        break;
    case 6:
        p[9] = 4;
        (*(unsigned char **)0x70003B6C)[0xA] = 0;
        D_00810D38[0] = 0;
        break;
    }
}
