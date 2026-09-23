// NEARMISS func_0015AE20  (vram 0x0015AE20, 0x178 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.98% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). Object similarity does not prove semantic equivalence.
// Remaining differences in this candidate:
// Only residual: the expected object renders scratchpad 0x70003B92 as literal operands (not in build.py _SPAD_SYMS) while this C uses relocated externs; linked bytes identical. objdiff 100.0% with _SPAD_SYMS += 0x70003B92.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// MATCH NOTE (m1-firstlevel-matching lane): the scratchpad globals at 0x70003B92
// are referenced as relocated externs (D_7000xxxx), as the original build did
// (see tools/match/spad_symbolize.py). A literal address let mwcc CSE and
// speculate `lui at,0x7000` into delay slots that the target leaves as nop.
// objdiff is 100.0% once build.py _SPAD_SYMS lists these addresses (so the
// expected object carries the same relocations); the linked bytes are identical.
//
// Per-frame state step for an entity, dispatched on the state byte at arg0+5.
// State 0: only when the +0xB flag has bit 4 set, advance the state and pick a
// sound/event to play -- if the global D_008104A0 == 0x2D or D_008104E6 != 0 it
// plays D_00248480, else it picks a tier by comparing the float at arg0+0xB4
// against (6.0f + D_00810354) and (7.0f + that): below the low threshold writes
// code 0x42 to the global D_00248354, between writes 0x41, above writes 0x40,
// then plays D_002482C0 (func_001BA1A0(arg1, ...)). State 1: if func_001BA1F0()
// succeeds, bump the counter at arg0+4. Always: if the scratchpad flag
// D_70003B92 is clear run func_001F1180(self); and if func_001B17A0(self)
// succeeds, invoke the virtual at arg0+0x4C.

extern unsigned char D_70003B92;
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
    if (D_70003B92 == 0) {
        func_001F1180(arg0);
    }
    if (func_001B17A0(arg0) != 0) {
        (*(void (**)(char *))(arg0 + 0x4C))(arg0);
    }
}
