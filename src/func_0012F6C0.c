// NEARMISS func_0012F6C0  (vram 0x0012F6C0, 0x2BC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.90% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// NOT A WALL — 100.0 IS VERIFIED, gated on a config change I am not permitted to make (matched=false to avoid overclaiming). Sole residual was the scratchpad address-lui peel: mwcc speculates `lui at,(0x700038A0>>16)` — the address-hi of case 9's first store — into the `beq v1,a1` inner-dispatch de...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// REQUIRES the scratchpad-symbol opt-in to be extended to the 0x700038A0 quad:
//   tools/decomp/build.py  _SPAD_SYMS += "0x700038A0","0x700038A4","0x700038A8","0x700038AC"
//   config/SCUS_971.12.lcf  D_700038A0 = 0x700038A0;  (and A4/A8/AC)
// Without it objdiff reports 99.897% (reloc-vs-literal spelling only); the LINKED
// bytes are identical either way. CAUTION: 279 src files already mention
// D_700038A0 as `extern char D_700038A0[]` for address-taken uses, so the opt-in
// predicate MUST become per-file explicit before enabling this address globally
// (this is the s86 "TESTED AND REJECTED" failure — it failed because only the .s
// was symbolized; symbolizing the C declaration too is what makes it match).
//
// SEMANTICS: 3-state actor tick keyed on p[6] (arg0=p actor, arg1=e brain).
// State 0 seeds e's anim fields (e+0x63, e+0x54, e+0x34) and starts
// anim_clip_init(p,8,10,0). State 1 gates on the e+0x58 0x8000 flag then advances.
// State 2 dispatches on p+0x2C (short): case 8 starts clip 9 when e+0x58 & 0x7000;
// case 9 builds the scratch vector (0,3,45,1) at D_700038A0, calls
// func_00131F20/func_001028B8, then probes func_001B3250/func_001B1560/
// func_001B13F0 to decide whether to restart clip 10; case 10 resets p/e state when
// e+0x58 & 0x1000. Shared tail decrements the e+0x54 SFX timer (or reseeds it from
// func_00122BB8 and fires func_001EFD90 0x8000001D) and calls func_00132490(p,e).
//
// The four scratch words are declared as FLOAT SCALARS and written by value
// (D_700038AC = 1.0f is the target's 0x3F800000); their addresses are passed with
// `&`. Declaring them `char[]` and storing through `*(float *)0x700038A0` is what
// let mwcc speculate the store's address-`lui` into the case-9 dispatch delay slot.

extern void anim_clip_init(char *self, int clip, float a, float b);
extern void func_00131F20(unsigned char *p, void *a, void *b);
extern void func_001028B8(void *a, void *b, void *c);
extern int func_001B3250(unsigned char *p, void *v, float r);
extern int func_001B1560(unsigned char *p, void *v, float r);
extern int func_001B13F0(float r, void *a, void *b);
extern int func_00122BB8(void);
extern void func_001EFD90(int mode, void *b, void *c);
extern void func_00132490(unsigned char *p, unsigned char *e);

extern float D_00810360;
extern float D_700038A0;
extern float D_700038A4;
extern float D_700038A8;
extern float D_700038AC;

void func_0012F6C0(unsigned char *p, unsigned char *e) {
    unsigned short t;

    switch (p[6]) {
    case 0:
        p[6] = p[6] + 1;
        e[0x63] = 0;
        *(short *)(e + 0x54) = 0;
        *(int *)(e + 0x34) = 0x3F800000;
        anim_clip_init((char *)p, 8, 10.0f, 0.0f);
        /* fallthrough */
    case 1:
        if (*(unsigned short *)(e + 0x58) & 0x8000) {
            break;
        }
        p[6] = p[6] + 1;
        /* fallthrough */
    case 2:
        switch (*(short *)(p + 0x2C)) {
        case 8:
            if (*(unsigned short *)(e + 0x58) & 0x7000) {
                anim_clip_init((char *)p, 9, 1.0f, 0.0f);
            }
            break;
        case 9:
            D_700038A0 = 0.0f;
            D_700038A4 = 3.0f;
            D_700038A8 = 45.0f;
            D_700038AC = 1.0f;
            func_00131F20(p, &D_700038A0, &D_700038A0);
            func_001028B8(&D_700038A0, &D_700038A0, p + 0xB0);
            if (func_001B3250(p, &D_700038A0, 15.0f) != 0) {
                anim_clip_init((char *)p, 10, 1.0f, 0.0f);
            } else if (func_001B1560(p, &D_00810360, 1.0471976f) == 0 ||
                       func_001B13F0(45.0f, &D_00810360, p + 0xB0) != 0) {
                anim_clip_init((char *)p, 10, 1.0f, 0.0f);
            }
            break;
        case 10:
            if (*(unsigned short *)(e + 0x58) & 0x1000) {
                p[5] = 0;
                p[6] = 0;
                e[0x62] = 0;
                *(int *)(e + 0x34) = 0x3F800000;
            }
            break;
        }
        if (e[0x69] & 1) {
            t = *(unsigned short *)(e + 0x54);
            if (t != 0) {
                *(short *)(e + 0x54) = (short) (t - 1);
            } else {
                *(short *)(e + 0x54) = ((func_00122BB8() >> 9) & 7) + 5;
                D_700038A0 = *(float *)(p + 0xB0);
                D_700038A4 = *(float *)(p + 0xB4);
                D_700038A8 = *(float *)(p + 0xB8);
                D_700038AC = 1.0f;
                D_700038A4 = *(float *)(e + 0x40);
                func_001EFD90(0x8000001D, &D_700038A0, p + 0xC0);
            }
        }
        func_00132490(p, e);
        break;
    }
}
