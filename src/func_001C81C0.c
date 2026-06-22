// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// Per-bone clip pose builder. Resolves clip `clip_id` (a1, sign-extended
// short) via anim_clip_resolve(arg0, clip_id); then for each bone
// [0, count) samples rotation/trans/scale channels at `time` from the
// shared channel state (D_008111F0), NLERPs the resulting quat
// (D_00811220/30 @ t=D_00811240) into the VU0 scratch quat (D_70003600),
// and writes a 3x4 bone matrix to `out` (advancing 0x40). Bone count from
// *(u16 *)*D_00275BF8.
extern void anim_clip_resolve(int, int);
extern void anim_sample_rotation(char *, int, int);
extern void func_001C90D0(char *, int, int);
extern void func_001C92C0(char *, int, int);
extern void quat_nlerp(float *, float *, float *, float);
extern void quat_to_mat3(char *, float *, char *);
extern unsigned short *D_00275BF8;
extern char D_008111F0[];
extern float D_00811220[4];
extern float D_00811230[4];
extern float D_00811240[2];
extern float D_70003600[4];

void func_001C81C0(int arg0, short clip_id, int time, char *out)
{
    int i;

    anim_clip_resolve(arg0, clip_id);
    for (i = 0; i < (int)*D_00275BF8; i++) {
        anim_sample_rotation(D_008111F0, i, time);
        func_001C90D0(D_008111F0, i, time);
        func_001C92C0(D_008111F0, i, time);
        quat_nlerp(D_70003600, D_00811220, D_00811230, D_00811240[0]);
        quat_to_mat3(out, D_70003600, D_008111F0);
        out += 0x40;
    }
}
