// INCLUDE_ASM anim_sample_rotation  (vram unknown, 440 bytes)
// UNDECOMPILED placeholder. The byte-identical machine code for this
// function is assembled from the local splat disassembly (git-ignored;
// regenerate with `build.py setup` from your own disc) and linked by
// fill_unmatched.py — so the rebuilt ELF stays byte-identical with or
// without this file. build.py does NOT compile INCLUDE_ASM stubs.
//
// To decompile: replace this file with C that compiles byte-identical,
// verified with objdiff against build/expected/anim_sample_rotation.o. See
// docs/PROGRESS.md for the matching idioms and the function index in
// docs/FUNCTIONS.csv.
//
// ---------------------------------------------------------------------------
// BEST ATTEMPT 2026-06-09: 82.45% (objdiff, default -O4,p).
//
// Semantics (verified against docs/FINDINGS.md "Keyframe stream format"):
// per-bone ROTATION keyframe directory walk + interval sampler. Resolves the
// bone's keyframe stream via the section-1 directory (D_00275BF4 +
// u32-offset[bone_idx]), walks 12-byte records counting steps into
// actor->step (+0x66, seeded 1) until t_prev(+0xA) <= time < t_next(+0x16),
// then decodes both straddling rotation keyframes via func_001C84D0 into
// actor quat A (+0x30) and quat B (+0x40) and writes the NLERP drive state:
// dur (+0x60) = t_next - time, inv_dur (+0x54) = 1/(t_next - t_prev),
// blend (+0x50) = inv_dur * (time - t_prev). u16 times convert through the
// (float)(unsigned int) idiom; `time` converts signed.
//
// Everything matches EXCEPT documented genuine walls (all rows accounted):
// * Dead `b epilogue` coalescing (CONFIRMED GENUINE WALL #1): CW emits
//   `b epilogue` after the sample body, keeping the advance block BETWEEN
//   body and epilogue; mwcc coalesces the jump into fallthrough and moves
//   the advance block AFTER the epilogue (13 rows incl. the two guard
//   branch offsets). `goto done; done: return;` does not survive; -O2,p
//   trades this for far worse (62.8%).
// * Dead-dup srl: CW's (float)(unsigned) intrinsic hoists `srl r,r,1` into
//   the bltz delay slot AND leaves the dead original before the negative-
//   path label; mwcc always deletes the dead copy (3 rows).
// * div.s stall: CW pads two nops after `div.s` before the dependent use;
//   mwcc does not (2 rows).
// * Minor scheduling: the initial `sh 1` placed 2 slots earlier, and the
//   first call's two arg-setup instructions swapped (hoisting the arg into
//   a block-scoped local regresses the whole prologue — do not).
//
// Idioms that DID crack sub-walls here (reusable):
// * struct field access (BoneChan) instead of `*(float *)(p + 0x54)`
//   defeats mwcc's address-CSE (`addiu a1, s4, 0x54` + 0x0(a1) accesses)
//   when the same field is stored then reloaded — CW folds the offset both
//   times.
// * Inlining `(float)time` into both consuming expressions (instead of a
//   `fnow` local) lets CSE place the single conversion at first use,
//   matching CW's evaluation order utof(t_next) BEFORE itof(time).
// * goto-shaped loop (guards jumping forward to a bottom advance block)
//   reproduces CW's non-rotated while-loop layout; mwcc rotates a plain
//   `while` (entry jump to a bottom condition) which diverges much more.
//
#if 0
typedef struct BoneChan {
    char pad00[0x30];
    float quat_a[4];        /* 0x30 */
    float quat_b[4];        /* 0x40 */
    float blend;            /* 0x50 */
    float inv_dur;          /* 0x54 */
    char pad58[0x8];
    float dur;              /* 0x60 */
    char pad64[0x2];
    unsigned short step;    /* 0x66 */
} BoneChan;

extern char *D_00275BF4;
extern void func_001C84D0(unsigned short *, float *);

void anim_sample_rotation(BoneChan *actor, int bone_idx, int time) {
    char *rec;
    int t_prev;
    int t_next;
    float fdur;

    rec = D_00275BF4 + *(int *)(D_00275BF4 + bone_idx * 4);
    actor->step = 1;
top:
    t_prev = *(unsigned short *)(rec + 0xA);
    if (time < t_prev) goto advance;
    t_next = *(unsigned short *)(rec + 0x16);
    if (time >= t_next) goto advance;

    fdur = (float)(unsigned int)t_prev;
    func_001C84D0((unsigned short *)rec, actor->quat_a);
    fdur = (float)(unsigned int)*(unsigned short *)(rec + 0x16) - fdur;
    func_001C84D0((unsigned short *)(rec + 0xC), actor->quat_b);
    actor->dur = (float)(unsigned int)t_next - (float)time;
    actor->inv_dur = 1.0f / fdur;
    actor->blend = actor->inv_dur * ((float)time - (float)(unsigned int)t_prev);
    goto done;

advance:
    actor->step += 1;
    rec += 0xC;
    goto top;

done:
    return;
}
#endif
