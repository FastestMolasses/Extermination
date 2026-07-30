// NEARMISS func_001AC070  (vram 0x001AC070, 0x334 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.95% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 97.95 with mwcc233 -O4,p -sdatathreshold 4 (o991: 93.1). Dispatch, all 7 case bodies, the bnezl/andi branch-likely and the whole tail are byte-identical. Two residual clusters, 216 expected instrs: (1) state 0, ONE extra instruction -- expected `bnez v0,else; nop` but mwcc speculates the else blo...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// SEMANTICS: one tick of the game-mode / screen-flow state machine.  The state
// block lives behind the scratchpad pointer at 0x70003B6C (`GS` below); byte 8 is
// the current state, byte 9 and byte 0xA are per-tick "edge" flags cleared on every
// transition, byte 0xC an aux flag, byte 0xE the requested sub-mode and byte 0xF a
// selector latched by the caller.  Byte 0x70003B90 is cleared unconditionally at
// entry (a per-frame request flag).  jtbl_0026DC90 dispatches states 0..6; any
// other state falls straight through to the tail.
//   0 boot        : reset the retry counter D_00275BD4, func_001AEDB0(0),
//                   func_001D1EF0(); then D_00275BDC picks the next state --
//                   0 -> state 1 / submode 1, otherwise state 2 / submode 3.
//   1 wait-load   : when func_001AC3B0() reports ready, go to state 2.
//   2 menu        : func_001AC480() returns the menu verdict.
//                     3 -> toggle between submode 3 (state 3) and submode 1
//                          (state 1) depending on the current submode.
//                     1 -> branch on the latched selector GS[0xF]:
//                            0 -> state 4, clear D_00275BE0
//                            1 -> func_00225A00(), state 5, D_00275BE0 = 1
//                          other -> state 6, clear GS[0xC]
//                   both verdicts clear the edge flags.
//   3 in-game     : anim_frame_top_a() returning 2 or 3 means "leave"; bump the
//                   retry counter (wrapping at 3), tear down with func_001FBC50 /
//                   func_001D2880 / func_001AEDB0(0) and return to state 2.
//   4 handoff     : install func_001ACEC0 as the new top-level handler via
//                   func_001AB790 and return WITHOUT running the common tail.
//   5 save/load   : func_00225AC0(0) -> 1 back to state 2, 2 means func_001AF150()
//                   then D_00275BE0 = 1 and state 4.
//   6 fade        : when func_00200A40() completes, back to state 2.
// Every path except state 4 ends with func_001D2830(3, 1).

extern void func_001AEDB0(int);
extern void func_001D1EF0(void);
extern int  func_001AC3B0(void);
extern int  func_001AC480(void);
extern void func_00225A00(void);
extern int  anim_frame_top_a(void);
extern void func_001FBC50(void);
extern void func_001D2880(void);
extern void func_001AB790(void *fn);
extern void func_001ACEC0(void);
extern int  func_00225AC0(int);
extern void func_001AF150(void);
extern int  func_00200A40(void);
extern void func_001D2830(int, int);

extern int           D_00275BD4;
extern unsigned char D_00275BDC;
extern unsigned char D_00275BE0;

#define GS (*(unsigned char *volatile *)0x70003B6C)

void func_001AC070(void)
{
    unsigned char *q;
    int r;
    int f;

    *(volatile unsigned char *)0x70003B90 = 0;

    switch (GS[8]) {
    case 0:
        D_00275BD4 = 0;
        func_001AEDB0(0);
        func_001D1EF0();
        if (D_00275BDC == 0) {
            GS[8] = 1;
            GS[0xE] = 1;
        } else {
            GS[8] = 2;
            GS[0xE] = 3;
        }
        break;
    case 1:
        if (func_001AC3B0() != 0) {
            GS[8] = 2;
            GS[9] = 0;
            GS[0xA] = 0;
        }
        break;
    case 2:
        r = func_001AC480();
        if (r == 3) {
            q = GS;
            if (q[0xE] == 1) {
                q[8] = 3;
                GS[0xE] = 3;
            } else {
                q[8] = 1;
                GS[0xE] = 1;
            }
            GS[9] = 0;
            GS[0xA] = 0;
        } else if (r == 1) {
            q = GS;
            f = q[0xF];
            if (f == 0) {
                q[8] = 4;
                D_00275BE0 = 0;
            } else if ((unsigned char)f == 1) {
                func_00225A00();
                GS[8] = 5;
                D_00275BE0 = 1;
            } else {
                q[8] = 6;
                GS[0xC] = 0;
            }
            GS[9] = 0;
            GS[0xA] = 0;
        }
        break;
    case 3:
        r = anim_frame_top_a();
        if ((unsigned int)(r - 2) < 2) {
            D_00275BD4 = D_00275BD4 + 1;
            if (D_00275BD4 > 2) {
                D_00275BD4 = 0;
            }
            func_001FBC50();
            func_001D2880();
            func_001AEDB0(0);
            GS[8] = 2;
            GS[9] = 0;
            GS[0xA] = 0;
            D_00275BE0 = 0;
        }
        break;
    case 4:
        func_001AB790(func_001ACEC0);
        return;
    case 5:
        r = func_00225AC0(0);
        if (r == 1) {
            GS[8] = 2;
            GS[9] = 0;
            GS[0xA] = 0;
        } else if (r == 2) {
            func_001AF150();
            q = GS;
            D_00275BE0 = 1;
            q[8] = 4;
            GS[9] = 0;
            GS[0xA] = 0;
        }
        break;
    case 6:
        if (func_00200A40() != 0) {
            GS[8] = 2;
        }
        break;
    }

    func_001D2830(3, 1);
}
