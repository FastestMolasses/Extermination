// NEARMISS func_001B0EA0  (vram 0x001B0EA0, 0xC0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.83% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Prologue instruction-scheduling artifact: body 100% identical except one instruction's position. The target schedules `lbu a1,0xd(a0)` (reading *(self+0xD) through the param reg a0) BEFORE the `paddub s0,a0,zero` self->s0 save; mwcc 2.3.3/991202 both emit the save first then read `lbu a1,0xd(s0)`...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// Animation/skeleton init for object `self`. Resolves an animation handle via
// func_001C6120(D_0028A59C, self+0xD) and binds it with func_001CA6E0; sets the
// bone count self+0xC = func_001C6150(*(self+0x44)). If the count exceeds the
// global cap D_00275BCC, flags self+4 = 3 (error/over-cap) and returns 1.
// Otherwise allocates per-bone state: for each of the self+0xC bones, stores
// func_001AF780() into the self+0x110 array (4-byte stride), records the final
// count at self+9, calls anim_bone_array_setup(self+0xC) and returns 0.
//
// Body byte-identical to the target on mwcc 2.3.3; the SOLE residual is one
// prologue instruction's schedule: the target emits `lbu a1,0xd(a0)` (reading
// the +0xD arg through the param reg a0) BEFORE copying self into s0, whereas
// mwcc emits the s0 save first then `lbu a1,0xd(s0)`. Register coloring
// (self=s0,i=s1,p=s2), control flow, and the gp-rel split (D_00275BCC gp-rel,
// D_0028A59C forced %hi/%lo via the >threshold array decl) all match exactly.
// Pure list-scheduler permutation -> permuter territory.
extern void anim_bone_array_setup(unsigned char);
extern int func_001AF780(void);
extern int func_001C6120(int, unsigned char);
extern unsigned char func_001C6150(int);
extern void func_001CA6E0(unsigned char *, int);
extern short D_00275BCC;
extern int D_0028A59C[3];

int func_001B0EA0(unsigned char *self) {
    int i;
    unsigned char *p;
    unsigned char n;

    func_001CA6E0(self, func_001C6120(D_0028A59C[0], *(unsigned char *)(self + 0xD)));
    *(unsigned char *)(self + 0xC) = func_001C6150(*(int *)(self + 0x44));
    if ((int)D_00275BCC < (int)*(unsigned char *)(self + 0xC)) {
        *(unsigned char *)(self + 4) = 3;
        return 1;
    }
    i = 0;
    p = self;
    while (i < (int)(n = *(unsigned char *)(self + 0xC))) {
        *(int *)(p + 0x110) = func_001AF780();
        p += 4;
        i++;
    }
    *(unsigned char *)(self + 9) = n;
    anim_bone_array_setup(*(unsigned char *)(self + 0xC));
    return 0;
}
