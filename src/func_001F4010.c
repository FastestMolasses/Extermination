// NEARMISS func_001F4010  (vram 0x001F4010, 0xA8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 92.38% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-coloring permutation. Full logic recovered; needs -sdatathreshold 8 (D_00275C40 must be gp-rel lw 0(gp), not absolute lui/lw) and slot computed/declared before e. Bottom half (counter advance/wrap) is byte-identical. Sole residual: target colors slot->s0, idx->s1, e->s2 (saving idx into ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// Per-call ring-buffer slot writer. Computes two struct pointers:
//   slot = &D_007709C0[D_00275C40 * 0x90]  (0x90-stride ring entry, index in
//          gp-rel counter D_00275C40)
//   e    = &D_0025A350[idx * 0x60]         (0x60-stride table entry, by idx)
// then runs func_001F2F90(arg1, slot, e, idx) and func_001F3340(slot, e, idx),
// advances the ring counter D_00275C40 and wraps it back to 0 at 0x80.
extern void func_001F2F90(int a, char *b, char *c, int d);
extern void func_001F3340(char *a, char *b, int c);
extern char D_0025A350[];
extern int D_00275C40;
extern char D_007709C0[];

void func_001F4010(int idx, int arg1)
{
  char *slot;
  char *e;
  slot = &D_007709C0[D_00275C40 * 0x90];
  e = &D_0025A350[idx * 0x60];
  func_001F2F90(arg1, slot, e, idx);
  func_001F3340(slot, e, idx);
  D_00275C40 += 1;
  if (D_00275C40 >= 0x80)
  {
    D_00275C40 = 0;
  }
}
