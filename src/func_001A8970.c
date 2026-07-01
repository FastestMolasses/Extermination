// NEARMISS func_001A8970  (vram 0x001A8970, 0x26C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.23% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Pure FP saved-register coloring: target keeps the f21 running total (proximity guard's half-extent sum) in one callee-saved float slot (fs0f), mwcc233 colors it into a different callee-saved slot (fs1) across the func_0011DF78 call -- same instruction sequence, plus one downstream branch-offset k...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Melee/proximity attack commit for an AI actor (self = arg0, opponent = arg1).
// Three early-out proximity gates against opponent's hitbox-extent table at
// arg1+0x30 (a float[3]: [0]=x-radius, [1]=y-radius/height term, [2]=z-radius):
// |self.B0 - opp.B0| <= opp_extent[0]; |self.B8 - opp.B8| <= opp_extent[2]; and
// |self.A4 + self_extent[1]/2 - opp.B4| <= self_extent[1]/2 + opp_extent[1],
// where self_extent is self->0x30. On passing all three it invokes the
// opponent's vtable hit-reaction function at arg1+0x34 with (arg1, self,
// self+0xB0). If the opponent's resulting state byte (arg1+0xD) is 5 (a hard
// knockdown/stagger reaction), self commits to a scripted follow-up: state
// byte 0xF=0xA, poses 0x224 from the current 0x220 blend value, driver state
// byte 0=3 (attack-commit), and stages+plays a transform via func_001028D0 /
// func_00102760 into self+0x70. Otherwise, only if self is currently in state
// 1 (idle/tracking) does it consider a follow-up: gated on either the
// opponent reaction NOT being a stagger-class value (a1-6 outside [0,2), i.e.
// not 6/7/8) OR a leftover/uninitialized float register comparison
// (f20 <= 0.8f*f20) inherited from the caller's register file -- a latent
// quirk of the original binary reproduced verbatim (the local is never
// written on this path in the disassembly, so this is preserved as literal
// undefined-behavior register content, not a bug in this decompilation).
// When taken, and if the opponent's reaction is exactly 0xE and
// func_0021BD10(self's driver byte, 0xE) approves, self's state byte 0xF is
// set to 2 (alternate stagger response). It then selects a per-region blend
// table (D_0024A7C0 or D_0024A800, chosen by the global D_0081070A toggle)
// and reads the opponent's reaction byte again to pick a pose blend value
// into self+0x224 (and also 0x22C when the reaction is 9 or in [0xC,0xE)),
// commits driver state 3, and plays the same func_001028D0/func_00102760
// transform sequence into self+0x70. Both commit paths end by clearing the
// pending-hit flag at the raw scratch address 0x70003B86.

extern void func_00102760(void *a, void *b);
extern void func_001028D0(void *a, void *b, void *c);
extern float func_0011DF78(float x);
extern int func_0021BD10(unsigned char a, unsigned char b);

extern float D_0024A7C0;
extern float D_0024A800;
extern unsigned char D_0081070A;
extern int D_700038A0;

void func_001A8970(char *arg0, char *arg1) {
    float f20;

    if (!(func_0011DF78(*(float *)(arg0 + 0xB0) - *(float *)(arg1 + 0xB0)) <= (*(float **)(arg1 + 0x30))[0])) {
        return;
    }
    if (!(func_0011DF78(*(float *)(arg0 + 0xB8) - *(float *)(arg1 + 0xB8)) <= (*(float **)(arg1 + 0x30))[2])) {
        return;
    }
    {
        float half = (*(float **)(arg0 + 0x30))[1] / 2.0f;
        float f21 = half + (*(float **)(arg1 + 0x30))[1];
        if (!(func_0011DF78((*(float *)(arg0 + 0xA4) + half) - *(float *)(arg1 + 0xB4)) <= f21)) {
            return;
        }
    }
    (*(void (**)(char *, char *, char *))(arg1 + 0x34))(arg1, arg0, arg0 + 0xB0);

    if (*(unsigned char *)(arg1 + 0xD) == 5) {
        *(unsigned char *)(arg0 + 0xF) = 0xA;
        *(float *)(arg0 + 0x224) = *(float *)(arg0 + 0x220);
        *(unsigned char *)(arg0 + 0) = 3;
        func_001028D0(&D_700038A0, arg0 + 0xA0, arg1 + 0xB0);
        *(int *)0x700038AC = 0x3F800000;
        func_00102760(arg0 + 0x70, &D_700038A0);
        *(short *)0x70003B86 = 0;
        return;
    }

    if (*(unsigned char *)(arg0 + 0) != 1) {
        return;
    }

    {
        int a1 = *(unsigned char *)(arg1 + 0xD);
        if ((unsigned int)(a1 - 6) >= 2U || f20 <= 0.8f * f20) {
            float *v1;
            unsigned char a0;

            if (a1 == 0xE && func_0021BD10(*(unsigned char *)(arg0 + 0), a1) != 0) {
                *(unsigned char *)(arg0 + 0xF) = 2;
            }
            if (D_0081070A == 0) {
                v1 = &D_0024A7C0;
            } else {
                v1 = &D_0024A800;
            }
            a0 = *(unsigned char *)(arg1 + 0xD);
            if (a0 == 9 || (unsigned int)(a0 - 0xC) < 2U) {
                *(float *)(arg0 + 0x22C) = v1[a0];
                *(float *)(arg0 + 0x224) = v1[*(unsigned char *)(arg1 + 0xD)];
            } else {
                *(float *)(arg0 + 0x224) = v1[a0];
            }
            *(unsigned char *)(arg0 + 0) = 3;
            func_001028D0(&D_700038A0, arg0 + 0xA0, arg1 + 0xB0);
            *(int *)0x700038AC = 0x3F800000;
            func_00102760(arg0 + 0x70, &D_700038A0);
            *(short *)0x70003B86 = 0;
        }
    }
}
