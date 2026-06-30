// NEARMISS func_001C3BE0  (vram 0x001C3BE0, 0x180 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.79% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP register-coloring permutation: target colors the final vec3 copy temps f2/f1/f0 (descending), mwcc233 uses f0/f1/ft0 (+2 structural offset). 99.79% (4 instr), body fully matched. Permuter territory.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS 99.79% (mwcc233). Body/logic fully recovered. Sole residual:
// FP register-coloring permutation on the final 0x3600->0x3000 vec3 copy.
// Target colors the three temps f2,f1,f0 (descending, CW temp-pool from
// top); mwcc233 colors them f0,f1,ft0 (+2 structural offset). The three
// load addresses (0x3600/04/08) and store addresses (0x3000/04/08) and
// all 14 leading scratchpad stores match exactly. Permuter territory
// (FP coloring). All non-volatile relocated pointer args (&D_700036xx)
// and the $at-form volatile absolute stores both reproduce CW exactly.
//
// Semantics: VU0/scratchpad (0x70003000 region) matrix setup. Loads a
// transform from arg1 (offsets 0x70-0x88) into scratchpad rows, builds a
// rotation via func_00102718, copies the result (0x3600->0x3000), then
// func_00102B08/BB0 apply two scalar params (arg0+0xC0/0xC4) and
// func_001026D0 composes into 0x3000.

extern void func_001029C0(void *);
extern void func_00102718(void *, void *, void *);
extern void func_00102B08(void *, void *, float);
extern void func_00102BB0(void *, void *, float);
extern void func_001026D0(void *, void *, void *);

extern unsigned char D_70003000;
extern unsigned char D_70003440;
extern unsigned char D_70003600;
extern unsigned char D_70003610;
extern unsigned char D_70003620;

void func_001C3BE0(char *arg0, char *arg1) {
    func_001029C0(&D_70003000);
    *(volatile float *)0x70003020 = *(float *)(arg1 + 0x70);
    *(volatile float *)0x70003024 = *(float *)(arg1 + 0x74);
    *(volatile float *)0x70003028 = *(float *)(arg1 + 0x78);
    *(volatile float *)0x70003010 = *(float *)(arg1 + 0x80);
    *(volatile float *)0x70003014 = *(float *)(arg1 + 0x84);
    *(volatile float *)0x70003018 = *(float *)(arg1 + 0x88);
    *(volatile float *)0x70003610 = *(float *)(arg1 + 0x80);
    *(volatile float *)0x70003614 = *(float *)(arg1 + 0x84);
    *(volatile float *)0x70003618 = *(float *)(arg1 + 0x88);
    *(volatile int *)0x7000361C = 0x3F800000;
    *(volatile float *)0x70003620 = *(float *)(arg1 + 0x70);
    *(volatile float *)0x70003624 = *(float *)(arg1 + 0x74);
    *(volatile float *)0x70003628 = *(float *)(arg1 + 0x78);
    *(volatile int *)0x7000362C = 0x3F800000;
    func_00102718(&D_70003600, &D_70003610, &D_70003620);
    {
        float a = *(volatile float *)0x70003600;
        float b = *(volatile float *)0x70003604;
        float c = *(volatile float *)0x70003608;
        *(volatile float *)0x70003000 = a;
        *(volatile float *)0x70003004 = b;
        *(volatile float *)0x70003008 = c;
    }
    func_001029C0(&D_70003440);
    func_00102B08(&D_70003440, &D_70003440, *(float *)(arg0 + 0xC0));
    func_00102BB0(&D_70003440, &D_70003440, *(float *)(arg0 + 0xC4));
    func_001026D0(&D_70003000, &D_70003000, &D_70003440);
}
