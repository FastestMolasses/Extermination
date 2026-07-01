// NEARMISS func_00138900  (vram 0x00138900, 0x314 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.54% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Two genuine residual instructions (anim_clip_init(arg0,4,0.0f,0.0f) call in state 0): target materializes the trailing 0.0f into f13 first then mov.s into f12 (mtc1 zero,fa0f / mov.s fa0,fa0f), mwcc233 always materializes into f12 first then copies to f13 regardless of z=0.0f temp / volatile / as...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void anim_clip_init(unsigned char *, int, float, float);
extern void func_0013BA20(unsigned char *, unsigned char *);
extern void func_001FBD50(unsigned char *, int, int, float);
extern float func_001B12B0(float, float, float);
extern float func_001B15D0(unsigned char *, float *);

extern unsigned char D_008106C7;
extern float D_00810360;

void func_00138900(unsigned char *arg0, unsigned char *arg1) {
    unsigned char st;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        if (*(unsigned short *)(arg1 + 0x20) != 0) {
            *(unsigned short *)(arg1 + 0x20) = *(unsigned short *)(arg1 + 0x20) - 1;
        } else if (!(*(unsigned char *)(arg0 + 0xD) & 1)) {
            *(unsigned char *)(arg0 + 6) = 4;
        } else {
            *(unsigned char *)(arg0 + 6) = st + 1;
            if (*(float *)(arg1 + 0x44) > 0.0f) {
                *(int *)(arg1 + 0x48) = 0xBC23D70A;
            } else {
                *(float *)(arg1 + 0x44) = 0.0f;
            }
            *(float *)(arg1 + 0x5C) = 1.5707964f;
            anim_clip_init(arg0, 4, 0.0f, 0.0f);
        }
        break;
    case 1:
        if (*(float *)(arg1 + 0x44) <= 0.0f) {
            *(int *)(arg1 + 0x48) = 0;
            *(float *)(arg1 + 0x44) = 0.0f;
        }
        if (*(signed char *)(arg1 + 0x81) & 4) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            *(unsigned short *)(arg1 + 0x20) = 0;
            *(float *)(arg1 + 0x50) = 0.0f;
            *(float *)(arg1 + 0x5C) = 0.0f;
            *(float *)(arg1 + 0x44) = 0.0f;
            *(int *)(arg1 + 0x48) = 0;
            *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) - 2.5f;
            anim_clip_init(arg0, 2, 0.0f, 0.0f);
        }
        break;
    case 2:
        if (D_008106C7 != 0 && func_001B15D0(arg0 + 0xB0, &D_00810360) <= 150.0f) {
            *(unsigned short *)(arg1 + 0x20) = *(unsigned short *)(arg1 + 0x20) + 1;
        } else {
            *(unsigned short *)(arg1 + 0x20) = 0;
        }
        if ((*(unsigned char *)(arg0 + 0xA) & 1) || *(short *)(arg1 + 0x20) > 0) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            *(unsigned short *)(arg1 + 0x20) = 0;
            anim_clip_init(arg0, 3, 0.0f, 0.0f);
            func_001FBD50(arg0, 0x816, 0, 300.0f);
        }
        break;
    case 3:
        if (*(float *)(arg0 + 0x3C) <= 25.0f) {
            *(float *)(arg1 + 0x5C) = -1.5707964f;
        }
        if (*(int *)(arg1 + 0x70) & 0x1000) {
            *(unsigned char *)(arg0 + 5) = 2;
            *(unsigned char *)(arg0 + 6) = 0;
            *(float *)(arg1 + 0x5C) = 0.0f;
            *(short *)(arg1 + 0x30) = 0;
            *(float *)(arg1 + 0x44) = 0.4f;
            *(short *)(arg1 + 0x22) = 0;
            *(unsigned short *)(arg1 + 0x20) = 0;
            anim_clip_init(arg0, 1, 0.0f, 0.0f);
        }
        break;
    case 4:
        *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
        *(unsigned char *)(arg0 + 6) = 0;
        *(float *)(arg1 + 0x5C) = 0.0f;
        *(short *)(arg1 + 0x30) = 0;
        *(float *)(arg1 + 0x44) = 0.4f;
        *(short *)(arg1 + 0x24) = 0;
        *(short *)(arg1 + 0x22) = 0;
        *(unsigned short *)(arg1 + 0x20) = 0;
        anim_clip_init(arg0, 1, 0.0f, 0.0f);
        break;
    }
    *(float *)(arg1 + 0x50) = func_001B12B0(*(float *)(arg1 + 0x5C), *(float *)(arg1 + 0x50), 0.026179939f);
    func_0013BA20(arg0, arg1);
}
