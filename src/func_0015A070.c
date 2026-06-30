// NEARMISS func_0015A070  (vram 0x0015A070, 0x190 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 88.90% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-coloring permutation on the outer switch dispatch: target keeps state byte live in $a1 and dispatches directly (literal 1 in $a2); mwcc copies it to $a0 and dispatches there (literal in $a1). The single coloring choice cascades a0/a1/a2/v1 across the whole function. Body, structure, s1-h...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS 88.9% (mwcc233). Body/structure fully recovered; sole residual is a
// register-coloring permutation on the outer switch dispatch: the target keeps
// the state byte (lbu a1,4(a0)) live in $a1 and dispatches on it directly with
// the literal 1 in $a2, while mwcc copies it to $a0 (paddub a0,a1,zero) and
// dispatches on $a0 with the literal in $a1. That single coloring choice
// cascades the a0/a1/a2/v1 assignments across the whole function; every
// control-flow edge, the s1=arg0+0x1F0 hoist, the func_00158590/func_00159FC0
// region and both epilogues otherwise match. Permuter territory (the idiom-19
// s1-hoist took it from 81% to 88.9%; the dispatch-reg cascade is all that's
// left).
//
// Actor state-machine step. case0: if func_001B0FD0(state,1)==0, run
// func_001C6380(actor); if the per-actor enable bit
// (D_00810841[D_00810700] >> actor.+0x2E) is set, sub-state(+5)=2 and return,
// else store &D_00275468 at +0x30 and set +0 = 1. case1: sub-switch on +5
// (0: func_00158590(1,1) + func_00159FC0(actor, actor+0x1F0), on success ++sub;
// 1: func_00158590(1,1) + func_001BA1F0, on success sub=2; 2: func_00158590(0,1)
// + set +0 = 2), then func_001B17A0(actor) and, if +3==6, call the vtable
// handler at +0x4C. case2/3: func_001AFC10(actor, state).
extern int func_001B0FD0(int, int);
extern void func_001C6380(char *);
extern void func_00158590(int, int);
extern int func_00159FC0(char *, char *);
extern int func_001BA1F0(char *);
extern void func_001B17A0(char *);
extern void func_001AFC10(char *, unsigned char);
extern int D_00275468;
extern unsigned char D_00810700;
extern unsigned char D_00810841;

void func_0015A070(char *arg0) {
    char *p1f0;
    unsigned char st;

    p1f0 = arg0 + 0x1F0;
    st = *(unsigned char *)(arg0 + 4);
    switch (st) {
    case 0:
        if (func_001B0FD0(st, 1) == 0) {
            func_001C6380(arg0);
            if ((*(&D_00810841 + D_00810700) & (1 << *(unsigned short *)(arg0 + 0x2E))) != 0) {
                *(unsigned char *)(arg0 + 5) = 2;
                return;
            }
            *(int *)(arg0 + 0x30) = (int)&D_00275468;
            *(char *)(arg0 + 0) = 1;
        }
        return;
    case 1:
        switch (*(unsigned char *)(arg0 + 5)) {
        case 0:
            func_00158590(1, 1);
            if (func_00159FC0(arg0, p1f0) != 0) {
                *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
            }
            break;
        case 1:
            func_00158590(1, 1);
            if (func_001BA1F0(arg0) != 0) {
                *(unsigned char *)(arg0 + 5) = 2;
            }
            break;
        case 2:
            func_00158590(0, 1);
            *(char *)(arg0 + 0) = 2;
            break;
        }
        func_001B17A0(arg0);
        if (*(unsigned char *)(arg0 + 3) == 6) {
            (*(void (**)(char *))(arg0 + 0x4C))(arg0);
        }
        return;
    case 2:
    case 3:
        func_001AFC10(arg0, st);
        return;
    }
}
