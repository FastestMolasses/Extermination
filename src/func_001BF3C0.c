// NEARMISS func_001BF3C0  (vram 0x001BF3C0, 0x1F0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 87.20% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation / constant-CSE + delay-slot-fill scheduling permutation. Body and control flow are fully recovered (89.22% with mwcc 991202). Residuals: (1) target holds the constant 3 in a single register ($a1) alive across the whole function and reuses it for the st==3 compare, the *(s0+4)=...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// State-machine update for an entity (arg0). Dispatches on the state byte
// *(u8*)(arg0+4): states 2 and 3 hand off to func_001AFC10(arg0,3,state);
// state 1 jumps straight to the clamp section; state 0 runs the enter-body
// (which checks the linked entity *(arg0+0x20)->state, plays a sound via
// func_001B1020 indexed by D_00275630[arg0[0xD]], and decrements the +0xA4
// timer by 3.0/6.0 depending on arg0[0xD]) before falling into the clamp.
// The clamp section bails if the linked entity reached state 3 (sets own
// state 3), force-completes when it is state 2 with a zero +0x38, then clamps
// the linked entity's +0x38 value by a per-mode cap (6/9/12), writes
// arg0+0xB4 = arg0+0xA4 + clamped, runs func_001C6380 and the +0x4C vtable cb.
//
// NEARMISS 89.2% (mwcc 991202) / 87.2% (mwcc 2.3.3). Logic fully recovered;
// residual is constant-CSE register coloring (the literal 3 held live in one
// reg across all compares/stores in the target) plus switch delay-slot fill.
// Not the clean-store nop. Permuter (regalloc + scheduling) territory.

extern short D_00275630[];

void func_001AFC10(char *, int, int);
void func_001B1020(short, int, int);
void func_001C6380(char *);

void func_001BF3C0(char *arg0) {
    char *s0;
    int st;
    float v;

    st = *(unsigned char *)(arg0 + 4);
    s0 = *(char **)(arg0 + 0x20);

    if (st == 3) goto afc;
    if (st == 2) goto afc;
    if (st == 1) goto clamp;
    if (st != 0) return;

    if (*(unsigned char *)(s0 + 4) == 3) {
        *(unsigned char *)(arg0 + 4) = 3;
        return;
    }
    func_001B1020(D_00275630[*(unsigned char *)(arg0 + 0xD)], -1, 0);
    if (*(unsigned char *)(arg0 + 0xD) == 1) {
        *(float *)(arg0 + 0xA4) = *(float *)(arg0 + 0xA4) - 3.0f;
    } else if (*(unsigned char *)(arg0 + 0xD) == 2) {
        *(float *)(arg0 + 0xA4) = *(float *)(arg0 + 0xA4) - 6.0f;
    }

clamp:
    if (*(unsigned char *)(s0 + 4) == 3) {
        *(unsigned char *)(arg0 + 4) = 3;
        return;
    }
    if (*(unsigned char *)(s0 + 4) == 2 && *(float *)(s0 + 0x38) == 0.0f) {
        *(unsigned char *)(arg0 + 4) = 3;
    }

    v = *(float *)(s0 + 0x38);
    switch (*(unsigned char *)(arg0 + 0xD)) {
    case 0:
        if (v > 6.0f) v = 6.0f;
        break;
    case 1:
        if (v > 9.0f) v = 9.0f;
        break;
    case 2:
        if (v > 12.0f) v = 12.0f;
        break;
    }
    *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xA4) + v;
    func_001C6380(arg0);
    (*(void (**)(char *))(arg0 + 0x4C))(arg0);
    return;

afc:
    func_001AFC10(arg0, 3, st);
}
