// NEARMISS func_0015AE20  (vram 0x0015AE20, 0x178 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.31% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Switch default-branch delay slot: when state >= 2 the target branches to the tail with an empty (nop) slot; mwcc 2.3.3 fills it speculatively with `lui at,0x7000` (start of the 0x70003B92 read), shifting the branch target by one word. Single branch-delay-slot scheduling artifact. 99.3% on 2.3.3.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS 99.3% (mwcc 2.3.3; 991202 reaches 92.8%). Per-frame state step for an
// entity, dispatched on the state byte at arg0+5. State 0: only when the +0xB
// flag has bit 4 set, advance the state and pick a sound/event to play --
// if the global D_008104A0 == 0x2D or D_008104E6 != 0 it plays D_00248480, else
// it picks a tier by comparing the float at arg0+0xB4 against (6.0f + D_00810354)
// and (7.0f + that): below the low threshold writes code 0x42 to the global
// D_00248354, between writes 0x41, above writes 0x40, then plays D_002482C0
// (func_001BA1A0(arg1, ...)). State 1: if func_001BA1F0() succeeds, bump the
// counter at arg0+4. Always: if the global flag at 0x70003B92 is clear run
// func_001F1180(self); and if func_001B17A0(self) succeeds, invoke the virtual
// at arg0+0x4C.
//
// Body, the switch dispatch, the float-threshold tiers, and both func_001BA1A0
// call sites are byte-correct. Two idioms were needed: the if/else-if chain for
// the 0x42/0x41/0x40 tiers (matches the target's bc1f sense and avoids a shared
// `code` temp), and the assign-in-arg `(pp = &D_xxx)` trick to force the pointer
// argument through a scratch reg + delay-slot move like the target. The sole
// residual is the switch default-branch delay slot: when state >= 2 the target
// branches to the tail with an empty (nop) slot, but mwcc fills it speculatively
// with the `lui at,0x7000` of the following 0x70003B92 read. A branch-delay-slot
// scheduling artifact.
extern int func_001B17A0(char *p);
extern void func_001BA1A0(int a, char *p);
extern int func_001BA1F0(void);
extern void func_001F1180(char *p);
extern char D_002482C0;
extern int D_00248354;
extern char D_00248480;
extern float D_00810354;
extern unsigned char D_008104A0;
extern unsigned char D_008104E6;

void func_0015AE20(char *arg0, int arg1) {
    unsigned char st;
    float lo;
    float v;
    char *pp;

    st = *(unsigned char *)(arg0 + 5);
    switch (st) {
    case 0:
        if (*(unsigned char *)(arg0 + 0xB) & 4) {
            *(unsigned char *)(arg0 + 5) = st + 1;
            if (D_008104A0 == 0x2D || D_008104E6 != 0) {
                func_001BA1A0(arg1, (pp = &D_00248480));
            } else {
                v = *(float *)(arg0 + 0xB4);
                lo = 6.0f + D_00810354;
                if (v < lo) {
                    D_00248354 = 0x42;
                } else if (v < 7.0f + lo) {
                    D_00248354 = 0x41;
                } else {
                    D_00248354 = 0x40;
                }
                func_001BA1A0(arg1, (pp = &D_002482C0));
            }
        }
        break;
    case 1:
        if (func_001BA1F0() != 0) {
            *(unsigned char *)(arg0 + 4) = *(unsigned char *)(arg0 + 4) + 1;
        }
        break;
    }
    if (*(unsigned char *)0x70003B92 == 0) {
        func_001F1180(arg0);
    }
    if (func_001B17A0(arg0) != 0) {
        (*(void (**)(char *))(arg0 + 0x4C))(arg0);
    }
}
