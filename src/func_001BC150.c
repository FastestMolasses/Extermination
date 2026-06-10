// INCLUDE_ASM func_001BC150  (vram 0x001BC150, 228 bytes)
// UNDECOMPILED placeholder. The byte-identical machine code for this
// function is assembled from the local splat disassembly (git-ignored;
// regenerate with `build.py setup` from your own disc) and linked by
// fill_unmatched.py — so the rebuilt ELF stays byte-identical with or
// without this file. build.py does NOT compile INCLUDE_ASM stubs.
//
// WALL-BLOCKED at 85.61% (2026-06-10, 3 attempts + O2/O3/O4 sweep — all
// shapes identical at O3/O4, O2 60.9%). The C below is semantically
// correct and the best attempt. Three residual walls:
//  1. TEMP-REGISTER POOL ORDER (new datum): for the four short pre-branch
//     temp webs (door-id byte, dest-table base pair, area byte, id
//     halfword) CW 2.3.1 allocates v1 / v0->a1 split reloc pair / a2 / v0;
//     mwcc 2.3 allocates a1 / v1 same-reg pair / a0 / v0 — it prefers
//     free ARGUMENT registers for short temp webs and never picks a2.
//     Idiom-7 fake-param pinning FAILS here: assigning a loaded value to
//     the fake param kill-renames the web and mwcc reallocates it by its
//     own pool order (idiom 7 worked because the matrix-copy temps' loads
//     were the webs' only definition points feeding stores, call-free).
//     Statement reorder (idiom 12) does not move the loads: the scheduler
//     hoists all three loads above the address pair regardless of source
//     order.
//  2. Prologue ADDRESS-pair split (documented wall, cf. func_001B61C0):
//     CW interleaves `lui v0,%hi(D_0024E140)` / `addiu a1,v0,%lo` around
//     the `sq s0` save inside the prologue; mwcc keeps the (same-register)
//     pair in the body below the first loads.
//  3. Wall #13 at the has-sub test: CW leaves `bnez v1,...; nop`; mwcc
//     fills the slot with the safe fall-through `addiu v1,zero,0xFF`.
//
// Semantics (verified live, docs/FINDINGS.md "AREA TRANSITION LIFECYCLE"):
// door transition COMMIT, called from sub-state 4 (func_001BC240). Record
// = D_0024E140[D_00810700] + 4*(door id +0x34 & 0x7F). Door-id bit 7 set
// -> inter-AREA change: func_001B0C00(4) (fade + audio fades), request
// {B8=1, B5=rec[0] next area, B7=rec[1] entry, B6=rec[2]?rec[3]:0xFF sub}.
// Bit 7 clear -> same-area room move: func_001AEDE0(4,0) fade only,
// {B8=2, B7=rec[side latch +0x2E]}.
//
// extern unsigned char *D_0024E140[];
// extern unsigned char D_00810700;
// extern unsigned char D_008106B5, D_008106B6, D_008106B7, D_008106B8;
// extern void func_001B0C00(int speed);
// extern void func_001AEDE0(int speed, int mode);
//
// void func_001BC150(unsigned char *self) {
//     unsigned char *rec;
//     unsigned char id;
//     short hw;
//     unsigned char area;
//
//     id = self[0x34];
//     hw = *(short *)(self + 0x34);
//     area = D_00810700;
//     rec = D_0024E140[area] + ((id & 0x7F) * 4);
//     if (hw & 0x80) {
//         func_001B0C00(4);
//         D_008106B8 = 1;
//         D_008106B5 = rec[0];
//         D_008106B7 = rec[1];
//         if (rec[2] == 0) {
//             D_008106B6 = 0xFF;
//         } else {
//             D_008106B6 = rec[3];
//         }
//     } else {
//         func_001AEDE0(4, 0);
//         D_008106B8 = 2;
//         D_008106B7 = rec[*(unsigned short *)(self + 0x2E)];
//     }
// }
//
// To decompile: replace this file with C that compiles byte-identical,
// verified with objdiff against build/expected/func_001BC150.o. See
// docs/PROGRESS.md for the matching idioms and the function index in
// docs/FUNCTIONS.csv.
