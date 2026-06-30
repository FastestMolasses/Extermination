// NEARMISS func_001F3FA0  (vram 0x001F3FA0, 0x64 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 72.60% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// loop-invariant-%hi-CSE / addressing-mode: target recomputes 'lui at,%hi(D_004E1340); addu at,a3,at; sh a0,%lo(D_004E1340)(at)' per iteration off the running base register; mwcc hoists the full address materialization of &D_004E1340 out of the loop (and when forced gp-rel via a small decl, emits %...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Initializes 0x80 elements of a 0x90-byte-stride array based at
// D_0028F700+0x4E12C0: clears 9 quadwords (0x90 bytes) per element, then
// sets a 16-bit flag at element offset 0x80 (symbol D_004E1340) to 1.
// Finally zeroes gp-rel globals D_00275C40 and D_00275C44.
// NEARMISS: logic byte-exact (nested clear loop + flag store + the two
// gp-rel zero stores). Residual is an addressing-mode / loop-invariant
// CSE artifact: the target recomputes %hi(D_004E1340) inside the loop and
// adds the running base pointer (addu at,a3,%hi; sh ..,%lo(at)), while
// mwcc hoists the full &D_004E1340 address out of the loop, plus a
// one-off register-allocation cascade (a2/a3/a4 vs a3/a1/...). The split
// '%gp_rel(D_00275C40/44) absolute D_0028F700/D_004E1340' addressing mix
// is reproduced, but mwcc will not be steered into the per-iteration %hi
// recompute without permuter-style scheduling pressure.
typedef int s128 __attribute__((mode(TI)));
extern int D_00275C40;
extern int D_00275C44;
extern char D_0028F700[16];
extern short D_004E1340;

void func_001F3FA0(void) {
    char *base = D_0028F700;
    int i = 0;
    do {
        s128 *p = (s128 *)(base + 0x4E12C0);
        int n = 9;
        do {
            *p = 0;
            n -= 1;
            p += 1;
        } while (n != 0);
        *(short *)(base + (int)&D_004E1340) = 1;
        i += 1;
        base += 0x90;
    } while (i < 0x80);
    D_00275C40 = 0;
    D_00275C44 = 0;
}
