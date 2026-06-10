// INCLUDE_ASM func_001B6990  (vram 0x001B6990, 324 bytes)
// UNDECOMPILED placeholder. The byte-identical machine code for this
// function is assembled from the local splat disassembly (git-ignored;
// regenerate with `build.py setup` from your own disc) and linked by
// fill_unmatched.py — so the rebuilt ELF stays byte-identical with or
// without this file. build.py does NOT compile INCLUDE_ASM stubs.
//
// To decompile: replace this file with C that compiles byte-identical,
// verified with objdiff against build/expected/func_001B6990.o. See
// docs/PROGRESS.md for the matching idioms and the function index in
// docs/FUNCTIONS.csv.
//
// SEMANTICS + match attempt (2026-06-10 s15): area-load actor SPAWNER.
// desc = D_0024D7C0[D_00810700]; table = desc[D_00810701]; for each
// 0x28-byte placement record until cls==0xFF: skip cls&0xFF==0x0B
// (scripted/deferred); else actor = func_001AFA90(cls&0xFF) (abort scan if
// pool empty) and copy: model->+3, flags2 (lh rec+2 >>8 &0xFF)->+0x2E(u16),
// param lo byte->+0xD, table index->+0x9A, kind->+0x54, link->+0x56,
// pos->+0xB0/B4/B8, rot->+0xC0/C4/C8, behavior fn->+0x10; uid->+0xE(u16)
// EXCEPT class 2 (link-special), which instead gets +0x9D=D_00810701 and
// +0x9E=uid lo byte. Goto-shaped readable C (-O4,p -sdatathreshold 0)
// reproduced every field op and the rotated loop, blocked ~93% by three
// confirmed walls: (a) delay-slot-fill — CW leaves `beqz/bne; nop` at the
// desc==0 guard and the class==2 test, mwcc hoists the following `lui $at`
// into both slots; (b) dead-dup — CW emits a dead `addiu v1,0xB` before
// the loop-top label (dup of its bottom-branch delay slot); (c) CW fills
// the bottom `bne` delay with that loop-invariant `addiu v1,0xB`, mwcc
// instead emits `bnel` with the `andi a0,0xFF` in the slot.
