// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
//
// SEMANTICS (resolved s33 — docs/FINDINGS.md "KIND-0xE COMPANION
// RESOLVED"): brain of the kind-0xE generator child — the breather
// pad's TENDRIL FIELD (12 ground spikes erupting around the player;
// no damage, no HP). Lifecycle dispatcher on state +0x04:
//   0 -> func_00154740(self, self+0x1F0)  init (spike model bind
//        D_0028A490[0x15], per-room tint D_00246800)
//   1 -> func_001549C0(self, self+0x1F0)  scan/deploy/hold/retract
//   2 -> state = 3
//   3 -> func_001AFC10(self)              free
//
// READABLE-C ATTEMPT 2026-06-10 — wall-blocked at 29/30 instructions
// (wall #13, same family as func_00153B50): a plain C
//   switch (self[4]) { case 0: func_00154740(self, self+0x1F0); ...
//   case 2: self[4] = 3; ... case 3: func_001AFC10(self); }
// reproduces the reverse-order beq chain, the $a1 scratch-pointer
// hoist into the beq(3) slot, the $a2 chain-constant reuse by case
// 2's sb, and the dup'd `lq $ra` in the default branch's slot — but
// mwcc fills the beq(case 2) delay slot with the next chain constant
// (`addiu $v1,1`) where CW left a nop: body one instruction short,
// every later branch offset off by one. Keep this stub.
extern void func_00154740(int, int, int, int);
extern void func_001549C0(int, int, int, int);
extern void func_001AFC10(int, int, int, int);

asm void func_001546C0(void) {
    addiu $sp, $sp, -0x10
    sq $ra, 0x0($sp)
    lbu $a3, 0x4($a0)
    addiu $a2, $zero, 0x3
    .word 0x10e60015
    addiu $a1, $a0, 0x1F0
    addiu $v1, $zero, 0x2
    .word 0x10e30010
    nop
    addiu $v1, $zero, 0x1
    .word 0x10e30009
    nop
    .word 0x10e00003
    nop
    .word 0x1000000e
    lq $ra, 0x0($sp)
    jal func_00154740
    nop
    .word 0x10000009
    nop
    jal func_001549C0
    nop
    .word 0x10000005
    nop
    .word 0x10000003
    sb $a2, 0x4($a0)
    jal func_001AFC10
    nop
    lq $ra, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x10
}
