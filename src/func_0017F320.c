// NEARMISS func_0017F320  (vram 0x0017F320, 0x2C4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.55% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Pure delay-slot-fill / scheduling permuter class. Logic fully recovered (97.6%). The three beqz guards each leave their delay slot fillable with the NEXT probe's first `lui <const>` (cone direction) and the target fills it; mwcc 2.3.3 instead schedules `lui at,0x7000` / the return-value `addiu v0...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS (best: 97.6% mwcc 2.3.3, 86.0% mwcc 991202). Logic fully recovered.
// Runs four directional cone/ray probes off the entity's transform (arg0+0xD0) and
// returns whether ANY of them was blocked. Each probe fills two scratch vectors at
// 0x700038A0/0xB0 (cone params: a few float bit-patterns -- direction +/-3.0f, length
// 20.0f/19.0f, and constants 0/1.0f/6.5f/1.0f), transforms them through
// func_001026A0 into 0x700038C0/0xD0, then casts via func_0019AFE0(self, c0, d0, 6);
// a nonzero result ORs a hit bit into `flags` (probes 1&3 set bit 0, probes 2&4 set
// bit 1). The four probes are +3.0f/-3.0f direction at length 20.0f, then +3.0f/-3.0f
// at length 19.0f. Returns 1 when no probe hit (flags==0), else 0 (matches the
// `v0=1; movn v0,zero,flags` epilogue). Note: the scratch writes use raw absolute
// 0x700038xx addresses while the call args use the &D_700038xx symbols, mirroring the
// target's mixed lui/ori vs %hi/%lo relocations.
//
// Residual wall (NOT the clean-store nop): the three `beqz` guards each leave their
// delay slot fillable with the NEXT probe's first `lui <const>` (cone direction), and
// the target fills it; mwcc 2.3.3 instead schedules `lui at,0x7000` / the return-value
// `addiu v0,1` there. Pure delay-slot-fill / scheduling permuter class.
extern void func_001026A0(float *dst, char *src, float *m);
extern int func_0019AFE0(int self, float *a, float *b, int n);
extern float D_700038A0;
extern float D_700038B0;
extern float D_700038C0;
extern float D_700038D0;

int func_0017F320(int arg0) {
    int flags;

    *(int *)0x700038A0 = 0x40400000;
    *(int *)0x700038B0 = 0x40400000;
    *(int *)0x700038A4 = 0x41A00000;
    *(int *)0x700038B4 = 0x41A00000;
    *(int *)0x700038A8 = 0;
    *(int *)0x700038AC = 0x3F800000;
    *(int *)0x700038B8 = 0x40D00000;
    *(int *)0x700038BC = 0x3F800000;
    flags = 0;
    func_001026A0(&D_700038C0, (char *)(arg0 + 0xD0), &D_700038A0);
    func_001026A0(&D_700038D0, (char *)(arg0 + 0xD0), &D_700038B0);
    if (func_0019AFE0(arg0, &D_700038C0, &D_700038D0, 6) != 0) {
        flags |= 1;
    }

    *(int *)0x700038A0 = 0xC0400000;
    *(int *)0x700038B0 = 0xC0400000;
    *(int *)0x700038A4 = 0x41A00000;
    *(int *)0x700038B4 = 0x41A00000;
    *(int *)0x700038A8 = 0;
    *(int *)0x700038AC = 0x3F800000;
    *(int *)0x700038B8 = 0x40D00000;
    *(int *)0x700038BC = 0x3F800000;
    func_001026A0(&D_700038C0, (char *)(arg0 + 0xD0), &D_700038A0);
    func_001026A0(&D_700038D0, (char *)(arg0 + 0xD0), &D_700038B0);
    if (func_0019AFE0(arg0, &D_700038C0, &D_700038D0, 6) != 0) {
        flags |= 2;
    }

    *(int *)0x700038A0 = 0x40400000;
    *(int *)0x700038B0 = 0x40400000;
    *(int *)0x700038A4 = 0x41980000;
    *(int *)0x700038B4 = 0x41980000;
    *(int *)0x700038A8 = 0;
    *(int *)0x700038AC = 0x3F800000;
    *(int *)0x700038B8 = 0x40D00000;
    *(int *)0x700038BC = 0x3F800000;
    func_001026A0(&D_700038C0, (char *)(arg0 + 0xD0), &D_700038A0);
    func_001026A0(&D_700038D0, (char *)(arg0 + 0xD0), &D_700038B0);
    if (func_0019AFE0(arg0, &D_700038C0, &D_700038D0, 6) != 0) {
        flags |= 1;
    }

    *(int *)0x700038A0 = 0xC0400000;
    *(int *)0x700038B0 = 0xC0400000;
    *(int *)0x700038A4 = 0x41980000;
    *(int *)0x700038B4 = 0x41980000;
    *(int *)0x700038A8 = 0;
    *(int *)0x700038AC = 0x3F800000;
    *(int *)0x700038B8 = 0x40D00000;
    *(int *)0x700038BC = 0x3F800000;
    func_001026A0(&D_700038C0, (char *)(arg0 + 0xD0), &D_700038A0);
    func_001026A0(&D_700038D0, (char *)(arg0 + 0xD0), &D_700038B0);
    if (func_0019AFE0(arg0, &D_700038C0, &D_700038D0, 6) != 0) {
        flags |= 2;
    }

    return (flags == 0) ? 1 : 0;
}
