// NEARMISS func_0016A8B0  (vram 0x0016A8B0, 0x394 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 94.06% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful.
// Wave 2026-10-27: the self+0xC0 / self+0x38 integrations are compound assignments (fixes the
// FP add operand order); anim_clip_arbiter returns int as in its definition.
// Remaining differences: in five of the eight anim_clip_arbiter calls of the switch the original
// loads self+0x28 after the 1.0f / clip-id setup (into the same scratch register as the 1.0f
// constant), and the case-1/2/3 clip-test branches then copy that constant setup into their delay
// slot; this compile always loads self+0x28 first. Tried: frame/clip/speed locals, int/short/float
// frame temps, struct field access, increment spellings, switch local, flipped tests, local
// declaration order, other compiler versions, a 9-minute permuter run (only dishonest
// assignment-in-argument hints).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern int anim_clip_arbiter(char *p, int clip, float speed, float f);
extern float func_0011DE90(float);
extern float func_001B1470(float);
extern void func_001029C0(void *a0);
extern void func_00102B08(void *a0, void *a1, float f12);
extern void func_00102BB0(void *a0, void *a1, float f12);
extern void func_00102918(void *a0, void *a1, char *a2);
extern void func_001026A0(void *a0, void *a1, void *a2);

extern float D_00248640[];
extern int D_700036A0;
extern int D_700038A0;

void func_0016A8B0(char *self) {
    if (*(short *)(self + 0x28) == 0) {
        if (*(int *)(self + 0x24C) == 0) {
            unsigned char lo = *(unsigned char *)(self + 0x25C);
            unsigned char hi = *(unsigned char *)(self + 0x23F);
            if (lo < hi) {
                *(unsigned char *)(self + 0x25C) = lo + 1;
            } else if (hi < lo) {
                *(unsigned char *)(self + 0x25C) = lo - 1;
            }
        } else {
            unsigned char v = *(unsigned char *)(self + 0x25C);
            if (v != 0) {
                *(unsigned char *)(self + 0x25C) = v - 1;
            }
        }
    }

    *(float *)(self + 0xC0) +=
        D_00248640[*(unsigned char *)(self + 0x25C)] * func_0011DE90(*(float *)(self + 0x38));
    *(float *)(self + 0xC0) = func_001B1470(*(float *)(self + 0xC0));
    *(float *)(self + 0x38) += 0.06283186f;
    *(float *)(self + 0x38) = func_001B1470(*(float *)(self + 0x38));

    func_001029C0(&D_700036A0);
    func_00102B08(&D_700036A0, &D_700036A0, *(float *)(self + 0xC0));
    func_00102BB0(&D_700036A0, &D_700036A0, *(float *)(self + 0xC4));
    func_00102918(&D_700036A0, &D_700036A0, self + 0x290);

    *(volatile int *)0x700038A0 = 0;
    *(volatile float *)0x700038A4 = -21.0f;
    *(volatile int *)0x700038A8 = 0;
    *(volatile float *)0x700038AC = 1.0f;
    func_001026A0(self + 0xB0, &D_700036A0, &D_700038A0);

    {
        switch (*(unsigned short *)(self + 0x2E)) {
        case 0:
            if (*(unsigned char *)(self + 0x25C) == 3) {
                anim_clip_arbiter(self, 0xDA, 1.0f, (float) *(short *)(self + 0x28));
            } else {
                anim_clip_arbiter(self, 0xD8, 1.0f, (float) *(short *)(self + 0x28));
            }
            if (*(short *)(self + 0x28) >= 0x18) {
                *(unsigned short *)(self + 0x2E) = *(unsigned short *)(self + 0x2E) + 1;
            } else {
                *(short *)(self + 0x28) = *(short *)(self + 0x28) + 1;
            }
            break;
        case 1:
            if (*(unsigned char *)(self + 0x25C) == 3) {
                anim_clip_arbiter(self, 0xDA, 1.0f, (float) *(short *)(self + 0x28));
            } else {
                anim_clip_arbiter(self, 0xD8, 1.0f, (float) *(short *)(self + 0x28));
            }
            if (*(short *)(self + 0x28) == 0) {
                *(unsigned short *)(self + 0x2E) = *(unsigned short *)(self + 0x2E) + 1;
            } else {
                *(short *)(self + 0x28) = *(short *)(self + 0x28) - 1;
            }
            break;
        case 2:
            if (*(unsigned char *)(self + 0x25C) == 3) {
                anim_clip_arbiter(self, 0xDB, 1.0f, (float) *(short *)(self + 0x28));
            } else {
                anim_clip_arbiter(self, 0xD9, 1.0f, (float) *(short *)(self + 0x28));
            }
            if (*(short *)(self + 0x28) >= 0x18) {
                *(unsigned short *)(self + 0x2E) = *(unsigned short *)(self + 0x2E) + 1;
            } else {
                *(short *)(self + 0x28) = *(short *)(self + 0x28) + 1;
            }
            break;
        case 3:
            if (*(unsigned char *)(self + 0x25C) == 3) {
                anim_clip_arbiter(self, 0xDB, 1.0f, (float) *(short *)(self + 0x28));
            } else {
                anim_clip_arbiter(self, 0xD9, 1.0f, (float) *(short *)(self + 0x28));
            }
            if (*(short *)(self + 0x28) == 0) {
                *(unsigned short *)(self + 0x2E) = 0;
            } else {
                *(short *)(self + 0x28) = *(short *)(self + 0x28) - 1;
            }
            break;
        }
    }
}
