// INCLUDE_ASM anim_sample_bones  (vram unknown, 448 bytes)
// UNDECOMPILED placeholder. The byte-identical machine code for this
// function is assembled from the local splat disassembly (git-ignored;
// regenerate with `build.py setup` from your own disc) and linked by
// fill_unmatched.py — so the rebuilt ELF stays byte-identical with or
// without this file. build.py does NOT compile INCLUDE_ASM stubs.
//
// To decompile: replace this file with C that compiles byte-identical,
// verified with objdiff against build/expected/anim_sample_bones.o. See
// docs/PROGRESS.md for the matching idioms and the function index in
// docs/FUNCTIONS.csv.
//
// ---------------------------------------------------------------------------
// BEST ATTEMPT 2026-06-09: 86.38% (objdiff, -O4,p -sdatathreshold 0).
//
// Semantics (verified against docs/FINDINGS.md "Per-bone sampler"):
// per-bone clip-change sampler driver. For each bone: (1) snapshot the
// current pose — NLERP quat A/B by the bone's blend into the VU0 scratch
// quat (0x70003600) and copy it back to quat A (+0x30); (2) sample the new
// rotation keyframe via anim_sample_rotation(channel state @D_008111F0,
// bone index, float_to_int(new_t)), NLERP the channel quats
// (D_00811220/30, t = D_00811240) into the scratch and copy to quat B
// (+0x40); (3) write rotation drive state: dur(+0x60)=prev_t,
// kind(+0x66)=D_00811256, blend(+0x50)=0, inv_dur(+0x54)=1/prev_t;
// (4) repeat for the translation channel (func_001C90D0, +0x58/+0x68,
// vec via func_001C86A0(bone+0xC, D_008111F0, bone)) and the scale channel
// (func_001C92C0, +0x5C/+0x6A, func_001C86A0(bone+0x24, D_00811208,
// bone+0x18)).
//
// Remaining 15 mismatched rows are all scheduler/regalloc walls:
// * Documented GENUINE slt-into-branch wall: loop back-edge `slt; bnez`
//   sinks into $at under mwcc, CW uses $v1 (2 rows), plus CW's trailing
//   nop after the back-edge delay slot (1 row, delay/nop-fill wall).
// * Load-delay fill choice: after each scratch-quat address pair, CW files
//   an independent lw (bone reload) between addiu and lq; mwcc issues the
//   lq immediately after its addiu (5 rows over both copies). An explicit
//   LHS-first temp (`b = bones[0];` then `b->quat_a.q = ...`) is
//   copy-propagated away — no effect.
// * Arg-setup lui cluster order for the 2nd quat_nlerp call and the
//   lui-$at hoists above swc1 in the channel-state blocks (CW hoists the
//   lui 2 slots earlier than mwcc; 10 rows total across 3 sites).
// * The bones-pointer save `paddub s1, a0` is hoisted above the loop
//   pre-test by mwcc, CW emits it after (2 rows).
//
// CRACKED here (reusable):
// * Saved-register ORDER for loop drivers: copying the pointer param into
//   an explicit local (`bones = arg_bones`) moves its web start ahead of
//   the count param's, flipping mwcc's s1/s2 assignment to match CW
//   (i->s0 in the guard delay slot, bones->s1, n->s2). This recovered 11
//   rows — a working COUNTER-EXAMPLE to the "registers or placement,
//   never both" wall datum for the param-save case.
// * 128-bit quat copies as a union field (`Quat { float f[4]; uint128 q; }`)
//   make mwcc fold the +0x30/+0x40 offsets into sq directly; a
//   `*(uint128 *)bone->quat_a`-style cast of a float[4] field emits a
//   separate addiu instead.
// * Writing `bones[0]->field` per statement (no cached bone local)
//   reproduces CW's per-statement reload of the bone pointer, which both
//   compilers must do anyway after stores through unknown aliases/calls.
//
#if 0
typedef unsigned __int128 uint128;

typedef union Quat {
    float f[4];
    uint128 q;
} Quat;

typedef struct Bone {
    char pad00[0xC];
    float trans_a[3];       /* 0x0C */
    float scratch18[3];     /* 0x18 */
    float scale_a[3];       /* 0x24 */
    Quat quat_a;            /* 0x30 */
    Quat quat_b;            /* 0x40 */
    float blend;            /* 0x50 */
    float inv_dur;          /* 0x54 */
    float dur_t;            /* 0x58 */
    float dur_s;            /* 0x5C */
    float dur;              /* 0x60 */
    char pad64[0x2];
    unsigned short kind_r;  /* 0x66 */
    unsigned short kind_t;  /* 0x68 */
    unsigned short kind_s;  /* 0x6A */
} Bone;

extern float D_70003600[4];
extern char D_008111F0[];
extern float D_00811208[4];
extern float D_00811220[4];
extern float D_00811230[4];
extern float D_00811240;
extern unsigned short D_00811256;
extern unsigned short D_00811258;
extern unsigned short D_0081125A;

extern void quat_nlerp(float *, float *, float *, float);
extern int float_to_int(float);
extern void anim_sample_rotation(char *, int, int);
extern void func_001C90D0(char *, int, int);
extern void func_001C92C0(char *, int, int);
extern void func_001C86A0(float *, float *, float *, float);

void anim_sample_bones(Bone **arg_bones, int n, float new_t, float prev_t) {
    int i;
    Bone **bones;

    for (i = 0, bones = arg_bones; i < n; i++, bones++) {
        quat_nlerp(D_70003600, bones[0]->quat_a.f, bones[0]->quat_b.f, bones[0]->blend);
        bones[0]->quat_a.q = *(uint128 *)D_70003600;
        anim_sample_rotation(D_008111F0, i, float_to_int(new_t));
        quat_nlerp(D_70003600, D_00811220, D_00811230, D_00811240);
        bones[0]->quat_b.q = *(uint128 *)D_70003600;
        bones[0]->dur = prev_t;
        bones[0]->kind_r = D_00811256;
        *(int *)&bones[0]->blend = 0;
        bones[0]->inv_dur = 1.0f / prev_t;
        func_001C90D0(D_008111F0, i, float_to_int(new_t));
        bones[0]->dur_t = prev_t;
        bones[0]->kind_t = D_00811258;
        func_001C86A0(bones[0]->trans_a, (float *)D_008111F0, (float *)bones[0], prev_t);
        func_001C92C0(D_008111F0, i, float_to_int(new_t));
        bones[0]->dur_s = prev_t;
        bones[0]->kind_s = D_0081125A;
        func_001C86A0(bones[0]->scale_a, D_00811208, bones[0]->scratch18, prev_t);
    }
}
#endif
