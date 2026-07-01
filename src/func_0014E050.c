// NEARMISS func_0014E050  (vram 0x0014E050, 0x270 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.24% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Two stacked genuine compiler artifacts (body/structure fully recovered): (1) the documented case-0 dispatch branch-likely dup-add wall (identical class to func_0014A0D0 above and func_00131740's parked near-miss); (2) an FP-register-coloring artifact on the func_001B12B0(0.0f, cur, rate) call -- ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Per-tick state handler for actor arg0 (state byte at arg0+6) against companion
// record arg1. state 0: advance state, set arg1+0x20 = 0.5, play anim clip 0x14
// (rate 5.0). state 1: ease arg1+0x28 toward 0 via func_001B12B0 (rate pi/90); if
// it reaches 0 and arg1+0 has bit 0x1000: advance state, play clip 0x15 (rate 0),
// copy_qw4 a matrix from *(arg0+0x140)+0x90 into scratch D_700036A0, seed a
// translation vec4 at D_700038A0 (10,0,0,1), combine with D_700036A0 via
// func_001026A0 into D_700038A0, then run a 13-step loop building a rotation
// (D_700038B4 = deg*15 in radians) and calling func_001EFD90(3, D_700038A0,
// D_700038B0) each pass; finally fire event 0x861 via func_001FBD50. state 2: if
// arg1+0 has bit 0x1000, advance state, play clip 0x16 (rate 0). state 3: if
// arg1+0 has bit 0x1000, reset arg0 (clear 5/6), set arg1+0x20 = 1.0, and write
// the low byte-slice of func_00122BB8()>>19 into arg1+0x50 (stored twice by the
// target, likely a benign redundant store).
extern void anim_clip_init(int self, int clip, float a, float b);
extern void copy_qw4(void *dst, void *src);
extern int func_00122BB8(void);
extern void func_001026A0(void *a, void *b, void *c);
extern float func_001B12B0(float goal, float cur, float rate);
extern void func_001EFD90(int mode, void *a, void *b);
extern void func_001FBD50(char *p, int a, int b, float f);
extern char D_700036A0[];
extern char D_700038A0[];
extern char D_700038B0[];

void func_0014E050(char *arg0, char *arg1) {
    unsigned char state = *(unsigned char *)(arg0 + 6);
    float t;

    switch (state) {
    case 0:
        *(unsigned char *)(arg0 + 6) = state + 1;
        *(float *)(arg1 + 0x20) = 0.5f;
        anim_clip_init((int)arg0, 0x14, 5.0f, 0.0f);
        return;
    case 1:
        *(float *)(arg1 + 0x28) = func_001B12B0(0.0f, *(float *)(arg1 + 0x28), 0.0349065848f);
        if (*(float *)(arg1 + 0x28) == 0.0f && *(int *)(arg1 + 0) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            anim_clip_init((int)arg0, 0x15, 0.0f, 0.0f);
            copy_qw4(D_700036A0, (char *)*(int *)(arg0 + 0x140) + 0x90);
            *(float *)0x700038A0 = 10.0f;
            *(int *)0x700038A4 = 0;
            *(int *)0x700038A8 = 0;
            *(int *)0x700038AC = 0x3F800000;
            func_001026A0(D_700038A0, D_700036A0, D_700038A0);
            *(int *)0x700038B8 = 0;
            *(float *)0x700038B4 = 0.0f;
            *(int *)0x700038B0 = 0;
            t = 0.0f;
            *(int *)0x700038BC = 0x3F800000;
            do {
                *(float *)0x700038B4 = (3.1415927f * (15.0f * t)) / 180.0f;
                func_001EFD90(3, D_700038A0, D_700038B0);
                t += 1.0f;
            } while (t < 13.0f);
            func_001FBD50(arg0, 0x861, 0, 300.0f);
            return;
        }
        return;
    case 2:
        if (*(int *)(arg1 + 0) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = state + 1;
            anim_clip_init((int)arg0, 0x16, 0.0f, 0.0f);
            return;
        }
        return;
    case 3:
        if (*(int *)(arg1 + 0) & 0x1000) {
            short h;
            *(char *)(arg0 + 5) = 0;
            *(unsigned char *)(arg0 + 6) = 0;
            *(float *)(arg1 + 0x20) = 1.0f;
            h = (short)((func_00122BB8() >> 19) & 0xFF);
            *(short *)(arg1 + 0x50) = h;
            *(short *)(arg1 + 0x50) = h;
        }
        return;
    }
}
