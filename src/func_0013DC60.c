// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// Behaviour state 3 of the func_0013D600 actor class, (e, st), sub-step in
// e[6]. Sub-step 0: advance to sub-step 1, set st+0x40 = 1.0f, clear the
// st+0x48/0x50/0x4C words, and start clip 10 with
// anim_clip_init(e, 10, 10.0f, 0.0f). Sub-step 1: when the clip is settled
// (e+0x2C bit 0x8000 clear) and the float at e+0x3C is exactly 62.0f, fire
// cue 0x806 via func_001FBD50(e, 0x806, 0, 300.0f); then, once the animation
// flags word st+0x30 has bit 0x1000 set, switch to behaviour state 4
// (e[5] = 4, e[6] = 0). Other sub-steps do nothing.
extern void anim_clip_init(char *e, int clip, float blend, float frame);
extern int func_001FBD50(char *e, int id, int a2, float f);

void func_0013DC60(char *e, char *st) {
    switch (*(unsigned char *)(e + 6)) {
    case 0:
        *(unsigned char *)(e + 6) += 1;
        *(float *)(st + 0x40) = 1.0f;
        *(int *)(st + 0x48) = 0;
        *(int *)(st + 0x50) = 0;
        *(int *)(st + 0x4C) = 0;
        anim_clip_init(e, 10, 10.0f, 0.0f);
        break;
    case 1:
        if (!(*(short *)(e + 0x2C) & 0x8000) && *(float *)(e + 0x3C) == 62.0f) {
            func_001FBD50(e, 0x806, 0, 300.0f);
        }
        if (*(int *)(st + 0x30) & 0x1000) {
            *(unsigned char *)(e + 5) = 4;
            *(unsigned char *)(e + 6) = 0;
        }
        break;
    }
}
