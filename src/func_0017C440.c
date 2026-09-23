// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// MATCH NOTE (m1-firstlevel-matching lane): the scratchpad globals at 0x70003A20
// are referenced as relocated externs (D_7000xxxx), as the original build did
// (see tools/match/spad_symbolize.py). A literal address let mwcc CSE and
// speculate `lui at,0x7000` into delay slots that the target leaves as nop.
// objdiff is 100.0% once build.py _SPAD_SYMS lists these addresses (so the
// expected object carries the same relocations); the linked bytes are identical.
//
// Run-stop interruption request, called by 0017C030 mode 4 after gait > 1.
// The movement mode is a real argument forwarded to 00178B90; the old
// void(void) declaration merely happened to leave a0/a1 live in registers.
//
// Select gait-1 and its speed, translate once, then request the normal gait
// clip with a four-tick blend at length-18 (tier 2) or length-46. The enclosing
// walk callback translates again afterward. Original frame 4134 and the native
// original-instruction oracle independently confirm the forwarded argument 1.
// See sibling port docs/FIRST_CONTROL.md and tools/test_player_reentry_reference.py.
extern float D_70003A20;
extern float D_00248870[];
extern void func_00178B90(char *p, int mode);
extern int func_0017B490(char *p, int a, unsigned char b, unsigned char c);
extern int func_001C61D0(int a, int b);
extern int anim_clip_arbiter(char *p, int v, float a, float b);

void func_0017C440(char *p, int mode) {
    int v;

    *(unsigned char *)(p + 0x25C) = *(unsigned char *)(p + 0x23F) - 1;
    *(float *)(p + 0x38) = D_00248870[*(unsigned char *)(p + 0x25C)];
    func_00178B90(p, mode);
    v = (short)func_0017B490(p, 1, *(unsigned char *)(p + 0x235), *(unsigned char *)(p + 0x25C));
    D_70003A20 = (float)func_001C61D0(*(int *)(p + 0x40), v);
    if (*(unsigned char *)(p + 0x25C) == 2) {
        anim_clip_arbiter(p, v, 4.0f, D_70003A20 - 18.0f);
    } else {
        anim_clip_arbiter(p, v, 4.0f, D_70003A20 - 46.0f);
    }
    *(char *)(p + 0x1F0) = 1;
}
