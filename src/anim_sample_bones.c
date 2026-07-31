// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Per-bone clip-change sampler driver. For each of n bones: NLERP the current
// pose quat A/B by blend into the VU0 scratch quat (0x70003600) and copy back
// to quat A (+0x30); sample the new rotation keyframe (anim_sample_rotation, channel
// state @D_008111F0), NLERP the channel quats (D_00811220/30, t=D_00811240)
// into scratch and copy to quat B (+0x40); write rotation drive state
// (dur+0x60=prev_t, kind+0x66=D_00811256, blend+0x50=0, inv_dur+0x54=1/prev_t);
// then the translation channel (func_001C90D0, +0x58/+0x68, vec via
// func_001C86A0(bone+0xC,D_008111F0,bone)) and scale channel (func_001C92C0,
// +0x5C/+0x6A, func_001C86A0(bone+0x24,D_00811208,bone+0x18)).
//
// mwcc 2.3.3 matches byte-identical; the pinned 991202 build caps at 88.04%.
// The sole residual the body-identical near-miss had was the prologue: CW
// emits the loop pre-test `beqz` with `i=0` (s0) in its delay slot and the
// bones-pointer save AFTER the branch. Computing `bones = arg_bones + i`
// INSIDE the loop body (not in the for-init) moves the bones save after the
// pre-test, matching CW (the wall anim_sample_bones documented at 86%).
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

    for (i = 0; i < n; i++) {
        bones = arg_bones + i;
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
