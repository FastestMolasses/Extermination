// NEARMISS func_001BBBF0  (vram 0x001BBBF0, 0x12C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 84.97% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// instruction-scheduling permutation: original CW 2.3.1 schedules the absolute-address minuend load (lui 0x7000/lwc1 *0x700038A0) before the FP-constant load (lui 0x4150=13.0f) in the 2nd/3rd arithmetic blocks; mwcc 2.3.3 and 991202 both order constant-first. Not the clean-store nop class; 2.3.3 ca...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Builds a camera/anchor vector at the scratchpad block 0x700038A0..AC from the
// entity at arg0, then calls func_00102948 twice (copy/transform into D_008105E0
// then D_008105D0). func_0011DE90=cos, func_0011E2A8=sin. First pass: 38A0 =
// field(0xB0) - 8.0*cos(field(0xC4)); 38A4 = 10.0 + field(0xB4); 38A8 =
// field(0xB8) + 8.0*sin(field(0xC4)); 38AC = 1.0f bits. Second pass refines with
// the global angle D_00810374: 38A0 -= 13.0*sin(angle); 38A4 = 12.0 + field(0xB4);
// 38A8 -= 13.0*cos(angle).
//
// NEARMISS (best 84.97% on mwcc 2.3.3, 81.57% on 991202). Logic fully recovered;
// the entire first arithmetic block (through the 38A8 store + first func_00102948
// call) is byte-identical. Sole residual: in the SECOND/THIRD arithmetic blocks
// the minuend is an ABSOLUTE-address load (lui 0x7000 / lwc1 *0x700038A0); the
// original CW 2.3.1 schedules that address-load BEFORE the float constant setup
// (lui 0x4150 = 13.0f), whereas both mwcc 2.3.3 and 991202 emit the constant
// first. Pure instruction-scheduling permutation of an absolute-address load vs.
// an FP-constant load; not the clean-store delay-slot-nop class, so 2.3.3 does
// not fix it. Tried temp-load-minuend and sdatathreshold 4/8 (both worse).
extern float func_0011DE90(float a);   /* cos */
extern float func_0011E2A8(float a);   /* sin */
extern void func_00102948(void *dst, void *src);
extern float D_00810374;
extern char D_008105D0;
extern char D_008105E0;
extern float D_700038A0;

int func_001BBBF0(char *arg0) {
    *(float *)0x700038A0 = *(float *)(arg0 + 0xB0) - 8.0f * func_0011DE90(*(float *)(arg0 + 0xC4));
    *(float *)0x700038A4 = 10.0f + *(float *)(arg0 + 0xB4);
    *(float *)0x700038A8 = *(float *)(arg0 + 0xB8) + 8.0f * func_0011E2A8(*(float *)(arg0 + 0xC4));
    *(int *)0x700038AC = 0x3F800000;
    func_00102948(&D_008105E0, &D_700038A0);
    *(float *)0x700038A0 = *(float *)0x700038A0 - 13.0f * func_0011E2A8(D_00810374);
    *(float *)0x700038A4 = 12.0f + *(float *)(arg0 + 0xB4);
    *(float *)0x700038A8 = *(float *)0x700038A8 - 13.0f * func_0011DE90(D_00810374);
    func_00102948(&D_008105D0, &D_700038A0);
    return 1;
}
