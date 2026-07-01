// NEARMISS func_0014A730  (vram 0x0014A730, 0x264 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.84% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body/structure fully recovered (3-state switch/fallthrough dispatcher on arg0+6, anim_clip_init self-arg + trailing-0.0f pair, func_001FBD50 self-arg, D_00245BD0[][2] 4-byte-stride clip table). Residual is a register-allocation/tail-duplication scheduling artifact in the shared 'v1=6; if(f<=20.0f...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void anim_clip_init(char *self, int clip, float a, float b);
extern void func_001FBD50(char *p, int a, int b, float f);
extern int func_00122BB8(void);
extern short D_00245BD0[][2];

void func_0014A730(char *arg0, char *arg1) {
    unsigned char state;
    int v1;

    state = *(unsigned char *)(arg0 + 6);
    switch (state) {
    case 0:
        *(unsigned char *)(arg0 + 6) = state + 1;
        *(int *)(arg1 + 0x40) = 0x3F800000;
        *(char *)(arg1 + 0x88) = 0;
        *(char *)(arg1 + 0x83) = 0;
        *(int *)(arg1 + 0x50) = 0;
        *(int *)(arg1 + 0x4C) = 0;
        if (*(char *)(arg1 + 0x87) != 0) {
            anim_clip_init(arg0, 0x30, 1.0f, 0.0f);
        } else if (*(short *)(arg0 + 0x36) & 0x8000) {
            func_001FBD50(arg0, 0x7E9, 0, 300.0f);
            anim_clip_init(arg0, 0x2E, 1.0f, 0.0f);
        } else {
            *(char *)(arg0 + 0) = 1;
            *(short *)(arg0 + 0x36) = 0;
            *(char *)(arg1 + 0x85) = 0;
            func_001FBD50(arg0, 0x7E7, 0, 300.0f);
            anim_clip_init(arg0, D_00245BD0[(func_00122BB8() >> 0x11) % 3][0], 1.0f, 0.0f);
        }
        /* fallthrough */
    case 1:
        if (*(int *)(arg1 + 0x20) & 0x1000) {
            if (*(char *)(arg1 + 0x87) != 0) {
                *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
                anim_clip_init(arg0, 0x2D, 0.0f, 0.0f);
                return;
            }
            *(char *)(arg0 + 4) = 1;
            *(unsigned char *)(arg0 + 6) = 0;
            *(char *)(arg0 + 0) = 1;
            *(short *)(arg1 + 0x72) = 0;
            *(short *)(arg0 + 0x36) = 0;
            *(char *)(arg1 + 0x85) = 0x3C;
            v1 = 6;
            if (*(float *)(arg1 + 0x68) <= 20.0f) {
                v1 = 8;
            }
            *(char *)(arg0 + 5) = v1;
        }
        break;
    case 2:
        if (*(float *)(arg0 + 0x3C) <= 45.0f) {
            *(char *)(arg1 + 0x87) = 0;
        }
        if (*(int *)(arg1 + 0x20) & 0x1000) {
            *(char *)(arg0 + 4) = 1;
            *(unsigned char *)(arg0 + 6) = 0;
            *(char *)(arg1 + 0x87) = 0;
            *(char *)(arg0 + 0) = 1;
            *(short *)(arg1 + 0x72) = 0;
            *(short *)(arg0 + 0x36) = 0;
            *(char *)(arg1 + 0x85) = 0x3C;
            v1 = 6;
            if (*(float *)(arg1 + 0x68) <= 20.0f) {
                v1 = 8;
            }
            *(char *)(arg0 + 5) = v1;
        }
        break;
    }
}
