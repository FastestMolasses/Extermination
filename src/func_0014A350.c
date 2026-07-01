// NEARMISS func_0014A350  (vram 0x0014A350, 0x2C4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.06% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Idiom-13b dead-const re-materialization: two dispatch-chain constants (the case-2/3 `addiu v0,a1,1` state-advance and the case-3-entry `li v0,0x64` for e[0x85]) are speculated into branch-likely (beqzl/beql) delay slots and then genuinely CSE-reused at the branch-likely target label in mwcc-233's...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS (mwcc 2.3.3, 97.06% objdiff; 991202 86.5%). 5-state actor tick keyed
// on p[6] (arg0=p, arg1=e). State 0 seeds e's anim/timer fields and starts
// anim_clip_init(p,0x32,5,0). State 1 gates on e+0x20 & 0x1000, then seeds a
// e+0x28 countdown via func_00122BB8 and starts clip 0x33 + SFX 0x7EC. State 2
// counts e+0x28 down to 0, then advances to state 3 (func_001EFE00 0x8000002B,
// clip 0x34, SFX 0x7ED). State 3 (reached directly OR via state-2's advance)
// sets e+0x85=0x64; when e+0x20 & 0x1000 and p+0x60==1.15 advances to state 4
// (clip 0x35, SFX 0x7EE); always rebuilds a scratch vec at 0x700038A0
// (0.001,0.001,0.001,0) and calls func_001028B8(p+0x60,...,p+0x60), clamping
// p+0x60..0x6C to (1.15,1.15,1.15,1.0) once p+0x60 exceeds 1.15. State 4 sets
// e+0x85=0x64 and, when e+0x20 & 0x1000, resets p[5]/p[6], sets p[0xD]|=0x80,
// picks a p+0x34 timer from D_0081070A (0x190 or 0xFA), and fires SFX 0x7EF.
extern void anim_clip_init(char *self, int clip, float a, float b);
extern int func_00122BB8(void);
extern void func_001EFE00(int mode, char *p);
extern void func_001FBD50(char *p, int a, int b, float f);
extern void func_001028B8(void *a, void *b, void *c);

extern unsigned char D_0081070A;
extern char D_700038A0[];

void func_0014A350(unsigned char *p, unsigned char *e) {
    int v;

    switch (p[6]) {
    case 0:
        p[6] = p[6] + 1;
        *(int *)(e + 0x40) = 0x3F800000;
        e[0x80] = 0;
        *(int *)(e + 0x50) = 0;
        *(int *)(e + 0x4C) = 0;
        anim_clip_init((char *)p, 0x32, 5.0f, 0.0f);
        return;
    case 1:
        if (*(int *)(e + 0x20) & 0x1000) {
            p[6] = p[6] + 1;
            *(int *)(e + 0x28) = (int) (((func_00122BB8() >> 11) & 0xFF) + 0x3C);
            anim_clip_init((char *)p, 0x33, 0.0f, 0.0f);
            func_001FBD50((char *)p, 0x7EC, 0, 300.0f);
        }
        return;
    case 2:
        v = *(int *)(e + 0x28);
        if (v != 0) {
            *(int *)(e + 0x28) = (int) (v - 1);
            return;
        }
        p[6] = p[6] + 1;
        e[0x85] = 0x64;
        func_001EFE00(0x8000002B, (char *)p);
        anim_clip_init((char *)p, 0x34, 5.0f, 0.0f);
        func_001FBD50((char *)p, 0x7ED, 0, 300.0f);
        return;
    case 3:
        e[0x85] = 0x64;
        if (*(int *)(e + 0x20) & 0x1000) {
            if (*(float *)(p + 0x60) == 1.15f) {
                p[6] = p[6] + 1;
                anim_clip_init((char *)p, 0x35, 0.0f, 0.0f);
                func_001FBD50((char *)p, 0x7EE, 0, 300.0f);
            }
        }
        *(float *)0x700038A0 = 0.001f;
        *(float *)0x700038A4 = 0.001f;
        *(float *)0x700038A8 = 0.001f;
        *(int *)0x700038AC = 0;
        func_001028B8(p + 0x60, D_700038A0, p + 0x60);
        if (*(float *)(p + 0x60) <= 1.15f) {
            return;
        }
        *(float *)(p + 0x60) = 1.15f;
        *(float *)(p + 0x64) = 1.15f;
        *(float *)(p + 0x68) = 1.15f;
        *(float *)(p + 0x6C) = 1.0f;
        return;
    case 4:
        e[0x85] = 0x64;
        if (*(int *)(e + 0x20) & 0x1000) {
            p[5] = 0;
            p[6] = 0;
            p[0xD] = (unsigned char) (p[0xD] | 0x80);
            if (D_0081070A != 0) {
                *(short *)(p + 0x34) = 0x190;
            } else {
                *(short *)(p + 0x34) = 0xFA;
            }
            func_001FBD50((char *)p, 0x7EF, 0, 300.0f);
        }
        return;
    }
}
