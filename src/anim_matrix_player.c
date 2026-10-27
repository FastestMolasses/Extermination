// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 98.09% NEARMISS). Fixes read from the original instructions:
//  - One variable carries the requested clip in both arms: the tier-0 re-request result and
//    the next/previous tier clip share `next` (the target keeps both webs apart only by
//    liveness); separate variables put self and the clip in swapped saved registers.
//  - The tier-0 arm's current clip and lengths are block-local to that arm.
//  - The current clip is loaded into its own short and copied into the restart variable
//    (an int), which the target keeps as a separate saved register filled in a call slot.
//  - The two pose copy loops index the matrix buffers (&D_00288D40[i * 0x40]); only the
//    blend loop walks two pointers.
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
// Corrected in decomp lane round 8 from the original instructions:
//  - func_0017B490 takes (obj, cmd, idx, tbl): self, 1, the byte +0x235 and the
//    byte +0x25C (+1 / -1).
//  - anim_clip_arbiter is (p, v, float blend, float frame): 0.0 blend, the position.
//  - func_001C61D0 returns an int (clip length), converted to float by the caller.
//  - The next-clip position re-reads +0x3C after func_00179D20.
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

void anim_matrix_player(char *arg0) {
    char *self = arg0;
    int restart_clip;
    int i;
    short cur_clip;
    int next;
    float pos;
    float curlen;
    float nextlen;

    if (*(unsigned char *)(self + 0x1F1) == 0) {
        short cur;
        float len;
        float nlen;

        next = (short) func_0017B490(self, 1, *(unsigned char *)(self + 0x235), (int *)(*(unsigned char *)(self + 0x25C)));
        cur = *(short *)(self + 0x20C);
        if (next != cur) {
            len = func_001C61D0(*(int *)(self + 0x40), cur);
            nlen = func_001C61D0(*(int *)(self + 0x40), next);
            anim_clip_arbiter(self, next, 0.0f, nlen * ((len - *(float *)(self + 0x3C)) / len));
        }
        return;
    }
    if (*(unsigned char *)(self + 0x1F1) == 1) {
        next = func_0017B490(self, 1, *(unsigned char *)(self + 0x235), (int *)(*(unsigned char *)(self + 0x25C) + 1));
    } else {
        next = func_0017B490(self, 1, *(unsigned char *)(self + 0x235), (int *)(*(unsigned char *)(self + 0x25C) - 1));
    }
    cur_clip = *(short *)(self + 0x20C);
    pos = *(float *)(self + 0x3C);
    restart_clip = cur_clip;
    curlen = func_001C61D0(*(int *)(self + 0x40), cur_clip);
    nextlen = func_001C61D0(*(int *)(self + 0x40), next);
    func_00179D20((unsigned char *)self);

    for (i = 0; i < *(unsigned char *)(self + 0xC); i++) {
        copy_qw4(&D_00288D40[i * 0x40], D_00275B40[i] + 0x90);
    }

    anim_clip_arbiter(self, next, 0.0f, nextlen * ((curlen - *(float *)(self + 0x3C)) / curlen));
    func_00179D20((unsigned char *)self);

    for (i = 0; i < *(unsigned char *)(self + 0xC); i++) {
        copy_qw4(&D_00287F40[i * 0x40], D_00275B40[i] + 0x90);
    }

    {
        char *next_m = D_00287F40;
        char *cur_m = D_00288D40;
        for (i = 0; i < *(unsigned char *)(self + 0xC); i++) {
            func_001C9D50(D_00275B40[i] + 0x90, cur_m, next_m, *(float *)(self + 0x208));
            next_m += 0x40;
            cur_m += 0x40;
        }
    }

    func_00179FF0((unsigned char *)self);
    if (*(float *)(self + 0x208) < 1.0f) {
        anim_clip_arbiter(self, restart_clip, 0.0f, curlen - pos);
    }
}
