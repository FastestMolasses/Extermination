// NEARMISS func_001348E0  (vram 0x001348E0, 0x4D8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.51% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// See detailed wall description above (dead-block artifact + FPU register-pairing permutation).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

extern int func_00122BB8(void);
extern int func_001B1380(void *a, void *b, float c);
extern void anim_clip_init(int self, int clip, float a, float b);
extern void copy_qw4(void *dst, void *src);
extern void func_001026A0(void *dst, void *mat, void *src);
extern void func_001B3C30(int self, void *a, void *b, float fa, float fb);
extern void func_001FBD50(int self, int a, int b, float c);
extern float func_001B1470(float a);

extern int D_00275B40;
extern float D_00810360[16];
extern unsigned char D_0081070A[64];
extern char D_700036A0[];
extern char D_700038A0[];
extern char D_700038B0[];

void func_001348E0(unsigned char *p, unsigned char *e) {
    unsigned char st = p[6];

    switch (st) {
    case 0:
        if ((*(signed char *)(e + 0x5B) & 3) == 0) {
            p[6] = 3;
            return;
        }
        p[6] = st + 1;
        *(unsigned char *)(e + 0x5D) = 1;
        *(int *)(e + 0x2C) = 0;
        *(int *)(e + 0x30) = 0;
        anim_clip_init((int)p, 0x11, 5.0f, 0.0f);
        return;
    case 1:
        if ((*(int *)e & 0x1000) == 0) {
            return;
        }
        p[6] = st + 1;
        *(int *)(e + 4) = (func_00122BB8() >> 13) & 7;
        *(int *)(e + 8) = (func_00122BB8() >> 8) & 7;
        *(int *)(e + 0xC) = (func_00122BB8() >> 13) & 1;
        anim_clip_init((int)p, 0x12, 0.0f, 0.0f);
        if (func_001B1380(D_00810360, p + 0xB0, *(float *)(p + 0xC4)) != 0) {
            *(float *)(e + 0x3C) = 0.017453292f;
        } else {
            *(float *)(e + 0x3C) = -0.017453292f;
        }
        return;
    case 2:
        if (*(signed char *)(e + 0x5B) & 1) {
            if (*(int *)(e + 4) != 0) {
                *(int *)(e + 4) = *(int *)(e + 4) - 1;
            } else {
                *(int *)(e + 4) = 10;
                copy_qw4(D_700036A0, (void *)(*(int *)(D_00275B40 + 0xAC) + 0x90));
                *(float *)0x700038A0 = 15.0f;
                *(float *)0x700038A4 = 0.0f;
                *(float *)0x700038A8 = 0.0f;
                *(float *)0x700038AC = 1.0f;
                func_001026A0(D_700038A0, D_700036A0, D_700038A0);
                {
                    float f3 = *(float *)0x700038A0;
                    float f2 = *(float *)0x700038A4;
                    float f1 = *(float *)0x700038A8;
                    float f0 = *(float *)0x700038AC;
                    *(float *)0x700036D0 = f3;
                    *(float *)0x700036D4 = f2;
                    *(float *)0x700036D8 = f1;
                    *(float *)0x700036DC = f0;
                }
                *(float *)0x700038B0 = 0.8f;
                *(float *)0x700038B4 = 0.8f;
                *(float *)0x700038B8 = 0.8f;
                *(float *)0x700038BC = 1.0f;
                if (D_0081070A[0] != 0) {
                    func_001B3C30((int)p, D_700036A0, D_700038B0, 200.0f, 20.0f);
                } else {
                    func_001B3C30((int)p, D_700036A0, D_700038B0, 200.0f, 15.0f);
                }
                func_001FBD50((int)p, 0x164, 0, 300.0f);
            }
        }
        if (*(signed char *)(e + 0x5B) & 2) {
            if (*(int *)(e + 8) != 0) {
                *(int *)(e + 8) = *(int *)(e + 8) - 1;
            } else {
                *(int *)(e + 8) = 10;
                copy_qw4(D_700036A0, (void *)(*(int *)(D_00275B40 + 0x84) + 0x90));
                *(float *)0x700038A0 = 13.0f;
                *(float *)0x700038A4 = -0.5f;
                *(float *)0x700038A8 = 0.0f;
                *(float *)0x700038AC = 1.0f;
                func_001026A0(D_700038A0, D_700036A0, D_700038A0);
                {
                    float f3 = *(float *)0x700038A0;
                    float f2 = *(float *)0x700038A4;
                    float f1 = *(float *)0x700038A8;
                    float f0 = *(float *)0x700038AC;
                    *(float *)0x700036D0 = f3;
                    *(float *)0x700036D4 = f2;
                    *(float *)0x700036D8 = f1;
                    *(float *)0x700036DC = f0;
                }
                *(float *)0x700038B0 = 0.8f;
                *(float *)0x700038B4 = 0.8f;
                *(float *)0x700038B8 = 0.8f;
                *(float *)0x700038BC = 1.0f;
                if (D_0081070A[0] != 0) {
                    func_001B3C30((int)p, D_700036A0, D_700038B0, 200.0f, 20.0f);
                } else {
                    func_001B3C30((int)p, D_700036A0, D_700038B0, 200.0f, 15.0f);
                }
                func_001FBD50((int)p, 0x164, 0, 300.0f);
            }
        }
        if (*(int *)e & 0x1000) {
            if (*(int *)(e + 0xC) != 0) {
                *(int *)(e + 0xC) = *(int *)(e + 0xC) - 1;
                anim_clip_init((int)p, 0x12, 0.0f, 0.0f);
            } else {
                p[6] = p[6] + 1;
                anim_clip_init((int)p, 0x13, 0.0f, 0.0f);
            }
        }
        *(float *)(p + 0xC4) =
            func_001B1470(*(float *)(p + 0xC4) + *(float *)(e + 0x3C));
        return;
    case 3:
        if ((*(int *)e & 0x1000) == 0) {
            return;
        }
        p[5] = 0;
        p[6] = 0;
        *(unsigned char *)(e + 0x5D) = 0;
        *(short *)(e + 0x50) = (short)((func_00122BB8() >> 11) & 0x1F);
        return;
    }
}
