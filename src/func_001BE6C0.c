// NEARMISS func_001BE6C0  (vram 0x001BE6C0, 0x3F4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.29% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation-ORDER permutation (idiom exhaustion reached): after applying the 64-bit sign-extend idiom (long long cast unlocks the dsll32/dsra32 clean-store match) and matching the tail/other declaration order (fixes the s0/s1 swap), the SOLE residual is which physical temp registers (a0/a...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_001B1020(int a, int b, int c);
extern void func_001C6380(unsigned char *self);
extern void func_001C63D0(char *obj);
extern void func_001B17A0(char *p);
extern void func_001026A0(void *a, void *b, void *c);
extern void func_001026D0(void *a0, void *a1, void *a2);
extern void func_00102760(void *a0, void *a1);
extern void func_001028D0(void *a, void *b, void *c);
extern void func_00102948(void *dst, void *src);
extern void func_001029C0(void *a0);
extern void func_00102B08(void *obj, void *obj2, float v);
extern void copy_qw4(void *d, void *s);
extern void func_001EFD90(int a0, void *a1, void *a2);
extern void func_001FBD50(char *p, int a, int b, float f);
extern void func_001F4A00(void *a, void *b);
extern void func_001AFC10(unsigned char *handle);
extern int D_0028A524;
extern void *D_00275618;
extern float D_00810350[];
extern float D_70003610[];
extern float D_700036A0[];
extern float D_700036E0[];
extern float D_700038A0[];
extern float D_700038B0[];

void func_001BE6C0(unsigned char *self) {
    unsigned char *tail = self + 0x1F0;
    unsigned char *other = *(unsigned char **)(self + 0x20);

    switch (self[4]) {
    case 0:
        if (other[4] == 3) {
            self[4] = 3;
            return;
        }
        func_001B1020(4, -1, 0);
        *(int *)(self + 0x58) = D_0028A524;
        self[0x5D] = 0x81;
        self[0] = 1;
        *(void **)(self + 0x30) = &D_00275618;
        *(int *)(tail + 0x1C) = 0;
        return;
    case 1:
        if (other[4] >= 2) {
            self[4] = 2;
        }
        if (other[1] != 0) {
            int cnt = *(int *)(tail + 0x1C);
            if (cnt != 0) {
                *(int *)(tail + 0x1C) = cnt - 1;
            }
            {
                short h = *(short *)(self + 0x36);
                if (h != 0) {
                    if (other[0] == 1) {
                        if ((long long)h & 0x5000) {
                            if (*(int *)(tail + 0x1C) == 0 && *(int *)(other + 0x1F0 + 0x1C) == 0) {
                                *(int *)(tail + 0x1C) = 0x3C;
                                *(short *)(other + 0x36) = *(short *)(self + 0x36);
                                other[0] = 3;
                                func_001028D0(D_70003610, D_00810350, self + 0xB0);
                                func_00102760(D_70003610, D_70003610);
                                *(int *)0x7000361C = 0x3F800000;
                                func_001EFD90(0x80000076, self + 0x70, D_70003610);
                                func_001FBD50((char *)self, 0x15D, 0, 300.0f);
                            }
                        } else {
                            *(short *)(other + 0x36) = h;
                            other[0] = 3;
                            func_001028D0(D_70003610, D_00810350, self + 0xB0);
                            func_00102760(D_70003610, D_70003610);
                            *(int *)0x7000361C = 0x3F800000;
                            func_001EFD90(0x80000076, self + 0x70, D_70003610);
                            func_001FBD50((char *)self, 0x15D, 0, 300.0f);
                        }
                    }
                    *(short *)(self + 0x36) = 0;
                    self[0] = 1;
                }
            }
            if (self[0xD] == 1) {
                func_001026A0(self + 0xB0, other + 0xD0, self + 0xA0);
                func_001029C0(D_700036A0);
                func_00102B08(D_700036A0, D_700036A0, 0.5f);
                copy_qw4(D_700036E0, other + 0xD0);
                *(int *)0x70003710 = 0;
                *(int *)0x70003714 = 0;
                *(int *)0x70003718 = 0;
                *(int *)0x7000371C = 0;
                func_001026D0(self + 0xD0, D_700036E0, D_700036A0);
                func_00102948(self + 0x100, self + 0xB0);
                func_001C63D0((char *)self);
            } else {
                func_001C6380(self);
            }
            func_001B17A0((char *)self);
            (*(void (**)(unsigned char *))(self + 0x4C))(self);
            *(int *)0x700038A0 = 0;
            *(float *)0x700038A4 = 2.5f;
            *(int *)0x700038A8 = 0;
            *(float *)0x700038AC = 1.0f;
            func_001026A0(D_700038A0, self + 0xD0, D_700038A0);
            *(int *)0x700038B0 = 0x20;
            *(int *)0x700038B4 = 0x70;
            *(int *)0x700038B8 = 0x80;
            *(int *)0x700038BC = 0x80;
            func_001F4A00(D_700038A0, D_700038B0);
        }
        return;
    case 2:
    {
        float t = *(float *)(self + 0x64) - 0.05f;
        *(float *)(self + 0x64) = t;
        if (t <= 0.0f) {
            *(float *)(self + 0x64) = 0.0f;
            self[4] = 3;
        }
        func_001C6380(self);
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        return;
    }
    case 3:
        func_001AFC10(self);
        break;
    }
}
