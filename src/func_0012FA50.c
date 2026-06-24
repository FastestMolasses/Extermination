// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Per-tick update for an actor (arg0) against a companion record (arg1),
// dispatched on the actor's state byte at arg0+6, then always tail-calls
// func_00132490(arg0, arg1).
//   state 0 (intro): advance the state byte; clear arg1 flags at +0x61/+0x63;
//     mask arg1+0x69 down to bit0; write 1.0f to arg1+0x34; clear arg1+0x6B;
//     start anim clip 0xC at speed 5.0; if a pending sub-record at arg0+0x20
//     exists, set its byte +4 to 3 and clear the pointer. FALLS THROUGH to
//     state 1.
//   state 1 (run): if arg1+0x69 == 1, set its bit1, stage the actor's launch
//     point (arg0+0xB0..B8) plus a 1.0 w into the GS scratch block 0x700038A0,
//     overwrite +0x700038A4 with arg1+0x40, then func_001EFD90(0x80000016,
//     &D_700038A0, arg0+0xC0) and func_001E8B90(&D_700038A0, 5.0f). Then if the
//     clip timer arg1+0x38 has expired (<= 0) AND arg1+0x60 has bit 0x2, bump
//     the state byte, clear arg1+0x3C, play clip 0xD at 1.0, and fire event
//     0x7D3 via func_001FBD50(arg0, 0x7D3, 0, 300.0f).
//   state 2 (despawn check): if arg1+0x58 (halfword) has bit 0x1000, clear
//     arg0+5 and arg0+6.
//
// mwcc 2.3.3 (mwcps2-2.3.3-000906): the pinned 991202 build caps at 91.85% (it
// fills the state-0 anim_clip_init jal delay slot with the 0x34=1.0 store). The
// unlock is ordering arg1+0x6B=0 immediately before the anim_clip_init call so
// mwcc puts that zero-store in the delay slot, exactly as the target does.
// D_700038A0 stays hi/lo (sized [8]); the raw 0x700038xx stores are absolute.
// Verified objdiff 100.0% vs build/expected/func_0012FA50.o.
extern void anim_clip_init(char *p, int clip, float speed, float d);
extern void func_00132490(char *a, char *b);
extern void func_001E8B90(int *p, float f);
extern void func_001EFD90(int mode, int *p, char *q);
extern void func_001FBD50(char *p, int a, int b, float f);
extern int D_700038A0[8];

void func_0012FA50(char *arg0, char *arg1) {
    unsigned char state = *(unsigned char *)(arg0 + 6);
    char *clip;
    unsigned char f;

    switch (state) {
    case 0:
        *(unsigned char *)(arg0 + 6) = state + 1;
        *(char *)(arg1 + 0x61) = 0;
        *(char *)(arg1 + 0x63) = 0;
        f = *(unsigned char *)(arg1 + 0x69);
        *(unsigned char *)(arg1 + 0x69) = f & 1;
        *(int *)(arg1 + 0x34) = 0x3F800000;
        *(char *)(arg1 + 0x6B) = 0;
        anim_clip_init(arg0, 0xC, 5.0f, 0.0f);
        clip = *(char **)(arg0 + 0x20);
        if (clip != 0) {
            *(char *)(clip + 4) = 3;
            *(int *)(arg0 + 0x20) = 0;
        }
        /* fallthrough */
    case 1:
        if (*(unsigned char *)(arg1 + 0x69) == 1) {
            *(unsigned char *)(arg1 + 0x69) = *(unsigned char *)(arg1 + 0x69) | 2;
            *(float *)0x700038A0 = *(float *)(arg0 + 0xB0);
            *(float *)0x700038A4 = *(float *)(arg0 + 0xB4);
            *(float *)0x700038A8 = *(float *)(arg0 + 0xB8);
            *(int *)0x700038AC = 0x3F800000;
            *(float *)0x700038A4 = *(float *)(arg1 + 0x40);
            func_001EFD90(0x80000016, D_700038A0, arg0 + 0xC0);
            func_001E8B90(D_700038A0, 5.0f);
        }
        if (*(float *)(arg1 + 0x38) <= 0.0f && (*(unsigned char *)(arg1 + 0x60) & 2)) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            *(int *)(arg1 + 0x3C) = 0;
            anim_clip_init(arg0, 0xD, 1.0f, 0.0f);
            func_001FBD50(arg0, 0x7D3, 0, 300.0f);
        }
        break;
    case 2:
        if (*(unsigned short *)(arg1 + 0x58) & 0x1000) {
            *(char *)(arg0 + 5) = 0;
            *(char *)(arg0 + 6) = 0;
        }
        break;
    }
    func_00132490(arg0, arg1);
}
