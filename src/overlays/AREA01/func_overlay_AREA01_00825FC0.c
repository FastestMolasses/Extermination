// 2-insn epilogue stub: tears down a 0x20-byte stack frame and returns.
// Standalone callable (likely a shared epilogue for several callers in AREA01).
asm void func_overlay_AREA01_00825FC0(void) {
    jr    $ra
    addiu $sp, $sp, 0x20
}
