// INCLUDE_ASM func_001B61C0  (vram 0x001B61C0, 132 bytes)
// UNDECOMPILED placeholder. The byte-identical machine code for this
// function is assembled from the local splat disassembly (git-ignored;
// regenerate with `build.py setup` from your own disc) and linked by
// fill_unmatched.py — so the rebuilt ELF stays byte-identical with or
// without this file. build.py does NOT compile INCLUDE_ASM stubs.
//
// SEMANTICS (resolved — docs/FINDINGS.md "WEAPON SYSTEM" §6): pad-actuator
// rumble request, func_001B61C0(bigMotor, smallPower, duration, force).
// Gated on rumble-enable byte D_00810119, actuator-ready (pad block
// D_00810E40 +0x12) and pause/mode byte D_00275BE0 != 2; unless `force`,
// an active rumble (+0x16) is not restarted. Writes +0x16 active flag,
// +0x28 duration, +0x18 big-motor on/off, +0x19 small-motor power, then
// submits via libpad func_00111018(port +0x04, slot +0x08, &block[0x18]).
//
// DECOMP ATTEMPT 2026-06-10 — wall-blocked at 93.6% (one scheduling row +
// the reloc rows it blocks). EVERYTHING else matches, including the
// idiom-7 fake-param pinning of the pause byte to $t0 and the pad-block
// pointer to $t1, the bnezl(force)+dup'd `addiu v0,1` likely-slot shape,
// all four preserved branch-delay nops (the lbu candidates are volatile,
// so mwcc did NOT fill — wall #13 does not bite here), and the `sh dur`
// fill of the `beqz big` slot. The single divergence is the PROLOGUE
// ADDRESS-PAIR SPLIT: CW emits `addiu sp; lui v1,%hi(D_00810E40);
// addiu t1,v1,%lo; sq ra` (pair adjacent BEFORE the ra save — the
// float-constant/address prologue-hoist wall family); mwcc 2.3 fills the
// lui->addiu stall with the sq ra (`lui; sq ra; addiu`) at -O3,p/-O4,s/
// -O4/-O4,p, and hoists sq ra above the whole pair at -O2,p. Falsified:
// all four opt levels, p assigned before/after the first guard (after =
// worse, pair sinks into the beqz slot). Once the bytes match,
// inject_relocs.py would supply the D_00810119 HI16/LO16 (it refuses
// while .text differs).
//
// Best attempt (93.6%, semantics verified; regs all match):
//   typedef struct PadBlock { char pad00[4]; int port; int slot;
//       char pad0c[6]; volatile unsigned char ready; char pad13[3];
//       volatile unsigned char active; char pad17; unsigned char bigOn;
//       unsigned char smallPower; char pad1a[14];
//       unsigned short duration; } PadBlock;
//   extern PadBlock D_00810E40;
//   extern volatile unsigned char D_00275BE0; /* gp-rel */
//   extern void func_00111018(int port, int slot, unsigned char *act);
//   void func_001B61C0(int big, int small, int dur, int force,
//                      int mode, PadBlock *p) { /* idiom-7 fakes: t0,t1 */
//       int one;
//       p = &D_00810E40;
//       if (*(volatile unsigned char *)0x00810119 == 0) return;
//       if (p->ready == 0) return;
//       mode = D_00275BE0;
//       if (mode == 2) return;
//       if (force == 0) { if (p->active != 0) return; }
//       one = 1;
//       p->active = one;
//       p->duration = dur;
//       if (big != 0) p->bigOn = one;
//       p->smallPower = small;
//       func_00111018(p->port, p->slot, &p->bigOn);
//   }
//
// To decompile: replace this file with C that compiles byte-identical,
// verified with objdiff against build/expected/func_001B61C0.o. See
// docs/PROGRESS.md for the matching idioms and the function index in
// docs/FUNCTIONS.csv.
