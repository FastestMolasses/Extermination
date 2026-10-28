// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 2.80% NEARMISS).
// The C was already exact; splat had merged this 0xC-byte handler and the
// opcode 0x0A handler at 0x001B9A00 (now its own symbol and file,
// src/func_001B9A00.c) into one 0x1AC symbol. config/symbol_addrs.txt now gives
// this function size 0xC; the word at 0x1B99FC is alignment padding.
//
// Script opcode 0x09 CALL NATIVE (see func_001BA1F0): tail-jump to the
// record's own handler.

int func_001B99F0(void *actor, void *blk, unsigned int *rec)
{
    return ((int (*)(void *, void *, unsigned int *))rec[1])(actor, blk, rec);
}
