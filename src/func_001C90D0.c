// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// Per-bone TRANSLATION keyframe directory walk + interval sampler. Resolves
// the bone's keyframe stream via the section-1 directory (D_00275BF0 +
// u32-offset[bone_idx]), walks 12-byte records counting steps into the
// step counter (+0x68, seeded 1) until t_prev(+0xA) <= time < t_next(+0x16),
// then decodes both straddling translation keyframes via
// anim_decode_translation into the dst vec (+0x0) and a delta vec (+0xC),
// scales the delta into place via func_001C86A0(dst+0xC, dst+0xC, dst, ftmp),
// writes dur(+0x58)=t_next-time, and advances the position vec
// dst[0..2] += delta[0..2] * fblend where fblend = time - t_prev.
// u16 times convert through the (float)(unsigned int) idiom; time signed.
//
// mwcc 2.3.3 matches byte-identical; the pinned 991202 build caps at 87.62%.
// Keys: -sdatathreshold 8 puts D_00275BF0 through gp-rel matching CW;
// `t_prev > time` for the first guard fixes the $at-vs-$v0 compare coloring;
// and the three position-advance lines written as compound assignment
// (`*p += delta*fblend`, NOT `*p = *p + delta*fblend`) make mwcc keep CW's
// FP register coloring (delta into $f1, base into $f0) on lines 2 and 3 (the
// expanded form regressed those two lines to swapped $f0/$f1).
extern char *D_00275BF0;
extern void anim_decode_translation(unsigned short *, float *);
extern void func_001C86A0(float *dst, const float *a, const float *b, float divisor);

void func_001C90D0(char *actor, int bone_idx, int time) {
    char *rec;
    int t_prev;
    int t_next;
    float fblend;
    float ftmp;

    rec = D_00275BF0 + *(int *)(D_00275BF0 + bone_idx * 4);
    *(unsigned short *)(actor + 0x68) = 1;
top:
    t_prev = *(unsigned short *)(rec + 0xA);
    if (t_prev > time) goto advance;
    t_next = *(unsigned short *)(rec + 0x16);
    if (time >= t_next) goto advance;

    fblend = (float)time - (float)(unsigned int)t_prev;
    ftmp = (float)(unsigned int)t_prev;
    anim_decode_translation((unsigned short *)rec, (float *)actor);
    ftmp = (float)(unsigned int)*(unsigned short *)(rec + 0x16) - ftmp;
    anim_decode_translation((unsigned short *)(rec + 0xC), (float *)(actor + 0xC));
    func_001C86A0((float *)(actor + 0xC), (float *)(actor + 0xC), (float *)actor, ftmp);
    *(float *)(actor + 0x58) = (float)(unsigned int)t_next - (float)time;
    *(float *)(actor + 0) += *(float *)(actor + 0xC) * fblend;
    *(float *)(actor + 4) += *(float *)(actor + 0x10) * fblend;
    *(float *)(actor + 8) += *(float *)(actor + 0x14) * fblend;
    goto done;

advance:
    *(unsigned short *)(actor + 0x68) += 1;
    rec += 0xC;
    goto top;

done:
    return;
}
