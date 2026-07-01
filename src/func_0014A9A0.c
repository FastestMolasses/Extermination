// NEARMISS func_0014A9A0  (vram 0x0014A9A0, 0x318 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.92% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Two residual instructions: the anim_clip_init(arg0,0x2B,0.0f,0.0f) call at the end of state 0's true-branch has the same f12/f13 materialization-order artifact documented for func_00138900 (mtc1 zero,fa0f / mov.s fa0,fa0f in target vs mtc1 zero,fa0 / mov.s fa1,fa0 in ours); z-temp/volatile/assign...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

extern void anim_clip_init(unsigned char *, int, float, float);
extern void func_001FBD50(unsigned char *, int, int, float);
extern int func_00122BB8(void);
extern float func_0011E620(float, float);
extern float func_001B1470(float);
extern float func_0011DF78(float);

extern int D_00275B40;

void func_0014A9A0(unsigned char *arg0, unsigned char *arg1) {
    unsigned char st;
    float ang;
    int p;
    int v;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = st + 1;
        *(float *)(arg1 + 0x40) = 1.0f;
        *(signed char *)(arg1 + 0x80) = -0x80;
        *(unsigned char *)(arg1 + 0x88) = 0;
        *(unsigned char *)(arg1 + 0x83) = 0;
        *(float *)(arg1 + 0x48) = 1.5f;
        *(int *)(arg1 + 0x50) = 0;
        func_001FBD50(arg0, 0x7E7, 0, 300.0f);
        *(float *)(arg1 + 0x44) = 0.13962634f * ((float)((func_00122BB8() >> 19) & 0xFF) / 255.0f);
        if ((func_00122BB8() >> 12) & 1) {
            float negone = -1.0f;
            *(float *)(arg1 + 0x44) = *(float *)(arg1 + 0x44) * negone;
        }
        ang = func_001B1470(func_0011E620(*(float *)(arg0 + 0x70), *(float *)(arg0 + 0x78)) - *(float *)(arg0 + 0xC4));
        *(volatile float *)0x70003A20 = ang;
        if (!(func_0011DF78(ang) < 1.5707964f)) {
            *(float *)(arg1 + 0x4C) = -1.0f;
            *(float *)(arg0 + 0xC4) = func_001B1470(*(float *)(arg0 + 0xC4) + *(volatile float *)0x70003A20 + 3.1415927f);
            anim_clip_init(arg0, 0x2A, 0.0f, 0.0f);
        } else {
            *(float *)(arg1 + 0x4C) = 1.0f;
            *(float *)(arg0 + 0xC4) = func_001B1470(*(float *)(arg0 + 0xC4) + *(volatile float *)0x70003A20);
            anim_clip_init(arg0, 0x2B, 0.0f, 0.0f);
        }
        break;
    case 1:
        v = D_00275B40;
        *(float *)(*(int *)((p = D_00275B40) + 4) + 0x74) = func_001B1470(*(float *)(*(int *)(v + 4) + 0x74) + *(float *)(arg1 + 0x44));
        if (*(signed char *)(arg1 + 0x81) & 2) {
            *(int *)(arg1 + 0x4C) = 0;
            if (*(int *)(arg1 + 0x20) & 0x1000) {
                int q;
                int q2;
                *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
                *(unsigned char *)(arg1 + 0x87) = 1;
                func_001FBD50(arg0, 0x7E8, 0, 300.0f);
                q = D_00275B40;
                *(float *)(arg0 + 0xC4) = func_001B1470(*(float *)(arg0 + 0xC4) + *(float *)(*(int *)(q + 4) + 0x74));
                q2 = D_00275B40;
                *(float *)(*(int *)(q2 + 4) + 0x74) = 0.0f;
                if (*(short *)(arg0 + 0x34) != 0) {
                    anim_clip_init(arg0, 0x2D, 1.0f, 0.0f);
                } else {
                    *(unsigned char *)(arg0 + 5) = 2;
                    *(unsigned char *)(arg0 + 6) = 0;
                }
            }
        }
        break;
    case 2:
        if (*(int *)(arg1 + 0x20) & 0x1000) {
            *(unsigned char *)(arg0 + 4) = 1;
            *(unsigned char *)(arg0 + 6) = 0;
            *(unsigned char *)(arg1 + 0x87) = 0;
            *(unsigned char *)(arg1 + 0x80) = 0;
            *(unsigned char *)(arg0 + 0) = 1;
            *(short *)(arg1 + 0x72) = 0;
            *(short *)(arg0 + 0x36) = 0;
            *(unsigned char *)(arg1 + 0x85) = 0x3C;
            if (*(float *)(arg1 + 0x68) <= 20.0f) {
                *(unsigned char *)(arg0 + 5) = 8;
            } else {
                *(unsigned char *)(arg0 + 5) = 6;
            }
        }
        break;
    }
}
