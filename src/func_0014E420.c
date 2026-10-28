// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Two-step intro state on self+6. State 0: advance the state, zero ctx+0x28,
// ctx+0x20 = 1.0f, self+7 = 1, the short timer ctx+0x56 = 240, ctx+0x3C = 2.5f,
// clear the tilt flags ctx+0x59/ctx+0x58, start clip 0x17 (anim_clip_init(self,
// 0x17, 5.0f, 0.0f)) and play sound/effect 0x867 via func_001FBD50(self, 0x867,
// 0, 300.0f). State 1: keep ctx+0x56 at 240; once bit 0x1000 of ctx+0 (ctx+0 holds the
// last anim_advance_time result) is set, go to state self+4 = 1
// with self+5 = self+6 = 0.
extern void anim_clip_init(int self, int clip, float a, float b);
extern void func_001FBD50(char *p, int a, int b, float f);

void func_0014E420(char *arg0, char *arg1) {
    unsigned char st;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = st + 1;
        *(int *)(arg1 + 0x28) = 0;
        *(float *)(arg1 + 0x20) = 1.0f;
        *(unsigned char *)(arg0 + 7) = 1;
        *(short *)(arg1 + 0x56) = 0xF0;
        *(float *)(arg1 + 0x3C) = 2.5f;
        *(char *)(arg1 + 0x59) = 0;
        *(char *)(arg1 + 0x58) = 0;
        anim_clip_init((int)arg0, 0x17, 5.0f, 0.0f);
        func_001FBD50(arg0, 0x867, 0, 300.0f);
        break;
    case 1:
        *(short *)(arg1 + 0x56) = 0xF0;
        if (*(int *)arg1 & 0x1000) {
            *(char *)(arg0 + 4) = 1;
            *(char *)(arg0 + 5) = 0;
            *(char *)(arg0 + 6) = 0;
        }
        break;
    }
}
