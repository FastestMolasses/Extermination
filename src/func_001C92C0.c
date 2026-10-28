// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 86.84% NEARMISS).
// Fixes read from the original instructions:
// - Not an FP register-coloring wall: the body is the exact twin of the
//   byte-matched func_001C90D0 (translation channel, D_00275BF0, +0x68/+0x58,
//   dst +0x0/+0xC); this one uses D_00275BEC, counter +0x6A, duration +0x5C,
//   dst vec +0x18 and delta vec +0x24. Spelled like the twin: int t_prev/t_next,
//   `t_prev > time` first guard, goto loop, and the three position-advance
//   lines as compound assignments.
//
// Per-bone keyframe directory walk + interval sampler for the second
// translation-type channel. Resolves the bone's keyframe stream via the
// directory (D_00275BEC + u32-offset[bone_idx]), walks 12-byte records
// counting steps into the step counter (+0x6A, seeded 1) until
// t_prev(+0xA) <= time < t_next(+0x16), decodes both straddling keyframes via
// anim_decode_translation into the vec at +0x18 and a delta vec at +0x24,
// scales the delta via func_001C86A0(+0x24, +0x24, +0x18, t_next - t_prev),
// writes dur(+0x5C) = t_next - time, and advances vec[0..2] += delta[0..2] *
// (time - t_prev). u16 times convert through the (float)(unsigned int) idiom.
extern char *D_00275BEC;
extern void anim_decode_translation(unsigned short *, float *);
extern void func_001C86A0(float *dst, const float *a, const float *b, float divisor);

void func_001C92C0(char *actor, int bone_idx, int time) {
    char *rec;
    int t_prev;
    int t_next;
    float fblend;
    float ftmp;

    rec = D_00275BEC + *(int *)(D_00275BEC + bone_idx * 4);
    *(unsigned short *)(actor + 0x6A) = 1;
top:
    t_prev = *(unsigned short *)(rec + 0xA);
    if (t_prev > time) goto advance;
    t_next = *(unsigned short *)(rec + 0x16);
    if (time >= t_next) goto advance;

    fblend = (float)time - (float)(unsigned int)t_prev;
    ftmp = (float)(unsigned int)t_prev;
    anim_decode_translation((unsigned short *)rec, (float *)(actor + 0x18));
    ftmp = (float)(unsigned int)*(unsigned short *)(rec + 0x16) - ftmp;
    anim_decode_translation((unsigned short *)(rec + 0xC), (float *)(actor + 0x24));
    func_001C86A0((float *)(actor + 0x24), (float *)(actor + 0x24), (float *)(actor + 0x18), ftmp);
    *(float *)(actor + 0x5C) = (float)(unsigned int)t_next - (float)time;
    *(float *)(actor + 0x18) += *(float *)(actor + 0x24) * fblend;
    *(float *)(actor + 0x1C) += *(float *)(actor + 0x28) * fblend;
    *(float *)(actor + 0x20) += *(float *)(actor + 0x2C) * fblend;
    goto done;

advance:
    *(unsigned short *)(actor + 0x6A) += 1;
    rec += 0xC;
    goto top;

done:
    return;
}
