// NEARMISS func_0015A070  (vram 0x0015A070, 0x190 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.35% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful.
// Remaining differences: one register-colouring permutation in the case-0 enable-bit
// test. The original keeps the table index in $a2, the table address in $a1, the
// shift count in $v1 and the literal 1 in $a0 (that 1 is reused for the +0 store);
// mwcc picks a0/v1/a1/a2 for the same values. Every instruction, branch and delay
// slot otherwise matches. Only a no-op comma spelling found by the permuter closes
// it; that spelling was rejected as unreadable.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// Fixes over the earlier 88.90% version (matching wave 2026-10-27), read from the
// original instructions:
// - func_001B0FD0 and func_001AFC10 take the actor only (one parameter, per their
//   definitions in src/); func_00158590 takes (actor, a, mode), so the calls are
//   (actor,1,1), (actor,1,1) and (actor,0,1).
// - D_00275468 is $gp-relative: -sdatathreshold 4, with D_00810700/D_00810841 as
//   arrays so they stay absolute.
// - Every outer case ends in `break`; case 0 is an early return plus an if/else.
//
// Actor state-machine step. case0: unless func_001B0FD0(actor) reports busy,
// run func_001C6380(actor); if the per-actor enable bit
// (D_00810841[D_00810700] bit actor.+0x2E) is set, sub-state(+5)=2, else store
// &D_00275468 at +0x30 and set +0 = 1. case1: sub-switch on +5
// (0: func_00158590(actor,1,1) + func_00159FC0(actor, actor+0x1F0), on success
// ++sub; 1: func_00158590(actor,1,1) + func_001BA1F0, on success sub=2;
// 2: func_00158590(actor,0,1) + set +0 = 2), then func_001B17A0(actor) and, if
// +3==6, call the vtable handler at +0x4C. case2/3: func_001AFC10(actor).
extern int func_001B0FD0(char *);
extern void func_001C6380(char *);
extern void func_00158590(char *, int, int);
extern int func_00159FC0(char *, char *);
extern int func_001BA1F0(char *);
extern void func_001B17A0(char *);
extern void func_001AFC10(char *);
extern int D_00275468;
extern unsigned char D_00810700[];
extern unsigned char D_00810841[];

void func_0015A070(char *arg0) {
    char *p1f0;
    unsigned char st;

    p1f0 = arg0 + 0x1F0;
    st = *(unsigned char *)(arg0 + 4);
    switch (st) {
    case 0:
        if (func_001B0FD0(arg0) != 0) {
            return;
        }
        func_001C6380(arg0);
        if (((1 << *(unsigned short *)(arg0 + 0x2E)) & D_00810841[D_00810700[0]]) != 0) {
            *(unsigned char *)(arg0 + 5) = 2;
        } else {
            *(int *)(arg0 + 0x30) = (int)&D_00275468;
            *(char *)(arg0 + 0) = 1;
        }
        break;
    case 1:
        switch (*(unsigned char *)(arg0 + 5)) {
        case 0:
            func_00158590(arg0, 1, 1);
            if (func_00159FC0(arg0, p1f0) != 0) {
                *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
            }
            break;
        case 1:
            func_00158590(arg0, 1, 1);
            if (func_001BA1F0(arg0) != 0) {
                *(unsigned char *)(arg0 + 5) = 2;
            }
            break;
        case 2:
            func_00158590(arg0, 0, 1);
            *(char *)(arg0 + 0) = 2;
            break;
        }
        func_001B17A0(arg0);
        if (*(unsigned char *)(arg0 + 3) == 6) {
            (*(void (**)(char *))(arg0 + 0x4C))(arg0);
        }
        break;
    case 2:
    case 3:
        func_001AFC10(arg0);
        break;
    }
}
