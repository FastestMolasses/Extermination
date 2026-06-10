// INCLUDE_ASM func_001BC350  (vram 0x001BC350, 516 bytes)
// UNDECOMPILED placeholder. The byte-identical machine code for this
// function is assembled from the local splat disassembly (git-ignored;
// regenerate with `build.py setup` from your own disc) and linked by
// fill_unmatched.py — so the rebuilt ELF stays byte-identical with or
// without this file. build.py does NOT compile INCLUDE_ASM stubs.
//
// To decompile: replace this file with C that compiles byte-identical,
// verified with objdiff against build/expected/func_001BC350.o. See
// docs/PROGRESS.md for the matching idioms and the function index in
// docs/FUNCTIONS.csv.
//
// SEMANTICS (2026-06-10 s15): DOOR behavior (placement +0x24 target for
// class-5 double doors). Outer lifecycle byte +0x04: 0=INIT
// (func_001BBDA0: model bind, +0x34=door id from +0x2E, scale from link
// flags 0x40/0x80, then status byte +0 = 1), 1=RUN, 2/3=FREE
// (func_001AFC10). RUN: inner sub-state byte +0x05 via jtbl_0026E1C0:
//   0 closed   model 0x15 + unlock bit (D_00810841[area] >> +0x34) set ->
//              func_001BBE40(self, blk=+0x1F0, 0) -> sub 3 (walk-through);
//              bit clear -> func_001BBE40(.., 1) -> sub 1 (locked seq);
//              other models -> func_001BBE40(.., 0) -> sub 3
//   1 locked   func_001BC0E0 pump; done -> func_001BA1A0(blk, D_0024DBC0)
//              (queue script) -> sub 2
//   2 locked2  func_001BC0E0 pump; done -> +0xB=0, sub 0
//   3 opening  func_001BC0E0 pump; done -> sub 4
//   4 commit   func_001BC240: advance clip + func_001BC150 (transition
//              request D_008106B5..B8) -> sub 5
//   5 close    func_001BC290: advance clip; when D_008106B8==0 reset clip,
//              +0xB=0, sub 0
// then ALWAYS func_001BC300 (evaluate articulation via func_001C68C0,
// cull at pos+(0,10,0), run +0x4C method). See docs/FINDINGS.md s15.
