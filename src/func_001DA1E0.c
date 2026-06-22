// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Object/effect-slot initialiser. Indexes a gp-rel pool-base table
// (D_00275670, an array of per-index struct pointers) by arg0; the entry's
// +0x10 field is a bump-allocator cursor into a 0x80-byte record block. Stamps
// the new record's header (byte +0x3 = 0x10, word +0x4 = 0, half +0x0 = 7),
// advances the cursor by 0x80, then fills the record: zero the first quadword
// (+0x10), a GIFtag-style word 0x50000006 (+0x1C), the doubleword
// 0x50224000_00008001 (+0x20) and 0x44441 (+0x28), three zero words
// (+0x30/34/38), arg2 (+0x3C), then four quadwords copied from arg1 (the
// caller's 0x40-byte vector/matrix payload) into +0x40..0x70. Returns the
// record body pointer (cursor +0x10).
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906). Two non-obvious steers were
// needed for byte-exactness: (1) writing the base address as the array-index
// form `&D_00275670[arg0]` makes mwcc load the gp-rel base pointer BEFORE
// computing arg0*4 (CW's operand-evaluation order) — the `(char*)D_00275670 +
// arg0*4` form evaluates the shift first and diverges; (2) the explicit
// `ret = t1 + 0x10` local reproduces CW's early `addiu v0, t1, 0x10`
// materialization of the return value. The four 0x40-byte payload slots use a
// 128-bit type to emit the interleaved lq/sq block. Verified objdiff 100% vs
// build/expected/func_001DA1E0.o (991202 = 65.88%).
typedef unsigned __int128 u128;
extern char **D_00275670;
char *func_001DA1E0(int arg0, char *arg1, int arg2) {
  char *t1;
  char *ret;
  char **t3;
  t3 = (char **)&D_00275670[arg0];
  *(char *)(t3[4] + 3) = 0x10;
  *(int *)(t3[4] + 4) = 0;
  *(short *)(t3[4] + 0) = 7;
  t1 = t3[4];
  ret = t1 + 0x10;
  t3[4] = t1 + 0x80;
  *(u128 *)ret = 0;
  *(int *)(t1 + 0x1C) = 0x50000006;
  *(long long *)(t1 + 0x20) = (long long)0x8001 | ((long long)0x50224000 << 0x20);
  *(long long *)(t1 + 0x28) = 0x44441;
  *(int *)(t1 + 0x30) = 0;
  *(int *)(t1 + 0x34) = 0;
  *(int *)(t1 + 0x38) = 0;
  *(int *)(t1 + 0x3C) = arg2;
  *(u128 *)(t1 + 0x40) = *(u128 *)(arg1 + 0x00);
  *(u128 *)(t1 + 0x50) = *(u128 *)(arg1 + 0x10);
  *(u128 *)(t1 + 0x60) = *(u128 *)(arg1 + 0x20);
  *(u128 *)(t1 + 0x70) = *(u128 *)(arg1 + 0x30);
  return ret;
}
