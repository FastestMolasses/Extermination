// NEARMISS anim_matrix_player  (vram 0x0017B660, 0x2A8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.09% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). Object similarity does not prove semantic equivalence.
// Remaining differences in this candidate:
// Saved-register colouring only, same instructions and size: the target has self in $s0, the clip id in $s1, loop cursors in $s2-$s4 and +0x3C / current / next length in $f20/$f21/$f22; mwcc gives self $s1 and rotates the rest. 1500 declaration orders and mwcc 2.4/991202 did not move it.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// Player bone-matrix transition writer: the writer of D_00287F40 / D_00288D40,
// driven by D_00275B40, the per-bone source-pointer table published by
// anim_bone_array_setup. With self+0x1F1 == 0 it re-requests the tier clip
// func_0017B490(self, 1, self[0x235], self[0x25C]) and, when that differs from
// the current clip +0x20C, restarts it through anim_clip_arbiter at the same
// relative position. Otherwise it takes the next (+0x1F1 == 1) or previous tier's
// clip, evaluates the current and the next pose into the two matrix buffers,
// blends them per bone by +0x208 (func_001C9D50) and publishes the result
// (func_00179FF0); while the blend +0x208 is below 1.0 it requests the current
// clip again at (current length - +0x3C).
//
// Corrected in decomp lane round 8 from the original instructions (82.85% before):
//  - func_0017B490 takes (obj, cmd, idx, tbl): the target passes self in $a0,
//    1 in $a1, the byte +0x235 in $a2 and the byte +0x25C (+1 / -1) in $a3. The
//    old C passed (1, +0x235, +0x25C), one argument short and shifted.
//  - anim_clip_arbiter is (p, v, float blend, float frame): the target passes
//    0.0 in $f12 and the position in $f13; the old C passed an integer 0.
//  - func_001C61D0 returns an int (clip length), converted to float by the
//    caller; the old C declared it float and read $f0.
//  - The next-clip position re-reads +0x3C after func_00179D20 instead of reusing
//    the value loaded before it.
//  - D_00287F40 / D_00288D40 are addressed absolutely (declared as arrays; the
//    size only keeps them out of small data).

extern void copy_qw4(void *, void *);
extern int anim_clip_arbiter(char *p, int v, float blend, float frame);
extern void func_00179D20(unsigned char *arg0);
extern void func_00179FF0(unsigned char *self);
extern short func_0017B490(char *obj, int cmd, int idx, int *tbl);
extern int func_001C61D0(int a0, int a1);
extern void func_001C9D50(char *out, char *a, char *b, float blend);
extern char **D_00275B40;
extern char D_00287F40[0xE00];
extern char D_00288D40[0xE00];

void anim_matrix_player(char *s0) {
    int i;
    char *p;
    short a1;
    short s1;
    int s5;
    float f20;
    float f21;
    float f22;
    float f0;

    if (*(unsigned char *)(s0 + 0x1F1) == 0) {
        s1 = (short) func_0017B490(s0, 1, *(unsigned char *)(s0 + 0x235), (int *)(*(unsigned char *)(s0 + 0x25C)));
        a1 = *(short *)(s0 + 0x20C);
        if (s1 != a1) {
            f20 = func_001C61D0(*(int *)(s0 + 0x40), a1);
            f0 = func_001C61D0(*(int *)(s0 + 0x40), s1);
            anim_clip_arbiter(s0, s1, 0.0f, f0 * ((f20 - *(float *)(s0 + 0x3C)) / f20));
        }
        return;
    }
    if (*(unsigned char *)(s0 + 0x1F1) == 1) {
        s5 = func_0017B490(s0, 1, *(unsigned char *)(s0 + 0x235), (int *)(*(unsigned char *)(s0 + 0x25C) + 1));
    } else {
        s5 = func_0017B490(s0, 1, *(unsigned char *)(s0 + 0x235), (int *)(*(unsigned char *)(s0 + 0x25C) - 1));
    }
    a1 = *(short *)(s0 + 0x20C);
    f20 = *(float *)(s0 + 0x3C);
    s1 = a1;
    f21 = func_001C61D0(*(int *)(s0 + 0x40), a1);
    f22 = func_001C61D0(*(int *)(s0 + 0x40), s5);
    func_00179D20((unsigned char *)s0);

    p = D_00288D40;
    for (i = 0; i < *(unsigned char *)(s0 + 0xC); i++) {
        copy_qw4(p, D_00275B40[i] + 0x90);
        p += 0x40;
    }

    anim_clip_arbiter(s0, s5, 0.0f, f22 * ((f21 - *(float *)(s0 + 0x3C)) / f21));
    func_00179D20((unsigned char *)s0);

    p = D_00287F40;
    for (i = 0; i < *(unsigned char *)(s0 + 0xC); i++) {
        copy_qw4(p, D_00275B40[i] + 0x90);
        p += 0x40;
    }

    {
        char *p2 = D_00287F40;
        char *p3 = D_00288D40;
        for (i = 0; i < *(unsigned char *)(s0 + 0xC); i++) {
            func_001C9D50(D_00275B40[i] + 0x90, p3, p2, *(float *)(s0 + 0x208));
            p2 += 0x40;
            p3 += 0x40;
        }
    }

    func_00179FF0((unsigned char *)s0);
    if (*(float *)(s0 + 0x208) < 1.0f) {
        anim_clip_arbiter(s0, s1, 0.0f, f21 - f20);
    }
}
