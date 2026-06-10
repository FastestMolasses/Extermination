// INCLUDE_ASM func_00179BC0  (vram 0x00179BC0, 212 bytes)
// UNDECOMPILED placeholder. The byte-identical machine code for this
// function is assembled from the local splat disassembly (git-ignored;
// regenerate with `build.py setup` from your own disc) and linked by
// fill_unmatched.py — so the rebuilt ELF stays byte-identical with or
// without this file. build.py does NOT compile INCLUDE_ASM stubs.
//
// WALL-BLOCKED at 96.23% (2026-06-09 attempt; objdiff vs expected). This is
// bone_matrix_publish. Semantics are fully understood; three independent
// blockers keep it from byte-matching from C:
//   1. slt-into-branch regalloc (documented wall): loop guard needs
//      `slt $v1`, mwcc emits `slt $at`.
//   2. Dead-instruction-after-branch (documented wall): CW leaves a dead
//      `lui $v0,0x3F80` dup after the then-branch's `b`.
//   3. NEW datum — saved-register allocation order: mwcc assigns s0..s3 by
//      WEB-START (emission) position, CW by DECLARATION order. CW gives the
//      loop counter/byte-offset (initialized late, in the for-init, emitted
//      after both calls) s0/s1 and the saved params s2/s3. With mwcc you can
//      have EITHER correct regs (init i/off at declaration -> inits wrongly
//      emitted in the prologue) OR correct placement (for-init -> params
//      steal s0/s1). Both early+late init fails too (dead-store elim runs
//      before allocation). `register` hints and -O3,p change nothing.
//
// Best attempt (compiles, correct logic, 96.23% — all remaining diffs are
// the three blockers above plus the induced s0<->s2/s1<->s3 permutation):
//
//   typedef unsigned __int128 uint128;
//   typedef struct Mat4 { uint128 q[4]; } Mat4;
//   extern unsigned char **D_00275B40;            // bone NODE ptr array base
//   extern int func_001749F0(unsigned char *obj, int id, float a, float b);
//   extern void func_001C6DA0(unsigned char *obj); // anim_eval_skeleton
//   extern void func_00102958(Mat4 *dst, Mat4 *src);
//
//   void func_00179BC0(unsigned char *objp, int id, Mat4 *dstp) {
//       int i;
//       int off;
//       Mat4 *dst = dstp;
//       unsigned char *obj = objp;
//       unsigned char type = objp[0x1F0];
//
//       if (type == 0x31 || type == 0x34) {
//           func_001749F0(obj, id, 0.0f, (float)*(short *)(obj + 0x276));
//       } else {
//           func_001749F0(obj, id, 0.0f, 1.0f);
//       }
//       func_001C6DA0(obj);
//
//       for (i = 0, off = 0; i < obj[0xC]; i++) {
//           func_00102958(dst,
//               (Mat4 *)(*(unsigned char **)((unsigned char *)D_00275B40 + off) + 0x90));
//           off += 4;
//           dst++;
//       }
//   }
//
// Note: the incoming `id` (a1) is forwarded untouched to func_001749F0 —
// passing the param straight through generates no code, matching the asm.
