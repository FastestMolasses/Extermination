// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// SEMANTICS: per-frame update dispatcher for one entity/actor.
// It first runs the common pre-update func_0021C440(e), then dispatches on the
// unsigned byte state id at e[5] through jtbl_0026D450 (26 entries, ids 0..25;
// any other value falls straight through to the epilogue).  Every handler takes
// the entity pointer as its only argument.
//   0, 23 -> func_0021D800     1 -> func_0021E240     2, 24 -> func_0021E490
//   3 -> func_0021E830         4 -> func_00221FC0     5 -> func_00222580
//   6 -> func_00222AD0         7 -> func_002230A0     8 -> nothing
//   9 -> func_002236F0        10 -> func_00223C70    11 -> func_0021F330
//  12 -> func_0021F850        15 -> func_002202C0    16 -> func_0021DBB0
//  17 -> func_0021E9C0        18 -> func_0021EAD0    19 -> func_0021EAD0
//  20 -> func_0021EF30        21 -> func_00224FE0    22 -> func_00225570
// States 13 and 14 are two-level: they sub-dispatch on the unsigned byte at
// e[0xD] (a phase counter, 0..4) via a compare chain.  State 13 runs one of
// func_0021FB40 / func_002208C0 / func_00220D30 / func_00221630 / func_00221C70
// for phases 0..4; state 14 runs func_0021FED0 / func_00220B50 / func_00221060 /
// func_002217C0 for phases 0..3 and does nothing for phase 4.
// State 25 clears the byte at e[1] and calls func_002255C0(e).
// Note ids 18 and 19 call the same handler from two distinct code blocks, and
// ids 23/24 are aliases of ids 0/2 -- that is how the original table is built.
//

extern void func_0021C440();
extern void func_0021D800();
extern void func_0021DBB0();
extern void func_0021E240();
extern void func_0021E490();
extern void func_0021E830();
extern void func_0021E9C0();
extern void func_0021EAD0();
extern void func_0021EF30();
extern void func_0021F330();
extern void func_0021F850();
extern void func_0021FB40();
extern void func_0021FED0();
extern void func_002202C0();
extern void func_002208C0();
extern void func_00220B50();
extern void func_00220D30();
extern void func_00221060();
extern void func_00221630();
extern void func_002217C0();
extern void func_00221C70();
extern void func_00221FC0();
extern void func_00222580();
extern void func_00222AD0();
extern void func_002230A0();
extern void func_002236F0();
extern void func_00223C70();
extern void func_00224FE0();
extern void func_00225570();
extern void func_002255C0();

void func_0015B770(unsigned char *p)
{
    func_0021C440(p);

    switch (p[5]) {
    case 0:
    case 23:
        func_0021D800(p);
        break;
    case 1:
        func_0021E240(p);
        break;
    case 2:
    case 24:
        func_0021E490(p);
        break;
    case 3:
        func_0021E830(p);
        break;
    case 4:
        func_00221FC0(p);
        break;
    case 5:
        func_00222580(p);
        break;
    case 6:
        func_00222AD0(p);
        break;
    case 7:
        func_002230A0(p);
        break;
    case 8:
        break;
    case 9:
        func_002236F0(p);
        break;
    case 10:
        func_00223C70(p);
        break;
    case 11:
        func_0021F330(p);
        break;
    case 12:
        func_0021F850(p);
        break;
    case 13:
        switch (p[13]) {
        case 0:
            func_0021FB40(p);
            break;
        case 1:
            func_002208C0(p);
            break;
        case 2:
            func_00220D30(p);
            break;
        case 3:
            func_00221630(p);
            break;
        case 4:
            func_00221C70(p);
            break;
        }
        break;
    case 14:
        switch (p[13]) {
        case 0:
            func_0021FED0(p);
            break;
        case 1:
            func_00220B50(p);
            break;
        case 2:
            func_00221060(p);
            break;
        case 3:
            func_002217C0(p);
            break;
        case 4:
            break;
        }
        break;
    case 15:
        func_002202C0(p);
        break;
    case 16:
        func_0021DBB0(p);
        break;
    case 17:
        func_0021E9C0(p);
        break;
    case 18:
        func_0021EAD0(p);
        break;
    case 19:
        func_0021EAD0(p);
        break;
    case 20:
        func_0021EF30(p);
        break;
    case 21:
        func_00224FE0(p);
        break;
    case 22:
        func_00225570(p);
        break;
    case 25:
        p[1] = 0;
        func_002255C0(p);
        break;
    }
}
