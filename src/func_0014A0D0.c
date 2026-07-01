// NEARMISS func_0014A0D0  (vram 0x0014A0D0, 0x274 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 93.66% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Two stacked genuine compiler artifacts (body/structure fully recovered, byte-verified equivalent logic): (1) the documented case-0 dispatch branch-likely dup-add wall (target's beql delay slot computes v0=a1+1 once and reuses it; mwcc233 emits the same add a second time after the branch target la...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Per-tick state handler for actor arg0 (state byte at arg0+6) against companion
// record arg1. state 0: advance state, reset arg1's clip fields (0x28, 0x40=2.0,
// 0x83=0, 0x88=1, 0x50=0, 0x4C=0), play anim clip 0x10 (rate 5.0). state 1: if
// arg1+0x20 has bit 0x1000, advance state, set arg1+0x40 = 1.0, play clip 0x11
// (rate 0). state 2: if arg0+0x3C == 7.0 fire event 0x7E2; ease arg1+0x44 toward
// D_00810360/D_00810368 via func_001B1240/func_001B12B0 into arg0+0xC4; if
// arg1+0x28 == 0 and func_0021BE40/func_001A7B80 gate passes, latch arg1+0x28 = 1,
// set D_008102B0 bit 2, pick D_008104D4 from arg0+0xD bit 0x80 (20.0 or 30.0),
// seed the scratch delta D_00810320/0x324/0x328 from D_00810360/0x368 minus
// arg1+0xB0/0xB8, normalize via func_00102760, and call func_001B55E0(arg1, 0).
// If arg1+0x20 has bit 0x1000, advance state and play clip 0x14 (rate 0).
// state 3: if arg1+0x20 has bit 0x1000, reset arg0 (clear 5/6), clear arg1+0x88,
// reset arg1+0x40 = 1.0.
extern void anim_clip_init(int self, int clip, float a, float b);
extern void func_00102760(void *a, void *b);
extern int func_001A7B80(char *p);
extern float func_001B1240(void *obj, float px, float py);
extern float func_001B12B0(float goal, float cur, float rate);
extern void func_001B55E0(char *p, int a);
extern void func_001FBD50(char *p, int a, int b, float f);
extern int func_0021BE40(void *a, char *p);
extern unsigned char D_008102B0;
extern float D_00810320;
extern int D_00810324;
extern float D_00810328;
extern float D_00810360;
extern float D_00810368;
extern int D_008104D4;

void func_0014A0D0(char *arg0, char *arg1) {
    unsigned char state = *(unsigned char *)(arg0 + 6);

    switch (state) {
    case 0:
        *(unsigned char *)(arg0 + 6) = state + 1;
        *(int *)(arg1 + 0x28) = 0;
        *(float *)(arg1 + 0x40) = 2.0f;
        *(char *)(arg1 + 0x83) = 0;
        *(char *)(arg1 + 0x88) = 1;
        *(int *)(arg1 + 0x50) = 0;
        *(int *)(arg1 + 0x4C) = 0;
        anim_clip_init((int)arg0, 0x10, 5.0f, 0.0f);
        return;
    case 1:
        if (*(int *)(arg1 + 0x20) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = state + 1;
            *(float *)(arg1 + 0x40) = 1.0f;
            anim_clip_init((int)arg0, 0x11, 0.0f, 0.0f);
            return;
        }
        return;
    case 2:
        if (*(float *)(arg0 + 0x3C) == 7.0f) {
            func_001FBD50(arg0, 0x7E2, 0, 300.0f);
        }
        *(float *)(arg1 + 0x44) = func_001B1240(arg0 + 0xB0, D_00810360, D_00810368);
        *(float *)(arg0 + 0xC4) = func_001B12B0(*(float *)(arg1 + 0x44), *(float *)(arg0 + 0xC4), 0.0698131695f);
        if (*(int *)(arg1 + 0x28) == 0 && func_0021BE40(&D_008102B0, arg0) == 0 && func_001A7B80(arg0) != 0) {
            *(int *)(arg1 + 0x28) = 1;
            D_008102B0 |= 2;
            if (*(unsigned char *)(arg0 + 0xD) & 0x80) {
                D_008104D4 = 0x41F00000;
            } else {
                D_008104D4 = 0x41A00000;
            }
            D_00810320 = D_00810360 - *(float *)(arg0 + 0xB0);
            D_00810324 = 0;
            D_00810328 = D_00810368 - *(float *)(arg0 + 0xB8);
            func_00102760(&D_00810320, &D_00810320);
            func_001B55E0(arg0, 0);
        }
        if (*(int *)(arg1 + 0x20) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            anim_clip_init((int)arg0, 0x14, 0.0f, 0.0f);
            return;
        }
        break;
    case 3:
        if (*(int *)(arg1 + 0x20) & 0x1000) {
            *(char *)(arg0 + 5) = 0;
            *(unsigned char *)(arg0 + 6) = 0;
            *(char *)(arg1 + 0x88) = 0;
            *(float *)(arg1 + 0x40) = 1.0f;
        }
        break;
    }
}
