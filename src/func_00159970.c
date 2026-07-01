// NEARMISS func_00159970  (vram 0x00159970, 0x214 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 77.35% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Stacked artifacts (NOT clean-store nop): (1) argument hoisting -- target computes anim=arg0+0x1F0 and constants 2,3 once and schedules the arg moves into the top state-dispatch branch delay slots, reused across every state helper call; forcing explicit 4-arg calls makes mwcc emit a jump table and...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS (233: 77.4%, 991202: 76.2%). Per-entity state machine dispatched on
// the state byte at arg0+4 (values 0/1/2/3); anim = arg0+0x1F0 is the animation
// sub-object passed to the clip helpers (real EE code hoists anim + the constants
// 2,3 once and reuses them across every state call). State 0: if the anim clip
// func_001B0FD0(anim,2,3) reports done (==0), fires the spawn path -- func_001C6380
// (self), clears arg0+0xA, installs handler &D_00275458 at arg0+0x30, sets state
// byte arg0+0 = 1, seeds four floats at 0x700038A0.. (0, 0.3f, 0, 0.2f), and
// kicks func_001C5570(self,&D_700038A0,a2,1) where a2 = 0x71 when (arg0+2 & 0x1F)
// == 6 else 0x7B. State 1: sub-state machine on arg0+5 (0..3) driving
// func_00157CE0 / func_001BA1A0 / func_001BA1F0 clip steps and advancing arg0+5;
// then always runs func_001B17A0(self) and calls the vtable slot at arg0+0x4C.
// State 2: bumps the state byte. State 3: func_001AFC10(anim,3).
//
// WALL: body/logic fully recovered. Residuals are stacked compiler artifacts the
// readable C cannot force: (1) argument hoisting -- the target computes anim,
// 2, 3 once and schedules the arg moves into the top dispatch branch delay slots,
// shared by all state calls (writing explicit 4-arg calls makes mwcc emit a jump
// table and collapses the score to ~12%); (2) branch-likely (beql/bnel) in the
// inner arg0+5 sub-state machine. Neither is the clean-store nop; 233 does not
// close them. Parked as readable near-miss.
extern int func_00157CE0(char *, int, int);
extern void func_001AFC10(char *, int);
extern int func_001B0FD0(char *, int, int);
extern void func_001B17A0(char *);
extern void func_001BA1A0(char *, int *, int, int);
extern int func_001BA1F0(char *, int, int);
extern void func_001C5570(char *, int *, int, int);
extern void func_001C6380(char *);
extern void func_001FB9F0(int, int, int, int);
extern int D_00247020;
extern int D_00275458;
extern int D_700038A0;

void func_00159970(char *arg0) {
    unsigned char state;
    unsigned char sub;
    char *anim;
    int a2;

    state = *(unsigned char *)(arg0 + 4);
    anim = arg0 + 0x1F0;
    switch (state) {
    case 0:
        if (func_001B0FD0(anim, 2, 3) == 0) {
            func_001C6380(arg0);
            *(char *)(arg0 + 0xA) = 0;
            *(int *)(arg0 + 0x30) = (int)&D_00275458;
            *(char *)(arg0 + 0) = 1;
            *(int *)0x700038A0 = 0;
            *(int *)0x700038A4 = 0x3E99999A;
            *(int *)0x700038A8 = 0;
            *(int *)0x700038AC = 0x3E4CCCCD;
            a2 = 0x7B;
            if ((*(unsigned char *)(arg0 + 2) & ~0xE0) == 6) {
                a2 = 0x71;
            }
            func_001C5570(arg0, &D_700038A0, a2, 1);
        }
        return;
    case 1:
        sub = *(unsigned char *)(arg0 + 5);
        switch (sub) {
        case 0:
            switch (func_00157CE0(anim, 2, 3)) {
            case 0:
                break;
            case 2:
                *(unsigned char *)(arg0 + 5) = 2;
                break;
            case 3:
                *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
                break;
            default:
                *(unsigned char *)(arg0 + 5) = 3;
                func_001FB9F0(0x3E9, 0x1000, 0x1000, 0x1000);
                break;
            }
            break;
        case 1:
            *(unsigned char *)(arg0 + 5) = sub + 1;
            func_001BA1A0(anim, &D_00247020, 2, 3);
            break;
        case 2:
            if (func_001BA1F0(anim, 2, 3) != 0) {
                *(char *)(arg0 + 0xB) = 0;
                *(char *)(arg0 + 0) = 1;
                *(unsigned char *)(arg0 + 5) = 0;
            }
            break;
        case 3:
            if (func_001BA1F0(anim, 2, 3) != 0) {
                *(unsigned char *)(arg0 + 5) = 1;
            }
            break;
        }
        func_001B17A0(arg0);
        (*(void (**)(char *))(arg0 + 0x4C))(arg0);
        return;
    case 2:
        *(unsigned char *)(arg0 + 4) = state + 1;
        return;
    case 3:
        func_001AFC10(anim, 3);
        return;
    }
}
