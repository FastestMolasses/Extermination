// INCLUDE_ASM func_00187350  (vram 0x00187350, 828 bytes)
// UNDECOMPILED placeholder. The byte-identical machine code for this
// function is assembled from the local splat disassembly (git-ignored;
// regenerate with `build.py setup` from your own disc) and linked by
// fill_unmatched.py — so the rebuilt ELF stays byte-identical with or
// without this file. build.py does NOT compile INCLUDE_ASM stubs.
//
// SEMANTICS (s37, FINDINGS "FOOTSTEP SURFACE TABLE"): per-frame player
// FOOTSTEP slice. Switches on the action byte +0x1F0:
//   - 1/2/0x2F/0x41 (locomotion): reads the per-anim property row
//     D_00248C90[+0x20C].frameA/frameB (both must be nonzero); a
//     3-state step phase +0x25E (0 -> fire at clip time <= frameA,
//     left foot; 1 -> fire at <= frameB, right foot; else re-arm when
//     +0x200 & 0xB000). Each fire = func_00182430(self, gait +0x25C)
//     surface+gear sounds, then func_00187EE0(self, footL, footR)
//     decal/FX with the skeleton foot nodes *(D_00275B40)+0x44/+0x48.
//   - 0x36/0x37 (melee) and the default path: consume the +0x25E
//     mailbox 0x80|n (planted by the melee impact gates, s36) ->
//     func_00182430(self, n); default path also fires the decal with
//     actor +0xB0/+0xD0.
//   Tail (every frame): wet-feet timer +0x212 = 120 while floor attr
//   +0x23A is 6 or 0x5B, else decrement; wade layer while +0x23C != 0
//   (water depth state) and moving: ripple effect 0x8000001D + level
//   func_001E8B90(pos, 0.3*speed), gated off when D_00810700 == 0x15.
//
// MATCHING: wall #13 (delay-slot fill). The +0x1F0 compare chain is
// beq;nop pairs whose fall-through candidates are the next chain
// constants (safe to speculate — mwcc fills the slots; CW 2.3.1 only
// fills from the taken path). Same shape as func_00153B50 (idiom 17
// residual). Not attemptable from C until that wall cracks.
//
// To decompile: replace this file with C that compiles byte-identical,
// verified with objdiff against build/expected/func_00187350.o. See
// docs/PROGRESS.md for the matching idioms and the function index in
// docs/FUNCTIONS.csv.
